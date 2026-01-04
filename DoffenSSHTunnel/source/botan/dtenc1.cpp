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

/* =====================================================================
 * DTENC1 FILE FORMAT (VERSION 2 – SINGLE SUPPORTED FORMAT)
 * =====================================================================
 *
 * Base64(
 *   magic[6]        = "DTENC1"
 *   version u8      = 2
 *
 *   salt_len u16    = 16 (BE, fixed)
 *   nonce_len u16   = 24 (BE, fixed)
 *
 *   argon_m_kib u32 = memory cost in KiB
 *   argon_t     u32 = time/iteration cost
 *   argon_p     u32 = parallelism
 *
 *   salt[16]
 *   nonce[24]
 *   ciphertext || auth_tag (Poly1305, 16 bytes)
 * )
 *
 * Authenticated Associated Data (AAD):
 *   magic || version || salt_len || nonce_len ||
 *   argon_m_kib || argon_t || argon_p
 *
 * SECURITY NOTES:
 *  - Header fields are authenticated to prevent downgrade or DoS attacks.
 *  - Fixed salt/nonce lengths eliminate parsing ambiguity.
 *  - Argon2 parameters are bounded to avoid resource exhaustion.
 */

constexpr char MAGIC[6] = { 'D','T','E','N','C','1' };
constexpr uint8_t VERSION = 2;

/* === Cryptographic parameters === */

constexpr size_t SALT_LEN  = 16;
constexpr size_t NONCE_LEN = 24;  // XChaCha20-Poly1305 in Botan when using 24-byte nonce
constexpr size_t KEY_LEN   = 32;
constexpr size_t TAG_LEN   = 16;

/* === Resource limits (DoS protection) === */

constexpr size_t MAX_DECODED_BLOB = 64ull * 1024 * 1024; // 64 MiB DoS guard

/* === Argon2id defaults ===
 *
 * These should be tuned to ~250–750ms on target hardware.
 * Stored in header, so future tuning does not break compatibility.
 */
constexpr uint32_t ARGON_M_KIB_DEFAULT = 64u * 1024u; // 64 MiB
constexpr uint32_t ARGON_T_DEFAULT     = 3;
constexpr uint32_t ARGON_P_DEFAULT     = 1;

/* === Argon2id bounds (DoS protection) === */

constexpr uint32_t ARGON_M_KIB_MIN = 8u * 1024u;
constexpr uint32_t ARGON_M_KIB_MAX = 512u * 1024u; // 512 MiB
constexpr uint32_t ARGON_T_MIN = 1;
constexpr uint32_t ARGON_T_MAX = 10;
constexpr uint32_t ARGON_P_MIN = 1;
constexpr uint32_t ARGON_P_MAX = 8;

/* =====================================================================
 * Helper utilities (serialization / validation)
 * ===================================================================== */

static void append_u16(std::vector<uint8_t>& out, uint16_t v)
{
    out.push_back(static_cast<uint8_t>((v >> 8) & 0xFF));
    out.push_back(static_cast<uint8_t>(v & 0xFF));
}

static void append_u32(std::vector<uint8_t>& out, uint32_t v)
{
    out.push_back(static_cast<uint8_t>((v >> 24) & 0xFF));
    out.push_back(static_cast<uint8_t>((v >> 16) & 0xFF));
    out.push_back(static_cast<uint8_t>((v >>  8) & 0xFF));
    out.push_back(static_cast<uint8_t>(v & 0xFF));
}

static uint16_t read_u16(const uint8_t*& p, const uint8_t* end)
{
    if(end - p < 2) throw std::runtime_error("DTENC1: truncated header (u16)");
    uint16_t v = (static_cast<uint16_t>(p[0]) << 8) | static_cast<uint16_t>(p[1]);
    p += 2;
    return v;
}

