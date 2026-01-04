# Security Overview – DTENC1

## Scope

This document describes the security properties, threat model, and guarantees of the **DTENC1** encryption format as used by **Doffen SSH Tunnel**.

DTENC1 is used to protect the local password database stored in:

```
DoffenSSHTunnel.pwd
```

This file contains sensitive credentials required to establish SSH tunnels and **must be protected against offline compromise**.

---

## What DTENC1 Protects

DTENC1 is designed to protect the password database against:

* Theft of the `.pwd` file from disk
* Offline brute-force or dictionary attacks
* File tampering or corruption
* Downgrade or parameter-manipulation attacks

DTENC1 does **not** attempt to protect against:

* A fully compromised running system (malware, debugger, memory scraping)
* Keyloggers capturing the password at entry time
* Attacks where the user’s password itself is weak

---

## High-Level Design

DTENC1 is a **password-based authenticated encryption format** with the following properties:

* **Memory-hard key derivation** using Argon2id
* **Authenticated encryption** using XChaCha20-Poly1305
* **Per-file random salt and nonce**
* **Self-describing container** with authenticated parameters
* **Strict parsing and DoS protections**

All cryptographic operations are implemented using the **Botan** cryptography library.

---

## Cryptographic Construction

### Key Derivation Function (KDF)

* **Algorithm:** Argon2id
* **Purpose:** Derive a strong symmetric encryption key from a user-supplied password
* **Properties:**

  * Memory-hard (resistant to GPU/ASIC attacks)
  * Tunable time and memory cost
  * Per-file random salt

The derived key is **never stored**. It is computed only in memory when encrypting or decrypting the database.

### Encryption Algorithm

* **Algorithm:** XChaCha20-Poly1305 (AEAD)
* **Key size:** 256 bits
* **Nonce size:** 192 bits (24 bytes)
* **Authentication tag:** 128 bits

This provides:

* Confidentiality (data cannot be read without the password)
* Integrity (any modification of the file is detected)
* Authenticity (wrong passwords always fail authentication)

### Associated Data (AAD)

All security-relevant header fields (magic, version, salt length, nonce length, Argon2 parameters) are authenticated as **Associated Data**.

This prevents:

* Parameter tampering
* Downgrade attacks
* Parser confusion or ambiguity

---

## DTENC1 File Format (Conceptual)

At a high level, `DoffenSSHTunnel.pwd` contains:

* A fixed magic identifier (`DTENC1`)
* A format version
* Argon2id parameters (memory, iterations, parallelism)
* A random salt
* A random nonce
* Encrypted password database + authentication tag

The entire structure is base64-encoded for safe storage.

---

## Password Requirements

The security of DTENC1 ultimately depends on the **strength of the user’s password**.

Recommendations:

* Use a **long passphrase** (12+ characters minimum, more is better)
* Avoid reused or common passwords
* Prefer randomly generated passphrases where possible

DTENC1 deliberately makes password guessing expensive, but it **cannot compensate for extremely weak passwords**.

---

## Offline Attack Resistance

If an attacker obtains `DoffenSSHTunnel.pwd`:

* They must perform an **offline Argon2id computation per guess**
* Each guess consumes significant memory and CPU time
* Authentication failure leaks no partial information

This makes large-scale brute-force attacks **economically and technically expensive**.

---

## Integrity and Tamper Detection

Any of the following will cause decryption to fail:

* Wrong password
* Modified ciphertext
* Modified header or parameters
* Truncated or corrupted file

No plaintext is released unless authentication succeeds.

---

## Backward / Forward Compatibility

* DTENC1 is a **single, strict format**
* Unsupported versions or parameters are rejected
* KDF parameters are stored in the file, allowing future tuning without breaking compatibility

There is no silent fallback to weaker encryption.

---

## Cryptographic Parameters Rationale

This section explains *why* the chosen cryptographic parameters exist and how they may be safely tuned in the future.

### Argon2id Parameters

DTENC1 uses **Argon2id** because it is memory-hard and specifically designed to resist GPU and ASIC acceleration.

The following parameters are stored in every encrypted file:

