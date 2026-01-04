#include "botanwrapper.h"
#include "dtenc1.h"

#include <cstdint>
#include <vector>
#include <string_view>
#include <stdexcept>
#include <cstring>

#ifdef Q_OS_WIN
    #include "win/botan_all.h"
#endif

#ifdef Q_OS_MACOS
    #include "mac/botan_all.h"
#endif

#ifdef Q_OS_LINUX
    #include "linux/botan_all.h"
#endif

/*
  =============================================================================
  Layman explanation (what is happening)

  You now support TWO ciphertext formats:

  1) New format (DTENC1):
     - EncryptWithPassword() outputs BASE64 text.
     - After base64 decoding, the bytes start with "DTENC1".
     - Then a small header tells how long SALT and NONCE are.
     - Then: SALT + NONCE + (CIPHERTEXT + AUTH TAG)

     Decrypt() first tries to base64-decode and check for "DTENC1".
     If it finds it, it decrypts using modern primitives.

  2) Old format (CryptoBox legacy):
     - This is whatever CryptoBox::encrypt produced previously (often armored text).
     - If input is NOT DTENC1, Decrypt() falls back to CryptoBox::decrypt.
     - This keeps backward compatibility.

  Why salt + HKDF if Password is BCryptGenerate output?
     - Your “Password” is not a human password; it is a bcrypt hash string (ASCII).
     - Still, you want each encryption to use a different key even with the same bcrypt string.
     - So we create a random SALT per message, and use HKDF(SHA-256) to derive a fresh key.

  AEAD cipher:
     - ChaCha20Poly1305 provides confidentiality + integrity.
     - Wrong key or modified ciphertext causes decryption to fail (tag mismatch).
  =============================================================================
*/

// Toggle legacy support in one place.
// Later, when you want to delete CryptoBox compatibility, set this to 0.
#ifndef DOFFEN_ENABLE_CRYPTOBOX_LEGACY
#define DOFFEN_ENABLE_CRYPTOBOX_LEGACY 1
#endif

namespace {

// =======================
// DTENC1 binary container
// =======================
//
// After base64 decoding:
//
//   magic[6]  = "DTENC1"
//   ver u8    = 1
//   salt_len  u16 big-endian
//   nonce_len u16 big-endian
//   salt[salt_len]
//   nonce[nonce_len]
//   ct[...]   (ciphertext + tag)
//
// We keep this tiny and versioned so you can evolve it later.
//
constexpr char MAGIC[6] = {'D','T','E','N','C','1'};
constexpr uint8_t VERSION = 1;

static void append_u16(std::vector<uint8_t>& out, uint16_t v)
{
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>((v     ) & 0xFF));
}

static uint16_t read_u16(const uint8_t*& p, const uint8_t* end)
{
    if(end - p < 2)
        throw std::runtime_error("Truncated DTENC1 header");
    uint16_t v = (static_cast<uint16_t>(p[0]) << 8) | static_cast<uint16_t>(p[1]);
    p += 2;
    return v;
}

/*
  Key derivation: HKDF(SHA-256) using bcrypt output as "secret".

  We use the KDF::kdf(...) pointer-based API to avoid std::span usage.
  Botan 3 marks KDF::kdf as deprecated in favor of span/string_view overloads,
  but it is present in your botan_all.h and works fine for this use case.

  Domain separation:
    INFO makes sure these derived keys are only used for this purpose/version.
*/
static std::vector<uint8_t> hkdf_derive_key(
    std::string_view bcrypt_secret,
    const uint8_t* salt,
    size_t salt_len,
    size_t key_len)
{
    static constexpr uint8_t INFO[] =
        {'D','o','f','f','e','n','S','S','H','T','u','n','n','e','l','|',
         'B','o','t','a','n','W','r','a','p','p','e','r','|','v','1'};

    auto kdf = Botan::KDF::create_or_throw("HKDF(SHA-256)");

    std::vector<uint8_t> key(key_len);

    // KDF::kdf is present in your botan_all.h (deprecated, but available).
    kdf->kdf(
        key.data(),
        key.size(),
        reinterpret_cast<const uint8_t*>(bcrypt_secret.data()),
        bcrypt_secret.size(),
        salt,
        salt_len,
        INFO,
        sizeof(INFO));

    return key;
}