static uint32_t read_u32(const uint8_t*& p, const uint8_t* end)
{
    if(end - p < 4) throw std::runtime_error("DTENC1: truncated header (u32)");
    uint32_t v = (static_cast<uint32_t>(p[0]) << 24) |
                 (static_cast<uint32_t>(p[1]) << 16) |
                 (static_cast<uint32_t>(p[2]) <<  8) |
                 (static_cast<uint32_t>(p[3]));
    p += 4;
    return v;
}

/**
 * Validate Argon2 parameters.
 *
 * This prevents attacker-controlled headers from forcing excessive
 * memory or CPU consumption during key derivation.
 */
static void validate_argon_params(uint32_t m_kib, uint32_t t, uint32_t p)
{
    if(m_kib < ARGON_M_KIB_MIN || m_kib > ARGON_M_KIB_MAX)
        throw std::runtime_error("DTENC1: invalid Argon2 memory");
    if(t < ARGON_T_MIN || t > ARGON_T_MAX)
        throw std::runtime_error("DTENC1: invalid Argon2 iterations");
    if(p < ARGON_P_MIN || p > ARGON_P_MAX)
        throw std::runtime_error("DTENC1: invalid Argon2 parallelism");
}

/**
 * Derive encryption key using Argon2id.
 *
 * Rationale:
 *  - Argon2id is memory-hard and significantly more resistant to GPU/ASIC
 *    attacks than PBKDF2.
 *  - Per-message salt ensures unique keys per encryption.
 */
static Botan::secure_vector<uint8_t> derive_key_argon2id(std::string_view password,
                                                         const uint8_t* salt,
                                                         size_t salt_len,
                                                         uint32_t m_kib,
                                                         uint32_t t,
                                                         uint32_t par)
{
    validate_argon_params(m_kib, t, par);

    auto family = Botan::PasswordHashFamily::create_or_throw("Argon2id");
    auto ph = family->from_params(static_cast<size_t>(m_kib),
                                  static_cast<size_t>(t),
                                  static_cast<size_t>(par));

    Botan::secure_vector<uint8_t> key(KEY_LEN);
    ph->derive_key(key.data(), key.size(),
                   password.data(), password.size(),
                   salt, salt_len);
    return key;
}

/**
 * Build Associated Data (AAD).
 *
 * AAD binds all security-relevant header fields to the ciphertext,
 * preventing tampering, downgrade, or parameter substitution attacks.
 */
static std::vector<uint8_t> build_aad(uint16_t salt_len, uint16_t nonce_len,
                                      uint32_t m_kib, uint32_t t, uint32_t p)
{
    std::vector<uint8_t> aad;
    aad.reserve(6 + 1 + 2 + 2 + 4 + 4 + 4);
    aad.insert(aad.end(), std::begin(MAGIC), std::end(MAGIC));
    aad.push_back(VERSION);
    append_u16(aad, salt_len);
    append_u16(aad, nonce_len);
    append_u32(aad, m_kib);
    append_u32(aad, t);
    append_u32(aad, p);
    return aad;
}

} // namespace