* **Memory cost (`m_kib`)**

  * Default: 64 MiB
  * Rationale: Memory cost is the primary defense against parallel offline attacks.
  * Guidance:

    * Increasing memory cost is generally more effective than increasing iterations.
    * 64–128 MiB is appropriate for modern desktop systems.
    * Lower values reduce resistance and should only be used on constrained systems.

* **Time cost (`t`)**

  * Default: 3 iterations
  * Rationale: Increases total computation per password guess.
  * Guidance:

    * Increase when memory cannot be raised further.
    * Values above 4 typically show diminishing returns.

* **Parallelism (`p`)**

  * Default: 1
  * Rationale: Limits peak CPU usage and avoids excessive contention.
  * Guidance:

    * Increasing parallelism rarely improves security for offline resistance.
    * Keep low unless you have strong reasons and extensive testing.

The recommended tuning target is **~250–750 ms per key derivation** on the user’s machine.

Because parameters are stored in the file header and authenticated, they may be safely increased over time without breaking existing data.

---

## Security Assumptions

DTENC1’s guarantees rely on the following assumptions. If any of these assumptions are violated, confidentiality and/or integrity may be compromised regardless of the cryptographic design.

1. **Host integrity at unlock time**

   * The operating system and the running Doffen SSH Tunnel process are assumed not to be compromised while the user enters their password and while the database is decrypted in memory.
   * Malware, debuggers, memory scrapers, or injected code can exfiltrate plaintext and/or the password.

2. **Password secrecy and strength**

   * The user’s password is assumed to be kept secret and to have sufficient entropy.
   * DTENC1 increases the cost of guessing but cannot prevent compromise if the password is weak or reused.

3. **Cryptographically secure randomness**

   * The RNG used to generate salts and nonces is assumed to be cryptographically secure.
   * DTENC1 relies on Botan’s RNG (AutoSeeded_RNG / system entropy sources).

4. **Correct library implementation**

   * Botan is assumed to correctly implement Argon2id and (X)ChaCha20-Poly1305 and to be used according to its API requirements.

5. **Local file system access model**

   * DTENC1 assumes attackers may read or copy `DoffenSSHTunnel.pwd` (offline attack model).
   * DTENC1 does not assume attackers are prevented from deleting the file or replacing it; such actions are detected only as decryption failures.

6. **No secure deletion guarantees**

   * DTENC1 does not guarantee secure deletion of old file contents due to filesystem journaling, SSD wear leveling, backups, or OS caching.

---

## Versioning Policy

DTENC1 follows a **conservative, explicit versioning model**.

### General Principles

* DTENC1 versions are **never silently upgraded or downgraded**.
* Unsupported versions cause a **hard failure**.
* There is **no automatic fallback** to weaker formats.

### When the Version May Change

The DTENC1 version number may change **only** if one of the following is true:

* A cryptographic primitive is deprecated or broken
* A fundamentally stronger construction replaces the existing one
* A format change is required to support materially improved security properties

Parameter tuning (e.g. Argon2 memory or iteration count) **does not require** a version change.

### Backward Compatibility

* New versions may choose to explicitly support older DTENC1 versions.
* Older versions must never attempt to guess or reinterpret newer formats.

### Migration Strategy

If DTENC1 is ever revised:

1. Existing files remain readable using their original version.
2. Files are re-encrypted using the new version only after successful decryption.
3. Migration is explicit and intentional.

This avoids silent security regressions or downgrade attacks.

---

## Security Level Statement

**DTENC1 provides strong, modern cryptographic protection suitable for:

* Local password databases
* User-protected credential storage
* Offline data-at-rest encryption**

When used with a reasonably strong password, DTENC1 is considered:

> **Resistant to practical offline attacks by non-state adversaries**

Security is comparable to modern password managers and encrypted key stores using Argon2id and AEAD encryption.

---

## Operational Notes

* Loss of the password means **permanent loss of access** to the password database
* There is no recovery mechanism by design
* The encryption format is intentionally conservative and explicit to support security review and auditing

---

## Contact / Review

Changes to DTENC1 **must be reviewed with security in mind**.

Any modification to:

* Cryptographic algorithms
* KDF parameters
* File format
* Error handling semantics

should be treated as a **security sensitive change**.
