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
// magic[6]   = "DTENC1"
// salt_len   u16 (BE)
// nonce_len  u16 (BE)
// kdf_iter   u32 (BE)
// salt[salt_len]
// nonce[nonce_len]
// ciphertext || auth_tag
constexpr char MAGIC[6] = { 'D','T','E','N','C','1' };

constexpr size_t SALT_LEN  = 16;
constexpr size_t NONCE_LEN = 24;
constexpr uint32_t PBKDF2_ITERS = 200'000;

static void append_u16(std::vector<uint8_t>& out, uint16_t v)
{
    out.push_back(uint8_t(v >> 8));
    out.push_back(uint8_t(v));
}

static void append_u32(std::vector<uint8_t>& out, uint32_t v)
{
    out.push_back(uint8_t(v >> 24));
    out.push_back(uint8_t(v >> 16));
    out.push_back(uint8_t(v >> 8));
    out.push_back(uint8_t(v));
}

static uint16_t read_u16(const uint8_t*& p, const uint8_t* end)
{
    if(end - p < 2)
        throw std::runtime_error("DTENC1 truncated (u16)");
    uint16_t v = (uint16_t(p[0]) << 8) | p[1];
    p += 2;
    return v;
}

static uint32_t read_u32(const uint8_t*& p, const uint8_t* end)
{
    if(end - p < 4)
        throw std::runtime_error("DTENC1 truncated (u32)");
    uint32_t v =
        (uint32_t(p[0]) << 24) |
        (uint32_t(p[1]) << 16) |
        (uint32_t(p[2]) << 8)  |
        (uint32_t(p[3]));
    p += 4;
    return v;
}

static Botan::secure_vector<uint8_t> derive_key_pbkdf2(
    const std::string& password,
    const uint8_t* salt,
    size_t salt_len,
    uint32_t iterations,
    size_t key_len)
{
    auto family =
        Botan::PasswordHashFamily::create_or_throw("PBKDF2(SHA-256)");
    auto ph = family->from_iterations(iterations);

    Botan::secure_vector<uint8_t> key(key_len);
    ph->derive_key(key.data(), key.size(),
                   password.data(), password.size(),
                   salt, salt_len);
    return key;
}

} // namespace

// ---------------------------------------------------------------------------

std::string encrypt_base64(const uint8_t* plaintext,
                           size_t plaintext_len,
                           const std::string& password)
{
    Botan::AutoSeeded_RNG rng;

    std::vector<uint8_t> salt(SALT_LEN);
    std::vector<uint8_t> nonce(NONCE_LEN);
    rng.randomize(salt.data(), salt.size());
    rng.randomize(nonce.data(), nonce.size());

    auto enc =
        Botan::Cipher_Mode::create_or_throw(
            "ChaCha20Poly1305",
            Botan::Cipher_Dir::Encryption);

    const auto key =
        derive_key_pbkdf2(password,
                          salt.data(), salt.size(),
                          PBKDF2_ITERS,
                          32);

    enc->set_key(key.data(), key.size());

    Botan::secure_vector<uint8_t> buf(
        plaintext, plaintext + plaintext_len);

    enc->start(nonce.data(), nonce.size());
    enc->finish(buf);

    std::vector<uint8_t> blob;
    blob.reserve(6 + 2 + 2 + 4 + salt.size() + nonce.size() + buf.size());

    blob.insert(blob.end(), MAGIC, MAGIC + 6);
    append_u16(blob, uint16_t(salt.size()));
    append_u16(blob, uint16_t(nonce.size()));
    append_u32(blob, PBKDF2_ITERS);
    blob.insert(blob.end(), salt.begin(), salt.end());
    blob.insert(blob.end(), nonce.begin(), nonce.end());
    blob.insert(blob.end(), buf.begin(), buf.end());

    return Botan::base64_encode(blob.data(), blob.size());
}

// ---------------------------------------------------------------------------

bool try_decrypt_base64(const std::string& ciphertext_b64,
                        const std::string& password,
                        std::vector<uint8_t>& out_plaintext)
{
    Botan::secure_vector<uint8_t> blob;

    try
    {
        blob = Botan::base64_decode(ciphertext_b64);
    }
    catch(...)
    {
        return false; // not base64 → not DTENC1
    }

    if(blob.size() < 6 + 2 + 2 + 4)
        return false;

    const uint8_t* p   = blob.data();
    const uint8_t* end = blob.data() + blob.size();

    if(std::memcmp(p, MAGIC, 6) != 0)
        return false;
    p += 6;

    const uint16_t salt_len  = read_u16(p, end);
    const uint16_t nonce_len = read_u16(p, end);
    const uint32_t iters     = read_u32(p, end);

    if(iters < 10'000)
        throw std::runtime_error("DTENC1: PBKDF2 iterations too low");

    if(end - p < ptrdiff_t(salt_len + nonce_len + 16))
        throw std::runtime_error("DTENC1 truncated payload");

    const uint8_t* salt  = p; p += salt_len;
    const uint8_t* nonce = p; p += nonce_len;
    const uint8_t* ct    = p;
    const size_t ct_len  = size_t(end - p);

    auto dec =
        Botan::Cipher_Mode::create_or_throw(
            "ChaCha20Poly1305",
            Botan::Cipher_Dir::Decryption);

    const auto key =
        derive_key_pbkdf2(password,
                          salt, salt_len,
                          iters,
                          32);

    dec->set_key(key.data(), key.size());

    Botan::secure_vector<uint8_t> buf(ct, ct + ct_len);

    dec->start(nonce, nonce_len);

    try
    {
        dec->finish(buf);
    }
    catch(const Botan::Invalid_Authentication_Tag&)
    {
        throw std::runtime_error("DTENC1: wrong password or corrupted data");
    }

    out_plaintext.assign(buf.begin(), buf.end());
    return true;
}

} // namespace dtenc1