std::string encrypt_base64(const uint8_t* plaintext,
                           size_t plaintext_len,
                           const std::string& password)
{
    if(plaintext == nullptr && plaintext_len != 0)
        throw std::runtime_error("DTENC1: null plaintext pointer");

    Botan::AutoSeeded_RNG rng;

    std::vector<uint8_t> salt(SALT_LEN);
    std::vector<uint8_t> nonce(NONCE_LEN);
    rng.randomize(salt.data(), salt.size());
    rng.randomize(nonce.data(), nonce.size());

    const uint32_t m_kib = ARGON_M_KIB_DEFAULT;
    const uint32_t t     = ARGON_T_DEFAULT;
    const uint32_t p     = ARGON_P_DEFAULT;

    const auto key = derive_key_argon2id(password, salt.data(), salt.size(), m_kib, t, p);

    // IMPORTANT: Use AEAD_Mode to access set_associated_data
    auto enc = Botan::AEAD_Mode::create_or_throw("ChaCha20Poly1305", Botan::Cipher_Dir::Encryption);
    enc->set_key(key.data(), key.size());

    const auto aad = build_aad(static_cast<uint16_t>(SALT_LEN),
                               static_cast<uint16_t>(NONCE_LEN),
                               m_kib, t, p);

    // IMPORTANT: Botan requires AD set after set_key() and before start()
    enc->set_associated_data(aad.data(), aad.size());
    enc->start(nonce.data(), nonce.size());

    Botan::secure_vector<uint8_t> buf;
    buf.assign(plaintext, plaintext + plaintext_len);

    enc->finish(buf); // ciphertext + tag

    std::vector<uint8_t> blob;
    blob.reserve(aad.size() + SALT_LEN + NONCE_LEN + buf.size());

    blob.insert(blob.end(), std::begin(MAGIC), std::end(MAGIC));
    blob.push_back(VERSION);
    append_u16(blob, static_cast<uint16_t>(SALT_LEN));
    append_u16(blob, static_cast<uint16_t>(NONCE_LEN));
    append_u32(blob, m_kib);
    append_u32(blob, t);
    append_u32(blob, p);

    blob.insert(blob.end(), salt.begin(), salt.end());
    blob.insert(blob.end(), nonce.begin(), nonce.end());
    blob.insert(blob.end(), buf.begin(), buf.end());

    return Botan::base64_encode(blob.data(), blob.size());
}

bool try_decrypt_base64(const std::string& ciphertext_b64,
                        const std::string& password,
                        std::vector<uint8_t>& out_plaintext)
{
    out_plaintext.clear();

    Botan::secure_vector<uint8_t> blob;
    try
    {
        blob = Botan::base64_decode(std::string_view(ciphertext_b64));
    }
    catch(...)
    {
        return false; // not base64 => not DTENC1
    }

    if(blob.size() > MAX_DECODED_BLOB)
        throw std::runtime_error("DTENC1: blob too large");

    constexpr size_t MIN_HDR = 6 + 1 + 2 + 2 + 4 + 4 + 4;
    if(blob.size() < MIN_HDR)
        return false;

    const uint8_t* p = blob.data();
    const uint8_t* end = blob.data() + blob.size();

    if(std::memcmp(p, MAGIC, 6) != 0)
        return false;
    p += 6;

    const uint8_t ver = *p++;
    if(ver != VERSION)
        throw std::runtime_error("DTENC1: unsupported version");

    const uint16_t salt_len  = read_u16(p, end);
    const uint16_t nonce_len = read_u16(p, end);

    const uint32_t m_kib = read_u32(p, end);
    const uint32_t t     = read_u32(p, end);
    const uint32_t par   = read_u32(p, end);

    if(salt_len != SALT_LEN)
        throw std::runtime_error("DTENC1: invalid salt length");
    if(nonce_len != NONCE_LEN)
        throw std::runtime_error("DTENC1: invalid nonce length");

    validate_argon_params(m_kib, t, par);

    const size_t remaining = static_cast<size_t>(end - p);
    const size_t need_min = SALT_LEN + NONCE_LEN + TAG_LEN;
    if(remaining < need_min)
        throw std::runtime_error("DTENC1: truncated payload");

    const uint8_t* salt  = p; p += SALT_LEN;
    const uint8_t* nonce = p; p += NONCE_LEN;
    const uint8_t* ct    = p;
    const size_t ct_len  = static_cast<size_t>(end - p);

    const auto key = derive_key_argon2id(password, salt, SALT_LEN, m_kib, t, par);

    auto dec = Botan::AEAD_Mode::create_or_throw("ChaCha20Poly1305", Botan::Cipher_Dir::Decryption);
    dec->set_key(key.data(), key.size());

    const auto aad = build_aad(salt_len, nonce_len, m_kib, t, par);

    // IMPORTANT: set AD before start()
    dec->set_associated_data(aad.data(), aad.size());
    dec->start(nonce, NONCE_LEN);

    Botan::secure_vector<uint8_t> buf(ct, ct + ct_len);

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
