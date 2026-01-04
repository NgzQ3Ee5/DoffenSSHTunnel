#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dtenc1 {

/**
 * Encrypt plaintext and return a base64 string holding a DTENC1 container.
 *
 * bcrypt_secret:
 *   In your project this is the ASCII output from BCryptGenerate(...).
 *   We treat it as secret keying material and derive an AEAD key using HKDF(SHA-256)
 *   plus a per-message random salt (stored in the container).
 */
std::string encrypt_base64(const uint8_t* plaintext, size_t plaintext_len,
                           const std::string& password);

/**
 * Attempt to decrypt a base64 DTENC1 ciphertext.
 *
 * Return semantics (important for auto-detection):
 *   - returns true  -> input recognized as DTENC1 AND decrypted successfully
 *   - returns false -> input is NOT DTENC1 (caller may fall back to legacy)
 *   - throws        -> input LOOKS like DTENC1 but is invalid/unsupported/wrong key
 */
bool try_decrypt_base64(const std::string& ciphertext_b64,
                        const std::string& password,
                        std::vector<uint8_t>& out_plaintext);

} // namespace dtenc1
