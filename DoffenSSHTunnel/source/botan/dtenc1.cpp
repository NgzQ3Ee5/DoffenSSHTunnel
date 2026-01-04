#include "dtenc1.h"

#ifdef Q_OS_WIN
#include "win/botan_all.h"
#endif

#ifdef Q_OS_MACOS
#include "mac/botan_all.h"
#endif

#ifdef Q_OS_LINUX
#include "linux/botan_all.h"
#endif

#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <string_view>
#include <vector>

namespace dtenc1 {

namespace {

// DTENC1 container (after base64 decode):
//   magic[6]  = "DTENC1"
//   ver u8    = 1
//   salt_len  u16 big-endian
//   nonce_len u16 big-endian
//   salt[salt_len]
//   nonce[nonce_len]
//   ct[...]   (ciphertext + tag)
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
        throw std::runtime_error("Truncated DTENC1 header (u16)");
    uint16_t v = (static_cast<uint16_t>(p[0]) << 8) | static_cast<uint16_t>(p[1]);
    p += 2;
    return v;
}

// HKDF(SHA-256) over bcrypt_secret + per-message salt
static std::vector<uint8_t> hkdf_derive_key(
    std::string_view bcrypt_secret,
    const uint8_t* salt,
    size_t salt_len,
    size_t key_len)
{
    static constexpr uint8_t INFO[] =
        {'D','o','f','f','e','n','S','S','H','T','u','n','n','e','l','|',
         'D','T','E','N','C','1','|','v','1'};

    auto kdf = Botan::KDF::create_or_throw("HKDF(SHA-256)");

    std::vector<uint8_t> key(key_len);

    // Pointer/length overload for maximum compatibility (no Botan::span)
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

} // namespace

std::string encrypt_base64(const uint8_t* plaintext, size_t plaintext_len,
                           const std::string& bcrypt_secret)
{
    Botan::AutoSeeded_RNG rng;

    // AEAD
    auto enc = Botan::Cipher_Mode::create_or_throw("ChaCha20Poly1305", Botan::Cipher_Dir::Encryption);

    // We store nonce_len in the container; we use 24 bytes for large nonce space.
    constexpr size_t SALT_LEN  = 16;
    constexpr size_t NONCE_LEN = 24;

    std::vector<uint8_t> salt(SALT_LEN);
    std::vector<uint8_t> nonce(NONCE_LEN);
    rng.randomize(salt.data(), salt.size());
    rng.randomize(nonce.data(), nonce.size());

    const std::vector<uint8_t> key = hkdf_derive_key(bcrypt_secret, salt.data(), salt.size(), 32);
    enc->set_key(key);

    Botan::secure_vector<uint8_t> buf(plaintext, plaintext + plaintext_len);

    enc->start(nonce.data(), nonce.size());
    enc->finish(buf); // ciphertext + tag

    // Build DTENC1 binary container
    std::vector<uint8_t> blob;
    blob.reserve(6 + 1 + 2 + 2 + salt.size() + nonce.size() + buf.size());

    blob.insert(blob.end(), std::begin(MAGIC), std::end(MAGIC));
    blob.push_back(VERSION);
    append_u16(blob, static_cast<uint16_t>(salt.size()));
    append_u16(blob, static_cast<uint16_t>(nonce.size()));
    blob.insert(blob.end(), salt.begin(), salt.end());
    blob.insert(blob.end(), nonce.begin(), nonce.end());
    blob.insert(blob.end(), buf.begin(), buf.end());

    // Encode as base64 string (text-safe ciphertext, similar to CryptoBox usage patterns)
    return Botan::base64_encode(blob.data(), blob.size());
}

bool try_decrypt_base64(const std::string& ciphertext_b64,
                        const std::string& bcrypt_secret,
                        std::vector<uint8_t>& out_plaintext)
{
    Botan::secure_vector<uint8_t> blob;

    // If base64 decode fails, it is not DTENC1 (return false for auto-detect)
    try
    {
        blob = Botan::base64_decode(std::string_view(ciphertext_b64));
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

    // Require at least 16 bytes tag after salt+nonce
    if(end - p < static_cast<ptrdiff_t>(salt_len + nonce_len + 16))
        throw std::runtime_error("Truncated DTENC1 ciphertext");

    const uint8_t* salt  = p; p += salt_len;
    const uint8_t* nonce = p; p += nonce_len;
    const uint8_t* ct = p;
    const size_t ct_len = static_cast<size_t>(end - p);

    auto dec = Botan::Cipher_Mode::create_or_throw("ChaCha20Poly1305", Botan::Cipher_Dir::Decryption);

    const std::vector<uint8_t> key = hkdf_derive_key(bcrypt_secret, salt, salt_len, 32);
    dec->set_key(key);

    Botan::secure_vector<uint8_t> buf(ct, ct + ct_len);

    dec->start(nonce, nonce_len);
    try
    {
        dec->finish(buf); // verifies tag
    }
    catch(const Botan::Invalid_Authentication_Tag&)
    {
        throw std::runtime_error("Invalid password or corrupted DTENC1 ciphertext");
    }

    out_plaintext.assign(buf.begin(), buf.end());
    return true;
}

} // namespace dtenc1
