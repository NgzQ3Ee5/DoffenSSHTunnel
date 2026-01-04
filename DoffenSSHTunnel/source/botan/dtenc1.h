#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dtenc1 {

/**
 * Encrypt arbitrary binary data using a user-supplied plaintext password.
 *
 * Security properties:
 *  - Password-based key derivation using Argon2id (memory-hard, GPU-resistant).
 *  - Per-message random salt ensures unique derived keys even for identical passwords.
 *  - Authenticated encryption using XChaCha20-Poly1305.
 *  - Header fields are authenticated via AEAD Associated Data (AAD).
 *
 * Format:
 *  - Produces a base64-encoded DTENC1 binary container.
 *  - The container is fully self-describing (KDF parameters included).
 *
 * Failure modes:
 *  - Throws std::runtime_error on invalid arguments or internal failures.
 *
 * Thread safety:
 *  - This function is thread-safe provided Botan is correctly initialized.
 *
 * @param plaintext Pointer to data to encrypt (may be null if length is zero).
 * @param plaintext_len Length of plaintext in bytes.
 * @param password User-supplied plaintext password.
 *
 * @return Base64-encoded DTENC1 ciphertext.
 */
std::string encrypt_base64(const uint8_t* plaintext, size_t plaintext_len,
                           const std::string& password);

/**
 * Attempt to decrypt a DTENC1 base64 ciphertext using the supplied password.
 *
 * This function is designed to support **format auto-detection**:
 *
 * Return semantics:
 *  - returns false:
 *      - Input is not valid base64, OR
 *      - Base64 decoded data does not begin with DTENC1 magic.
 *
 *  - returns true:
 *      - Input is valid DTENC1 and decryption succeeded.
 *
 *  - throws std::runtime_error:
 *      - Input is DTENC1 but malformed,
 *      - Unsupported version or parameters,
 *      - Password is incorrect,
 *      - Authentication tag verification fails.
 *
 * IMPORTANT:
 *  - Callers MUST NOT silently fall back to weaker legacy formats if this
 *    function throws. A throw indicates DTENC1 was detected.
 *
 * Security properties:
 *  - Authentication is verified before any plaintext is released.
 *  - Header parameters are authenticated (AAD) to prevent tampering.
 *  - DoS protections limit memory and CPU usage.
 *
 * @param ciphertext_b64 Base64-encoded DTENC1 ciphertext.
 * @param password User-supplied plaintext password.
 * @param out_plaintext Output buffer receiving decrypted plaintext on success.
 *
 * @return true if DTENC1 detected and decrypted successfully, false if not DTENC1.
 */
bool try_decrypt_base64(const std::string& ciphertext_b64,
                        const std::string& password,
                        std::vector<uint8_t>& out_plaintext);

} // namespace dtenc1