/*
  Try decrypt DTENC1.

  Returns:
    - true  -> input recognized as DTENC1 and decrypted successfully (out_plaintext set)
    - false -> input is not DTENC1 (caller may try legacy CryptoBox)
    - throws -> input looked like DTENC1 but is invalid/unsupported/wrong key
*/
static bool try_decrypt_dtenc1_base64(
    const QByteArray& in,
    const std::string& bcryptSecret,
    QByteArray& out_plaintext)
{
    // Base64 decode. If decode fails, it is not DTENC1.
    Botan::secure_vector<uint8_t> blob;
    try
    {
        // Botan 3 provides base64_decode(std::string_view) -> secure_vector<uint8_t>
        blob = Botan::base64_decode(std::string_view(in.constData(), static_cast<size_t>(in.size())));
    }
    catch(...)
    {
        return false;
    }

    if(blob.size() < 6 + 1 + 2 + 2)
        return false;

    const uint8_t* p = blob.data();
    const uint8_t* end = blob.data() + blob.size();

    if(std::memcmp(p, MAGIC, 6) != 0)
        return false;
    p += 6;

    const uint8_t ver = *p++;
    if(ver != VERSION)
        throw std::runtime_error("Unsupported DTENC1 version");

    const uint16_t salt_len  = read_u16(p, end);
    const uint16_t nonce_len = read_u16(p, end);

    if(salt_len == 0 || nonce_len == 0)
        throw std::runtime_error("Invalid DTENC1 header lengths");

    // Require at least auth tag (16 bytes) after salt+nonce
    if(end - p < static_cast<ptrdiff_t>(salt_len + nonce_len + 16))
        throw std::runtime_error("Truncated DTENC1 ciphertext");

    const uint8_t* salt  = p; p += salt_len;
    const uint8_t* nonce = p; p += nonce_len;

    const uint8_t* ct = p;
    const size_t ct_len = static_cast<size_t>(end - p);

    auto dec = Botan::Cipher_Mode::create_or_throw("ChaCha20Poly1305", Botan::Cipher_Dir::Decryption);

    const std::vector<uint8_t> key = hkdf_derive_key(bcryptSecret, salt, salt_len, 32);
    dec->set_key(key);

    Botan::secure_vector<uint8_t> buf(ct, ct + ct_len);

    // Use pointer+length overload (no span)
    dec->start(nonce, nonce_len);

    try
    {
        dec->finish(buf); // verifies tag and strips it
    }
    catch(const Botan::Invalid_Authentication_Tag&)
    {
        throw std::runtime_error("Invalid password or corrupted DTENC1 ciphertext");
    }

    out_plaintext = QByteArray(reinterpret_cast<const char*>(buf.data()),
                               static_cast<int>(buf.size()));
    return true;
}

#if DOFFEN_ENABLE_CRYPTOBOX_LEGACY
/*
  Legacy CryptoBox decrypt (backward compatibility).

  Easy to delete later:
    - set DOFFEN_ENABLE_CRYPTOBOX_LEGACY to 0
    - remove this function
    - remove the include <botan/cryptobox.h>
    - remove the fallback block in Decrypt()
*/
static QByteArray decrypt_legacy_cryptobox(
    const QByteArray& in,
    const std::string& passwordStd)
{
    const std::string pt = Botan::CryptoBox::decrypt(
        reinterpret_cast<const Botan::byte*>(in.constData()),
        static_cast<size_t>(in.size()),
        passwordStd);

    return QByteArray(pt.data(), static_cast<int>(pt.size()));
}
#endif

} // namespace

BotanWrapper::BotanWrapper(QObject* parent)
    : QObject(parent)
{
}

QString BotanWrapper::EncryptWithPassword(QString Data, QString Password)
{
    const QByteArray encrypted = EncryptWithPassword(Data.toUtf8(), Password);
    // CryptoBox returns PEM (ASCII) -> Latin1 is correct and explicit
    return QString::fromLatin1(encrypted.constData(), encrypted.size());
}

// =============================================================================
// Modern EncryptWithPassword -> DTENC1 base64
// =============================================================================
QByteArray BotanWrapper::EncryptWithPassword(QByteArray Data, QString Password)
{
    const std::string bcryptSecret = Password.toUtf8().toStdString();

    const std::string out_b64 = dtenc1::encrypt_base64(
        reinterpret_cast<const uint8_t*>(Data.constData()),
        static_cast<size_t>(Data.size()),
        bcryptSecret);

    return QByteArray(out_b64.data(), static_cast<int>(out_b64.size()));
}

QString BotanWrapper::Decrypt(QString Data, QString Password, DecryptFormat* OutFormat)
{
    const QByteArray decrypted = Decrypt(Data.toLatin1(), Password, OutFormat); // PEM is ASCII
    return QString::fromUtf8(decrypted.constData(), decrypted.size());
}

// =============================================================================
// Backward-compatible Decrypt
// =============================================================================
QByteArray BotanWrapper::Decrypt(QByteArray Data, QString Password, DecryptFormat* OutFormat)
{
    const std::string bcryptSecret = Password.toUtf8().toStdString();

    if(OutFormat)
        *OutFormat = DecryptFormat::Unknown;

    // DTENC1 expects base64 text as std::string
    const std::string ciphertextB64(Data.constData(), static_cast<size_t>(Data.size()));

    // 1) Try DTENC1 first
    std::vector<uint8_t> pt;
    if(dtenc1::try_decrypt_base64(ciphertextB64, bcryptSecret, pt))
    {
        if(OutFormat)
            *OutFormat = DecryptFormat::DTENC1;

        return QByteArray(reinterpret_cast<const char*>(pt.data()),
                          static_cast<int>(pt.size()));
    }

#if DOFFEN_ENABLE_CRYPTOBOX_LEGACY
    // 2) Fallback to legacy CryptoBox
    QByteArray legacyPt = decrypt_legacy_cryptobox(Data, bcryptSecret);

    if(OutFormat)
        *OutFormat = DecryptFormat::CryptoBoxLegacy;

    return legacyPt;
#else
    throw std::runtime_error("Unsupported ciphertext format (not DTENC1)");
#endif
}

QString BotanWrapper::BCryptGenerate(QString Password)
{
    Botan::AutoSeeded_RNG rng;

    // bcrypt input should be bytes; UTF-8 is the most predictable cross-platform choice
    const std::string pass = Password.toUtf8().toStdString();

    const std::string hash = Botan::generate_bcrypt(pass, rng, 12 /*work factor*/);
    return QString::fromStdString(hash);
}

bool BotanWrapper::BCryptCheck(QString Password, QString Hash)
{
    const std::string pass = Password.toUtf8().toStdString();
    const std::string hash = Hash.toStdString();

    return Botan::check_bcrypt(pass, hash);
}
