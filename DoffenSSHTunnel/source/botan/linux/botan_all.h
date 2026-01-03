/*
* Botan 3.10.0 Amalgamation
* (C) 1999-2023 The Botan Authors
*
* Botan is released under the Simplified BSD License (see license.txt)
*/

#ifndef BOTAN_AMALGAMATION_H_
#define BOTAN_AMALGAMATION_H_

#include <algorithm>
#include <array>
#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <exception>
#include <functional>
#include <iosfwd>
#include <memory>
#include <optional>
#include <ranges>
#include <span>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

/**
* @file  build.h
* @brief Build configuration for Botan 3.10.0
*/

/* NOLINTBEGIN(*-macro-usage,*-macro-to-enum) */

/**
 * @defgroup buildinfo Build Information
 */

/**
 * @ingroup buildinfo
 * @defgroup buildinfo_version Build version information
 * @{
 */

/**
* The major version of the release
*/
#define BOTAN_VERSION_MAJOR 3

/**
* The minor version of the release
*/
#define BOTAN_VERSION_MINOR 10

/**
* The patch version of the release
*/
#define BOTAN_VERSION_PATCH 0

/**
 * Expands to an integer of the form YYYYMMDD if this is an official
 * release, or 0 otherwise. For instance, 2.19.0, which was released
 * on January 19, 2022, has a `BOTAN_VERSION_DATESTAMP` of 20220119.
 *
 * This macro is deprecated; use version_datestamp from version.h
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_VERSION_DATESTAMP 20251106

/**
 * A string set to the release type
 *
 * This macro is deprecated
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_VERSION_RELEASE_TYPE "release"

/**
 * A macro expanding to a string that is set to a revision identifier
 * corresponding to the source, or "unknown" if this could not be
 * determined. It is set for all official releases.
 *
 * This macro is deprecated; use version_vc_revision from version.h
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_VERSION_VC_REVISION "git:e27d332089247f04e89854a2e60a3af2ec4e430f"

/**
 * A macro expanding to a string that is set at build time using the
 * `--distribution-info` option. It allows a packager of the library
 * to specify any distribution-specific patches. If no value is given
 * at build time, the value is the string "unspecified".
 *
 * This macro is deprecated; use version_distribution_info from version.h
 *
 * TODO(Botan4) remove this
 */
#define BOTAN_DISTRIBUTION_INFO "unspecified"

/**
 * @}
 */

/**
 * @ingroup buildinfo
 * @defgroup buildinfo_configuration Build configurations
 * @{
 */




#ifndef BOTAN_DLL
  #define BOTAN_DLL 
#endif

/* Target identification and feature test macros */

#define BOTAN_TARGET_OS_HAS_FILESYSTEM
#define BOTAN_TARGET_OS_HAS_THREADS



/**
 * @}
 */

/**
 * @ingroup buildinfo
 * @defgroup buildinfo_modules Enabled modules and API versions
 * @{
 */

/*
* Module availability definitions
*/
#define BOTAN_HAS_ASN1 20201106
#define BOTAN_HAS_AUTO_RNG 20161126
#define BOTAN_HAS_AUTO_SEEDING_RNG 20160821
#define BOTAN_HAS_BASE64_CODEC 20131128
#define BOTAN_HAS_BCRYPT 20131128
#define BOTAN_HAS_BIGINT 20240529
#define BOTAN_HAS_BLOCK_CIPHER 20131128
#define BOTAN_HAS_BLOWFISH 20180718
#define BOTAN_HAS_CIPHER_MODES 20180124
#define BOTAN_HAS_CRYPTO_BOX 20131128
#define BOTAN_HAS_CTR_BE 20131128
#define BOTAN_HAS_ENTROPY_SOURCE 20151120
#define BOTAN_HAS_HASH 20180112
#define BOTAN_HAS_HEX_CODEC 20131128
#define BOTAN_HAS_HMAC 20131128
#define BOTAN_HAS_HMAC_DRBG 20140319
#define BOTAN_HAS_KDF 20250528
#define BOTAN_HAS_KDF_BASE 20131128
#define BOTAN_HAS_MAC 20150626
#define BOTAN_HAS_MODES 20150626
#define BOTAN_HAS_NUMBERTHEORY 20201108
#define BOTAN_HAS_PASSWORD_HASHING 20210419
#define BOTAN_HAS_PBKDF 20180902
#define BOTAN_HAS_PBKDF2 20180902
#define BOTAN_HAS_PEM_CODEC 20131128
#define BOTAN_HAS_PUBLIC_KEY_CRYPTO 20131128
#define BOTAN_HAS_RSA_ENCRYPTION_PADDING 20250720
#define BOTAN_HAS_SERPENT 20131128
#define BOTAN_HAS_SHA2_32 20131128
#define BOTAN_HAS_SHA2_64 20131128
#define BOTAN_HAS_SHA_224 20250130
#define BOTAN_HAS_SHA_256 20250130
#define BOTAN_HAS_SHA_384 20250130
#define BOTAN_HAS_SHA_512 20250130
#define BOTAN_HAS_SHA_512_256 20250130
#define BOTAN_HAS_STATEFUL_RNG 20160819
#define BOTAN_HAS_STREAM_CIPHER 20131128
#define BOTAN_HAS_SYSTEM_RNG 20141202


/*
* Internal module feature definitions
*
* These macros have been in the past visible in build.h as feature macros
* but in the future these will be only visible in an internal header.
* Applications should not rely on or check for these macros.
*/
#define BOTAN_HAS_BIGINT_MP 20151225
#define BOTAN_HAS_DYNAMIC_LOADER 20160310
#define BOTAN_HAS_MDX_HASH_FUNCTION 20131128
#define BOTAN_HAS_OS_UTILS 20241202


/**
 * @}
 */

/* NOLINTEND(*-macro-usage,*-macro-to-enum) */

// NOLINTBEGIN(*-macro-usage)

/**
* Used to annotate API exports which are public and supported.
* These APIs will not be broken/removed unless strictly required for
* functionality or security, and only in new major versions.
* @param maj The major version this public API was released in
* @param min The minor version this public API was released in
*/
#define BOTAN_PUBLIC_API(maj, min) BOTAN_DLL

/**
* Used to annotate API exports which are public, but are now deprecated
* and which will be removed in a future major release.
*/
#define BOTAN_DEPRECATED_API(msg) BOTAN_DEPRECATED(msg) BOTAN_DLL

/**
* Used to annotate API exports which are public and can be used by
* applications if needed, but which are intentionally not documented,
* and which may change incompatibly in a future major version.
*/
#define BOTAN_UNSTABLE_API BOTAN_DLL

/**
* Used to annotate API exports which are exported but only for the
* purposes of testing. They should not be used by applications and
* may be removed or changed without notice.
*/
#define BOTAN_TEST_API BOTAN_DLL

/**
* This is used to mark constructors which are currently not `explicit`
* but which in a future major release be modified as such.
*
* TODO(Botan4) remove this macro and replace with `explicit`
*/
#if defined(__clang_analyzer__) || defined(BOTAN_DISABLE_DEPRECATED_FEATURES)
   #define BOTAN_FUTURE_EXPLICIT explicit
#else
   #define BOTAN_FUTURE_EXPLICIT
#endif

/**
* Used to annotate API exports which are exported but only for the
* purposes of fuzzing. They should not be used by applications and
* may be removed or changed without notice.
*
* They are only exported if the fuzzers are being built
*/
#if defined(BOTAN_FUZZERS_ARE_BEING_BUILT)
   #define BOTAN_FUZZER_API BOTAN_DLL
#else
   #define BOTAN_FUZZER_API
#endif

/*
* Define BOTAN_DEPRECATED
*/
#if !defined(BOTAN_NO_DEPRECATED_WARNINGS) && !defined(BOTAN_AMALGAMATION_H_) && !defined(BOTAN_IS_BEING_BUILT)

   #define BOTAN_DEPRECATED(msg) [[deprecated(msg)]]

   #if defined(__clang__)
      #define BOTAN_DEPRECATED_HEADER(hdr) _Pragma("message \"this header is deprecated\"")
      #define BOTAN_FUTURE_INTERNAL_HEADER(hdr) _Pragma("message \"this header will be made internal in the future\"")
   #elif defined(_MSC_VER)
      #define BOTAN_DEPRECATED_HEADER(hdr) __pragma(message("this header is deprecated"))
      #define BOTAN_FUTURE_INTERNAL_HEADER(hdr) __pragma(message("this header will be made internal in the future"))
   #elif defined(__GNUC__)
      #define BOTAN_DEPRECATED_HEADER(hdr) _Pragma("GCC warning \"this header is deprecated\"")
      #define BOTAN_FUTURE_INTERNAL_HEADER(hdr) \
         _Pragma("GCC warning \"this header will be made internal in the future\"")
   #endif

#endif

#if !defined(BOTAN_DEPRECATED)
   #define BOTAN_DEPRECATED(msg)
#endif

#if !defined(BOTAN_DEPRECATED_HEADER)
   #define BOTAN_DEPRECATED_HEADER(hdr)
#endif

#if !defined(BOTAN_FUTURE_INTERNAL_HEADER)
   #define BOTAN_FUTURE_INTERNAL_HEADER(hdr)
#endif

#if defined(__clang__)
   #define BOTAN_DIAGNOSTIC_PUSH _Pragma("clang diagnostic push")
   #define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS \
      _Pragma("clang diagnostic ignored \"-Wdeprecated-declarations\"")
   #define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE
   #define BOTAN_DIAGNOSTIC_POP _Pragma("clang diagnostic pop")
#elif defined(__GNUG__)
   #define BOTAN_DIAGNOSTIC_PUSH _Pragma("GCC diagnostic push")
   #define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS \
      _Pragma("GCC diagnostic ignored \"-Wdeprecated-declarations\"")
   #define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE
   #define BOTAN_DIAGNOSTIC_POP _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
   #define BOTAN_DIAGNOSTIC_PUSH __pragma(warning(push))
   #define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS __pragma(warning(disable : 4996))
   #define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE __pragma(warning(disable : 4250))
   #define BOTAN_DIAGNOSTIC_POP __pragma(warning(pop))
#else
   #define BOTAN_DIAGNOSTIC_PUSH
   #define BOTAN_DIAGNOSTIC_IGNORE_DEPRECATED_DECLARATIONS
   #define BOTAN_DIAGNOSTIC_IGNORE_INHERITED_VIA_DOMINANCE
   #define BOTAN_DIAGNOSTIC_POP
#endif

// NOLINTEND(*-macro-usage)

/**
* MSVC does define __cplusplus but pins it at 199711L, because "legacy".
* Note: There is a compiler switch to enable standard behavior (/Zc:__cplusplus),
*       but we can't control that in downstream applications.
*
* See: https://learn.microsoft.com/en-us/cpp/build/reference/zc-cplusplus
*/
#if defined(_MSVC_LANG)
   #define BOTAN_CPLUSPLUS _MSVC_LANG
#else
   #define BOTAN_CPLUSPLUS __cplusplus
#endif

#if BOTAN_CPLUSPLUS < 202002L
   #error "Botan 3.x requires at least C++20"
#endif

namespace Botan {

/**
* @mainpage Botan Crypto Library API Reference
*
* <dl>
* <dt>Abstract Base Classes<dd>
*        BlockCipher, HashFunction, KDF, MessageAuthenticationCode, RandomNumberGenerator,
*        StreamCipher, SymmetricAlgorithm, AEAD_Mode, Cipher_Mode, XOF
* <dt>Public Key Interface Classes<dd>
*        PK_Key_Agreement, PK_Signer, PK_Verifier, PK_Encryptor, PK_Decryptor, PK_KEM_Encryptor, PK_KEM_Decryptor
* <dt>Authenticated Encryption Modes<dd>
*        @ref Ascon_AEAD128 "Ascon-AEAD128" @ref CCM_Mode "CCM", @ref ChaCha20Poly1305_Mode "ChaCha20Poly1305",
*        @ref EAX_Mode "EAX", @ref GCM_Mode "GCM", @ref OCB_Mode "OCB", @ref SIV_Mode "SIV"
* <dt>Block Ciphers<dd>
*        @ref aria.h "ARIA", @ref aes.h "AES", @ref Blowfish, @ref camellia.h "Camellia", @ref Cascade_Cipher "Cascade",
*        @ref CAST_128 "CAST-128", @ref CAST_128 DES, @ref TripleDES "3DES",
*        @ref GOST_28147_89 "GOST 28147-89", IDEA, Kuznyechik, Lion, Noekeon, SEED, Serpent, SHACAL2, SM4,
*        @ref Threefish_512 "Threefish", Twofish
* <dt>Stream Ciphers<dd>
*        ChaCha, @ref CTR_BE "CTR", OFB, RC4, Salsa20
* <dt>Hash Functions<dd>
*        @ref Ascon_Hash256 "Ascon-Hash256", BLAKE2b, @ref GOST_34_11 "GOST 34.11", @ref Keccak_1600 "Keccak", MD4,
*        MD5, @ref RIPEMD_160 "RIPEMD-160", @ref SHA_1 "SHA-1", @ref SHA_224 "SHA-224", @ref SHA_256 "SHA-256",
*        @ref SHA_384 "SHA-384", @ref SHA_512 "SHA-512", @ref Skein_512 "Skein-512", SM3, Streebog, Whirlpool
* <dt>Non-Cryptographic Checksums<dd>
*        Adler32, CRC24, CRC32
* <dt>Message Authentication Codes<dd>
*        @ref BLAKE2bMAC "BLAKE2b", CMAC, HMAC, KMAC, Poly1305, SipHash, ANSI_X919_MAC
* <dt>Random Number Generators<dd>
*        AutoSeeded_RNG, HMAC_DRBG, Processor_RNG, System_RNG
* <dt>Key Derivation<dd>
*        HKDF, @ref KDF1 "KDF1 (IEEE 1363)", @ref KDF1_18033 "KDF1 (ISO 18033-2)", @ref KDF2 "KDF2 (IEEE 1363)",
*        @ref sp800_108.h "SP800-108", @ref SP800_56C "SP800-56C", @ref PKCS5_PBKDF2 "PBKDF2 (PKCS#5)"
* <dt>Password Hashing<dd>
*        @ref argon2.h "Argon2", @ref scrypt.h "scrypt", @ref bcrypt.h "bcrypt", @ref passhash9.h "passhash9"
* <dt>Public Key Cryptosystems<dd>
*        @ref dlies.h "DLIES", @ref ecies.h "ECIES", @ref elgamal.h "ElGamal",
*        @ref rsa.h "RSA", @ref mceliece.h "McEliece", @ref sm2.h "SM2"
* <dt>Key Encapsulation Mechanisms<dd>
*        @ref cmce.h "Classic McEliece", @ref frodokem.h "FrodoKEM", @ref kyber.h "ML-KEM/Kyber", @ref rsa.h "RSA"
* <dt>Public Key Signature Schemes<dd>
*        @ref dsa.h "DSA", @ref dilithium.h "ML-DSA/Dilithium", @ref ecdsa.h "ECDSA", @ref ecgdsa.h "ECGDSA",
*        @ref eckcdsa.h "ECKCDSA", @ref gost_3410.h "GOST 34.10-2001", @ref hss_lms.h "HSS/LMS", @ref sm2.h "SM2",
         @ref sphincsplus.h "SLH-DSA/SPHINCS+", @ref xmss.h "XMSS"
* <dt>Key Agreement<dd>
*        @ref dh.h "DH", @ref ecdh.h "ECDH"
* <dt>Compression<dd>
*        @ref bzip2.h "bzip2", @ref lzma.h "lzma", @ref zlib.h "zlib"
* <dt>TLS<dd>
*        TLS::Client, TLS::Server, TLS::Policy, TLS::Protocol_Version, TLS::Callbacks, TLS::Ciphersuite,
*        TLS::Session, TLS::Session_Summary, TLS::Session_Manager, Credentials_Manager
* <dt>X.509<dd>
*        X509_Certificate, X509_CRL, X509_CA, Certificate_Extension, PKCS10_Request, X509_Cert_Options,
*        Certificate_Store, Certificate_Store_In_SQL, Certificate_Store_In_SQLite
* <dt>eXtendable Output Functions<dd>
*        @ref Ascon_XOF128 "Ascon-XOF128", @ref SHAKE_XOF "SHAKE"
* </dl>
*/

using std::int32_t;
using std::int64_t;
using std::size_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;
using std::uint8_t;

#if !defined(BOTAN_IS_BEING_BUILT)
/*
* These typedefs are no longer used within the library headers
* or code. They are kept only for compatibility with software
* written against older versions.
*/
using byte = std::uint8_t;
using u16bit = std::uint16_t;
using u32bit = std::uint32_t;
using u64bit = std::uint64_t;
using s32bit = std::int32_t;
#endif

static constexpr bool HasNative64BitRegisters = sizeof(void*) >= 8;

using word = std::conditional_t<HasNative64BitRegisters, std::uint64_t, uint32_t>;

#if defined(__SIZEOF_INT128__)
   #define BOTAN_TARGET_HAS_NATIVE_UINT128

// GCC complains if this isn't marked with __extension__
__extension__ typedef unsigned __int128 uint128_t;
#endif

/*
* Should this assert fail on your system please contact the developers
* for assistance in porting.
*/
static_assert(sizeof(std::size_t) == 8 || sizeof(std::size_t) == 4, "This platform has an unexpected size for size_t");

/**
* How much to allocate for a buffer of no particular size
*/
constexpr size_t DefaultBufferSize = 4096;

}  // namespace Botan

namespace Botan {

/*
* Define BOTAN_MALLOC_FN
*/
#if defined(__clang__) || defined(__GNUG__)
   #define BOTAN_MALLOC_FN __attribute__((malloc))
#elif defined(_MSC_VER)
   #define BOTAN_MALLOC_FN __declspec(restrict)
#else
   #define BOTAN_MALLOC_FN
#endif

/**
* Allocate a memory buffer by some method. This should only be used for
* primitive types (uint8_t, uint32_t, etc).
*
* @param elems the number of elements
* @param elem_size the size of each element
* @return pointer to allocated and zeroed memory, or throw std::bad_alloc on failure
*/
BOTAN_PUBLIC_API(2, 3) BOTAN_MALLOC_FN void* allocate_memory(size_t elems, size_t elem_size);

/**
* Free a pointer returned by allocate_memory
* @param p the pointer returned by allocate_memory
* @param elems the number of elements, as passed to allocate_memory
* @param elem_size the size of each element, as passed to allocate_memory
*/
BOTAN_PUBLIC_API(2, 3) void deallocate_memory(void* p, size_t elems, size_t elem_size);

/**
* Ensure the allocator is initialized
*/
void BOTAN_UNSTABLE_API initialize_allocator();

class Allocator_Initializer final {
   public:
      Allocator_Initializer() { initialize_allocator(); }
};

}  // namespace Botan

namespace Botan {

/**
* Different types of errors that might occur
*/
enum class ErrorType : uint16_t {
   /** Some unknown error */
   Unknown = 1,
   /** An error while calling a system interface */
   SystemError = 2,
   /** An operation seems valid, but not supported by the current version */
   NotImplemented = 3,
   /** Memory allocation failure */
   OutOfMemory = 4,
   /** An internal error occurred */
   InternalError = 5,
   /** An I/O error occurred */
   IoError = 6,

   /** Invalid object state */
   InvalidObjectState = 100,
   /** A key was not set on an object when this is required */
   KeyNotSet = 101,
   /** The application provided an argument which is invalid */
   InvalidArgument = 102,
   /** A key with invalid length was provided */
   InvalidKeyLength = 103,
   /** A nonce with invalid length was provided */
   InvalidNonceLength = 104,
   /** An object type was requested but cannot be found */
   LookupError = 105,
   /** Encoding a message or datum failed */
   EncodingFailure = 106,
   /** Decoding a message or datum failed */
   DecodingFailure = 107,
   /** A TLS error (error_code will be the alert type) */
   TLSError = 108,
   /** An error during an HTTP operation */
   HttpError = 109,
   /** A message with an invalid authentication tag was detected */
   InvalidTag = 110,
   /** An error during Roughtime validation */
   RoughtimeError = 111,

   /** An error when interacting with CommonCrypto API */
   CommonCryptoError = 201,
   /** An error when interacting with a PKCS11 device */
   Pkcs11Error = 202,
   /** An error when interacting with a TPM device */
   TPMError = 203,
   /** An error when interacting with a database */
   DatabaseError = 204,

   /** An error when interacting with zlib */
   ZlibError = 300,
   /** An error when interacting with bzip2 */
   Bzip2Error = 301,
   /** An error when interacting with lzma */
   LzmaError = 302,

};

//! \brief Convert an ErrorType to string
std::string BOTAN_PUBLIC_API(2, 11) to_string(ErrorType type);

/**
* Base class for all exceptions thrown by the library
*/
class BOTAN_PUBLIC_API(2, 0) Exception : public std::exception {
   public:
      /**
      * Return a descriptive string which is hopefully comprehensible to
      * a developer. It will likely not be useful for an end user.
      *
      * The string has no particular format, and the content of exception
      * messages may change from release to release. Thus the main use of this
      * function is for logging or debugging.
      */
      const char* what() const noexcept override { return m_msg.c_str(); }

      /**
      * Return the "type" of error which occurred.
      */
      virtual ErrorType error_type() const noexcept { return ErrorType::Unknown; }

      /**
      * Return an error code associated with this exception, or otherwise 0.
      *
      * The domain of this error varies depending on the source, for example on
      * POSIX systems it might be errno, while on a Windows system it might be
      * the result of GetLastError or WSAGetLastError.
      */
      virtual int error_code() const noexcept { return 0; }

      /**
      * Avoid throwing base Exception, use a subclass
      */
      explicit Exception(std::string_view msg);

      /**
      * Avoid throwing base Exception, use a subclass
      */
      Exception(const char* prefix, std::string_view msg);

      /**
      * Avoid throwing base Exception, use a subclass
      */
      Exception(std::string_view msg, const std::exception& e);

   private:
      std::string m_msg;
};

/**
* An invalid argument was provided to an API call.
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_Argument : public Exception {
   public:
      explicit Invalid_Argument(std::string_view msg);

      explicit Invalid_Argument(std::string_view msg, std::string_view where);

      Invalid_Argument(std::string_view msg, const std::exception& e);

      ErrorType error_type() const noexcept override { return ErrorType::InvalidArgument; }
};

/**
* An invalid/unknown field name was passed to Public_Key::get_int_field
*/
class BOTAN_PUBLIC_API(3, 0) Unknown_PK_Field_Name final : public Invalid_Argument {
   public:
      Unknown_PK_Field_Name(std::string_view algo_name, std::string_view field_name);
};

/**
* An invalid key length was used
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_Key_Length final : public Invalid_Argument {
   public:
      Invalid_Key_Length(std::string_view name, size_t length);

      ErrorType error_type() const noexcept override { return ErrorType::InvalidKeyLength; }
};

/**
* An invalid nonce length was used
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_IV_Length final : public Invalid_Argument {
   public:
      Invalid_IV_Length(std::string_view mode, size_t bad_len);

      ErrorType error_type() const noexcept override { return ErrorType::InvalidNonceLength; }
};

/**
* Invalid_Algorithm_Name Exception
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_Algorithm_Name final : public Invalid_Argument {
   public:
      explicit Invalid_Algorithm_Name(std::string_view name);
};

/**
* Encoding_Error Exception
*/
class BOTAN_PUBLIC_API(2, 0) Encoding_Error final : public Exception {
   public:
      explicit Encoding_Error(std::string_view name);

      ErrorType error_type() const noexcept override { return ErrorType::EncodingFailure; }
};

/**
* A decoding error occurred.
*/
class BOTAN_PUBLIC_API(2, 0) Decoding_Error : public Exception {
   public:
      explicit Decoding_Error(std::string_view name);

      Decoding_Error(std::string_view category, std::string_view err);

      Decoding_Error(std::string_view msg, const std::exception& e);

      ErrorType error_type() const noexcept override { return ErrorType::DecodingFailure; }
};

/**
* Invalid state was encountered. A request was made on an object while the
* object was in a state where the operation cannot be performed.
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_State : public Exception {
   public:
      explicit Invalid_State(std::string_view err) : Exception(err) {}

      ErrorType error_type() const noexcept override { return ErrorType::InvalidObjectState; }
};

/**
* A PRNG was called on to produce output while still unseeded
*/
class BOTAN_PUBLIC_API(2, 0) PRNG_Unseeded final : public Invalid_State {
   public:
      explicit PRNG_Unseeded(std::string_view algo);
};

/**
* The key was not set on an object. This occurs with symmetric objects where
* an operation which requires the key is called prior to set_key being called.
*/
class BOTAN_PUBLIC_API(2, 4) Key_Not_Set : public Invalid_State {
   public:
      explicit Key_Not_Set(std::string_view algo);

      ErrorType error_type() const noexcept override { return ErrorType::KeyNotSet; }
};

/**
* A request was made for some kind of object which could not be located
*/
class BOTAN_PUBLIC_API(2, 0) Lookup_Error : public Exception {
   public:
      explicit Lookup_Error(std::string_view err) : Exception(err) {}

      Lookup_Error(std::string_view type, std::string_view algo, std::string_view provider = "");

      ErrorType error_type() const noexcept override { return ErrorType::LookupError; }
};

/**
* Algorithm_Not_Found Exception
*
* @warning This exception type will be removed in the future. Instead
* just catch Lookup_Error.
*/
class BOTAN_PUBLIC_API(2, 0) Algorithm_Not_Found final : public Lookup_Error {
   public:
      explicit Algorithm_Not_Found(std::string_view name);
};

/**
* Provider_Not_Found is thrown when a specific provider was requested
* but that provider is not available.
*
* @warning This exception type will be removed in the future. Instead
* just catch Lookup_Error.
*/
class BOTAN_PUBLIC_API(2, 0) Provider_Not_Found final : public Lookup_Error {
   public:
      Provider_Not_Found(std::string_view algo, std::string_view provider);
};

/**
* An AEAD or MAC check detected a message modification
*
* In versions before 2.10, Invalid_Authentication_Tag was named
* Integrity_Failure, it was renamed to make its usage more clear.
*/
class BOTAN_PUBLIC_API(2, 0) Invalid_Authentication_Tag final : public Exception {
   public:
      explicit Invalid_Authentication_Tag(std::string_view msg);

      ErrorType error_type() const noexcept override { return ErrorType::InvalidTag; }
};

/**
* For compatibility with older versions
*/
typedef Invalid_Authentication_Tag Integrity_Failure;

/**
* An error occurred while operating on an IO stream
*/
class BOTAN_PUBLIC_API(2, 0) Stream_IO_Error final : public Exception {
   public:
      explicit Stream_IO_Error(std::string_view err);

      ErrorType error_type() const noexcept override { return ErrorType::IoError; }
};

/**
* System_Error
*
* This exception is thrown in the event of an error related to interacting
* with the operating system.
*
* This exception type also (optionally) captures an integer error code eg
* POSIX errno or Windows GetLastError.
*/
class BOTAN_PUBLIC_API(2, 9) System_Error : public Exception {
   public:
      explicit System_Error(std::string_view msg) : Exception(msg), m_error_code(0) {}

      System_Error(std::string_view msg, int err_code);

      ErrorType error_type() const noexcept override { return ErrorType::SystemError; }

      int error_code() const noexcept override { return m_error_code; }

   private:
      int m_error_code;
};

/**
* An internal error occurred. If observed, please file a bug.
*/
class BOTAN_PUBLIC_API(2, 0) Internal_Error : public Exception {
   public:
      explicit Internal_Error(std::string_view err);

      ErrorType error_type() const noexcept override { return ErrorType::InternalError; }
};

/**
* Not Implemented Exception
*
* This is thrown in the situation where a requested operation is
* logically valid but is not implemented by this version of the library.
*/
class BOTAN_PUBLIC_API(2, 0) Not_Implemented final : public Exception {
   public:
      explicit Not_Implemented(std::string_view err);

      ErrorType error_type() const noexcept override { return ErrorType::NotImplemented; }
};

template <typename E, typename... Args>
inline void do_throw_error(const char* file, int line, const char* func, Args... args) {
   throw E(file, line, func, args...);
}

}  // namespace Botan

#if !defined(BOTAN_IS_BEING_BUILT) && !defined(BOTAN_DISABLE_DEPRECATED_FEATURES)
   // TODO(Botan4) remove this
   #include <deque>
#endif

namespace Botan {

template <typename T>
#if !defined(_ITERATOR_DEBUG_LEVEL) || _ITERATOR_DEBUG_LEVEL == 0
/*
  * Check exists to prevent someone from doing something that will
  * probably crash anyway (like secure_vector<non_POD_t> where ~non_POD_t
  * deletes a member pointer which was zeroed before it ran).
  * MSVC in debug mode uses non-integral proxy types in container types
  * like std::vector, thus we disable the check there.
 */
   requires std::is_integral_v<T> || std::is_enum_v<T>
#endif
class secure_allocator {

   public:
      typedef T value_type;
      typedef std::size_t size_type;

      secure_allocator() noexcept = default;
      secure_allocator(const secure_allocator&) noexcept = default;
      secure_allocator& operator=(const secure_allocator&) noexcept = default;
      secure_allocator(secure_allocator&&) noexcept = default;
      secure_allocator& operator=(secure_allocator&&) noexcept = default;

      ~secure_allocator() noexcept = default;

      template <typename U>
      explicit secure_allocator(const secure_allocator<U>& /*other*/) noexcept {}

      T* allocate(std::size_t n) { return static_cast<T*>(allocate_memory(n, sizeof(T))); }

      void deallocate(T* p, std::size_t n) { deallocate_memory(p, n, sizeof(T)); }
};

template <typename T, typename U>
inline bool operator==(const secure_allocator<T>& /*a*/, const secure_allocator<U>& /*b*/) {
   return true;
}

template <typename T, typename U>
inline bool operator!=(const secure_allocator<T>& /*a*/, const secure_allocator<U>& /*b*/) {
   return false;
}

template <typename T>
using secure_vector = std::vector<T, secure_allocator<T>>;

#if !defined(BOTAN_IS_BEING_BUILT) && !defined(BOTAN_DISABLE_DEPRECATED_FEATURES)
template <typename T>
using secure_deque = std::deque<T, secure_allocator<T>>;
#endif

// For better compatibility with 1.10 API
template <typename T>
using SecureVector = secure_vector<T>;

template <typename T>
secure_vector<T> lock(const std::vector<T>& in) {
   return secure_vector<T>(in.begin(), in.end());
}

template <typename T>
std::vector<T> unlock(const secure_vector<T>& in) {
   return std::vector<T>(in.begin(), in.end());
}

template <typename T, typename Alloc, typename Alloc2>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, const std::vector<T, Alloc2>& in) {
   out.insert(out.end(), in.begin(), in.end());
   return out;
}

template <typename T, typename Alloc>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, std::span<const T> in) {
   out.insert(out.end(), in.begin(), in.end());
   return out;
}

template <typename T, typename Alloc>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, T in) {
   out.push_back(in);
   return out;
}

template <typename T, typename Alloc, typename L>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, const std::pair<const T*, L>& in) {
   out.insert(out.end(), in.first, in.first + in.second);
   return out;
}

template <typename T, typename Alloc, typename L>
std::vector<T, Alloc>& operator+=(std::vector<T, Alloc>& out, const std::pair<T*, L>& in) {
   out.insert(out.end(), in.first, in.first + in.second);
   return out;
}

/**
* Zeroise the values; length remains unchanged
* @param vec the vector to zeroise
*/
template <typename T, typename Alloc>
void zeroise(std::vector<T, Alloc>& vec) {
   std::fill(vec.begin(), vec.end(), static_cast<T>(0));
}

/**
* Zeroise the values then free the memory
* @param vec the vector to zeroise and free
*/
template <typename T, typename Alloc>
void zap(std::vector<T, Alloc>& vec) {
   zeroise(vec);
   vec.clear();
   vec.shrink_to_fit();
}

}  // namespace Botan

namespace Botan {

class BER_Decoder;
class DER_Encoder;

/**
* ASN.1 Class Tags
*/
enum class ASN1_Class : uint32_t /* NOLINT(performance-enum-size) */ {
   Universal = 0b0000'0000,
   Application = 0b0100'0000,
   ContextSpecific = 0b1000'0000,
   Private = 0b1100'0000,

   Constructed = 0b0010'0000,
   ExplicitContextSpecific = Constructed | ContextSpecific,

   NoObject = 0xFF00
};

/**
* ASN.1 Type Tags
*/
enum class ASN1_Type : uint32_t /* NOLINT(performance-enum-size) */ {
   Eoc = 0x00,
   Boolean = 0x01,
   Integer = 0x02,
   BitString = 0x03,
   OctetString = 0x04,
   Null = 0x05,
   ObjectId = 0x06,
   Enumerated = 0x0A,
   Sequence = 0x10,
   Set = 0x11,

   Utf8String = 0x0C,
   NumericString = 0x12,
   PrintableString = 0x13,
   TeletexString = 0x14,
   Ia5String = 0x16,
   VisibleString = 0x1A,
   UniversalString = 0x1C,
   BmpString = 0x1E,

   UtcTime = 0x17,
   GeneralizedTime = 0x18,

   NoObject = 0xFF00,
};

inline bool intersects(ASN1_Class x, ASN1_Class y) {
   return (static_cast<uint32_t>(x) & static_cast<uint32_t>(y)) != 0;
}

inline ASN1_Type operator|(ASN1_Type x, ASN1_Type y) {
   return static_cast<ASN1_Type>(static_cast<uint32_t>(x) | static_cast<uint32_t>(y));
}

inline ASN1_Class operator|(ASN1_Class x, ASN1_Class y) {
   return static_cast<ASN1_Class>(static_cast<uint32_t>(x) | static_cast<uint32_t>(y));
}

inline uint32_t operator|(ASN1_Type x, ASN1_Class y) {
   return static_cast<uint32_t>(x) | static_cast<uint32_t>(y);
}

inline uint32_t operator|(ASN1_Class x, ASN1_Type y) {
   return static_cast<uint32_t>(x) | static_cast<uint32_t>(y);
}

std::string BOTAN_UNSTABLE_API asn1_tag_to_string(ASN1_Type type);
std::string BOTAN_UNSTABLE_API asn1_class_to_string(ASN1_Class type);

/**
* Basic ASN.1 Object Interface
*/
class BOTAN_PUBLIC_API(2, 0) ASN1_Object {
   public:
      /**
      * Encode whatever this object is into to
      * @param to the DER_Encoder that will be written to
      */
      virtual void encode_into(DER_Encoder& to) const = 0;

      /**
      * Decode whatever this object is from from
      * @param from the BER_Decoder that will be read from
      */
      virtual void decode_from(BER_Decoder& from) = 0;

      /**
      * Return the encoding of this object. This is a convenience
      * method when just one object needs to be serialized. Use
      * DER_Encoder for complicated encodings.
      */
      std::vector<uint8_t> BER_encode() const;

      ASN1_Object() = default;
      ASN1_Object(const ASN1_Object&) = default;
      ASN1_Object& operator=(const ASN1_Object&) = default;
      ASN1_Object(ASN1_Object&&) = default;
      ASN1_Object& operator=(ASN1_Object&&) = default;
      virtual ~ASN1_Object() = default;
};

/**
* BER Encoded Object
*/
class BOTAN_PUBLIC_API(2, 0) BER_Object final {
   public:
      BER_Object() = default;

      BER_Object(const BER_Object& other) = default;
      BER_Object(BER_Object&& other) = default;
      BER_Object& operator=(const BER_Object& other) = default;
      BER_Object& operator=(BER_Object&& other) = default;
      ~BER_Object() = default;

      bool is_set() const { return m_type_tag != ASN1_Type::NoObject; }

      uint32_t tagging() const { return type_tag() | class_tag(); }

      ASN1_Type type_tag() const { return m_type_tag; }

      ASN1_Class class_tag() const { return m_class_tag; }

      ASN1_Type type() const { return m_type_tag; }

      ASN1_Class get_class() const { return m_class_tag; }

      const uint8_t* bits() const { return m_value.data(); }

      size_t length() const { return m_value.size(); }

      std::span<const uint8_t> data() const { return std::span{m_value}; }

      void assert_is_a(ASN1_Type type_tag, ASN1_Class class_tag, std::string_view descr = "object") const;

      bool is_a(ASN1_Type type_tag, ASN1_Class class_tag) const;

      bool is_a(int type_tag, ASN1_Class class_tag) const;

   private:
      ASN1_Type m_type_tag = ASN1_Type::NoObject;
      ASN1_Class m_class_tag = ASN1_Class::Universal;
      secure_vector<uint8_t> m_value;

      friend class BER_Decoder;

      void set_tagging(ASN1_Type type_tag, ASN1_Class class_tag);

      uint8_t* mutable_bits(size_t length) {
         m_value.resize(length);
         return m_value.data();
      }
};

/*
* ASN.1 Utility Functions
*/
class DataSource;

namespace ASN1 {

std::vector<uint8_t> put_in_sequence(const std::vector<uint8_t>& val);
std::vector<uint8_t> put_in_sequence(const uint8_t bits[], size_t len);
std::string to_string(const BER_Object& obj);

/**
* Heuristics tests; is this object possibly BER?
* @param src a data source that will be peeked at but not modified
*/
bool maybe_BER(DataSource& src);

}  // namespace ASN1

/**
* General BER Decoding Error Exception
*/
class BOTAN_PUBLIC_API(2, 0) BER_Decoding_Error : public Decoding_Error {
   public:
      explicit BER_Decoding_Error(std::string_view err);
};

/**
* Exception For Incorrect BER Taggings
*/
class BOTAN_PUBLIC_API(2, 0) BER_Bad_Tag final : public BER_Decoding_Error {
   public:
      BER_Bad_Tag(std::string_view msg, uint32_t tagging);
};

/**
* This class represents ASN.1 object identifiers.
*/
class BOTAN_PUBLIC_API(2, 0) OID final : public ASN1_Object {
   public:
      /**
      * Create an uninitialised OID object
      */
      explicit OID() = default;

      /**
      * Construct an OID from a string.
      * @param str a string in the form "a.b.c" etc., where a,b,c are integers
      *
      * Note: it is currently required that each integer fit into 32 bits
      */
      explicit OID(std::string_view str);

      /**
      * Initialize an OID from a sequence of integer values
      */
      OID(std::initializer_list<uint32_t> init);

      /**
      * Initialize an OID from a vector of integer values
      */
      explicit OID(std::vector<uint32_t>&& init);

      /**
      * Construct an OID from a string.
      * @param str a string in the form "a.b.c" etc., where a,b,c are numbers
      *        or any known OID name (for example "RSA" or "X509v3.SubjectKeyIdentifier")
      */
      static OID from_string(std::string_view str);

      /**
      * Construct an OID from a name
      * @param name any known OID name (for example "RSA" or "X509v3.SubjectKeyIdentifier")
      */
      static std::optional<OID> from_name(std::string_view name);

      /**
      * Register a new OID in the internal table
      */
      static void register_oid(const OID& oid, std::string_view name);

      void encode_into(DER_Encoder& to) const override;
      void decode_from(BER_Decoder& from) override;

      /**
      * Find out whether this OID is empty
      * @return true is no OID value is set
      */
      bool empty() const { return m_id.empty(); }

      /**
      * Find out whether this OID has a value
      * @return true is this OID has a value
      */
      bool has_value() const { return !empty(); }

      /**
      * Get this OID as a dotted-decimal string
      * @return string representing this OID
      */
      std::string to_string() const;

      /**
      * If there is a known name associated with this OID, return that.
      * Otherwise return the result of to_string
      */
      std::string to_formatted_string() const;

      /**
      * If there is a known name associated with this OID, return that.
      * Otherwise return the empty string.
      */
      std::string human_name_or_empty() const;

      /**
      * Return true if the OID in *this is registered in the internal
      * set of constants as a known OID.
      */
      bool registered_oid() const;

      /**
      * Compare two OIDs.
      * @return true if they are equal, false otherwise
      */
      bool operator==(const OID& other) const { return m_id == other.m_id; }

      /**
      * Return a hash code for this OID
      *
      * This value is only meant as a std::unordered_map hash and
      * can change value from release to release.
      */
      uint64_t hash_code() const;

      /**
      * Check if this OID matches the provided value
      */
      bool matches(std::initializer_list<uint32_t> other) const;

      /**
      * Get this OID as list (vector) of its components.
      * @return vector representing this OID
      */
      BOTAN_DEPRECATED("Do not access the integer values, use eg to_string")
      const std::vector<uint32_t>& get_components() const {
         return m_id;
      }

      BOTAN_DEPRECATED("Do not access the integer values, use eg to_string")
      const std::vector<uint32_t>& get_id() const {
         return m_id;
      }

   private:
      std::vector<uint32_t> m_id;
};

inline std::ostream& operator<<(std::ostream& out, const OID& oid) {
   out << oid.to_string();
   return out;
}

/**
* Compare two OIDs.
* @param a the first OID
* @param b the second OID
* @return true if a is not equal to b
*/
inline bool operator!=(const OID& a, const OID& b) {
   return !(a == b);
}

/**
* Compare two OIDs.
* @param a the first OID
* @param b the second OID
* @return true if a is lexicographically smaller than b
*/
BOTAN_PUBLIC_API(2, 0) bool operator<(const OID& a, const OID& b);

/**
* Time (GeneralizedTime/UniversalTime)
*/
class BOTAN_PUBLIC_API(2, 0) ASN1_Time final : public ASN1_Object {
   public:
      /// DER encode a ASN1_Time
      void encode_into(DER_Encoder& to) const override;

      // Decode a BER encoded ASN1_Time
      void decode_from(BER_Decoder& from) override;

      /// Return an internal string representation of the time
      std::string to_string() const;

      /// Returns a human friendly string replesentation of no particular formatting
      std::string readable_string() const;

      /// Return if the time has been set somehow
      bool time_is_set() const;

      ///  Compare this time against another
      int32_t cmp(const ASN1_Time& other) const;

      /// Create an invalid ASN1_Time
      ASN1_Time() = default;

      /// Create a ASN1_Time from a time point
      explicit ASN1_Time(const std::chrono::system_clock::time_point& time);

      /// Create an ASN1_Time from seconds since epoch
      static ASN1_Time from_seconds_since_epoch(uint64_t seconds);

      /// Create an ASN1_Time from string
      BOTAN_FUTURE_EXPLICIT ASN1_Time(std::string_view t_spec);

      /// Create an ASN1_Time from string and a specified tagging (Utc or Generalized)
      ASN1_Time(std::string_view t_spec, ASN1_Type tag);

      /// Returns a STL timepoint object
      std::chrono::system_clock::time_point to_std_timepoint() const;

      /// Return time since epoch
      uint64_t time_since_epoch() const;

   private:
      void set_to(std::string_view t_spec, ASN1_Type type);
      bool passes_sanity_check() const;

      uint32_t m_year = 0;
      uint32_t m_month = 0;
      uint32_t m_day = 0;
      uint32_t m_hour = 0;
      uint32_t m_minute = 0;
      uint32_t m_second = 0;
      ASN1_Type m_tag = ASN1_Type::NoObject;
};

/*
* Comparison Operations
*/
BOTAN_PUBLIC_API(2, 0) bool operator==(const ASN1_Time& x, const ASN1_Time& y);
BOTAN_PUBLIC_API(2, 0) bool operator!=(const ASN1_Time& x, const ASN1_Time& y);
BOTAN_PUBLIC_API(2, 0) bool operator<=(const ASN1_Time& x, const ASN1_Time& y);
BOTAN_PUBLIC_API(2, 0) bool operator>=(const ASN1_Time& x, const ASN1_Time& y);
BOTAN_PUBLIC_API(2, 0) bool operator<(const ASN1_Time& x, const ASN1_Time& y);
BOTAN_PUBLIC_API(2, 0) bool operator>(const ASN1_Time& x, const ASN1_Time& y);

typedef ASN1_Time X509_Time;

/**
* ASN.1 string type
* This class normalizes all inputs to a UTF-8 std::string
*/
class BOTAN_PUBLIC_API(2, 0) ASN1_String final : public ASN1_Object {
   public:
      void encode_into(DER_Encoder& to) const override;
      void decode_from(BER_Decoder& from) override;

      ASN1_Type tagging() const { return m_tag; }

      const std::string& value() const { return m_utf8_str; }

      size_t size() const { return value().size(); }

      bool empty() const { return m_utf8_str.empty(); }

      /**
      * Return true iff this is a tag for a known string type we can handle.
      */
      static bool is_string_type(ASN1_Type tag);

      bool operator==(const ASN1_String& other) const { return value() == other.value(); }

      friend bool operator<(const ASN1_String& a, const ASN1_String& b) { return a.value() < b.value(); }

      explicit ASN1_String(std::string_view utf8 = "");
      ASN1_String(std::string_view utf8, ASN1_Type tag);

   private:
      std::vector<uint8_t> m_data;
      std::string m_utf8_str;
      ASN1_Type m_tag;
};

/**
* Algorithm Identifier
*/
class BOTAN_PUBLIC_API(2, 0) AlgorithmIdentifier final : public ASN1_Object {
   public:
      enum Encoding_Option : uint8_t { USE_NULL_PARAM, USE_EMPTY_PARAM }; /* NOLINT(*-use-enum-class) */

      void encode_into(DER_Encoder& to) const override;
      void decode_from(BER_Decoder& from) override;

      AlgorithmIdentifier() = default;

      AlgorithmIdentifier(const OID& oid, Encoding_Option enc);
      AlgorithmIdentifier(std::string_view oid_name, Encoding_Option enc);

      AlgorithmIdentifier(const OID& oid, const std::vector<uint8_t>& params);
      AlgorithmIdentifier(std::string_view oid_name, const std::vector<uint8_t>& params);

      const OID& oid() const { return m_oid; }

      const std::vector<uint8_t>& parameters() const { return m_parameters; }

      BOTAN_DEPRECATED("Use AlgorithmIdentifier::oid") const OID& get_oid() const { return m_oid; }

      BOTAN_DEPRECATED("Use AlgorithmIdentifier::parameters") const std::vector<uint8_t>& get_parameters() const {
         return m_parameters;
      }

      bool parameters_are_null() const;

      bool parameters_are_empty() const { return m_parameters.empty(); }

      bool parameters_are_null_or_empty() const { return parameters_are_empty() || parameters_are_null(); }

      bool empty() const { return m_oid.empty() && m_parameters.empty(); }

   private:
      OID m_oid;
      std::vector<uint8_t> m_parameters;
};

/*
* Comparison Operations
*/
BOTAN_PUBLIC_API(2, 0) bool operator==(const AlgorithmIdentifier& x, const AlgorithmIdentifier& y);
BOTAN_PUBLIC_API(2, 0) bool operator!=(const AlgorithmIdentifier& x, const AlgorithmIdentifier& y);

}  // namespace Botan

template <>
class std::hash<Botan::OID> {
   public:
      size_t operator()(const Botan::OID& oid) const noexcept { return static_cast<size_t>(oid.hash_code()); }
};

namespace Botan {

class BigInt;
class BER_Decoder;

/**
* Format ASN.1 data and call a virtual to format
*/
class BOTAN_PUBLIC_API(2, 4) ASN1_Formatter /* NOLINT(*-special-member-functions) */ {
   public:
      virtual ~ASN1_Formatter() = default;

      /**
      * @param print_context_specific if true, try to parse nested context specific data.
      * @param max_depth do not recurse more than this many times. If zero, recursion
      *        is unbounded.
      */
      ASN1_Formatter(bool print_context_specific, size_t max_depth) :
            m_print_context_specific(print_context_specific), m_max_depth(max_depth) {}

      void print_to_stream(std::ostream& out, const uint8_t in[], size_t len) const;

      std::string print(const uint8_t in[], size_t len) const;

      template <typename Alloc>
      std::string print(const std::vector<uint8_t, Alloc>& vec) const {
         return print(vec.data(), vec.size());
      }

   protected:
      /**
      * This is called for each element
      */
      virtual std::string format(
         ASN1_Type type_tag, ASN1_Class class_tag, size_t level, size_t length, std::string_view value) const = 0;

      /**
      * This is called to format binary elements that we don't know how to
      * convert to a string. The result will be passed as value to format; the
      * tags are included as a hint to aid decoding.
      */
      virtual std::string format_bin(ASN1_Type type_tag,
                                     ASN1_Class class_tag,
                                     const std::vector<uint8_t>& vec) const = 0;

      /**
      * This is called to format integers
      */
      virtual std::string format_bn(const BigInt& bn) const = 0;

   private:
      void decode(std::ostream& output, BER_Decoder& decoder, size_t level) const;

      const bool m_print_context_specific;
      const size_t m_max_depth;
};

/**
* Format ASN.1 data into human readable output. The exact form of the output for
* any particular input is not guaranteed and may change from release to release.
*/
class BOTAN_PUBLIC_API(2, 4) ASN1_Pretty_Printer final : public ASN1_Formatter {
   public:
      /**
      * @param print_limit strings larger than this are not printed
      * @param print_binary_limit binary strings larger than this are not printed
      * @param print_context_specific if true, try to parse nested context specific data.
      * @param initial_level the initial depth (0 or 1 are the only reasonable values)
      * @param value_column ASN.1 values are lined up at this column in output
      * @param max_depth do not recurse more than this many times. If zero, recursion
      *        is unbounded.
      */
      explicit ASN1_Pretty_Printer(size_t print_limit = 4096,
                                   size_t print_binary_limit = 2048,
                                   bool print_context_specific = true,
                                   size_t initial_level = 0,
                                   size_t value_column = 60,
                                   size_t max_depth = 64) :
            ASN1_Formatter(print_context_specific, max_depth),
            m_print_limit(print_limit),
            m_print_binary_limit(print_binary_limit),
            m_initial_level(initial_level),
            m_value_column(value_column) {}

   private:
      std::string format(
         ASN1_Type type_tag, ASN1_Class class_tag, size_t level, size_t length, std::string_view value) const override;

      std::string format_bin(ASN1_Type type_tag, ASN1_Class class_tag, const std::vector<uint8_t>& vec) const override;

      std::string format_bn(const BigInt& bn) const override;

      const size_t m_print_limit;
      const size_t m_print_binary_limit;
      const size_t m_initial_level;
      const size_t m_value_column;
};

}  // namespace Botan

BOTAN_FUTURE_INTERNAL_HEADER(assert.h)

namespace Botan {

// NOLINTBEGIN(*-macro-usage)

/**
* Called when an assertion fails
* Throws an Exception object
*/
[[noreturn]] void BOTAN_PUBLIC_API(2, 0)
   assertion_failure(const char* expr_str, const char* assertion_made, const char* func, const char* file, int line);

/**
* Called when an invalid argument is used
* Throws Invalid_Argument
*/
[[noreturn]] void BOTAN_UNSTABLE_API throw_invalid_argument(const char* message, const char* func, const char* file);

#define BOTAN_ARG_CHECK(expr, msg)                               \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                        \
   do {                                                          \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */              \
      if(!(expr)) {                                              \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */     \
         Botan::throw_invalid_argument(msg, __func__, __FILE__); \
      }                                                          \
   } while(0)

/**
* Called when an invalid state is encountered
* Throws Invalid_State
*/
[[noreturn]] void BOTAN_UNSTABLE_API throw_invalid_state(const char* message, const char* func, const char* file);

#define BOTAN_STATE_CHECK(expr)                                 \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                       \
   do {                                                         \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */             \
      if(!(expr)) {                                             \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */    \
         Botan::throw_invalid_state(#expr, __func__, __FILE__); \
      }                                                         \
   } while(0)

/**
* Make an assertion
*/
#define BOTAN_ASSERT(expr, assertion_made)                                              \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                                               \
   do {                                                                                 \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */                                     \
      if(!(expr)) {                                                                     \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */                            \
         Botan::assertion_failure(#expr, assertion_made, __func__, __FILE__, __LINE__); \
      }                                                                                 \
   } while(0)

/**
* Make an assertion
*/
#define BOTAN_ASSERT_NOMSG(expr)                                            \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                                   \
   do {                                                                     \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */                         \
      if(!(expr)) {                                                         \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */                \
         Botan::assertion_failure(#expr, "", __func__, __FILE__, __LINE__); \
      }                                                                     \
   } while(0)

/**
* Assert that value1 == value2
*/
#define BOTAN_ASSERT_EQUAL(expr1, expr2, assertion_made)                                               \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                                                              \
   do {                                                                                                \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */                                                    \
      if((expr1) != (expr2)) {                                                                         \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */                                           \
         Botan::assertion_failure(#expr1 " == " #expr2, assertion_made, __func__, __FILE__, __LINE__); \
      }                                                                                                \
   } while(0)

/**
* Assert that expr1 (if true) implies expr2 is also true
*/
#define BOTAN_ASSERT_IMPLICATION(expr1, expr2, msg)                                              \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                                                        \
   do {                                                                                          \
      /* NOLINTNEXTLINE(*-simplify-boolean-expr) */                                              \
      if((expr1) && !(expr2)) {                                                                  \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */                                     \
         Botan::assertion_failure(#expr1 " implies " #expr2, msg, __func__, __FILE__, __LINE__); \
      }                                                                                          \
   } while(0)

/**
* Assert that a pointer is not null
*/
#define BOTAN_ASSERT_NONNULL(ptr)                                                         \
   /* NOLINTNEXTLINE(*-avoid-do-while) */                                                 \
   do {                                                                                   \
      if((ptr) == nullptr) {                                                              \
         /* NOLINTNEXTLINE(bugprone-lambda-function-name) */                              \
         Botan::assertion_failure(#ptr " is not null", "", __func__, __FILE__, __LINE__); \
      }                                                                                   \
   } while(0)

#if defined(BOTAN_ENABLE_DEBUG_ASSERTS)

   #define BOTAN_DEBUG_ASSERT(expr) BOTAN_ASSERT_NOMSG(expr)

#else

   #define BOTAN_DEBUG_ASSERT(expr)       \
      do { /* NOLINT(*-avoid-do-while) */ \
      } while(0)

#endif

/**
* Mark variable as unused.
*
* Takes any number of arguments and marks all as unused, for instance
* BOTAN_UNUSED(a); or BOTAN_UNUSED(x, y, z);
*/
template <typename... T>
constexpr void ignore_params([[maybe_unused]] const T&... args) {}

#define BOTAN_UNUSED Botan::ignore_params

/*
* Define Botan::assert_unreachable and BOTAN_ASSERT_UNREACHABLE
*
* This is intended to be used in the same situations as `std::unreachable()`;
* a codepath that (should not) be reachable but where the compiler cannot
* tell that it is unreachable.
*
* Unlike `std::unreachable()`, or equivalent compiler builtins like GCC's
* `__builtin_unreachable`, this function is not UB. By default it will
* throw an exception. If `BOTAN_TERMINATE_ON_ASSERTS` is defined, it will
* instead print a message to stderr and abort.
*
* Due to this difference, and the fact that it is not inlined, calling
* this is significantly more costly than using `std::unreachable`.
*/
[[noreturn]] void BOTAN_UNSTABLE_API assert_unreachable(const char* file, int line);

#define BOTAN_ASSERT_UNREACHABLE() Botan::assert_unreachable(__FILE__, __LINE__)

// NOLINTEND(*-macro-usage)

}  // namespace Botan


namespace Botan {

template <typename T, typename Tag, typename... Capabilities>
class Strong;

template <typename... Ts>
struct is_strong_type : std::false_type {};

template <typename... Ts>
struct is_strong_type<Strong<Ts...>> : std::true_type {};

template <typename... Ts>
constexpr bool is_strong_type_v = is_strong_type<std::remove_const_t<Ts>...>::value;

template <typename T0 = void, typename... Ts>
struct all_same {
      static constexpr bool value = (std::is_same_v<T0, Ts> && ... && true);
};

template <typename... Ts>
static constexpr bool all_same_v = all_same<Ts...>::value;

namespace detail {

/**
 * Helper type to indicate that a certain type should be automatically
 * detected based on the context.
 */
struct AutoDetect {
      constexpr AutoDetect() = delete;
};

}  // namespace detail

namespace ranges {

/**
 * Models a std::ranges::contiguous_range that (optionally) restricts its
 * value_type to ValueT. In other words: a stretch of contiguous memory of
 * a certain type (optional ValueT).
 */
template <typename T, typename ValueT = std::ranges::range_value_t<T>>
concept contiguous_range = std::ranges::contiguous_range<T> && std::same_as<ValueT, std::ranges::range_value_t<T>>;

/**
 * Models a std::ranges::contiguous_range that satisfies
 * std::ranges::output_range with an arbitrary value_type. In other words: a
 * stretch of contiguous memory of a certain type (optional ValueT) that can be
 * written to.
 */
template <typename T, typename ValueT = std::ranges::range_value_t<T>>
concept contiguous_output_range = contiguous_range<T, ValueT> && std::ranges::output_range<T, ValueT>;

/**
 * Models a range that can be turned into a std::span<>. Typically, this is some
 * form of ranges::contiguous_range.
 */
template <typename T>
concept spanable_range = std::constructible_from<std::span<const std::ranges::range_value_t<T>>, T>;

/**
 * Models a range that can be turned into a std::span<> with a static extent.
 * Typically, this is a std::array or a std::span derived from an array.
 */
// clang-format off
template <typename T>
concept statically_spanable_range = spanable_range<T> &&
                                    decltype(std::span{std::declval<T&>()})::extent != std::dynamic_extent;

// clang-format on

/**
 * Find the length in bytes of a given contiguous range @p r.
 */
inline constexpr size_t size_bytes(const spanable_range auto& r) {
   return std::span{r}.size_bytes();
}

/**
 * Check that a given range @p r has a certain statically-known byte length. If
 * the range's extent is known at compile time, this is a static check,
 * otherwise a runtime argument check will be added.
 *
 * @throws Invalid_Argument  if range @p r has a dynamic extent and does not
 *                           feature the expected byte length.
 */
template <size_t expected, spanable_range R>
inline constexpr void assert_exact_byte_length(const R& r) {
   const std::span s{r};
   if constexpr(statically_spanable_range<R>) {
      static_assert(s.size_bytes() == expected, "memory region does not have expected byte lengths");
   } else {
      if(s.size_bytes() != expected) {
         throw Invalid_Argument("Memory regions did not have expected byte lengths");
      }
   }
}

/**
 * Check that a list of ranges (in @p r0 and @p rs) all have the same byte
 * lengths. If the first range's extent is known at compile time, this will be a
 * static check for all other ranges whose extents are known at compile time,
 * otherwise a runtime argument check will be added.
 *
 * @throws Invalid_Argument  if any range has a dynamic extent and not all
 *                           ranges feature the same byte length.
 */
template <spanable_range R0, spanable_range... Rs>
inline constexpr void assert_equal_byte_lengths(const R0& r0, const Rs&... rs)
   requires(sizeof...(Rs) > 0)
{
   const std::span s0{r0};

   if constexpr(statically_spanable_range<R0>) {
      constexpr size_t expected_size = s0.size_bytes();
      (assert_exact_byte_length<expected_size>(rs), ...);
   } else {
      const size_t expected_size = s0.size_bytes();
      const bool correct_size =
         ((std::span<const std::ranges::range_value_t<Rs>>{rs}.size_bytes() == expected_size) && ...);

      if(!correct_size) {
         throw Invalid_Argument("Memory regions did not have equal lengths");
      }
   }
}

}  // namespace ranges

namespace concepts {

// TODO: C++20 provides concepts like std::ranges::range or ::sized_range
//       but at the time of this writing clang had not caught up on all
//       platforms. E.g. clang 14 on Xcode does not support ranges properly.

template <typename IterT, typename ContainerT>
concept container_iterator =
   std::same_as<IterT, typename ContainerT::iterator> || std::same_as<IterT, typename ContainerT::const_iterator>;

template <typename PtrT, typename ContainerT>
concept container_pointer =
   std::same_as<PtrT, typename ContainerT::pointer> || std::same_as<PtrT, typename ContainerT::const_pointer>;

template <typename T>
concept container = requires(T a) {
   { a.begin() } -> container_iterator<T>;
   { a.end() } -> container_iterator<T>;
   { a.cbegin() } -> container_iterator<T>;
   { a.cend() } -> container_iterator<T>;
   { a.size() } -> std::same_as<typename T::size_type>;
   typename T::value_type;
};

template <typename T>
concept contiguous_container = container<T> && requires(T a) {
   { a.data() } -> container_pointer<T>;
};

template <typename T>
concept has_empty = requires(T a) {
   { a.empty() } -> std::same_as<bool>;
};

// clang-format off
template <typename T>
concept has_bounds_checked_accessors = container<T> && (
                                          requires(T a, const T ac, typename T::size_type s) {
                                             { a.at(s) } -> std::same_as<typename T::value_type&>;
                                             { ac.at(s) } -> std::same_as<const typename T::value_type&>;
                                          } ||
                                          requires(T a, const T ac, typename T::key_type k) {
                                             { a.at(k) } -> std::same_as<typename T::mapped_type&>;
                                             { ac.at(k) } -> std::same_as<const typename T::mapped_type&>;
                                          });
// clang-format on

template <typename T>
concept resizable_container = container<T> && requires(T& c, typename T::size_type s) {
   T(s);
   c.resize(s);
};

template <typename T>
concept reservable_container = container<T> && requires(T& c, typename T::size_type s) { c.reserve(s); };

template <typename T>
concept resizable_byte_buffer =
   contiguous_container<T> && resizable_container<T> && std::same_as<typename T::value_type, uint8_t>;

template <typename T>
concept streamable = requires(std::ostream& os, T a) { os << a; };

template <class T>
concept strong_type = is_strong_type_v<T>;

template <class T>
concept contiguous_strong_type = strong_type<T> && contiguous_container<T>;

template <class T>
concept integral_strong_type = strong_type<T> && std::integral<typename T::wrapped_type>;

template <class T>
concept unsigned_integral_strong_type = strong_type<T> && std::unsigned_integral<typename T::wrapped_type>;

template <typename T, typename Capability>
concept strong_type_with_capability = T::template has_capability<Capability>();

}  // namespace concepts

}  // namespace Botan


namespace Botan {

class Entropy_Sources;

/**
* An interface to a cryptographic random number generator
*/
class BOTAN_PUBLIC_API(2, 0) RandomNumberGenerator {
   public:
      /**
      * Userspace RNGs like HMAC_DRBG will reseed after a specified number
      * of outputs are generated. Set to zero to disable automatic reseeding.
      */
      static constexpr size_t DefaultReseedInterval = 1024;

      /**
      * Number of entropy bits polled for reseeding userspace RNGs like HMAC_DRBG
      */
      static constexpr size_t DefaultPollBits = 256;

      /**
      * Default poll timeout
      */
      static constexpr auto DefaultPollTimeout = std::chrono::milliseconds(50);

      virtual ~RandomNumberGenerator() = default;

      RandomNumberGenerator() = default;

      /*
      * Never copy a RNG, create a new one
      */
      RandomNumberGenerator(const RandomNumberGenerator& rng) = delete;
      RandomNumberGenerator& operator=(const RandomNumberGenerator& rng) = delete;

      RandomNumberGenerator(RandomNumberGenerator&& rng) = default;
      RandomNumberGenerator& operator=(RandomNumberGenerator&& rng) = default;

      /**
      * Randomize a byte array.
      *
      * May block shortly if e.g. the RNG is not yet initialized
      * or a retry because of insufficient entropy is needed.
      *
      * @param output the byte array to hold the random output.
      * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
      * @throws Exception if the RNG fails
      */
      void randomize(std::span<uint8_t> output) { this->fill_bytes_with_input(output, {}); }

      void randomize(uint8_t output[], size_t length) { this->randomize(std::span(output, length)); }

      /**
      * Returns false if it is known that this RNG object is not able to accept
      * externally provided inputs (via add_entropy, randomize_with_input, etc).
      * In this case, any such provided inputs are ignored.
      *
      * If this function returns true, then inputs may or may not be accepted.
      */
      virtual bool accepts_input() const = 0;

      /**
      * Incorporate some additional data into the RNG state. For
      * example adding nonces or timestamps from a peer's protocol
      * message can help hedge against VM state rollback attacks.
      * A few RNG types do not accept any externally provided input,
      * in which case this function is a no-op.
      *
      * @param input a byte array containing the entropy to be added
      * @throws Exception may throw if the RNG accepts input, but adding the entropy failed.
      */
      void add_entropy(std::span<const uint8_t> input) { this->fill_bytes_with_input({}, input); }

      void add_entropy(const uint8_t input[], size_t length) { this->add_entropy(std::span(input, length)); }

      /**
      * Incorporate some additional data into the RNG state.
      */
      template <typename T>
         requires std::is_standard_layout_v<T> && std::is_trivial_v<T>
      void add_entropy_T(const T& t) {
         this->add_entropy(reinterpret_cast<const uint8_t*>(&t), sizeof(T));
      }

      /**
      * Incorporate entropy into the RNG state then produce output.
      * Some RNG types implement this using a single operation, default
      * calls add_entropy + randomize in sequence.
      *
      * Use this to further bind the outputs to your current
      * process/protocol state. For instance if generating a new key
      * for use in a session, include a session ID or other such
      * value. See NIST SP 800-90 A, B, C series for more ideas.
      *
      * @param output buffer to hold the random output
      * @param input entropy buffer to incorporate
      * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
      * @throws Exception if the RNG fails
      * @throws Exception may throw if the RNG accepts input, but adding the entropy failed.
      */
      void randomize_with_input(std::span<uint8_t> output, std::span<const uint8_t> input) {
         this->fill_bytes_with_input(output, input);
      }

      void randomize_with_input(uint8_t output[], size_t output_len, const uint8_t input[], size_t input_len) {
         this->randomize_with_input(std::span(output, output_len), std::span(input, input_len));
      }

      /**
      * This calls `randomize_with_input` using some timestamps as extra input.
      *
      * For a stateful RNG using non-random but potentially unique data the
      * extra input can help protect against problems with fork, VM state
      * rollback, or other cases where somehow an RNG state is duplicated. If
      * both of the duplicated RNG states later incorporate a timestamp (and the
      * timestamps don't themselves repeat), their outputs will diverge.
      *
      * @param output buffer to hold the random output
      * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
      * @throws Exception if the RNG fails
      * @throws Exception may throw if the RNG accepts input, but adding the entropy failed.
      */
      void randomize_with_ts_input(std::span<uint8_t> output);

      void randomize_with_ts_input(uint8_t output[], size_t output_len) {
         this->randomize_with_ts_input(std::span(output, output_len));
      }

      /**
      * @return the name of this RNG type
      */
      virtual std::string name() const = 0;

      /**
      * Clear all internally held values of this RNG
      * @post is_seeded() == false if the RNG has an internal state that can be cleared.
      */
      virtual void clear() = 0;

      /**
      * Check whether this RNG is seeded.
      * @return true if this RNG was already seeded, false otherwise.
      */
      virtual bool is_seeded() const = 0;

      /**
      * Poll provided sources for up to poll_bits bits of entropy
      * or until the timeout expires. Returns estimate of the number
      * of bits collected.
      *
      * Sets the seeded state to true if enough entropy was added.
      */
      virtual size_t reseed(Entropy_Sources& srcs,
                            size_t poll_bits = RandomNumberGenerator::DefaultPollBits,
                            std::chrono::milliseconds poll_timeout = RandomNumberGenerator::DefaultPollTimeout);

      /**
      * Reseed by reading specified bits from the RNG
      *
      * Sets the seeded state to true if enough entropy was added.
      *
      * @throws Exception if RNG accepts input but reseeding failed.
      */
      virtual void reseed_from_rng(RandomNumberGenerator& rng,
                                   size_t poll_bits = RandomNumberGenerator::DefaultPollBits);

      // Some utility functions built on the interface above:

      /**
      * Fill a given byte container with @p bytes random bytes
      *
      * @todo deprecate this overload (in favor of randomize())
      *
      * @param  v     the container to be filled with @p bytes random bytes
      * @throws Exception if RNG fails
      */
      void random_vec(std::span<uint8_t> v) { this->randomize(v); }

      /**
      * Resize a given byte container to @p bytes and fill it with random bytes
      *
      * @tparam T     the desired byte container type (e.g std::vector<uint8_t>)
      * @param  v     the container to be filled with @p bytes random bytes
      * @param  bytes number of random bytes to initialize the container with
      * @throws Exception if RNG or memory allocation fails
      */
      template <concepts::resizable_byte_buffer T>
      void random_vec(T& v, size_t bytes) {
         v.resize(bytes);
         random_vec(v);
      }

      /**
      * Create some byte container type and fill it with some random @p bytes.
      *
      * @tparam T     the desired byte container type (e.g std::vector<uint8_t>)
      * @param  bytes number of random bytes to initialize the container with
      * @return       a container of type T with @p bytes random bytes
      * @throws Exception if RNG or memory allocation fails
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
         requires std::default_initializable<T>
      T random_vec(size_t bytes) {
         T result;
         random_vec(result, bytes);
         return result;
      }

      /**
       * Create a std::array of @p bytes random bytes
       */
      template <size_t bytes>
      std::array<uint8_t, bytes> random_array() {
         std::array<uint8_t, bytes> result{};
         random_vec(result);
         return result;
      }

      /**
      * Return a random byte
      * @return random byte
      * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
      * @throws Exception if the RNG fails
      */
      uint8_t next_byte() {
         uint8_t b = 0;
         this->fill_bytes_with_input(std::span(&b, 1), {});
         return b;
      }

      /**
      * @return a random byte that is greater than zero
      * @throws PRNG_Unseeded if the RNG fails because it has not enough entropy
      * @throws Exception if the RNG fails
      */
      uint8_t next_nonzero_byte() {
         uint8_t b = this->next_byte();
         while(b == 0) {
            b = this->next_byte();
         }
         return b;
      }

   protected:
      /**
      * Generic interface to provide entropy to a concrete implementation and to
      * fill a given buffer with random output. Both @p output and @p input may
      * be empty and should be ignored in that case. If both buffers are
      * non-empty implementations should typically first apply the @p input data
      * and then generate random data into @p output.
      *
      * This method must be implemented by all RandomNumberGenerator sub-classes.
      *
      * @param output  Byte buffer to write random bytes into. Implementations
      *                should not read from this buffer.
      * @param input   Byte buffer that may contain bytes to be incorporated in
      *                the RNG's internal state. Implementations may choose to
      *                ignore the bytes in this buffer.
      */
      virtual void fill_bytes_with_input(std::span<uint8_t> output, std::span<const uint8_t> input) = 0;
};

/**
* Convenience typedef
*/
typedef RandomNumberGenerator RNG;

/**
* Hardware_RNG exists to tag hardware RNG types (PKCS11_RNG, TPM_RNG, Processor_RNG)
*/
class BOTAN_PUBLIC_API(2, 0) Hardware_RNG : public RandomNumberGenerator {
   public:
      void clear() final { /* no way to clear state of hardware RNG */
      }
};

/**
* Null/stub RNG - fails if you try to use it for anything
* This is not generally useful except for in certain tests
*/
class BOTAN_PUBLIC_API(2, 0) Null_RNG final : public RandomNumberGenerator {
   public:
      bool is_seeded() const override { return false; }

      bool accepts_input() const override { return false; }

      void clear() override {}

      std::string name() const override { return "Null_RNG"; }

   private:
      void fill_bytes_with_input(std::span<uint8_t> output, std::span<const uint8_t> /* ignored */) override;
};

}  // namespace Botan


namespace Botan {

class Stateful_RNG;

/**
* A userspace PRNG
*/
class BOTAN_PUBLIC_API(2, 0) AutoSeeded_RNG final : public RandomNumberGenerator {
   public:
      bool is_seeded() const override;

      bool accepts_input() const override { return true; }

      /**
      * Mark state as requiring a reseed on next use
      */
      void force_reseed();

      size_t reseed(Entropy_Sources& srcs,
                    size_t poll_bits = RandomNumberGenerator::DefaultPollBits,
                    std::chrono::milliseconds poll_timeout = RandomNumberGenerator::DefaultPollTimeout) override;

      std::string name() const override;

      void clear() override;

      /**
      * Uses the system RNG (if available) or else a default group of
      * entropy sources (all other systems) to gather seed material.
      *
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      BOTAN_FUTURE_EXPLICIT AutoSeeded_RNG(size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval);

      /**
      * Create an AutoSeeded_RNG which will get seed material from some other
      * RNG instance. For example you could provide a reference to the system
      * RNG or a hardware RNG.
      *
      * @param underlying_rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      BOTAN_FUTURE_EXPLICIT AutoSeeded_RNG(RandomNumberGenerator& underlying_rng,
                                           size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval);

      /**
      * Create an AutoSeeded_RNG which will get seed material from a set of
      * entropy sources.
      *
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      BOTAN_FUTURE_EXPLICIT AutoSeeded_RNG(Entropy_Sources& entropy_sources,
                                           size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval);

      /**
      * Create an AutoSeeded_RNG which will get seed material from both an
      * underlying RNG and a set of entropy sources.
      *
      * @param underlying_rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      AutoSeeded_RNG(RandomNumberGenerator& underlying_rng,
                     Entropy_Sources& entropy_sources,
                     size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval);

      AutoSeeded_RNG(const AutoSeeded_RNG& other) = delete;
      AutoSeeded_RNG(AutoSeeded_RNG&& other) noexcept;
      AutoSeeded_RNG& operator=(const AutoSeeded_RNG& other) = delete;
      AutoSeeded_RNG& operator=(AutoSeeded_RNG&& other) = delete;

      ~AutoSeeded_RNG() override;

   private:
      void fill_bytes_with_input(std::span<uint8_t> out, std::span<const uint8_t> in) override;

   private:
      std::unique_ptr<Stateful_RNG> m_rng;
};

}  // namespace Botan

namespace Botan {

/**
* Perform base64 encoding
* @param output an array of at least base64_encode_max_output bytes
* @param input is some binary data
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param final_inputs true iff this is the last input, in which case
         padding chars will be applied if needed
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
   base64_encode(char output[], const uint8_t input[], size_t input_length, size_t& input_consumed, bool final_inputs);

/**
* Perform base64 encoding
* @param input some input
* @param input_length length of input in bytes
* @return base64adecimal representation of input
*/
std::string BOTAN_PUBLIC_API(2, 0) base64_encode(const uint8_t input[], size_t input_length);

/**
* Perform base64 encoding
* @param input some input
* @return base64adecimal representation of input
*/
inline std::string base64_encode(std::span<const uint8_t> input) {
   return base64_encode(input.data(), input.size());
}

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param final_inputs true iff this is the last input, in which case
         padding is allowed
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0) base64_decode(uint8_t output[],
                                            const char input[],
                                            size_t input_length,
                                            size_t& input_consumed,
                                            bool final_inputs,
                                            bool ignore_ws = true);

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param input_length length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
   base64_decode(uint8_t output[], const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param output an array of at least base64_decode_max_output bytes
* @param input some base64 input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(3, 0) base64_decode(uint8_t output[], std::string_view input, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param output a contiguous byte buffer of at least base64_decode_max_output bytes
* @param input some base64 input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(3, 0) base64_decode(std::span<uint8_t> output, std::string_view input, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param input some base64 input
* @param input_length the length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded base64 output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(2, 0)
   base64_decode(const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform base64 decoding
* @param input some base64 input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded base64 output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(3, 0) base64_decode(std::string_view input, bool ignore_ws = true);

/**
* Calculate the size of output buffer for base64_encode
* @param input_length the length of input in bytes
* @return the size of output buffer in bytes
*/
size_t BOTAN_PUBLIC_API(2, 1) base64_encode_max_output(size_t input_length);

/**
* Calculate the size of output buffer for base64_decode
* @param input_length the length of input in bytes
* @return the size of output buffer in bytes
*/
size_t BOTAN_PUBLIC_API(2, 1) base64_decode_max_output(size_t input_length);

}  // namespace Botan

namespace Botan {

class RandomNumberGenerator;

/**
* Create a password hash using Bcrypt
*
* Takes the @p password to hash, a @p rng, and a @p work_factor. The resulting
* password hash is returned as a string.
*
* Higher work factors increase the amount of time the algorithm runs, increasing
* the cost of cracking attempts. The increase is exponential, so a work factor
* of 12 takes roughly twice as long as work factor 11. The default work factor
* was set to 10 up until the 2.8.0 release.
*
* It is recommended to set the work factor as high as your system can tolerate
* (from a performance and latency perspective) since higher work factors greatly
* improve the security against GPU-based attacks.  For example, for protecting
* high value administrator passwords, consider using work factor 15 or 16; at
* these work factors each bcrypt computation takes several seconds. Since admin
* logins will be relatively uncommon, it might be acceptable for each login
* attempt to take some time. As of 2018, a good password cracking rig (with 8
* NVIDIA 1080 cards) can attempt about 1 billion bcrypt computations per month
* for work factor 13. For work factor 12, it can do twice as many.  For work
* factor 15, it can do only one quarter as many attempts.
*
* Due to bugs affecting various implementations of bcrypt, several different
* variants of the algorithm are defined. As of 2.7.0 Botan supports generating
* (or checking) the 2a, 2b, and 2y variants.  Since Botan has never been
* affected by any of the bugs which necessitated these version upgrades, all
* three versions are identical beyond the version identifier. Which variant to
* use is controlled by the @p version argument.
*
* The bcrypt @p work_factor must be at least 4 (though at this work factor
* bcrypt is not very secure). The bcrypt format allows up to 31, but Botan
* currently rejects all work factors greater than 18 since even that work factor
* requires roughly 15 seconds of computation on a fast machine.
*
* @warning The password is truncated at at most 72 characters; characters after
*          that do not have any effect on the resulting hash. To support longer
*          passwords, consider pre-hashing the password, for example by using
*          the hex encoding of SHA-256 of the password as the input to bcrypt.
*
* @param password the password.
* @param rng a random number generator
* @param work_factor how much work to do to slow down guessing attacks
* @param version which version to emit (may be 'a', 'b', or 'y' all of which
*        have identical behavior in this implementation).
*
* @see https://www.usenix.org/events/usenix99/provos/provos_html/
*
* TODO(Botan4) Convert work_factor to a size_t
*/
std::string BOTAN_PUBLIC_API(2, 0) generate_bcrypt(std::string_view password,
                                                   RandomNumberGenerator& rng,
                                                   uint16_t work_factor = 12,
                                                   char version = 'a');

/**
* Check a previously created password hash
*
* Takes a @p password and a bcrypt @p hash and returns true if the password is
* the same as the one that was used to generate the bcrypt hash.
*
* @param password the password to check against
* @param hash the stored hash to check against
*/
bool BOTAN_PUBLIC_API(2, 0) check_bcrypt(std::string_view password, std::string_view hash);

}  // namespace Botan

namespace Botan {

/**
* This class represents an abstract data source object.
*/
class BOTAN_PUBLIC_API(2, 0) DataSource {
   public:
      /**
      * Read from the source. Moves the internal offset so that every
      * call to read will return a new portion of the source.
      *
      * @param out the byte array to write the result to
      * @param length the length of the byte array out
      * @return length in bytes that was actually read and put
      * into out
      */
      [[nodiscard]] virtual size_t read(uint8_t out[], size_t length) = 0;

      virtual bool check_available(size_t n) = 0;

      /**
      * Read from the source but do not modify the internal
      * offset. Consecutive calls to peek() will return portions of
      * the source starting at the same position.
      *
      * @param out the byte array to write the output to
      * @param length the length of the byte array out
      * @param peek_offset the offset into the stream to read at
      * @return length in bytes that was actually read and put
      * into out
      */
      [[nodiscard]] virtual size_t peek(uint8_t out[], size_t length, size_t peek_offset) const = 0;

      /**
      * Test whether the source still has data that can be read.
      * @return true if there is no more data to read, false otherwise
      */
      virtual bool end_of_data() const = 0;

      /**
      * return the id of this data source
      * @return std::string representing the id of this data source
      */
      virtual std::string id() const { return ""; }

      /**
      * Read one byte.
      * @param out the byte to read to
      * @return length in bytes that was actually read and put
      * into out
      */
      size_t read_byte(uint8_t& out);

      /**
      * Read one byte.
      *
      * Returns nullopt if no further bytes are available
      */
      std::optional<uint8_t> read_byte();

      /**
      * Peek at one byte.
      * @param out an output byte
      * @return length in bytes that was actually read and put
      * into out
      */
      size_t peek_byte(uint8_t& out) const;

      /**
      * Discard the next N bytes of the data
      * @param N the number of bytes to discard
      * @return number of bytes actually discarded
      */
      size_t discard_next(size_t N);

      /**
      * @return number of bytes read so far.
      */
      virtual size_t get_bytes_read() const = 0;

      DataSource() = default;
      virtual ~DataSource() = default;
      DataSource(const DataSource&) = delete;
      DataSource(DataSource&&) = default;
      DataSource& operator=(const DataSource&) = delete;
      DataSource& operator=(DataSource&&) = default;
};

/**
* This class represents a Memory-Based DataSource
*/
class BOTAN_PUBLIC_API(2, 0) DataSource_Memory final : public DataSource {
   public:
      size_t read(uint8_t buf[], size_t length) override;
      size_t peek(uint8_t buf[], size_t length, size_t offset) const override;
      bool check_available(size_t n) override;
      bool end_of_data() const override;

      /**
      * Construct a memory source that reads from a string
      * @param in the string to read from
      */
      explicit DataSource_Memory(std::string_view in);

      /**
      * Construct a memory source that reads from a byte array
      * @param in the byte array to read from
      * @param length the length of the byte array
      */
      DataSource_Memory(const uint8_t in[], size_t length) : m_source(in, in + length), m_offset(0) {}

      /**
      * Construct a memory source that reads from a secure_vector
      * @param in the MemoryRegion to read from
      */
      explicit DataSource_Memory(secure_vector<uint8_t> in) : m_source(std::move(in)), m_offset(0) {}

      /**
      * Construct a memory source that reads from an arbitrary byte buffer
      * @param in the MemoryRegion to read from
      */
      explicit DataSource_Memory(std::span<const uint8_t> in) : m_source(in.begin(), in.end()), m_offset(0) {}

      /**
      * Construct a memory source that reads from a std::vector
      * @param in the MemoryRegion to read from
      */
      explicit DataSource_Memory(const std::vector<uint8_t>& in) : m_source(in.begin(), in.end()), m_offset(0) {}

      size_t get_bytes_read() const override { return m_offset; }

   private:
      secure_vector<uint8_t> m_source;
      size_t m_offset;
};

/**
* This class represents a Stream-Based DataSource.
*/
class BOTAN_PUBLIC_API(2, 0) DataSource_Stream final : public DataSource {
   public:
      size_t read(uint8_t buf[], size_t length) override;
      size_t peek(uint8_t buf[], size_t length, size_t offset) const override;
      bool check_available(size_t n) override;
      bool end_of_data() const override;
      std::string id() const override;

      BOTAN_FUTURE_EXPLICIT DataSource_Stream(std::istream& in, std::string_view id = "<std::istream>");

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)
      /**
      * Construct a Stream-Based DataSource from filesystem path
      * @param filename the path to the file
      * @param use_binary whether to treat the file as binary or not
      */
      BOTAN_FUTURE_EXPLICIT DataSource_Stream(std::string_view filename, bool use_binary = false);
#endif

      DataSource_Stream(const DataSource_Stream&) = delete;
      DataSource_Stream(DataSource_Stream&&) = delete;
      DataSource_Stream& operator=(const DataSource_Stream&) = delete;
      DataSource_Stream& operator=(DataSource_Stream&&) = delete;

      ~DataSource_Stream() override;

      size_t get_bytes_read() const override { return m_total_read; }

   private:
      const std::string m_identifier;

      std::unique_ptr<std::istream> m_source_memory;
      std::istream& m_source;
      size_t m_total_read;
};

}  // namespace Botan

namespace Botan {

class BigInt;

/**
* BER Decoding Object
*/
class BOTAN_PUBLIC_API(2, 0) BER_Decoder final {
   public:
      /**
      * Set up to BER decode the data in buf of length len
      */
      BER_Decoder(const uint8_t buf[], size_t len);

      /**
      * Set up to BER decode the data in buf of length len
      */
      BOTAN_FUTURE_EXPLICIT BER_Decoder(std::span<const uint8_t> buf) : BER_Decoder(buf.data(), buf.size()) {}

      /**
      * Set up to BER decode the data in vec
      */
      explicit BER_Decoder(const secure_vector<uint8_t>& vec);

      /**
      * Set up to BER decode the data in vec
      */
      explicit BER_Decoder(const std::vector<uint8_t>& vec);

      /**
      * Set up to BER decode the data in src
      */
      explicit BER_Decoder(DataSource& src);

      /**
      * Set up to BER decode the data in obj
      */
      BOTAN_FUTURE_EXPLICIT BER_Decoder(const BER_Object& obj) : BER_Decoder(obj.bits(), obj.length()) {}

      /**
      * Set up to BER decode the data in obj
      */
      BOTAN_FUTURE_EXPLICIT BER_Decoder(BER_Object&& obj) : BER_Decoder(std::move(obj), nullptr) {}

      BER_Decoder(const BER_Decoder& other);
      BER_Decoder(BER_Decoder&& other) = default;

      BER_Decoder& operator=(const BER_Decoder&) = delete;
      BER_Decoder& operator=(BER_Decoder&&) = default;

      /**
      * Get the next object in the data stream.
      * If EOF, returns an object with type NO_OBJECT.
      */
      BER_Object get_next_object();

      BER_Decoder& get_next(BER_Object& ber) {
         ber = get_next_object();
         return (*this);
      }

      /**
      * Peek at the next object without removing it from the stream
      *
      * If an object has been pushed, then it returns that object.
      * Otherwise it reads the next object and pushes it. Thus, a you
      * call peek_next_object followed by push_back without a
      * subsequent read, it will fail.
      */
      const BER_Object& peek_next_object();

      /**
      * Push an object back onto the stream. Throws if another
      * object was previously pushed and has not been subsequently
      * read out.
      */
      void push_back(const BER_Object& obj);

      /**
      * Push an object back onto the stream. Throws if another
      * object was previously pushed and has not been subsequently
      * read out.
      */
      void push_back(BER_Object&& obj);

      /**
      * Return true if there is at least one more item remaining
      */
      bool more_items() const;

      /**
      * Verify the stream is concluded, throws otherwise.
      * Returns (*this)
      */
      BER_Decoder& verify_end();

      /**
      * Verify the stream is concluded, throws otherwise.
      * Returns (*this)
      */
      BER_Decoder& verify_end(std::string_view err_msg);

      /**
      * Discard any data that remains unread
      * Returns (*this)
      */
      BER_Decoder& discard_remaining();

      BER_Decoder start_cons(ASN1_Type type_tag, ASN1_Class class_tag);

      BER_Decoder start_sequence() { return start_cons(ASN1_Type::Sequence, ASN1_Class::Universal); }

      BER_Decoder start_set() { return start_cons(ASN1_Type::Set, ASN1_Class::Universal); }

      BER_Decoder start_context_specific(uint32_t tag) {
         return start_cons(ASN1_Type(tag), ASN1_Class::ContextSpecific);
      }

      BER_Decoder start_explicit_context_specific(uint32_t tag) {
         return start_cons(ASN1_Type(tag), ASN1_Class::ExplicitContextSpecific);
      }

      /**
      * Finish decoding a constructed data, throws if any data remains.
      * Returns the parent of *this (ie the object on which start_cons was called).
      */
      BER_Decoder& end_cons();

      /**
      * Get next object and copy value to POD type
      * Asserts value length is equal to POD type sizeof.
      * Asserts Type tag and optional Class tag according to parameters.
      * Copy value to POD type (struct, union, C-style array, std::array, etc.).
      * @param out POD type reference where to copy object value
      * @param type_tag ASN1_Type enum to assert type on object read
      * @param class_tag ASN1_Type enum to assert class on object read (default: CONTEXT_SPECIFIC)
      * @return this reference
      */
      template <typename T>
      BER_Decoder& get_next_value(T& out, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific)
         requires std::is_standard_layout_v<T> && std::is_trivial_v<T>
      {
         BER_Object obj = get_next_object();
         obj.assert_is_a(type_tag, class_tag);

         if(obj.length() != sizeof(T)) {
            throw BER_Decoding_Error("Size mismatch. Object value size is " + std::to_string(obj.length()) +
                                     "; Output type size is " + std::to_string(sizeof(T)));
         }

         std::memcpy(reinterpret_cast<uint8_t*>(&out), obj.bits(), obj.length());

         return (*this);
      }

      /*
      * Save all the bytes remaining in the source
      */
      template <typename Alloc>
      BER_Decoder& raw_bytes(std::vector<uint8_t, Alloc>& out) {
         out.clear();
         uint8_t buf = 0;
         while(m_source->read_byte(buf)) {
            out.push_back(buf);
         }
         return (*this);
      }

      BER_Decoder& decode_null();

      /**
      * Decode a BER encoded BOOLEAN
      */
      BER_Decoder& decode(bool& out) { return decode(out, ASN1_Type::Boolean, ASN1_Class::Universal); }

      /*
      * Decode a small BER encoded INTEGER
      */
      BER_Decoder& decode(size_t& out) { return decode(out, ASN1_Type::Integer, ASN1_Class::Universal); }

      /*
      * Decode a BER encoded INTEGER
      */
      BER_Decoder& decode(BigInt& out) { return decode(out, ASN1_Type::Integer, ASN1_Class::Universal); }

      std::vector<uint8_t> get_next_octet_string() {
         std::vector<uint8_t> out_vec;
         decode(out_vec, ASN1_Type::OctetString);
         return out_vec;
      }

      /*
      * BER decode a BIT STRING or OCTET STRING
      */
      template <typename Alloc>
      BER_Decoder& decode(std::vector<uint8_t, Alloc>& out, ASN1_Type real_type) {
         return decode(out, real_type, real_type, ASN1_Class::Universal);
      }

      BER_Decoder& decode(bool& v, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      BER_Decoder& decode(size_t& v, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      BER_Decoder& decode(BigInt& v, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      BER_Decoder& decode(std::vector<uint8_t>& v,
                          ASN1_Type real_type,
                          ASN1_Type type_tag,
                          ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      BER_Decoder& decode(secure_vector<uint8_t>& v,
                          ASN1_Type real_type,
                          ASN1_Type type_tag,
                          ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      BER_Decoder& decode(ASN1_Object& obj,
                          ASN1_Type type_tag = ASN1_Type::NoObject,
                          ASN1_Class class_tag = ASN1_Class::NoObject);

      /**
      * Decode an integer value which is typed as an octet string
      */
      BER_Decoder& decode_octet_string_bigint(BigInt& b);

      uint64_t decode_constrained_integer(ASN1_Type type_tag, ASN1_Class class_tag, size_t T_bytes);

      template <typename T>
      BER_Decoder& decode_integer_type(T& out) {
         return decode_integer_type<T>(out, ASN1_Type::Integer, ASN1_Class::Universal);
      }

      template <typename T>
      BER_Decoder& decode_integer_type(T& out, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific) {
         out = static_cast<T>(decode_constrained_integer(type_tag, class_tag, sizeof(out)));
         return (*this);
      }

      template <typename T>
      BER_Decoder& decode_optional(T& out, ASN1_Type type_tag, ASN1_Class class_tag, const T& default_value = T()) {
         std::optional<T> optval;
         this->decode_optional(optval, type_tag, class_tag);
         out = optval ? *optval : default_value;
         return (*this);
      }

      template <typename T>
      BER_Decoder& decode_optional(std::optional<T>& out, ASN1_Type type_tag, ASN1_Class class_tag);

      template <typename T>
      BER_Decoder& decode_optional_implicit(T& out,
                                            ASN1_Type type_tag,
                                            ASN1_Class class_tag,
                                            ASN1_Type real_type,
                                            ASN1_Class real_class,
                                            const T& default_value = T());

      template <typename T>
      BER_Decoder& decode_list(std::vector<T>& out,
                               ASN1_Type type_tag = ASN1_Type::Sequence,
                               ASN1_Class class_tag = ASN1_Class::Universal);

      template <typename T>
      bool decode_optional_list(std::vector<T>& out,
                                ASN1_Type type_tag = ASN1_Type::Sequence,
                                ASN1_Class class_tag = ASN1_Class::Universal);

      template <typename T>
      BER_Decoder& decode_and_check(const T& expected, std::string_view error_msg) {
         T actual;
         decode(actual);

         if(actual != expected) {
            throw Decoding_Error(error_msg);
         }

         return (*this);
      }

      /*
      * Decode an OPTIONAL string type
      */
      template <typename Alloc>
      BER_Decoder& decode_optional_string(std::vector<uint8_t, Alloc>& out,
                                          ASN1_Type real_type,
                                          uint32_t expected_tag,
                                          ASN1_Class class_tag = ASN1_Class::ContextSpecific) {
         BER_Object obj = get_next_object();

         ASN1_Type type_tag = static_cast<ASN1_Type>(expected_tag);

         if(obj.is_a(type_tag, class_tag)) {
            if(class_tag == ASN1_Class::ExplicitContextSpecific) {
               BER_Decoder(std::move(obj)).decode(out, real_type).verify_end();
            } else {
               push_back(std::move(obj));
               decode(out, real_type, type_tag, class_tag);
            }
         } else {
            out.clear();
            push_back(std::move(obj));
         }

         return (*this);
      }

      template <typename Alloc>
      BER_Decoder& decode_optional_string(std::vector<uint8_t, Alloc>& out,
                                          ASN1_Type real_type,
                                          ASN1_Type expected_tag,
                                          ASN1_Class class_tag = ASN1_Class::ContextSpecific) {
         return decode_optional_string(out, real_type, static_cast<uint32_t>(expected_tag), class_tag);
      }

      ~BER_Decoder() = default;

   private:
      BER_Decoder(BER_Object&& obj, BER_Decoder* parent);

      BER_Decoder* m_parent = nullptr;
      BER_Object m_pushed;
      // either m_data_src.get() or an unowned pointer
      DataSource* m_source;
      mutable std::unique_ptr<DataSource> m_data_src;
};

/*
* Decode an OPTIONAL or DEFAULT element
*/
template <typename T>
BER_Decoder& BER_Decoder::decode_optional(std::optional<T>& optval, ASN1_Type type_tag, ASN1_Class class_tag) {
   BER_Object obj = get_next_object();

   if(obj.is_a(type_tag, class_tag)) {
      T out{};
      if(class_tag == ASN1_Class::ExplicitContextSpecific) {
         BER_Decoder(std::move(obj)).decode(out).verify_end();
      } else {
         this->push_back(std::move(obj));
         this->decode(out, type_tag, class_tag);
      }
      optval = std::move(out);
   } else {
      this->push_back(std::move(obj));
      optval = std::nullopt;
   }

   return (*this);
}

/*
* Decode an OPTIONAL or DEFAULT element
*/
template <typename T>
BER_Decoder& BER_Decoder::decode_optional_implicit(T& out,
                                                   ASN1_Type type_tag,
                                                   ASN1_Class class_tag,
                                                   ASN1_Type real_type,
                                                   ASN1_Class real_class,
                                                   const T& default_value) {
   BER_Object obj = get_next_object();

   if(obj.is_a(type_tag, class_tag)) {
      obj.set_tagging(real_type, real_class);
      push_back(std::move(obj));
      decode(out, real_type, real_class);
   } else {
      // Not what we wanted, push it back on the stream
      out = default_value;
      push_back(std::move(obj));
   }

   return (*this);
}

/*
* Decode a list of homogeneously typed values
*/
template <typename T>
BER_Decoder& BER_Decoder::decode_list(std::vector<T>& vec, ASN1_Type type_tag, ASN1_Class class_tag) {
   BER_Decoder list = start_cons(type_tag, class_tag);

   while(list.more_items()) {
      T value;
      list.decode(value);
      vec.push_back(std::move(value));
   }

   list.end_cons();

   return (*this);
}

/*
* Decode an optional list of homogeneously typed values
*/
template <typename T>
bool BER_Decoder::decode_optional_list(std::vector<T>& vec, ASN1_Type type_tag, ASN1_Class class_tag) {
   if(peek_next_object().is_a(type_tag, class_tag)) {
      decode_list(vec, type_tag, class_tag);
      return true;
   }

   return false;
}

}  // namespace Botan

namespace Botan {

class RandomNumberGenerator;

/**
 * Arbitrary precision integer
 */
class BOTAN_PUBLIC_API(2, 0) BigInt final {
   public:
      /**
       * Base enumerator for encoding and decoding
       */
      enum Base : uint16_t /* NOLINT(*-use-enum-class) */ {
         Decimal BOTAN_DEPRECATED("All functions using this enum are deprecated") = 10,
         Hexadecimal BOTAN_DEPRECATED("All functions using this enum are deprecated") = 16,
         Binary BOTAN_DEPRECATED("All functions using this enum are deprecated") = 256
      };

      /**
       * Sign symbol definitions for positive and negative numbers
       */
      enum Sign : uint8_t /* NOLINT(*-use-enum-class) */ { Negative = 0, Positive = 1 };

      /**
       * Create empty (zero) BigInt
       */
      BigInt() = default;

      /**
       * Create a 0-value BigInt
       */
      static BigInt zero() { return BigInt(); }

      /**
       * Create a 1-value BigInt
       */
      static BigInt one() { return BigInt::from_u64(1); }

      /**
       * Create BigInt from an unsigned 64 bit integer
       * @param n initial value of this BigInt
       */
      static BigInt from_u64(uint64_t n);

      /**
       * Create BigInt from a word (limb)
       * @param n initial value of this BigInt
       */
      //BOTAN_DEPRECATED("Use BigInt::from_u64 instead")
      static BigInt from_word(word n);

      /**
       * Create BigInt from a signed 32 bit integer
       * @param n initial value of this BigInt
       */
      BOTAN_DEPRECATED("Use BigInt::from_u64 plus negation if required instead") static BigInt from_s32(int32_t n);

      /**
       * Create BigInt from an unsigned 64 bit integer
       * @param n initial value of this BigInt
       *
       * Prefer BigInt::from_u64
       */
      BigInt(uint64_t n);  // NOLINT(*-explicit-conversions) TODO(Botan4) make this explicit

      /**
       * Copy Constructor
       * @param other the BigInt to copy
       */
      BigInt(const BigInt& other) = default;

      /**
       * Create BigInt from a string. If the string starts with 0x the
       * rest of the string will be interpreted as hexadecimal digits.
       * Otherwise, it will be interpreted as a decimal number.
       *
       * @param str the string to parse for an integer value
       */
      //BOTAN_DEPRECATED("Use BigInt::from_string")
      explicit BigInt(std::string_view str);

      /**
       * Create BigInt from a string.
       *
       * If the string starts with 0x the rest of the string will be
       * interpreted as hexadecimal digits. Otherwise, it will be
       * interpreted as a decimal number.
       *
       * A prefix of "-" will result in a negative integer
       *
       * @param str the string to parse for an integer value
       */
      static BigInt from_string(std::string_view str);

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the byte array holding the value
       * @param length size of buf
       */
      BigInt(const uint8_t buf[], size_t length) { assign_from_bytes(std::span{buf, length}); }

      /**
       * Create a BigInt from an integer in a byte array
       * @param bytes the byte vector holding the value
       */
      explicit BigInt(std::span<const uint8_t> bytes) { assign_from_bytes(bytes); }

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the byte array holding the value
       * @param length size of buf
       * @param base is the number base of the integer in buf
       */
      BOTAN_DEPRECATED("For hex/decimal use from_string") BigInt(const uint8_t buf[], size_t length, Base base);

      /**
       * Create a BigInt from an integer in a byte array
       *
       * Note this function is primarily used for implementing signature
       * schemes and is not useful in typical applications.
       *
       * @param buf the byte array holding the value
       * @param length size of buf
       * @param max_bits if the resulting integer is more than max_bits,
       *        it will be shifted so it is at most max_bits in length.
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      static BigInt from_bytes_with_max_bits(const uint8_t buf[], size_t length, size_t max_bits);

      /**
       * @brief Create a random BigInt of the specified size
       *
       * @param rng random number generator
       * @param bits size in bits
       * @param set_high_bit if true, the highest bit is always set
       *
       * @see randomize
       */
      BigInt(RandomNumberGenerator& rng, size_t bits, bool set_high_bit = true);

      /**
       * Create BigInt of specified size, all zeros
       * @param n size of the internal register in words
       */
      BOTAN_DEPRECATED("Deprecated no replacement") static BigInt with_capacity(size_t n);

      /**
       * Move constructor
       */
      BigInt(BigInt&& other) noexcept { this->swap(other); }

      ~BigInt() { _const_time_unpoison(); }

      /**
       * Move assignment
       */
      BigInt& operator=(BigInt&& other) noexcept {
         if(this != &other) {
            this->swap(other);
         }

         return (*this);
      }

      /**
       * Copy assignment
       */
      BigInt& operator=(const BigInt&) = default;

      /**
       * Swap this value with another
       * @param other BigInt to swap values with
       */
      void swap(BigInt& other) noexcept {
         m_data.swap(other.m_data);
         std::swap(m_signedness, other.m_signedness);
      }

      friend void swap(BigInt& x, BigInt& y) noexcept { x.swap(y); }

      BOTAN_DEPRECATED("Deprecated no replacement") void swap_reg(secure_vector<word>& reg) {
         m_data.swap(reg);
         // sign left unchanged
      }

      /**
       * += operator
       * @param y the BigInt to add to this
       */
      BigInt& operator+=(const BigInt& y) { return add(y._data(), y.sig_words(), y.sign()); }

      /**
       * += operator
       * @param y the word to add to this
       */
      BigInt& operator+=(word y) { return add(&y, 1, Positive); }

      /**
       * -= operator
       * @param y the BigInt to subtract from this
       */
      BigInt& operator-=(const BigInt& y) { return sub(y._data(), y.sig_words(), y.sign()); }

      /**
       * -= operator
       * @param y the word to subtract from this
       */
      BigInt& operator-=(word y) { return sub(&y, 1, Positive); }

      /**
       * *= operator
       * @param y the BigInt to multiply with this
       */
      BigInt& operator*=(const BigInt& y);

      /**
       * *= operator
       * @param y the word to multiply with this
       */
      BigInt& operator*=(word y);

      /**
       * /= operator
       * @param y the BigInt to divide this by
       */
      BigInt& operator/=(const BigInt& y);

      /**
       * Modulo operator
       * @param y the modulus to reduce this by
       */
      BigInt& operator%=(const BigInt& y);

      /**
       * Modulo operator
       * @param y the modulus (word) to reduce this by
       */
      word operator%=(word y);

      /**
       * Left shift operator
       * @param shift the number of bits to shift this left by
       */
      BigInt& operator<<=(size_t shift);

      /**
       * Right shift operator
       * @param shift the number of bits to shift this right by
       */
      BigInt& operator>>=(size_t shift);

      /**
       * Increment operator
       */
      BigInt& operator++() { return (*this += 1); }

      /**
       * Decrement operator
       */
      BigInt& operator--() { return (*this -= 1); }

      /**
       * Postfix increment operator
       */
      BigInt operator++(int) {
         BigInt x = (*this);
         ++(*this);
         return x;
      }

      /**
       * Postfix decrement operator
       */
      BigInt operator--(int) {
         BigInt x = (*this);
         --(*this);
         return x;
      }

      /**
       * Unary negation operator
       * @return negative this
       */
      BigInt operator-() const;

      /**
       * ! operator
       * @return true iff this is zero, otherwise false
       */
      bool operator!() const { return (!is_nonzero()); }

      //BOTAN_DEPRECATED("Just use operator+/operator-")
      static BigInt add2(const BigInt& x, const word y[], size_t y_words, Sign y_sign);

      //BOTAN_DEPRECATED("Just use operator+/operator-")
      BigInt& add(const word y[], size_t y_words, Sign sign);

      //BOTAN_DEPRECATED("Just use operator+/operator-")
      BigInt& sub(const word y[], size_t y_words, Sign sign) {
         return add(y, y_words, sign == Positive ? Negative : Positive);
      }

      /**
       * Multiply this with y
       * @param y the BigInt to multiply with this
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Just use operator*") BigInt& mul(const BigInt& y, secure_vector<word>& ws);

      /**
       * Square value of *this
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Deprecated no replacement") BigInt& square(secure_vector<word>& ws);

      /**
       * Set *this to y - *this
       * @param y the BigInt to subtract from as a sequence of words
       * @param y_words length of y in words
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      BigInt& rev_sub(const word y[], size_t y_words, secure_vector<word>& ws);

      /**
       * Set *this to (*this + y) % mod
       * This function assumes *this is >= 0 && < mod
       * @param y the BigInt to add - assumed y >= 0 and y < mod
       * @param mod the positive modulus
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      BigInt& mod_add(const BigInt& y, const BigInt& mod, secure_vector<word>& ws);

      /**
       * Set *this to (*this - y) % mod
       * This function assumes *this is >= 0 && < mod
       * @param y the BigInt to subtract - assumed y >= 0 and y < mod
       * @param mod the positive modulus
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      BigInt& mod_sub(const BigInt& y, const BigInt& mod, secure_vector<word>& ws);

      /**
       * Set *this to (*this * y) % mod
       * This function assumes *this is >= 0 && < mod
       * y should be small, less than 16
       * @param y the small integer to multiply by
       * @param mod the positive modulus
       * @param ws a temp workspace
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      BigInt& mod_mul(uint8_t y, const BigInt& mod, secure_vector<word>& ws);

      /**
       * Return *this % mod
       *
       * Assumes that *this is (if anything) only slightly larger than
       * mod and performs repeated subtractions. It should not be used if
       * *this is much larger than mod, instead use modulo operator.
       */
      BOTAN_DEPRECATED("Deprecated no replacement") size_t reduce_below(const BigInt& mod, secure_vector<word>& ws);

      /**
       * Return *this % mod
       *
       * Assumes that *this is (if anything) only slightly larger than mod and
       * performs repeated subtractions. It should not be used if *this is much
       * larger than mod, instead use modulo operator.
       *
       * Performs exactly bound subtractions, so if *this is >= bound*mod then the
       * result will not be fully reduced. If bound is zero, nothing happens.
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      void ct_reduce_below(const BigInt& mod, secure_vector<word>& ws, size_t bound);

      /**
       * Zeroize the BigInt. The size of the underlying register is not
       * modified.
       */
      void clear() {
         m_data.set_to_zero();
         m_signedness = Positive;
      }

      /**
       * Compare this to another BigInt
       * @param n the BigInt value to compare with
       * @param check_signs include sign in comparison?
       * @result if (this<n) return -1, if (this>n) return 1, if both
       * values are identical return 0 [like Perl's <=> operator]
       */
      int32_t cmp(const BigInt& n, bool check_signs = true) const;

      /**
       * Compare this to another BigInt
       * @param n the BigInt value to compare with
       * @result true if this == n or false otherwise
       */
      bool is_equal(const BigInt& n) const;

      /**
       * Compare this to another BigInt
       * @param n the BigInt value to compare with
       * @result true if this < n or false otherwise
       */
      bool is_less_than(const BigInt& n) const;

      /**
       * Compare this to an integer
       * @param n the value to compare with
       * @result if (this<n) return -1, if (this>n) return 1, if both
       * values are identical return 0 [like Perl's <=> operator]
       */
      int32_t cmp_word(word n) const;

      /**
       * Test if the integer has an even value
       * @result true if the integer is even, false otherwise
       */
      bool is_even() const { return !get_bit(0); }

      /**
       * Test if the integer has an odd value
       * @result true if the integer is odd, false otherwise
       */
      bool is_odd() const { return get_bit(0); }

      /**
       * Test if the integer is not zero
       * @result true if the integer is non-zero, false otherwise
       */
      bool is_nonzero() const { return (!is_zero()); }

      /**
       * Test if the integer is zero
       * @result true if the integer is zero, false otherwise
       */
      bool is_zero() const { return (sig_words() == 0); }

      /**
       * Set bit at specified position
       * @param n bit position to set
       */
      void set_bit(size_t n) { conditionally_set_bit(n, true); }

      /**
       * Conditionally set bit at specified position. Note if set_it is
       * false, nothing happens, and if the bit is already set, it
       * remains set.
       *
       * @param n bit position to set
       * @param set_it if the bit should be set
       */
      void conditionally_set_bit(size_t n, bool set_it) {
         const size_t which = n / (sizeof(word) * 8);
         const word mask = static_cast<word>(set_it) << (n % (sizeof(word) * 8));
         m_data.set_word_at(which, word_at(which) | mask);
      }

      /**
       * Clear bit at specified position
       * @param n bit position to clear
       */
      void clear_bit(size_t n);

      /**
       * Clear all but the lowest n bits
       * @param n amount of bits to keep
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void mask_bits(size_t n) { m_data.mask_bits(n); }

      /**
       * Return bit value at specified position
       * @param n the bit offset to test
       * @result true, if the bit at position n is set, false otherwise
       */
      bool get_bit(size_t n) const { return ((word_at(n / (sizeof(word) * 8)) >> (n % (sizeof(word) * 8))) & 1) == 1; }

      /**
       * Return (a maximum of) 32 bits of the complete value
       * @param offset the offset to start extracting
       * @param length amount of bits to extract (starting at offset)
       * @result the integer extracted from the register starting at
       * offset with specified length
       */
      BOTAN_DEPRECATED("Deprecated no replacement") uint32_t get_substring(size_t offset, size_t length) const;

      /**
       * Convert this value into a uint32_t, if it is in the range
       * [0 ... 2**32-1], or otherwise throw an exception.
       * @result the value as a uint32_t if conversion is possible
       */
      BOTAN_DEPRECATED("Deprecated no replacement") uint32_t to_u32bit() const;

      /**
       * Convert this value to a decimal string.
       * Warning: decimal conversions are relatively slow
       *
       * If the integer is zero then "0" is returned.
       * If the integer is negative then "-" is prefixed.
       */
      std::string to_dec_string() const;

      /**
       * Convert this value to a hexadecimal string.
       *
       * If the integer is negative then "-" is prefixed.
       * Then a prefix of "0x" is added.
       * Follows is a sequence of hexadecimal characters in uppercase.
       *
       * The number of hexadecimal characters is always an even number,
       * with a zero prefix being included if necessary.
       * For example encoding the integer "5" results in "0x05"
       */
      std::string to_hex_string() const;

      /**
       * @param n the offset to get a byte from
       * @result byte at offset n
       */
      uint8_t byte_at(size_t n) const;

      /**
       * Return the word at a specified position of the internal register
       * @param n position in the register
       * @return value at position n
       */
      word word_at(size_t n) const { return m_data.get_word_at(n); }

      BOTAN_DEPRECATED("Deprecated no replacement") void set_word_at(size_t i, word w) { m_data.set_word_at(i, w); }

      BOTAN_DEPRECATED("Deprecated no replacement") void set_words(const word w[], size_t len) {
         m_data.set_words(w, len);
      }

      /**
       * Tests if the sign of the integer is negative
       * @result true, iff the integer has a negative sign
       */
      bool is_negative() const { return (sign() == Negative); }

      /**
       * Tests if the sign of the integer is positive
       * @result true, iff the integer has a positive sign
       */
      bool is_positive() const { return (sign() == Positive); }

      /**
       * Return the sign of the integer
       * @result the sign of the integer
       */
      Sign sign() const { return (m_signedness); }

      /**
       * @result the opposite sign of the represented integer value
       */
      Sign reverse_sign() const {
         if(sign() == Positive) {
            return Negative;
         }
         return Positive;
      }

      /**
       * Flip the sign of this BigInt
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void flip_sign() { set_sign(reverse_sign()); }

      /**
       * Set sign of the integer
       * @param sign new Sign to set
       */
      void set_sign(Sign sign) {
         if(sign == Negative && is_zero()) {
            sign = Positive;
         }

         m_signedness = sign;
      }

      /**
       * @result absolute (positive) value of this
       */
      BigInt abs() const;

      /**
       * Give size of internal register
       * @result size of internal register in words
       */
      size_t size() const { return m_data.size(); }

      /**
       * Return how many words we need to hold this value
       * @result significant words of the represented integer value
       */
      size_t sig_words() const { return m_data.sig_words(); }

      /**
       * Give byte length of the integer
       * @result byte length of the represented integer value
       */
      size_t bytes() const;

      /**
       * Get the bit length of the integer
       * @result bit length of the represented integer value
       */
      size_t bits() const;

      /**
       * Get the number of high bits unset in the top (allocated) word
       * of this integer. Returns (sizeof(word) * 8) only iff *this is
       * zero. Ignores sign.
       */
      BOTAN_DEPRECATED("Deprecated no replacement") size_t top_bits_free() const;

      /**
       * Return a mutable pointer to the register
       * @result a pointer to the start of the internal register
       */
      BOTAN_DEPRECATED("Deprecated no replacement") word* mutable_data() { return m_data.mutable_data(); }

      /**
       * Return a const pointer to the register
       * @result a pointer to the start of the internal register
       */
      BOTAN_DEPRECATED("Deprecated no replacement") const word* data() const { return m_data.const_data(); }

      /**
       * Don't use this function in application code
       */
      BOTAN_DEPRECATED("Deprecated no replacement") secure_vector<word>& get_word_vector() {
         return m_data.mutable_vector();
      }

      /**
       * Don't use this function in application code
       */
      BOTAN_DEPRECATED("Deprecated no replacement") const secure_vector<word>& get_word_vector() const {
         return m_data.const_vector();
      }

      /**
       * Increase internal register buffer to at least n words
       * @param n new size of register
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void grow_to(size_t n) const { m_data.grow_to(n); }

      BOTAN_DEPRECATED("Deprecated no replacement") void resize(size_t s) { m_data.resize(s); }

      /**
       * Fill BigInt with a random number with size of bitsize
       *
       * If \p set_high_bit is true, the highest bit will be set, which causes
       * the entropy to be \a bits-1. Otherwise the highest bit is randomly chosen
       * by the rng, causing the entropy to be \a bits.
       *
       * @param rng the random number generator to use
       * @param bitsize number of bits the created random value should have
       * @param set_high_bit if true, the highest bit is always set
       */
      void randomize(RandomNumberGenerator& rng, size_t bitsize, bool set_high_bit = true);

      /**
       * Serialize the absolute value of this BigInt as a big endian
       * encoding.
       *
       * If out is smaller than the total bytes of the BigInt then
       * an exception is thrown.
       *
       * If out is larger than the total bytes of the BigInt then the
       * necessary number of zeros are prefixed to produce the desired
       * output length
       *
       * Zero-padding the binary encoding is useful to ensure that other
       * applications correctly parse the encoded value as "positive integer",
       * as a leading 1-bit may be interpreted as a sign bit. It also is
       * necessary when using a fixed size encoding for the integers.
       *
       * @param out destination byte span for the integer value
       */
      void serialize_to(std::span<uint8_t> out) const;

      /**
       * Serialize the value of this BigInt as a big endian encoding,
       * always returning the specified number of bytes.
       *
       * Throws if the BigInt is too large to encode in the length
       * specified.
       */
      template <typename T = std::vector<uint8_t>>
      T serialize(size_t len) const {
         // TODO this supports std::vector and secure_vector
         // it would be nice if this also could work with std::array as in
         //   bn.serialize_to<std::array<uint8_t, 32>>(32);
         T out(len);
         this->serialize_to(out);
         return out;
      }

      /**
       * Serialize the value of this BigInt as a big endian encoding.
       */
      template <typename T = std::vector<uint8_t>>
      T serialize() const {
         return serialize<T>(this->bytes());
      }

      /**
       * Store BigInt-value in a given byte array
       * @param buf destination byte array for the integer value
       */
      BOTAN_DEPRECATED("Use BigInt::serialize_to") void binary_encode(uint8_t buf[]) const {
         this->serialize_to(std::span{buf, this->bytes()});
      }

      /**
       * Store BigInt-value in a given byte array. If len is less than
       * the size of the value, then it will be truncated. If len is
       * greater than the size of the value, it will be zero-padded.
       * If len exactly equals this->bytes(), this function behaves identically
       * to binary_encode.
       *
       * Zero-padding the binary encoding is useful to ensure that other
       * applications correctly parse the encoded value as "positive integer",
       * as a leading 1-bit may be interpreted as a sign bit.
       *
       * @param buf destination byte array for the integer value
       * @param len how many bytes to write
       */
      BOTAN_DEPRECATED("Use BigInt::serialize_to") void binary_encode(uint8_t buf[], size_t len) const;

      /**
       * Read integer value from a byte array with given size
       * @param buf byte array buffer containing the integer
       * @param length size of buf
       */
      BOTAN_DEPRECATED("Use BigInt::from_bytes") void binary_decode(const uint8_t buf[], size_t length) {
         this->assign_from_bytes(std::span{buf, length});
      }

      /**
       * Read integer value from a byte vector
       * @param buf the vector to load from
       */
      BOTAN_DEPRECATED("Use BigInt::from_bytes") void binary_decode(std::span<const uint8_t> buf) {
         this->assign_from_bytes(buf);
      }

      /**
       * Place the value into out, zero-padding up to size words
       * Throw if *this cannot be represented in size words
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void encode_words(word out[], size_t size) const;

      /**
       * If predicate is true assign other to *this
       * Uses a masked operation to avoid side channels
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void ct_cond_assign(bool predicate, const BigInt& other);

      /**
       * If predicate is true swap *this and other
       * Uses a masked operation to avoid side channels
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void ct_cond_swap(bool predicate, BigInt& other);

      /**
       * If predicate is true add value to *this
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void ct_cond_add(bool predicate, const BigInt& value);

      /**
       * Shift @p shift bits to the left, runtime is independent of
       * the value of @p shift.
       */
      BOTAN_DEPRECATED("Deprecated no replacement") void ct_shift_left(size_t shift);

      /**
       * If predicate is true flip the sign of *this
       */
      void cond_flip_sign(bool predicate);

      BOTAN_DEPRECATED("replaced by internal API") void const_time_poison() const { _const_time_poison(); }

      BOTAN_DEPRECATED("replaced by internal API") void const_time_unpoison() const { _const_time_unpoison(); }

      /**
       * @param rng a random number generator
       * @param min the minimum value (must be non-negative)
       * @param max the maximum value (must be non-negative and > min)
       * @return random integer in [min,max)
       */
      static BigInt random_integer(RandomNumberGenerator& rng, const BigInt& min, const BigInt& max);

      /**
       * Create a power of two
       * @param n the power of two to create
       * @return bigint representing 2^n
       */
      static BigInt power_of_2(size_t n) {
         BigInt b;
         b.set_bit(n);
         return b;
      }

      /**
       * Encode the integer value from a BigInt to a std::vector of bytes
       * @param n the BigInt to use as integer source
       * @result secure_vector of bytes containing the bytes of the integer
       */
      BOTAN_DEPRECATED("Use BigInt::serialize") static std::vector<uint8_t> encode(const BigInt& n) {
         return n.serialize<std::vector<uint8_t>>(n.bytes());
      }

      /**
       * Encode the integer value from a BigInt to a secure_vector of bytes
       * @param n the BigInt to use as integer source
       * @result secure_vector of bytes containing the bytes of the integer
       */
      BOTAN_DEPRECATED("Use BigInt::serialize") static secure_vector<uint8_t> encode_locked(const BigInt& n) {
         return n.serialize<secure_vector<uint8_t>>(n.bytes());
      }

      /**
       * Create a BigInt from an integer in a byte array
       * @param bytes the binary value to load
       * @result BigInt representing the integer in the byte array
       */
      static BigInt from_bytes(std::span<const uint8_t> bytes);

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the binary value to load
       * @param length size of buf
       * @result BigInt representing the integer in the byte array
       */
      BOTAN_DEPRECATED("Use BigInt::from_bytes") static BigInt decode(const uint8_t buf[], size_t length) {
         return BigInt::from_bytes(std::span{buf, length});
      }

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the binary value to load
       * @result BigInt representing the integer in the byte array
       */
      BOTAN_DEPRECATED("Use BigInt::from_bytes") static BigInt decode(std::span<const uint8_t> buf) {
         return BigInt::from_bytes(buf);
      }

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the binary value to load
       * @param length size of buf
       * @param base number-base of the integer in buf
       * @result BigInt representing the integer in the byte array
       */
      BOTAN_DEPRECATED("For decimal/hex use from_string")
      static BigInt decode(const uint8_t buf[], size_t length, Base base);

      /**
       * Create a BigInt from an integer in a byte array
       * @param buf the binary value to load
       * @param base number-base of the integer in buf
       * @result BigInt representing the integer in the byte array
       */
      BOTAN_DEPRECATED("For decimal/hex use from_string") static BigInt decode(std::span<const uint8_t> buf, Base base);

      /**
       * Encode a BigInt to a byte array according to IEEE 1363
       * @param n the BigInt to encode
       * @param bytes the length of the resulting secure_vector<uint8_t>
       * @result a secure_vector<uint8_t> containing the encoded BigInt
       */
      BOTAN_DEPRECATED("Use BigInt::serialize")
      static secure_vector<uint8_t> encode_1363(const BigInt& n, size_t bytes) {
         return n.serialize<secure_vector<uint8_t>>(bytes);
      }

      BOTAN_DEPRECATED("Use BigInt::serialize_to") static void encode_1363(std::span<uint8_t> out, const BigInt& n) {
         n.serialize_to(out);
      }

      BOTAN_DEPRECATED("Use BigInt::serialize_to")
      static void encode_1363(uint8_t out[], size_t bytes, const BigInt& n) {
         n.serialize_to(std::span{out, bytes});
      }

      /**
       * Encode two BigInt to a byte array according to IEEE 1363
       * @param n1 the first BigInt to encode
       * @param n2 the second BigInt to encode
       * @param bytes the length of the encoding of each single BigInt
       * @result a secure_vector<uint8_t> containing the concatenation of the two encoded BigInt
       */
      BOTAN_DEPRECATED("Deprecated no replacement")
      static secure_vector<uint8_t> encode_fixed_length_int_pair(const BigInt& n1, const BigInt& n2, size_t bytes);

      /**
       * Return a span over the register
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer.
       *
       * @result span over the internal register
       */
      std::span<const word> _as_span() const { return m_data.const_span(); }

      /**
       * Return a const pointer to the register
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer.
       *
       * @result a pointer to the start of the internal register
       */
      const word* _data() const { return m_data.const_data(); }

      /**
       * Read integer value from a byte vector (big endian)
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer. In applications use
       * BigInt::from_bytes
       *
       * @param bytes the span of bytes to load
       */
      void _assign_from_bytes(std::span<const uint8_t> bytes) { assign_from_bytes(bytes); }

      /**
       * Create a BigInt from a word vector
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer.
       */
      static BigInt _from_words(secure_vector<word>& words) {
         BigInt bn;
         bn.m_data.swap(words);
         return bn;
      }

      /**
       * Mark this BigInt as holding secret data
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer.
       */
      void _const_time_poison() const;

      /**
       * Mark this BigInt as no longer holding secret data
       *
       * @warning this is an implementation detail which is not for
       * public use and not covered by SemVer.
       */
      void _const_time_unpoison() const;

   private:
      /**
       * Read integer value from a byte vector (big endian)
       * @param bytes the span of bytes to load
       */
      void assign_from_bytes(std::span<const uint8_t> bytes);

      class Data {
         public:
            word* mutable_data() {
               invalidate_sig_words();
               return m_reg.data();
            }

            const word* const_data() const { return m_reg.data(); }

            std::span<const word> const_span() const { return std::span{m_reg}; }

            secure_vector<word>& mutable_vector() {
               invalidate_sig_words();
               return m_reg;
            }

            const secure_vector<word>& const_vector() const { return m_reg; }

            word get_word_at(size_t n) const {
               if(n < m_reg.size()) {
                  return m_reg[n];
               }
               return 0;
            }

            void set_word_at(size_t i, word w) {
               invalidate_sig_words();
               if(i >= m_reg.size()) {
                  if(w == 0) {
                     return;
                  }
                  grow_to(i + 1);
               }
               m_reg[i] = w;
            }

            void set_words(const word w[], size_t len) {
               invalidate_sig_words();
               m_reg.assign(w, w + len);
            }

            void set_to_zero();

            void mask_bits(size_t n);

            void grow_to(size_t n) const {
               if(n > size()) {
                  if(n <= m_reg.capacity()) {
                     m_reg.resize(n);
                  } else {
                     m_reg.resize(n + (8 - (n % 8)));
                  }
               }
            }

            size_t size() const { return m_reg.size(); }

            void shrink_to_fit(size_t min_size = 0) {
               const size_t words = std::max(min_size, sig_words());
               m_reg.resize(words);
            }

            void resize(size_t s) { m_reg.resize(s); }

            void swap(Data& other) noexcept {
               m_reg.swap(other.m_reg);
               std::swap(m_sig_words, other.m_sig_words);
            }

            void swap(secure_vector<word>& reg) noexcept {
               m_reg.swap(reg);
               invalidate_sig_words();
            }

            void invalidate_sig_words() const noexcept { m_sig_words = sig_words_npos; }

            size_t sig_words() const {
               if(m_sig_words == sig_words_npos) {
                  m_sig_words = calc_sig_words();
               }
               return m_sig_words;
            }

         private:
            static const size_t sig_words_npos = static_cast<size_t>(-1);

            size_t calc_sig_words() const;

            mutable secure_vector<word> m_reg;
            mutable size_t m_sig_words = sig_words_npos;
      };

      Data m_data;
      Sign m_signedness = Positive;
};

/*
* Arithmetic Operators
*/
inline BigInt operator+(const BigInt& x, const BigInt& y) {
   return BigInt::add2(x, y._data(), y.sig_words(), y.sign());
}

inline BigInt operator+(const BigInt& x, word y) {
   return BigInt::add2(x, &y, 1, BigInt::Positive);
}

inline BigInt operator+(word x, const BigInt& y) {
   return y + x;
}

inline BigInt operator-(const BigInt& x, const BigInt& y) {
   return BigInt::add2(x, y._data(), y.sig_words(), y.reverse_sign());
}

inline BigInt operator-(const BigInt& x, word y) {
   return BigInt::add2(x, &y, 1, BigInt::Negative);
}

BOTAN_PUBLIC_API(2, 0) BigInt operator*(const BigInt& x, const BigInt& y);
BOTAN_PUBLIC_API(2, 8) BigInt operator*(const BigInt& x, word y);

inline BigInt operator*(word x, const BigInt& y) {
   return y * x;
}

BOTAN_PUBLIC_API(2, 0) BigInt operator/(const BigInt& x, const BigInt& d);
BOTAN_PUBLIC_API(2, 0) BigInt operator/(const BigInt& x, word m);
BOTAN_PUBLIC_API(2, 0) BigInt operator%(const BigInt& x, const BigInt& m);
BOTAN_PUBLIC_API(2, 0) word operator%(const BigInt& x, word m);
BOTAN_PUBLIC_API(2, 0) BigInt operator<<(const BigInt& x, size_t n);
BOTAN_PUBLIC_API(2, 0) BigInt operator>>(const BigInt& x, size_t n);

/*
 * Comparison Operators
 */
inline bool operator==(const BigInt& a, const BigInt& b) {
   return a.is_equal(b);
}

inline bool operator!=(const BigInt& a, const BigInt& b) {
   return !a.is_equal(b);
}

inline bool operator<=(const BigInt& a, const BigInt& b) {
   return (a.cmp(b) <= 0);
}

inline bool operator>=(const BigInt& a, const BigInt& b) {
   return (a.cmp(b) >= 0);
}

inline bool operator<(const BigInt& a, const BigInt& b) {
   return a.is_less_than(b);
}

inline bool operator>(const BigInt& a, const BigInt& b) {
   return b.is_less_than(a);
}

inline bool operator==(const BigInt& a, word b) {
   return (a.cmp_word(b) == 0);
}

inline bool operator!=(const BigInt& a, word b) {
   return (a.cmp_word(b) != 0);
}

inline bool operator<=(const BigInt& a, word b) {
   return (a.cmp_word(b) <= 0);
}

inline bool operator>=(const BigInt& a, word b) {
   return (a.cmp_word(b) >= 0);
}

inline bool operator<(const BigInt& a, word b) {
   return (a.cmp_word(b) < 0);
}

inline bool operator>(const BigInt& a, word b) {
   return (a.cmp_word(b) > 0);
}

/*
 * I/O Operators
 */
BOTAN_DEPRECATED("Use BigInt::to_{hex,dec}_string")
BOTAN_PUBLIC_API(2, 0) std::ostream& operator<<(std::ostream& stream, const BigInt& n);

BOTAN_DEPRECATED("Use BigInt::from_string")
BOTAN_PUBLIC_API(2, 0) std::istream& operator>>(std::istream& stream, BigInt& n);

}  // namespace Botan


namespace Botan {

class OctetString;

/**
* Represents the length requirements on an algorithm key
*/
class BOTAN_PUBLIC_API(2, 0) Key_Length_Specification final {
   public:
      /**
      * Constructor for fixed length keys
      * @param keylen the supported key length
      */
      explicit Key_Length_Specification(size_t keylen) : m_min_keylen(keylen), m_max_keylen(keylen), m_keylen_mod(1) {}

      /**
      * Constructor for variable length keys
      * @param min_k the smallest supported key length
      * @param max_k the largest supported key length
      * @param k_mod the number of bytes the key must be a multiple of
      */
      Key_Length_Specification(size_t min_k, size_t max_k, size_t k_mod = 1) :
            m_min_keylen(min_k), m_max_keylen(max_k > 0 ? max_k : min_k), m_keylen_mod(k_mod) {}

      /**
      * @param length is a key length in bytes
      * @return true iff this length is a valid length for this algo
      */
      bool valid_keylength(size_t length) const {
         return ((length >= m_min_keylen) && (length <= m_max_keylen) && (length % m_keylen_mod == 0));
      }

      /**
      * @return minimum key length in bytes
      */
      size_t minimum_keylength() const { return m_min_keylen; }

      /**
      * @return maximum key length in bytes
      */
      size_t maximum_keylength() const { return m_max_keylen; }

      /**
      * @return key length multiple in bytes
      */
      size_t keylength_multiple() const { return m_keylen_mod; }

      /*
      * Multiplies all length requirements with the given factor
      * @param n the multiplication factor
      * @return a key length specification multiplied by the factor
      */
      Key_Length_Specification multiple(size_t n) const {
         return Key_Length_Specification(n * m_min_keylen, n * m_max_keylen, n * m_keylen_mod);
      }

   private:
      size_t m_min_keylen, m_max_keylen, m_keylen_mod;
};

/**
* This class represents a symmetric algorithm object.
*/
class BOTAN_PUBLIC_API(2, 0) SymmetricAlgorithm {
   public:
      SymmetricAlgorithm() = default;
      virtual ~SymmetricAlgorithm() = default;
      SymmetricAlgorithm(const SymmetricAlgorithm& other) = default;
      SymmetricAlgorithm(SymmetricAlgorithm&& other) = default;
      SymmetricAlgorithm& operator=(const SymmetricAlgorithm& other) = default;
      SymmetricAlgorithm& operator=(SymmetricAlgorithm&& other) = default;

      /**
      * Reset the internal state. This includes not just the key, but
      * any partial message that may have been in process.
      */
      virtual void clear() = 0;

      /**
      * @return object describing limits on key size
      */
      virtual Key_Length_Specification key_spec() const = 0;

      /**
      * @return maximum allowed key length
      */
      size_t maximum_keylength() const { return key_spec().maximum_keylength(); }

      /**
      * @return minimum allowed key length
      */
      size_t minimum_keylength() const { return key_spec().minimum_keylength(); }

      /**
      * Check whether a given key length is valid for this algorithm.
      * @param length the key length to be checked.
      * @return true if the key length is valid.
      */
      bool valid_keylength(size_t length) const { return key_spec().valid_keylength(length); }

      /**
      * Set the symmetric key of this object.
      * @param key the SymmetricKey to be set.
      */
      void set_key(const OctetString& key);

      /**
      * Set the symmetric key of this object.
      * @param key the contiguous byte range to be set.
      */
      void set_key(std::span<const uint8_t> key);

      /**
      * Set the symmetric key of this object.
      * @param key the to be set as a byte array.
      * @param length in bytes of key param
      */
      void set_key(const uint8_t key[], size_t length) { set_key(std::span{key, length}); }

      /**
      * @return the algorithm name
      */
      virtual std::string name() const = 0;

      /**
      * @return true if a key has been set on this object
      */
      virtual bool has_keying_material() const = 0;

   protected:
      void assert_key_material_set() const { assert_key_material_set(has_keying_material()); }

      void assert_key_material_set(bool predicate) const {
         if(!predicate) {
            throw_key_not_set_error();
         }
      }

   private:
      void throw_key_not_set_error() const;

      /**
      * Run the key schedule
      * @param key the key
      */
      virtual void key_schedule(std::span<const uint8_t> key) = 0;
};

}  // namespace Botan

namespace Botan {

/**
* This class represents a block cipher object.
*/
class BOTAN_PUBLIC_API(2, 0) BlockCipher : public SymmetricAlgorithm {
   public:
      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to choose
      * @return a null pointer if the algo/provider combination cannot be found
      */
      static std::unique_ptr<BlockCipher> create(std::string_view algo_spec, std::string_view provider = "");

      /**
      * Create an instance based on a name, or throw if the
      * algo/provider combination cannot be found. If provider is
      * empty then best available is chosen.
      */
      static std::unique_ptr<BlockCipher> create_or_throw(std::string_view algo_spec, std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      * @param algo_spec algorithm name
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * Multiplier on a block cipher's native parallelism
      *
      * Usually notable performance gains come from further loop blocking,
      * at least for 2 or 4x
      */
      static constexpr size_t ParallelismMult = 4;

      /**
      * @return block size of this algorithm
      */
      virtual size_t block_size() const = 0;

      /**
      * @return native parallelism of this cipher in blocks
      */
      virtual size_t parallelism() const { return 1; }

      /**
      * @return preferred parallelism of this cipher in bytes
      */
      size_t parallel_bytes() const { return parallelism() * block_size() * BlockCipher::ParallelismMult; }

      /**
      * @return provider information about this implementation. Default is "base",
      * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
      */
      virtual std::string provider() const { return "base"; }

      /**
      * Encrypt a block.
      * @param in The plaintext block to be encrypted as a byte array.
      * Must be of length block_size().
      * @param out The byte array designated to hold the encrypted block.
      * Must be of length block_size().
      */
      void encrypt(const uint8_t in[], uint8_t out[]) const { encrypt_n(in, out, 1); }

      /**
      * Decrypt a block.
      * @param in The ciphertext block to be decrypted as a byte array.
      * Must be of length block_size().
      * @param out The byte array designated to hold the decrypted block.
      * Must be of length block_size().
      */
      void decrypt(const uint8_t in[], uint8_t out[]) const { decrypt_n(in, out, 1); }

      /**
      * Encrypt a block.
      * @param block the plaintext block to be encrypted
      * Must be of length block_size(). Will hold the result when the function
      * has finished.
      */
      void encrypt(uint8_t block[]) const { encrypt_n(block, block, 1); }

      /**
      * Decrypt a block.
      * @param block the ciphertext block to be decrypted
      * Must be of length block_size(). Will hold the result when the function
      * has finished.
      */
      void decrypt(uint8_t block[]) const { decrypt_n(block, block, 1); }

      /**
      * Encrypt one or more blocks
      * @param block the input/output buffer (multiple of block_size())
      */
      void encrypt(std::span<uint8_t> block) const {
         return encrypt_n(block.data(), block.data(), block.size() / block_size());
      }

      /**
      * Decrypt one or more blocks
      * @param block the input/output buffer (multiple of block_size())
      */
      void decrypt(std::span<uint8_t> block) const {
         return decrypt_n(block.data(), block.data(), block.size() / block_size());
      }

      /**
      * Encrypt one or more blocks
      * @param in the input buffer (multiple of block_size())
      * @param out the output buffer (same size as in)
      */
      void encrypt(std::span<const uint8_t> in, std::span<uint8_t> out) const {
         return encrypt_n(in.data(), out.data(), in.size() / block_size());
      }

      /**
      * Decrypt one or more blocks
      * @param in the input buffer (multiple of block_size())
      * @param out the output buffer (same size as in)
      */
      void decrypt(std::span<const uint8_t> in, std::span<uint8_t> out) const {
         return decrypt_n(in.data(), out.data(), in.size() / block_size());
      }

      /**
      * Encrypt one or more blocks
      * @param in the input buffer (multiple of block_size())
      * @param out the output buffer (same size as in)
      * @param blocks the number of blocks to process
      */
      virtual void encrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const = 0;

      /**
      * Decrypt one or more blocks
      * @param in the input buffer (multiple of block_size())
      * @param out the output buffer (same size as in)
      * @param blocks the number of blocks to process
      */
      virtual void decrypt_n(const uint8_t in[], uint8_t out[], size_t blocks) const = 0;

      BOTAN_DEPRECATED("Deprecated no replacement")
      void encrypt_n_xex(uint8_t data[], const uint8_t mask[], size_t blocks) const {
         const size_t BS = block_size();
         for(size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
         }
         encrypt_n(data, data, blocks);
         for(size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
         }
      }

      BOTAN_DEPRECATED("Deprecated no replacement")
      void decrypt_n_xex(uint8_t data[], const uint8_t mask[], size_t blocks) const {
         const size_t BS = block_size();
         for(size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
         }
         decrypt_n(data, data, blocks);
         for(size_t i = 0; i != blocks * BS; ++i) {
            data[i] ^= mask[i];
         }
      }

      /**
      * @return new object representing the same algorithm as *this
      */
      virtual std::unique_ptr<BlockCipher> new_object() const = 0;

      BlockCipher* clone() const { return this->new_object().release(); }
};

/**
* Tweakable block ciphers allow setting a tweak which is a non-keyed
* value which affects the encryption/decryption operation.
*/
class BOTAN_PUBLIC_API(2, 8) Tweakable_Block_Cipher : public BlockCipher {
   public:
      /**
      * Set the tweak value. This must be called after setting a key. The value
      * persists until either set_tweak, set_key, or clear is called.
      * Different algorithms support different tweak length(s). If called with
      * an unsupported length, Invalid_Argument will be thrown.
      */
      virtual void set_tweak(const uint8_t tweak[], size_t len) = 0;
};

/**
* Represents a block cipher with a single fixed block size
*/
template <size_t BS, size_t KMIN, size_t KMAX = 0, size_t KMOD = 1, typename BaseClass = BlockCipher>
class Block_Cipher_Fixed_Params : public BaseClass {
   public:
      enum { BLOCK_SIZE = BS }; /* NOLINT(*-enum-size,*-use-enum-class) */

      size_t block_size() const final { return BS; }

      Key_Length_Specification key_spec() const final { return Key_Length_Specification(KMIN, KMAX, KMOD); }
};

}  // namespace Botan

namespace Botan {

/**
* This class represents any kind of computation which uses an internal
* state, such as hash functions or MACs
*/
class BOTAN_PUBLIC_API(2, 0) Buffered_Computation /* NOLINT(*special-member-functions) */ {
   public:
      /**
      * @return length of the output of this function in bytes
      */
      virtual size_t output_length() const = 0;

      /**
      * Add new input to process.
      * @param in the input to process as a byte array
      * @param length of param in in bytes
      */
      void update(const uint8_t in[], size_t length) { add_data({in, length}); }

      /**
      * Add new input to process.
      * @param in the input to process as a contiguous data range
      */
      void update(std::span<const uint8_t> in) { add_data(in); }

      void update_be(uint16_t val);
      void update_be(uint32_t val);
      void update_be(uint64_t val);

      void update_le(uint16_t val);
      void update_le(uint32_t val);
      void update_le(uint64_t val);

      /**
      * Add new input to process.
      * @param str the input to process as a std::string_view. Will be interpreted
      * as a byte array based on the strings encoding.
      */
      void update(std::string_view str);

      /**
      * Process a single byte.
      * @param in the byte to process
      */
      void update(uint8_t in) { add_data({&in, 1}); }

      /**
      * Complete the computation and retrieve the
      * final result.
      * @param out The byte array to be filled with the result.
      * Must be of length output_length()
      */
      void final(uint8_t out[]) { final_result({out, output_length()}); }

      /**
      * Complete the computation and retrieve the
      * final result as a container of your choice.
      * @return a contiguous container holding the result
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T final() {
         T output(output_length());
         final_result(output);
         return output;
      }

      std::vector<uint8_t> final_stdvec() { return final<std::vector<uint8_t>>(); }

      void final(std::span<uint8_t> out);

      template <concepts::resizable_byte_buffer T>
      void final(T& out) {
         out.resize(output_length());
         final_result(out);
      }

      /**
      * Update and finalize computation. Does the same as calling update()
      * and final() consecutively.
      * @param in the input to process as a byte array
      * @param length the length of the byte array
      * @result the result of the call to final()
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T process(const uint8_t in[], size_t length) {
         update(in, length);
         return final<T>();
      }

      /**
      * Update and finalize computation. Does the same as calling update()
      * and final() consecutively.
      * @param in the input to process as a string
      * @result the result of the call to final()
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T process(std::string_view in) {
         update(in);
         return final<T>();
      }

      /**
      * Update and finalize computation. Does the same as calling update()
      * and final() consecutively.
      * @param in the input to process as a contiguous container
      * @result the result of the call to final()
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T process(std::span<const uint8_t> in) {
         update(in);
         return final<T>();
      }

      virtual ~Buffered_Computation() = default;

   private:
      /**
      * Add more data to the computation
      * @param input is an input buffer
      */
      virtual void add_data(std::span<const uint8_t> input) = 0;

      /**
      * Write the final output to out
      * @param out is an output buffer of output_length()
      */
      virtual void final_result(std::span<uint8_t> out) = 0;
};

}  // namespace Botan

namespace Botan {

/**
* The two possible directions a Cipher_Mode can operate in
*/
enum class Cipher_Dir : uint8_t {
   Encryption = 0,
   Decryption = 1,

   ENCRYPTION BOTAN_DEPRECATED("Use Cipher_Dir::Encryption") = Encryption,
   DECRYPTION BOTAN_DEPRECATED("Use Cipher_Dir::Decryption") = Decryption,
};

/**
* Interface for cipher modes
*/
class BOTAN_PUBLIC_API(2, 0) Cipher_Mode : public SymmetricAlgorithm {
   public:
      /**
      * @return list of available providers for this algorithm, empty if not available
      * @param algo_spec algorithm name
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * Create an AEAD mode
      * @param algo the algorithm to create
      * @param direction specify if this should be an encryption or decryption AEAD
      * @param provider optional specification for provider to use
      * @return an AEAD mode or a null pointer if not available
      */
      static std::unique_ptr<Cipher_Mode> create(std::string_view algo,
                                                 Cipher_Dir direction,
                                                 std::string_view provider = "");

      /**
      * Create an AEAD mode, or throw
      * @param algo the algorithm to create
      * @param direction specify if this should be an encryption or decryption AEAD
      * @param provider optional specification for provider to use
      * @return an AEAD mode, or throw an exception
      */
      static std::unique_ptr<Cipher_Mode> create_or_throw(std::string_view algo,
                                                          Cipher_Dir direction,
                                                          std::string_view provider = "");

   protected:
      /*
      * Prepare for processing a message under the specified nonce
      */
      virtual void start_msg(const uint8_t nonce[], size_t nonce_len) = 0;

      /*
      * Process message blocks
      * Input must be a multiple of update_granularity.
      */
      virtual size_t process_msg(uint8_t msg[], size_t msg_len) = 0;

      /*
      * Finishes a message
      */
      virtual void finish_msg(secure_vector<uint8_t>& final_block, size_t offset = 0) = 0;

   public:
      /**
      * Begin processing a message with a fresh nonce.
      *
      * @warning Typically one must not reuse the same nonce for more than one
      *          message under the same key. For most cipher modes this would
      *          mean total loss of security and/or authenticity guarantees.
      *
      * @note If reliably generating unique nonces is difficult in your
      *       environment, use SIV which retains security even if nonces
      *       are repeated.
      *
      * @param nonce the per message nonce
      */
      void start(std::span<const uint8_t> nonce) { start_msg(nonce.data(), nonce.size()); }

      /**
      * Begin processing a message with a fresh nonce.
      * @param nonce the per message nonce
      * @param nonce_len length of nonce
      */
      void start(const uint8_t nonce[], size_t nonce_len) { start_msg(nonce, nonce_len); }

      /**
      * Begin processing a message.
      *
      * The exact semantics of this depend on the mode. For many modes, the call
      * will fail since a nonce must be provided.
      *
      * For certain modes such as CBC this will instead cause the last
      * ciphertext block to be used as the nonce of the new message; doing this
      * isn't a good idea, but some (mostly older) protocols do this.
      */
      void start() { return start_msg(nullptr, 0); }

      /**
      * Process message blocks
      *
      * Input must be a multiple of update_granularity
      *
      * Processes msg in place and returns bytes written. Normally
      * this will be either msg_len (indicating the entire message was
      * processed) or for certain AEAD modes zero (indicating that the
      * mode requires the entire message be processed in one pass).
      *
      * @param msg the message to be processed
      * @return bytes written in-place
      */
      size_t process(std::span<uint8_t> msg) { return this->process_msg(msg.data(), msg.size()); }

      size_t process(uint8_t msg[], size_t msg_len) { return this->process_msg(msg, msg_len); }

      /**
      * Process some data. Input must be in size update_granularity() uint8_t
      * blocks. The @p buffer is an in/out parameter and may be resized. In
      * particular, some modes require that all input be consumed before any
      * output is produced; with these modes, @p buffer will be returned empty.
      *
      * The first @p offset bytes of @p buffer will be ignored (this allows in
      * place processing of a buffer that contains an initial plaintext header).
      *
      * @param buffer in/out parameter which will possibly be resized
      * @param offset an offset into blocks to begin processing
      */
      template <concepts::resizable_byte_buffer T>
      void update(T& buffer, size_t offset = 0) {
         const size_t written = process(std::span(buffer).subspan(offset));
         buffer.resize(offset + written);
      }

      /**
      * Complete procession of a message with a final input of @p buffer, which
      * is treated the same as with update(). If you have the entire message in
      * hand, calling finish() without ever calling update() is both efficient
      * and convenient.
      *
      * When using an AEAD_Mode, if the supplied authentication tag does not
      * validate, this will throw an instance of Invalid_Authentication_Tag.
      *
      * If this occurs, all plaintext previously output via calls to update must
      * be destroyed and not used in any way that an attacker could observe the
      * effects of. This could be anything from echoing the plaintext back
      * (perhaps in an error message), or by making an external RPC whose
      * destination or contents depend on the plaintext. The only thing you can
      * do is buffer it, and in the event of an invalid tag, erase the
      * previously decrypted content from memory.
      *
      * One simple way to assure this could never happen is to never call
      * update, and instead always marshal the entire message into a single
      * buffer and call finish on it when decrypting.
      *
      * @param final_block in/out parameter which must be at least
      *        minimum_final_size() bytes, and will be set to any final output
      * @param offset an offset into final_block to begin processing
      */
      void finish(secure_vector<uint8_t>& final_block, size_t offset = 0) { finish_msg(final_block, offset); }

      /**
      * Complete procession of a message.
      *
      * Note: Using this overload with anything but a Botan::secure_vector<>
      *       is copying the bytes in the in/out buffer.
      *
      * @param final_block in/out parameter which must be at least
      *        minimum_final_size() bytes, and will be set to any final output
      * @param offset an offset into final_block to begin processing
      */
      template <concepts::resizable_byte_buffer T>
      void finish(T& final_block, size_t offset = 0) {
         Botan::secure_vector<uint8_t> tmp(final_block.begin(), final_block.end());
         finish_msg(tmp, offset);
         final_block.resize(tmp.size());
         std::copy(tmp.begin(), tmp.end(), final_block.begin());
      }

      /**
      * Returns the size of the output if this transform is used to process a
      * message with input_length bytes. In most cases the answer is precise.
      * If it is not possible to precise (namely for CBC decryption) instead an
      * upper bound is returned.
      */
      virtual size_t output_length(size_t input_length) const = 0;

      /**
      * The :cpp:class:`Cipher_Mode` interface requires message processing in
      * multiples of the block size. This returns size of required blocks to
      * update. If the mode implementation does not require buffering it will
      * return 1.
      * @return size of required blocks to update
      */
      virtual size_t update_granularity() const = 0;

      /**
      * Return an ideal granularity. This will be a multiple of the result of
      * update_granularity but may be larger. If so it indicates that better
      * performance may be achieved by providing buffers that are at least that
      * size (due to SIMD execution, etc).
      */
      virtual size_t ideal_granularity() const = 0;

      /**
      * Certain modes require the entire message be available before
      * any processing can occur. For such modes, input will be consumed
      * but not returned, until `finish` is called, which returns the
      * entire message.
      *
      * This function returns true if this mode has this style of
      * operation.
      */
      virtual bool requires_entire_message() const { return false; }

      /**
      * @return required minimum size to finalize() - may be any
      *         length larger than this.
      */
      virtual size_t minimum_final_size() const = 0;

      /**
      * @return the default size for a nonce
      */
      virtual size_t default_nonce_length() const = 0;

      /**
      * @return true iff nonce_len is a valid length for the nonce
      */
      virtual bool valid_nonce_length(size_t nonce_len) const = 0;

      /**
      * Resets just the message specific state and allows encrypting again under the existing key
      */
      virtual void reset() = 0;

      /**
      * Return the length in bytes of the authentication tag this algorithm
      * generates. If the mode is not authenticated, this will return 0.
      *
      * @return true iff this mode provides authentication as well as
      *         confidentiality.
      */
      bool authenticated() const { return this->tag_size() > 0; }

      /**
      * @return the size of the authentication tag used (in bytes)
      */
      virtual size_t tag_size() const { return 0; }

      /**
      * @return provider information about this implementation. Default is "base",
      * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
      */
      virtual std::string provider() const { return "base"; }
};

/**
* Get a cipher mode by name (eg "AES-128/CBC" or "Serpent/XTS")
* @param algo_spec cipher name
* @param direction Cipher_Dir::Encryption or Cipher_Dir::Decryption
* @param provider provider implementation to choose
*/
BOTAN_DEPRECATED("Use Cipher_Mode::create")
inline Cipher_Mode* get_cipher_mode(std::string_view algo_spec, Cipher_Dir direction, std::string_view provider = "") {
   return Cipher_Mode::create(algo_spec, direction, provider).release();
}

}  // namespace Botan

// NOLINTBEGIN(*-macro-usage)

BOTAN_FUTURE_INTERNAL_HEADER(compiler.h)

/*
* Define BOTAN_COMPILER_HAS_BUILTIN
*/
#if defined(__has_builtin)
   #define BOTAN_COMPILER_HAS_BUILTIN(x) __has_builtin(x)
#else
   #define BOTAN_COMPILER_HAS_BUILTIN(x) 0
#endif

/*
* Define BOTAN_COMPILER_HAS_ATTRIBUTE
*/
#if defined(__has_attribute)
   #define BOTAN_COMPILER_HAS_ATTRIBUTE(x) __has_attribute(x)
   #define BOTAN_COMPILER_ATTRIBUTE(x) __attribute__((x))
#else
   #define BOTAN_COMPILER_HAS_ATTRIBUTE(x) 0
   #define BOTAN_COMPILER_ATTRIBUTE(x) /**/
#endif

/*
* Hack for Loongarch64 GCC bug
*
* For some reason __has_attribute(target) is true, but it does not support the
* target attribute... this supposedly is fixed in GCC 15 but this is untested.
*/
#if defined(__GNUC__) && defined(__loongarch64) && (__GNUC__ <= 14)
   #define BOTAN_COMPILER_DOES_NOT_HAVE_TARGET_ATTRIBUTE
#endif

/*
* Define BOTAN_FUNC_ISA
*
* TODO(Botan4) Move this to isa_extn.h
*/
#if BOTAN_COMPILER_HAS_ATTRIBUTE(target) && !defined(BOTAN_COMPILER_DOES_NOT_HAVE_TARGET_ATTRIBUTE)
   #define BOTAN_FUNC_ISA(isa) BOTAN_COMPILER_ATTRIBUTE(target(isa))
#else
   #define BOTAN_FUNC_ISA(isa)
#endif

/*
* Define BOTAN_FUNC_ISA_INLINE
*
* TODO(Botan4) Remove this
*/
#define BOTAN_FUNC_ISA_INLINE(isa) BOTAN_FUNC_ISA(isa) BOTAN_FORCE_INLINE

/*
* Define BOTAN_EARLY_INIT
*/
#if BOTAN_COMPILER_HAS_ATTRIBUTE(init_priority)
   #define BOTAN_EARLY_INIT(prio) BOTAN_COMPILER_ATTRIBUTE(init_priority(prio))
#else
   #define BOTAN_EARLY_INIT(prio) /**/
#endif

/*
* Define BOTAN_FORCE_INLINE
*/
#if !defined(BOTAN_FORCE_INLINE)

   #if BOTAN_COMPILER_HAS_ATTRIBUTE(always_inline)
      #define BOTAN_FORCE_INLINE inline BOTAN_COMPILER_ATTRIBUTE(always_inline)

   #elif defined(_MSC_VER)
      #define BOTAN_FORCE_INLINE __forceinline

   #else
      #define BOTAN_FORCE_INLINE inline
   #endif

#endif

// NOLINTEND(*-macro-usage)

namespace Botan {

class RandomNumberGenerator;

/**
* Octet String
*/
class BOTAN_PUBLIC_API(2, 0) OctetString final {
   public:
      /**
      * @return size of this octet string in bytes
      */
      size_t length() const { return m_data.size(); }

      size_t size() const { return m_data.size(); }

      bool empty() const { return m_data.empty(); }

      /**
      * @return this object as a secure_vector<uint8_t>
      */
      secure_vector<uint8_t> bits_of() const { return m_data; }

      /**
      * @return start of this string
      */
      const uint8_t* begin() const { return m_data.data(); }

      /**
      * @return end of this string
      */
      const uint8_t* end() const { return begin() + m_data.size(); }

      /**
      * @return this encoded as hex
      */
      std::string to_string() const;

      /**
      * XOR the contents of another octet string into this one
      * @param other octet string
      * @return reference to this
      */
      OctetString& operator^=(const OctetString& other);

      /**
      * Force to have odd parity
      *
      * Deprecated. There is no reason to use this outside of interacting with
      * some very old or weird system which requires DES and also which do not
      * automatically ignore the parity bits.
      */
      BOTAN_DEPRECATED("Why would you need to do this") void set_odd_parity();

      /**
      * Create a new OctetString
      * @param str is a hex encoded string
      */
      explicit OctetString(std::string_view str = "");

      /**
      * Create a new random OctetString
      * @param rng is a random number generator
      * @param len is the desired length in bytes
      */
      OctetString(RandomNumberGenerator& rng, size_t len);

      /**
      * Create a new OctetString
      * @param in is an array
      * @param len is the length of in in bytes
      */
      OctetString(const uint8_t in[], size_t len);

      /**
      * Create a new OctetString
      * @param in a bytestring
      */
      explicit OctetString(std::span<const uint8_t> in) : m_data(in.begin(), in.end()) {}

      /**
      * Create a new OctetString
      * @param in a bytestring
      */
      explicit OctetString(secure_vector<uint8_t> in) : m_data(std::move(in)) {}

   private:
      secure_vector<uint8_t> m_data;
};

/**
* Compare two strings
* @param x an octet string
* @param y an octet string
* @return if x is equal to y
*/
BOTAN_PUBLIC_API(2, 0) bool operator==(const OctetString& x, const OctetString& y);

/**
* Compare two strings
* @param x an octet string
* @param y an octet string
* @return if x is not equal to y
*/
BOTAN_PUBLIC_API(2, 0) bool operator!=(const OctetString& x, const OctetString& y);

/**
* Concatenate two strings
* @param x an octet string
* @param y an octet string
* @return x concatenated with y
*/
BOTAN_PUBLIC_API(2, 0) OctetString operator+(const OctetString& x, const OctetString& y);

/**
* XOR two strings
* @param x an octet string
* @param y an octet string
* @return x XORed with y
*/
BOTAN_PUBLIC_API(2, 0) OctetString operator^(const OctetString& x, const OctetString& y);

/**
* Alternate name for octet string showing intent to use as a key
*/
using SymmetricKey = OctetString;

/**
* Alternate name for octet string showing intent to use as an IV
*/
using InitializationVector = OctetString;

}  // namespace Botan

BOTAN_DEPRECATED_HEADER("cryptobox.h")

namespace Botan {

class RandomNumberGenerator;

/**
* This namespace holds various high-level crypto functions
*/
namespace CryptoBox {

/**
* Encrypt a message using a passphrase
* @param input the input data
* @param input_len the length of input in bytes
* @param passphrase the passphrase used to encrypt the message
* @param rng a ref to a random number generator, such as AutoSeeded_RNG
*/
BOTAN_DEPRECATED("CryptoBox interface is deprecated")
BOTAN_PUBLIC_API(2, 0) std::string
   encrypt(const uint8_t input[], size_t input_len, std::string_view passphrase, RandomNumberGenerator& rng);

/**
* Decrypt a message encrypted with CryptoBox::encrypt
* @param input the input data
* @param input_len the length of input in bytes
* @param passphrase the passphrase used to encrypt the message
*/
BOTAN_DEPRECATED("CryptoBox interface is deprecated")
BOTAN_PUBLIC_API(2, 3) secure_vector<uint8_t> decrypt_bin(const uint8_t input[],
                                                          size_t input_len,
                                                          std::string_view passphrase);

/**
* Decrypt a message encrypted with CryptoBox::encrypt
* @param input the input data
* @param passphrase the passphrase used to encrypt the message
*/
BOTAN_DEPRECATED("CryptoBox interface is deprecated")
BOTAN_PUBLIC_API(2, 3) secure_vector<uint8_t> decrypt_bin(std::string_view input, std::string_view passphrase);

/**
* Decrypt a message encrypted with CryptoBox::encrypt
* @param input the input data
* @param input_len the length of input in bytes
* @param passphrase the passphrase used to encrypt the message
*/
BOTAN_DEPRECATED("CryptoBox interface is deprecated")
BOTAN_PUBLIC_API(2, 0) std::string decrypt(const uint8_t input[], size_t input_len, std::string_view passphrase);

/**
* Decrypt a message encrypted with CryptoBox::encrypt
* @param input the input data
* @param passphrase the passphrase used to encrypt the message
*/
BOTAN_DEPRECATED("CryptoBox interface is deprecated")
BOTAN_PUBLIC_API(2, 0) std::string decrypt(std::string_view input, std::string_view passphrase);

}  // namespace CryptoBox

}  // namespace Botan

namespace Botan {

class BOTAN_PUBLIC_API(2, 0) SQL_Database /* NOLINT(*-special-member-functions) */ {
   public:
      class BOTAN_PUBLIC_API(2, 0) SQL_DB_Error final : public Exception {
         public:
            explicit SQL_DB_Error(std::string_view what) : Exception("SQL database", what), m_rc(0) {}

            SQL_DB_Error(std::string_view what, int rc) : Exception("SQL database", what), m_rc(rc) {}

            ErrorType error_type() const noexcept override { return ErrorType::DatabaseError; }

            int error_code() const noexcept override { return m_rc; }

         private:
            int m_rc;
      };

      class BOTAN_PUBLIC_API(2, 0) Statement /* NOLINT(*-special-member-functions) */ {
         public:
            /* Bind statement parameters */
            virtual void bind(int column, std::string_view str) = 0;

            virtual void bind(int column, size_t i) = 0;

            virtual void bind(int column, std::chrono::system_clock::time_point time) = 0;

            virtual void bind(int column, const std::vector<uint8_t>& blob) = 0;

            virtual void bind(int column, const uint8_t* data, size_t len) = 0;

            /* Get output */
            virtual std::pair<const uint8_t*, size_t> get_blob(int column) = 0;

            virtual std::string get_str(int column) = 0;

            virtual size_t get_size_t(int column) = 0;

            /* Run to completion */
            virtual size_t spin() = 0;

            /* Maybe update */
            virtual bool step() = 0;

            virtual ~Statement() = default;
      };

      /*
      * Create a new statement for execution.
      * Use ?1, ?2, ?3, etc for parameters to set later with bind
      */
      virtual std::shared_ptr<Statement> new_statement(std::string_view base_sql) const = 0;

      virtual size_t row_count(std::string_view table_name) = 0;

      virtual void create_table(std::string_view table_schema) = 0;

      virtual size_t rows_changed_by_last_statement() = 0;

      virtual size_t exec(std::string_view sql) { return new_statement(sql)->spin(); }

      virtual bool is_threadsafe() const { return false; }

      virtual ~SQL_Database() = default;
};

}  // namespace Botan

namespace Botan {

class BigInt;

/**
* General DER Encoding Object
*/
class BOTAN_PUBLIC_API(2, 0) DER_Encoder final {
   public:
      typedef std::function<void(const uint8_t[], size_t)> append_fn;

      /**
      * DER encode, writing to an internal buffer
      * Use get_contents or get_contents_unlocked to read the results
      * after all encoding is completed.
      */
      DER_Encoder() = default;

      /**
      * DER encode, writing to @param vec
      * If this constructor is used, get_contents* may not be called.
      */
      BOTAN_FUTURE_EXPLICIT DER_Encoder(secure_vector<uint8_t>& vec);

      /**
      * DER encode, writing to @param vec
      * If this constructor is used, get_contents* may not be called.
      */
      BOTAN_FUTURE_EXPLICIT DER_Encoder(std::vector<uint8_t>& vec);

      /**
      * DER encode, calling append to write output
      * If this constructor is used, get_contents* may not be called.
      */
      BOTAN_FUTURE_EXPLICIT DER_Encoder(append_fn append) : m_append_output(std::move(append)) {}

      secure_vector<uint8_t> get_contents();

      /**
      * Return the encoded contents as a std::vector
      *
      * If using this function, instead pass a std::vector to the
      * constructor of DER_Encoder where the output will be placed. This
      * avoids several unnecessary copies.
      */
      BOTAN_DEPRECATED("Use DER_Encoder(vector) instead") std::vector<uint8_t> get_contents_unlocked();

      DER_Encoder& start_cons(ASN1_Type type_tag, ASN1_Class class_tag);

      DER_Encoder& start_sequence() { return start_cons(ASN1_Type::Sequence, ASN1_Class::Universal); }

      DER_Encoder& start_set() { return start_cons(ASN1_Type::Set, ASN1_Class::Universal); }

      DER_Encoder& start_context_specific(uint32_t tag) {
         return start_cons(ASN1_Type(tag), ASN1_Class::ContextSpecific);
      }

      DER_Encoder& start_explicit_context_specific(uint32_t tag) {
         return start_cons(ASN1_Type(tag), ASN1_Class::ExplicitContextSpecific);
      }

      DER_Encoder& end_cons();

      DER_Encoder& start_explicit(uint16_t type_tag);
      DER_Encoder& end_explicit();

      /**
      * Insert raw bytes directly into the output stream
      */
      DER_Encoder& raw_bytes(const uint8_t val[], size_t len);

      template <typename Alloc>
      DER_Encoder& raw_bytes(const std::vector<uint8_t, Alloc>& val) {
         return raw_bytes(val.data(), val.size());
      }

      DER_Encoder& encode_null();
      DER_Encoder& encode(bool b);
      DER_Encoder& encode(size_t s);
      DER_Encoder& encode(const BigInt& n);
      DER_Encoder& encode(const uint8_t val[], size_t len, ASN1_Type real_type);

      template <typename Alloc>
      DER_Encoder& encode(const std::vector<uint8_t, Alloc>& vec, ASN1_Type real_type) {
         return encode(vec.data(), vec.size(), real_type);
      }

      DER_Encoder& encode(bool b, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      DER_Encoder& encode(size_t s, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      DER_Encoder& encode(const BigInt& n, ASN1_Type type_tag, ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      DER_Encoder& encode(const uint8_t v[],
                          size_t len,
                          ASN1_Type real_type,
                          ASN1_Type type_tag,
                          ASN1_Class class_tag = ASN1_Class::ContextSpecific);

      template <typename Alloc>
      DER_Encoder& encode(const std::vector<uint8_t, Alloc>& bytes,
                          ASN1_Type real_type,
                          ASN1_Type type_tag,
                          ASN1_Class class_tag) {
         return encode(bytes.data(), bytes.size(), real_type, type_tag, class_tag);
      }

      template <typename T>
      BOTAN_DEPRECATED("Use the version that takes a std::optional")
      DER_Encoder& encode_optional(const T& value, const T& default_value) {
         if(value != default_value) {
            encode(value);
         }
         return (*this);
      }

      template <typename T>
      DER_Encoder& encode_optional(const std::optional<T>& value) {
         if(value) {
            encode(*value);
         }
         return (*this);
      }

      template <typename T>
      DER_Encoder& encode_list(const std::vector<T>& values) {
         for(size_t i = 0; i != values.size(); ++i) {
            encode(values[i]);
         }
         return (*this);
      }

      /*
      * Request for an object to encode itself to this stream
      */
      DER_Encoder& encode(const ASN1_Object& obj);

      /*
      * Conditionally write some values to the stream
      */
      DER_Encoder& encode_if(bool pred, DER_Encoder& enc) {
         if(pred) {
            return raw_bytes(enc.get_contents());
         }
         return (*this);
      }

      DER_Encoder& encode_if(bool pred, const ASN1_Object& obj) {
         if(pred) {
            encode(obj);
         }
         return (*this);
      }

      DER_Encoder& encode_if(bool pred, size_t num) {
         if(pred) {
            encode(num);
         }
         return (*this);
      }

      DER_Encoder& encode_if(bool pred, bool num) {
         if(pred) {
            encode(num);
         }
         return (*this);
      }

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, const uint8_t rep[], size_t length);

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, std::span<const uint8_t> rep) {
         return add_object(type_tag, class_tag, rep.data(), rep.size());
      }

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, const std::vector<uint8_t>& rep) {
         return add_object(type_tag, class_tag, std::span{rep});
      }

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, const secure_vector<uint8_t>& rep) {
         return add_object(type_tag, class_tag, std::span{rep});
      }

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, std::string_view str);

      DER_Encoder& add_object(ASN1_Type type_tag, ASN1_Class class_tag, uint8_t val);

   private:
      class DER_Sequence final {
         public:
            uint32_t tag_of() const;

            void push_contents(DER_Encoder& der);

            void add_bytes(const uint8_t val[], size_t len);

            void add_bytes(const uint8_t hdr[], size_t hdr_len, const uint8_t val[], size_t val_len);

            DER_Sequence(ASN1_Type type_tag, ASN1_Class class_tag);

            DER_Sequence(DER_Sequence&& seq) noexcept :
                  m_type_tag(seq.m_type_tag),
                  m_class_tag(seq.m_class_tag),
                  m_contents(std::move(seq.m_contents)),
                  m_set_contents(std::move(seq.m_set_contents)) {}

            DER_Sequence& operator=(DER_Sequence&& seq) noexcept {
               std::swap(m_type_tag, seq.m_type_tag);
               std::swap(m_class_tag, seq.m_class_tag);
               std::swap(m_contents, seq.m_contents);
               std::swap(m_set_contents, seq.m_set_contents);
               return (*this);
            }

            DER_Sequence(const DER_Sequence& seq) = default;
            DER_Sequence& operator=(const DER_Sequence& seq) = default;
            ~DER_Sequence() = default;

         private:
            ASN1_Type m_type_tag;
            ASN1_Class m_class_tag;
            secure_vector<uint8_t> m_contents;
            std::vector<secure_vector<uint8_t>> m_set_contents;
      };

      append_fn m_append_output;
      secure_vector<uint8_t> m_default_outbuf;
      std::vector<DER_Sequence> m_subsequences;
};

}  // namespace Botan

namespace Botan {

class RandomNumberGenerator;

/**
* Abstract interface to a source of entropy
*/
class BOTAN_PUBLIC_API(2, 0) Entropy_Source {
   public:
      /**
      * Return a new entropy source of a particular type, or null
      * Each entropy source may require substantial resources (eg, a file handle
      * or socket instance), so try to share them among multiple RNGs, or just
      * use the preconfigured global list accessed by Entropy_Sources::global_sources()
      */
      static std::unique_ptr<Entropy_Source> create(std::string_view type);

      /**
      * @return name identifying this entropy source
      */
      virtual std::string name() const = 0;

      /**
      * Perform an entropy gathering poll
      * @param rng will be provided with entropy via calls to add_entropy
      * @return conservative estimate of actual entropy added to rng during poll
      */
      virtual size_t poll(RandomNumberGenerator& rng) = 0;

      Entropy_Source() = default;
      Entropy_Source(const Entropy_Source& other) = delete;
      Entropy_Source(Entropy_Source&& other) = delete;
      Entropy_Source& operator=(const Entropy_Source& other) = delete;
      Entropy_Source& operator=(Entropy_Source&& other) = delete;

      virtual ~Entropy_Source() = default;
};

class BOTAN_PUBLIC_API(2, 0) Entropy_Sources final {
   public:
      static Entropy_Sources& global_sources();

      void add_source(std::unique_ptr<Entropy_Source> src);

      std::vector<std::string> enabled_sources() const;

      /**
      * Poll all sources to collect @p bits of entropy with a @p timeout.
      * Entropy collection is aborted as soon as either the requested number of
      * bits are obtained or the timeout runs out. If the target system does not
      * provide a clock, the timeout is ignored.
      *
      * Note that the timeout is cooperative. If the poll() method of an entropy
      * source blocks forever, this invocation will potentially also block.
      *
      * @returns the number of bits collected from the entropy sources
      */
      size_t poll(RandomNumberGenerator& rng, size_t bits, std::chrono::milliseconds timeout);

      /**
      * Poll just a single named source. Ordinally only used for testing
      */
      size_t poll_just(RandomNumberGenerator& rng, std::string_view src);

      Entropy_Sources() = default;
      explicit Entropy_Sources(const std::vector<std::string>& sources);

      Entropy_Sources(const Entropy_Sources& other) = delete;
      Entropy_Sources(Entropy_Sources&& other) = delete;
      Entropy_Sources& operator=(const Entropy_Sources& other) = delete;
      Entropy_Sources& operator=(Entropy_Sources&& other) = delete;
      ~Entropy_Sources() = default;

   private:
      std::vector<std::unique_ptr<Entropy_Source>> m_srcs;
};

}  // namespace Botan

namespace Botan {

/**
* This class represents hash function (message digest) objects
*/
class BOTAN_PUBLIC_API(2, 0) HashFunction : public Buffered_Computation {
   public:
      /**
      * Create an instance based on a name, or return null if the
      * algo/provider combination cannot be found. If provider is
      * empty then best available is chosen.
      */
      static std::unique_ptr<HashFunction> create(std::string_view algo_spec, std::string_view provider = "");

      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to use
      * Throws Lookup_Error if not found.
      */
      static std::unique_ptr<HashFunction> create_or_throw(std::string_view algo_spec, std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      * @param algo_spec algorithm name
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * @return provider information about this implementation. Default is "base",
      * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
      */
      virtual std::string provider() const { return "base"; }

      /**
      * Reset the state.
      */
      virtual void clear() = 0;

      /**
      * @return the hash function name
      */
      virtual std::string name() const = 0;

      /**
      * @return hash block size as defined for this algorithm
      */
      virtual size_t hash_block_size() const { return 0; }

      /**
      * Return a new hash object with the same state as *this. This
      * allows computing the hash of several messages with a common
      * prefix more efficiently than would otherwise be possible.
      *
      * This function should be called `clone` but that was already
      * used for the case of returning an uninitialized object.
      * @return new hash object
      */
      virtual std::unique_ptr<HashFunction> copy_state() const = 0;

      /**
      * @return new object representing the same algorithm as *this
      */
      virtual std::unique_ptr<HashFunction> new_object() const = 0;

      /**
      * @return new object representing the same algorithm as *this
      */
      HashFunction* clone() const { return this->new_object().release(); }
};

}  // namespace Botan

namespace Botan {

/**
* Perform hex encoding
* @param output an array of at least input_length*2 bytes
* @param input is some binary data
* @param input_length length of input in bytes
* @param uppercase should output be upper or lower case?
*/
void BOTAN_PUBLIC_API(2, 0)
   hex_encode(char output[], const uint8_t input[], size_t input_length, bool uppercase = true);

/**
* Perform hex encoding
* @param input some input
* @param input_length length of input in bytes
* @param uppercase should output be upper or lower case?
* @return hexadecimal representation of input
*/
std::string BOTAN_PUBLIC_API(2, 0) hex_encode(const uint8_t input[], size_t input_length, bool uppercase = true);

/**
* Perform hex encoding
* @param input some input
* @param uppercase should output be upper or lower case?
* @return hexadecimal representation of input
*/
inline std::string hex_encode(std::span<const uint8_t> input, bool uppercase = true) {
   return hex_encode(input.data(), input.size(), uppercase);
}

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param input_length length of input in bytes
* @param input_consumed is an output parameter which says how many
*        bytes of input were actually consumed. If less than
*        input_length, then the range input[consumed:length]
*        should be passed in later along with more input.
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
   hex_decode(uint8_t output[], const char input[], size_t input_length, size_t& input_consumed, bool ignore_ws = true);

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param input_length length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(2, 0)
   hex_decode(uint8_t output[], const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform hex decoding
* @param output an array of at least input_length/2 bytes
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(3, 0) hex_decode(uint8_t output[], std::string_view input, bool ignore_ws = true);

/**
* Perform hex decoding
* @param output a contiguous byte buffer of at least input_length/2 bytes
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
*                  exception if whitespace is encountered
* @return number of bytes written to output
*/
size_t BOTAN_PUBLIC_API(3, 0) hex_decode(std::span<uint8_t> output, std::string_view input, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param input_length the length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
std::vector<uint8_t> BOTAN_PUBLIC_API(2, 0) hex_decode(const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
std::vector<uint8_t> BOTAN_PUBLIC_API(3, 0) hex_decode(std::string_view input, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param input_length the length of input in bytes
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(2, 0)
   hex_decode_locked(const char input[], size_t input_length, bool ignore_ws = true);

/**
* Perform hex decoding
* @param input some hex input
* @param ignore_ws ignore whitespace on input; if false, throw an
                   exception if whitespace is encountered
* @return decoded hex output
*/
secure_vector<uint8_t> BOTAN_PUBLIC_API(3, 0) hex_decode_locked(std::string_view input, bool ignore_ws = true);

}  // namespace Botan

namespace Botan {

/**
* This class represents Message Authentication Code (MAC) objects.
*/
class BOTAN_PUBLIC_API(2, 0) MessageAuthenticationCode : public Buffered_Computation,
                                                         public SymmetricAlgorithm {
   public:
      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to use
      * @return a null pointer if the algo/provider combination cannot be found
      */
      static std::unique_ptr<MessageAuthenticationCode> create(std::string_view algo_spec,
                                                               std::string_view provider = "");

      /*
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to use
      * Throws a Lookup_Error if algo/provider combination cannot be found
      */
      static std::unique_ptr<MessageAuthenticationCode> create_or_throw(std::string_view algo_spec,
                                                                        std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * Prepare for processing a message under the specified nonce
      *
      * Most MACs neither require nor support a nonce; for these algorithms
      * calling start() is optional and calling it with anything other than
      * an empty string is an error. One MAC which *requires* a per-message
      * nonce be specified is GMAC.
      *
      * Default implementation simply rejects all non-empty nonces
      * since most hash/MAC algorithms do not support randomization
      *
      * @param nonce the message nonce bytes
      */
      void start(std::span<const uint8_t> nonce) { start_msg(nonce); }

      /**
      * Begin processing a message.
      * @param nonce the per message nonce
      * @param nonce_len length of nonce
      */
      void start(const uint8_t nonce[], size_t nonce_len) { start_msg({nonce, nonce_len}); }

      /**
      * Begin processing a message.
      */
      void start() { return start_msg({}); }

      /**
      * Verify a MAC.
      * @param in the MAC to verify as a byte array
      * @param length the length of param in
      * @return true if the MAC is valid, false otherwise
      */
      bool verify_mac(const uint8_t in[], size_t length) { return verify_mac_result({in, length}); }

      /**
      * Verify a MAC.
      * @param in the MAC to verify as a byte array
      * @return true if the MAC is valid, false otherwise
      */
      bool verify_mac(std::span<const uint8_t> in) { return verify_mac_result(in); }

      /**
      * @return new object representing the same algorithm as *this
      */
      virtual std::unique_ptr<MessageAuthenticationCode> new_object() const = 0;

      /**
      * Get a new object representing the same algorithm as *this
      */
      MessageAuthenticationCode* clone() const { return this->new_object().release(); }

      /**
      * @return provider information about this implementation. Default is "base",
      * might also return "sse2", "avx2", "openssl", or some other arbitrary string.
      */
      virtual std::string provider() const { return "base"; }

      /**
      * @return if a fresh key must be set for each message that is processed.
      *
      * This is required for certain polynomial-based MACs which are insecure
      * if a key is ever reused for two different messages.
      */
      virtual bool fresh_key_required_per_message() const { return false; }

   protected:
      /**
      * Prepare for processing a message under the specified nonce
      *
      * If the MAC does not support nonces, it should not override the default
      * implementation.
      */
      virtual void start_msg(std::span<const uint8_t> nonce);

      /**
      * Verify the MACs final result
      */
      virtual bool verify_mac_result(std::span<const uint8_t> in);
};

typedef MessageAuthenticationCode MAC;

}  // namespace Botan

#if defined(BOTAN_TARGET_OS_HAS_THREADS)
   #include <mutex>
#endif

namespace Botan {

#if defined(BOTAN_TARGET_OS_HAS_THREADS)

using mutex_type = std::mutex;
using recursive_mutex_type = std::recursive_mutex;

template <typename T>
using lock_guard_type = std::scoped_lock<T>;

#else

// No threads

class noop_mutex final {
   public:
      void lock() {}

      void unlock() {}
};

using mutex_type = noop_mutex;
using recursive_mutex_type = noop_mutex;

template <typename Mutex>
class lock_guard final {
   public:
      explicit lock_guard(Mutex& m) : m_mutex(m) { m_mutex.lock(); }

      ~lock_guard() { m_mutex.unlock(); }

      lock_guard(const lock_guard& other) = delete;
      lock_guard& operator=(const lock_guard& other) = delete;

   private:
      Mutex& m_mutex;
};

template <typename T>
using lock_guard_type = lock_guard<T>;

#endif

}  // namespace Botan

namespace Botan {

/**
* Inherited by RNGs which maintain in-process state, like HMAC_DRBG.
* On Unix these RNGs are vulnerable to problems with fork, where the
* RNG state is duplicated, and the parent and child process RNGs will
* produce identical output until one of them reseeds. Stateful_RNG
* reseeds itself whenever a fork is detected, or after a set number of
* bytes have been output.
*
* Not implemented by RNGs which access an external RNG, such as the
* system PRNG or a hardware RNG.
*/
class BOTAN_PUBLIC_API(2, 0) Stateful_RNG : public RandomNumberGenerator {
   public:
      /**
      * @param rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      Stateful_RNG(RandomNumberGenerator& rng, Entropy_Sources& entropy_sources, size_t reseed_interval) :
            m_underlying_rng(&rng), m_entropy_sources(&entropy_sources), m_reseed_interval(reseed_interval) {}

      /**
      * @param rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      Stateful_RNG(RandomNumberGenerator& rng, size_t reseed_interval) :
            m_underlying_rng(&rng), m_reseed_interval(reseed_interval) {}

      /**
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed
      */
      Stateful_RNG(Entropy_Sources& entropy_sources, size_t reseed_interval) :
            m_entropy_sources(&entropy_sources), m_reseed_interval(reseed_interval) {}

      /**
      * In this case, automatic reseeding is impossible
      */
      Stateful_RNG() : m_reseed_interval(0) {}

      /**
      * Consume this input and mark the RNG as initialized regardless
      * of the length of the input or the current seeded state of
      * the RNG.
      */
      void initialize_with(std::span<const uint8_t> input);

      void initialize_with(const uint8_t input[], size_t length) { this->initialize_with(std::span(input, length)); }

      bool is_seeded() const final;

      bool accepts_input() const final { return true; }

      /**
      * Mark state as requiring a reseed on next use
      */
      void force_reseed();

      void reseed_from_rng(RandomNumberGenerator& rng, size_t poll_bits = RandomNumberGenerator::DefaultPollBits) final;

      /**
      * Poll provided sources for up to poll_bits bits of entropy
      * or until the timeout expires. Returns estimate of the number
      * of bits collected.
      */
      size_t reseed(Entropy_Sources& srcs,
                    size_t poll_bits = RandomNumberGenerator::DefaultPollBits,
                    std::chrono::milliseconds poll_timeout = RandomNumberGenerator::DefaultPollTimeout) override;

      /**
      * @return intended security level of this DRBG
      */
      virtual size_t security_level() const = 0;

      /**
      * Some DRBGs have a notion of the maximum number of bytes per
      * request.  Longer requests (to randomize) will be treated as
      * multiple requests, and may initiate reseeding multiple times,
      * depending on the values of max_number_of_bytes_per_request and
      * reseed_interval(). This function returns zero if the RNG in
      * question does not have such a notion.
      *
      * @return max number of bytes per request (or zero)
      */
      virtual size_t max_number_of_bytes_per_request() const = 0;

      size_t reseed_interval() const { return m_reseed_interval; }

      void clear() final;

   protected:
      void reseed_check();

      virtual void generate_output(std::span<uint8_t> output, std::span<const uint8_t> input) = 0;

      virtual void update(std::span<const uint8_t> input) = 0;

      virtual void clear_state() = 0;

   private:
      void generate_batched_output(std::span<uint8_t> output, std::span<const uint8_t> input);

      void fill_bytes_with_input(std::span<uint8_t> output, std::span<const uint8_t> input) final;

      void reset_reseed_counter();

      mutable recursive_mutex_type m_mutex;

      // A non-owned and possibly null pointer to shared RNG
      RandomNumberGenerator* m_underlying_rng = nullptr;

      // A non-owned and possibly null pointer to a shared Entropy_Source
      Entropy_Sources* m_entropy_sources = nullptr;

      const size_t m_reseed_interval;
      uint32_t m_last_pid = 0;

      /*
      * Set to 1 after a successful seeding, then incremented.  Reset
      * to 0 by clear() or a fork. This logic is used even if
      * automatic reseeding is disabled (via m_reseed_interval = 0)
      */
      size_t m_reseed_counter = 0;
};

}  // namespace Botan

namespace Botan {

class Entropy_Sources;

/**
* HMAC_DRBG from NIST SP800-90A
*/
class BOTAN_PUBLIC_API(2, 0) HMAC_DRBG final : public Stateful_RNG {
   public:
      /**
      * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
      *
      * Automatic reseeding is disabled completely, as it has no access to
      * any source for seed material.
      *
      * If a fork is detected, the RNG will be unable to reseed itself
      * in response. In this case, an exception will be thrown rather
      * than generating duplicated output.
      */
      explicit HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf);

      /**
      * Constructor taking a string for the hash
      */
      explicit HMAC_DRBG(std::string_view hmac_hash);

      /**
      * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
      *
      * Automatic reseeding from @p underlying_rng will take place after
      * @p reseed_interval many requests or after a fork was detected.
      *
      * @param prf MAC to use as a PRF
      * @param underlying_rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed (max. 2^24)
      * @param max_number_of_bytes_per_request requests that are in size higher
      * than max_number_of_bytes_per_request are treated as if multiple single
      * requests of max_number_of_bytes_per_request size had been made.
      * In theory SP 800-90A requires that we reject any request for a DRBG
      * output longer than max_number_of_bytes_per_request. To avoid inconveniencing
      * the caller who wants an output larger than max_number_of_bytes_per_request,
      * instead treat these requests as if multiple requests of
      * max_number_of_bytes_per_request size had been made. NIST requires for
      * HMAC_DRBG that every implementation set a value no more than 2**19 bits
      * (or 64 KiB). Together with @p reseed_interval = 1 you can enforce that for
      * example every 512 bit automatic reseeding occurs.
      */
      HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf,
                RandomNumberGenerator& underlying_rng,
                size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval,
                size_t max_number_of_bytes_per_request = 64 * 1024);

      /**
      * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
      *
      * Automatic reseeding from @p entropy_sources will take place after
      * @p reseed_interval many requests or after a fork was detected.
      *
      * @param prf MAC to use as a PRF
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed (max. 2^24)
      * @param max_number_of_bytes_per_request requests that are in size higher
      * than max_number_of_bytes_per_request are treated as if multiple single
      * requests of max_number_of_bytes_per_request size had been made.
      * In theory SP 800-90A requires that we reject any request for a DRBG
      * output longer than max_number_of_bytes_per_request. To avoid inconveniencing
      * the caller who wants an output larger than max_number_of_bytes_per_request,
      * instead treat these requests as if multiple requests of
      * max_number_of_bytes_per_request size had been made. NIST requires for
      * HMAC_DRBG that every implementation set a value no more than 2**19 bits
      * (or 64 KiB). Together with @p reseed_interval = 1 you can enforce that for
      * example every 512 bit automatic reseeding occurs.
      */
      HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf,
                Entropy_Sources& entropy_sources,
                size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval,
                size_t max_number_of_bytes_per_request = 64 * 1024);

      /**
      * Initialize an HMAC_DRBG instance with the given MAC as PRF (normally HMAC)
      *
      * Automatic reseeding from @p underlying_rng and @p entropy_sources
      * will take place after @p reseed_interval many requests or after
      * a fork was detected.
      *
      * @param prf MAC to use as a PRF
      * @param underlying_rng is a reference to some RNG which will be used
      * to perform the periodic reseeding
      * @param entropy_sources will be polled to perform reseeding periodically
      * @param reseed_interval specifies a limit of how many times
      * the RNG will be called before automatic reseeding is performed (max. 2^24)
      * @param max_number_of_bytes_per_request requests that are in size higher
      * than max_number_of_bytes_per_request are treated as if multiple single
      * requests of max_number_of_bytes_per_request size had been made.
      * In theory SP 800-90A requires that we reject any request for a DRBG
      * output longer than max_number_of_bytes_per_request. To avoid inconveniencing
      * the caller who wants an output larger than max_number_of_bytes_per_request,
      * instead treat these requests as if multiple requests of
      * max_number_of_bytes_per_request size had been made. NIST requires for
      * HMAC_DRBG that every implementation set a value no more than 2**19 bits
      * (or 64 KiB). Together with @p reseed_interval = 1 you can enforce that for
      * example every 512 bit automatic reseeding occurs.
      */
      HMAC_DRBG(std::unique_ptr<MessageAuthenticationCode> prf,
                RandomNumberGenerator& underlying_rng,
                Entropy_Sources& entropy_sources,
                size_t reseed_interval = RandomNumberGenerator::DefaultReseedInterval,
                size_t max_number_of_bytes_per_request = 64 * 1024);

      std::string name() const override;

      size_t security_level() const override;

      size_t max_number_of_bytes_per_request() const override { return m_max_number_of_bytes_per_request; }

   private:
      void update(std::span<const uint8_t> input) override;

      void generate_output(std::span<uint8_t> output, std::span<const uint8_t> input) override;

      void clear_state() override;

      std::unique_ptr<MessageAuthenticationCode> m_mac;
      secure_vector<uint8_t> m_V;
      secure_vector<uint8_t> m_T;
      const size_t m_max_number_of_bytes_per_request;
      const size_t m_security_level;
};

}  // namespace Botan

namespace Botan {

/**
* Key Derivation Function
*/
class BOTAN_PUBLIC_API(2, 0) KDF /* NOLINT(*-special-member-functions*) */ {
   public:
      virtual ~KDF() = default;

      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to choose
      * @return a null pointer if the algo/provider combination cannot be found
      */
      static std::unique_ptr<KDF> create(std::string_view algo_spec, std::string_view provider = "");

      /**
      * Create an instance based on a name, or throw if the
      * algo/provider combination cannot be found. If provider is
      * empty then best available is chosen.
      */
      static std::unique_ptr<KDF> create_or_throw(std::string_view algo_spec, std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * @return KDF name
      */
      virtual std::string name() const = 0;

      /**
      * Derive a key
      * @param key buffer holding the derived key, must be of length key_len
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param secret_len size of secret in bytes
      * @param salt a diversifier
      * @param salt_len size of salt in bytes
      * @param label purpose for the derived keying material
      * @param label_len size of label in bytes
      */
      BOTAN_DEPRECATED("Use KDF::derive_key")
      void kdf(uint8_t key[],
               size_t key_len,
               const uint8_t secret[],
               size_t secret_len,
               const uint8_t salt[],
               size_t salt_len,
               const uint8_t label[],
               size_t label_len) const {
         derive_key({key, key_len}, {secret, secret_len}, {salt, salt_len}, {label, label_len});
      }

      /**
      * Derive a key
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param secret_len size of secret in bytes
      * @param salt a diversifier
      * @param salt_len size of salt in bytes
      * @param label purpose for the derived keying material
      * @param label_len size of label in bytes
      * @return the derived key
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      BOTAN_DEPRECATED("Use std::span or std::string_view overloads")
      T derive_key(size_t key_len,
                   const uint8_t secret[],
                   size_t secret_len,
                   const uint8_t salt[],
                   size_t salt_len,
                   const uint8_t label[] = nullptr,
                   size_t label_len = 0) const {
         return derive_key<T>(key_len, {secret, secret_len}, {salt, salt_len}, {label, label_len});
      }

      /**
      * Derive a key
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T derive_key(size_t key_len,
                   std::span<const uint8_t> secret,
                   std::string_view salt = "",
                   std::string_view label = "") const {
         return derive_key<T>(key_len, secret, _as_span(salt), _as_span(label));
      }

      /**
      * Derive a key
      * @param key the output buffer for the to-be-derived key
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      */
      void derive_key(std::span<uint8_t> key,
                      std::span<const uint8_t> secret,
                      std::span<const uint8_t> salt,
                      std::span<const uint8_t> label) const {
         perform_kdf(key, secret, salt, label);
      }

      /**
      * Derive a key
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T derive_key(size_t key_len,
                   std::span<const uint8_t> secret,
                   std::span<const uint8_t> salt,
                   std::span<const uint8_t> label) const {
         T key(key_len);
         perform_kdf(key, secret, salt, label);
         return key;
      }

      /**
      * Derive a key
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param salt_len size of salt in bytes
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      BOTAN_DEPRECATED("Use std::span or std::string_view overloads")
      T derive_key(size_t key_len,
                   std::span<const uint8_t> secret,
                   const uint8_t salt[],
                   size_t salt_len,
                   std::string_view label = "") const {
         return derive_key<T>(key_len, secret, {salt, salt_len}, _as_span(label));
      }

      /**
      * Derive a key
      * @param key_len the desired output length in bytes
      * @param secret the secret input
      * @param secret_len size of secret in bytes
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      BOTAN_DEPRECATED("Use std::span or std::string_view overloads")
      T derive_key(size_t key_len,
                   const uint8_t secret[],
                   size_t secret_len,
                   std::string_view salt = "",
                   std::string_view label = "") const {
         return derive_key<T>(key_len, {secret, secret_len}, _as_span(salt), _as_span(label));
      }

      /**
      * Derive a key
      * @tparam key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <size_t key_len>
      std::array<uint8_t, key_len> derive_key(std::span<const uint8_t> secret,
                                              std::span<const uint8_t> salt = {},
                                              std::span<const uint8_t> label = {}) {
         std::array<uint8_t, key_len> key{};
         perform_kdf(key, secret, salt, label);
         return key;
      }

      /**
      * Derive a key
      * @tparam key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <size_t key_len>
      std::array<uint8_t, key_len> derive_key(std::span<const uint8_t> secret,
                                              std::span<const uint8_t> salt = {},
                                              std::string_view label = "") {
         return derive_key<key_len>(secret, salt, _as_span(label));
      }

      /**
      * Derive a key
      * @tparam key_len the desired output length in bytes
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      * @return the derived key
      */
      template <size_t key_len>
      std::array<uint8_t, key_len> derive_key(std::span<const uint8_t> secret,
                                              std::string_view salt = "",
                                              std::string_view label = "") {
         return derive_key<key_len>(secret, _as_span(salt), _as_span(label));
      }

      /**
      * @return new object representing the same algorithm as *this
      */
      virtual std::unique_ptr<KDF> new_object() const = 0;

      /**
      * @return new object representing the same algorithm as *this
      */
      KDF* clone() const { return this->new_object().release(); }

   protected:
      /**
      * Internal customization point for subclasses
      *
      * The byte size of the @p key span is the number of bytes to be produced
      * by the concrete key derivation function.
      *
      * @param key the output buffer for the to-be-derived key
      * @param secret the secret input
      * @param salt a diversifier
      * @param label purpose for the derived keying material
      */
      virtual void perform_kdf(std::span<uint8_t> key,
                               std::span<const uint8_t> secret,
                               std::span<const uint8_t> salt,
                               std::span<const uint8_t> label) const = 0;

   private:
      static std::span<const uint8_t> _as_span(std::string_view s);
};

/**
* Factory method for KDF (key derivation function)
* @param algo_spec the name of the KDF to create
* @return pointer to newly allocated object of that type
*
* Prefer KDF::create
*/
BOTAN_DEPRECATED("Use KDF::create")

inline KDF* get_kdf(std::string_view algo_spec) {
   if(algo_spec == "Raw") {
      return nullptr;
   }

   return KDF::create_or_throw(algo_spec).release();
}

}  // namespace Botan

BOTAN_FUTURE_INTERNAL_HEADER(mem_ops.h)

/*
The header mem_ops.h previously included the contents of allocator.h

Library code should always include allocator.h to see these
declarations; however when we are not building the library continue to
include the header here to avoid breaking application code.
*/
#if !defined(BOTAN_IS_BEING_BUILT)
#endif

namespace Botan {

/**
* Scrub memory contents in a way that a compiler should not elide,
* using some system specific technique. Note that this function might
* not zero the memory (for example, in some hypothetical
* implementation it might combine the memory contents with the output
* of a system PRNG), but if you can detect any difference in behavior
* at runtime then the clearing is side-effecting and you can just
* use `clear_mem`.
*
* Use this function to scrub memory just before deallocating it, or on
* a stack buffer before returning from the function.
*
* @param ptr a pointer to memory to scrub
* @param n the number of bytes pointed to by ptr
*/
BOTAN_PUBLIC_API(2, 0) void secure_scrub_memory(void* ptr, size_t n);

/**
* Scrub memory contents in a way that a compiler should not elide,
* using some system specific technique. Note that this function might
* not zero the memory.
*
* @param data  the data region to be scrubbed
*/
void secure_scrub_memory(ranges::contiguous_output_range auto&& data) {
   secure_scrub_memory(std::ranges::data(data), ranges::size_bytes(data));
}

/**
* Memory comparison, input insensitive
* @param x a pointer to an array
* @param y a pointer to another array
* @param len the number of Ts in x and y
* @return 0xFF iff x[i] == y[i] forall i in [0...n) or 0x00 otherwise
*/
BOTAN_DEPRECATED("This function is deprecated, use constant_time_compare()")
BOTAN_PUBLIC_API(2, 9) uint8_t ct_compare_u8(const uint8_t x[], const uint8_t y[], size_t len);

/**
 * Memory comparison, input insensitive
 * @param x a range of bytes
 * @param y another range of bytes
 * @return true iff x and y have equal lengths and x[i] == y[i] forall i in [0...n)
 */
BOTAN_PUBLIC_API(3, 3) bool constant_time_compare(std::span<const uint8_t> x, std::span<const uint8_t> y);

/**
* Memory comparison, input insensitive
* @param x a pointer to an array
* @param y a pointer to another array
* @param len the number of Ts in x and y
* @return true iff x[i] == y[i] forall i in [0...n)
*/
inline bool constant_time_compare(const uint8_t x[], const uint8_t y[], size_t len) {
   // simply assumes that *x and *y point to len allocated bytes at least
   return constant_time_compare({x, len}, {y, len});
}

/**
* Zero out some bytes. Warning: use secure_scrub_memory instead if the
* memory is about to be freed or otherwise the compiler thinks it can
* elide the writes.
*
* @param ptr a pointer to memory to zero
* @param bytes the number of bytes to zero in ptr
*/
inline constexpr void clear_bytes(void* ptr, size_t bytes) {
   if(bytes > 0) {
      std::memset(ptr, 0, bytes);
   }
}

/**
* Zero memory before use. This simply calls memset and should not be
* used in cases where the compiler cannot see the call as a
* side-effecting operation (for example, if calling clear_mem before
* deallocating memory, the compiler would be allowed to omit the call
* to memset entirely under the as-if rule.)
*
* @param ptr a pointer to an array of Ts to zero
* @param n the number of Ts pointed to by ptr
*/
template <typename T>
inline constexpr void clear_mem(T* ptr, size_t n) {
   clear_bytes(ptr, sizeof(T) * n);
}

/**
* Zero memory before use. This simply calls memset and should not be
* used in cases where the compiler cannot see the call as a
* side-effecting operation.
*
* @param mem a contiguous range of Ts to zero
*/
template <ranges::contiguous_output_range R>
inline constexpr void clear_mem(R&& mem)  // NOLINT(*-missing-std-forward)
   requires std::is_trivially_copyable_v<std::ranges::range_value_t<R>>
{
   clear_bytes(std::ranges::data(mem), ranges::size_bytes(mem));
}

/**
* Copy memory
* @param out the destination array
* @param in the source array
* @param n the number of elements of in/out
*/
template <typename T>
   requires std::is_trivial_v<std::decay_t<T>>
inline constexpr void copy_mem(T* out, const T* in, size_t n) {
   BOTAN_ASSERT_IMPLICATION(n > 0, in != nullptr && out != nullptr, "If n > 0 then args are not null");

   if(in != nullptr && out != nullptr && n > 0) {
      std::memmove(out, in, sizeof(T) * n);
   }
}

/**
* Copy memory
* @param out the destination array
* @param in the source array
*/
template <ranges::contiguous_output_range OutR, ranges::contiguous_range InR>
   requires std::is_same_v<std::ranges::range_value_t<OutR>, std::ranges::range_value_t<InR>> &&
            std::is_trivially_copyable_v<std::ranges::range_value_t<InR>>
inline constexpr void copy_mem(OutR&& out /* NOLINT(*-std-forward) */, const InR& in) {
   ranges::assert_equal_byte_lengths(out, in);
   if(std::is_constant_evaluated()) {
      std::copy(std::ranges::begin(in), std::ranges::end(in), std::ranges::begin(out));
   } else if(ranges::size_bytes(out) > 0) {
      std::memmove(std::ranges::data(out), std::ranges::data(in), ranges::size_bytes(out));
   }
}

/**
 * Copy a range of a trivially copyable type into another range of trivially
 * copyable type of matching byte length.
 */
template <ranges::contiguous_output_range ToR, ranges::contiguous_range FromR>
   requires std::is_trivially_copyable_v<std::ranges::range_value_t<FromR>> &&
            std::is_trivially_copyable_v<std::ranges::range_value_t<ToR>>
inline constexpr void typecast_copy(ToR&& out /* NOLINT(*-std-forward) */, const FromR& in) {
   ranges::assert_equal_byte_lengths(out, in);
   std::memcpy(std::ranges::data(out), std::ranges::data(in), ranges::size_bytes(out));
}

/**
 * Copy a range of trivially copyable type into an instance of trivially
 * copyable type with matching length.
 */
template <typename ToT, ranges::contiguous_range FromR>
   requires std::is_trivially_copyable_v<std::ranges::range_value_t<FromR>> && std::is_trivially_copyable_v<ToT> &&
            (!std::ranges::range<ToT>)
inline constexpr void typecast_copy(ToT& out, const FromR& in) {
   typecast_copy(std::span<ToT, 1>(&out, 1), in);
}

/**
 * Copy an instance of trivially copyable type into a range of trivially
 * copyable type with matching length.
 */
template <ranges::contiguous_output_range ToR, typename FromT>
   requires std::is_trivially_copyable_v<FromT> &&
            (!std::ranges::range<FromT>) && std::is_trivially_copyable_v<std::ranges::range_value_t<ToR>>
inline constexpr void typecast_copy(ToR&& out /* NOLINT(*-std-forward) */, const FromT& in) {
   typecast_copy(out, std::span<const FromT, 1>(&in, 1));
}

/**
 * Create a trivial type by bit-casting a range of trivially copyable type with
 * matching length into it.
 */
template <typename ToT, ranges::contiguous_range FromR>
   requires std::is_default_constructible_v<ToT> && std::is_trivially_copyable_v<ToT> &&
            std::is_trivially_copyable_v<std::ranges::range_value_t<FromR>>
inline constexpr ToT typecast_copy(const FromR& src) {
   ToT dst;  // NOLINT(*-member-init)
   typecast_copy(dst, src);
   return dst;
}

// TODO: deprecate and replace
template <typename T>
inline constexpr void typecast_copy(uint8_t out[], T in[], size_t N)
   requires std::is_trivially_copyable_v<T>
{
   // asserts that *in and *out point to the correct amount of memory
   typecast_copy(std::span<uint8_t>(out, sizeof(T) * N), std::span<const T>(in, N));
}

// TODO: deprecate and replace
template <typename T>
inline constexpr void typecast_copy(T out[], const uint8_t in[], size_t N)
   requires std::is_trivial_v<T>
{
   // asserts that *in and *out point to the correct amount of memory
   typecast_copy(std::span<T>(out, N), std::span<const uint8_t>(in, N * sizeof(T)));
}

// TODO: deprecate and replace
template <typename T>
inline constexpr void typecast_copy(uint8_t out[], const T& in) {
   // asserts that *out points to the correct amount of memory
   typecast_copy(std::span<uint8_t, sizeof(T)>(out, sizeof(T)), in);
}

// TODO: deprecate and replace
template <typename T>
   requires std::is_trivial_v<std::decay_t<T>>
inline constexpr void typecast_copy(T& out, const uint8_t in[]) {
   // asserts that *in points to the correct amount of memory
   typecast_copy(out, std::span<const uint8_t, sizeof(T)>(in, sizeof(T)));
}

// TODO: deprecate and replace
template <typename To>
   requires std::is_trivial_v<To>
inline constexpr To typecast_copy(const uint8_t src[]) noexcept {
   // asserts that *src points to the correct amount of memory
   return typecast_copy<To>(std::span<const uint8_t, sizeof(To)>(src, sizeof(To)));
}

#if !defined(BOTAN_IS_BEING_BUILT)
/**
* Set memory to a fixed value
* @param ptr a pointer to an array of bytes
* @param n the number of Ts pointed to by ptr
* @param val the value to set each byte to
*/
BOTAN_DEPRECATED("This function is deprecated") inline constexpr void set_mem(uint8_t* ptr, size_t n, uint8_t val) {
   if(n > 0) {
      std::memset(ptr, val, n);
   }
}
#endif

#if !defined(BOTAN_IS_BEING_BUILT)
inline const uint8_t* cast_char_ptr_to_uint8(const char* s) {
   return reinterpret_cast<const uint8_t*>(s);
}

inline uint8_t* cast_char_ptr_to_uint8(char* s) {
   return reinterpret_cast<uint8_t*>(s);
}
#endif

inline const char* cast_uint8_ptr_to_char(const uint8_t* b) {
   return reinterpret_cast<const char*>(b);
}

inline char* cast_uint8_ptr_to_char(uint8_t* b) {
   return reinterpret_cast<char*>(b);
}

#if !defined(BOTAN_IS_BEING_BUILT)
/**
* Memory comparison, input insensitive
* @param p1 a pointer to an array
* @param p2 a pointer to another array
* @param n the number of Ts in p1 and p2
* @return true iff p1[i] == p2[i] forall i in [0...n)
*/
template <typename T>
BOTAN_DEPRECATED("This function is deprecated")
inline bool same_mem(const T* p1, const T* p2, size_t n) {
   volatile T difference = 0;

   for(size_t i = 0; i != n; ++i) {
      difference = difference | (p1[i] ^ p2[i]);
   }

   return difference == 0;
}
#endif

#if !defined(BOTAN_IS_BEING_BUILT)

template <typename T, typename Alloc>
BOTAN_DEPRECATED("The buffer_insert functions are deprecated")
size_t buffer_insert(std::vector<T, Alloc>& buf, size_t buf_offset, const T input[], size_t input_length) {
   BOTAN_ASSERT_NOMSG(buf_offset <= buf.size());
   const size_t to_copy = std::min(input_length, buf.size() - buf_offset);
   if(to_copy > 0) {
      copy_mem(&buf[buf_offset], input, to_copy);
   }
   return to_copy;
}

template <typename T, typename Alloc, typename Alloc2>
BOTAN_DEPRECATED("The buffer_insert functions are deprecated")
size_t buffer_insert(std::vector<T, Alloc>& buf, size_t buf_offset, const std::vector<T, Alloc2>& input) {
   BOTAN_ASSERT_NOMSG(buf_offset <= buf.size());
   const size_t to_copy = std::min(input.size(), buf.size() - buf_offset);
   if(to_copy > 0) {
      copy_mem(&buf[buf_offset], input.data(), to_copy);
   }
   return to_copy;
}

#endif

/**
* XOR arrays. Postcondition out[i] = in[i] ^ out[i] forall i = 0...length
* @param out the input/output range
* @param in the read-only input range
*/
inline constexpr void xor_buf(ranges::contiguous_output_range<uint8_t> auto&& out,
                              ranges::contiguous_range<uint8_t> auto&& in) {
   ranges::assert_equal_byte_lengths(out, in);

   std::span<uint8_t> o(out);
   std::span<const uint8_t> i(in);

   for(; o.size_bytes() >= 32; o = o.subspan(32), i = i.subspan(32)) {
      auto x = typecast_copy<std::array<uint64_t, 4>>(o.template first<32>());
      const auto y = typecast_copy<std::array<uint64_t, 4>>(i.template first<32>());

      x[0] ^= y[0];
      x[1] ^= y[1];
      x[2] ^= y[2];
      x[3] ^= y[3];

      typecast_copy(o.template first<32>(), x);
   }

   for(size_t off = 0; off != o.size_bytes(); ++off) {
      o[off] ^= i[off];
   }
}

/**
* XOR arrays. Postcondition out[i] = in1[i] ^ in2[i] forall i = 0...length
* @param out the output range
* @param in1 the first input range
* @param in2 the second input range
*/
inline constexpr void xor_buf(ranges::contiguous_output_range<uint8_t> auto&& out,
                              ranges::contiguous_range<uint8_t> auto&& in1,
                              ranges::contiguous_range<uint8_t> auto&& in2) {
   ranges::assert_equal_byte_lengths(out, in1, in2);

   std::span o{out};
   std::span i1{in1};
   std::span i2{in2};

   for(; o.size_bytes() >= 32; o = o.subspan(32), i1 = i1.subspan(32), i2 = i2.subspan(32)) {
      auto x = typecast_copy<std::array<uint64_t, 4>>(i1.template first<32>());
      const auto y = typecast_copy<std::array<uint64_t, 4>>(i2.template first<32>());

      x[0] ^= y[0];
      x[1] ^= y[1];
      x[2] ^= y[2];
      x[3] ^= y[3];

      typecast_copy(o.template first<32>(), x);
   }

   for(size_t off = 0; off != o.size_bytes(); ++off) {
      o[off] = i1[off] ^ i2[off];
   }
}

/**
* XOR arrays. Postcondition out[i] = in[i] ^ out[i] forall i = 0...length
* @param out the input/output buffer
* @param in the read-only input buffer
* @param length the length of the buffers
*/
inline void xor_buf(uint8_t out[], const uint8_t in[], size_t length) {
   // simply assumes that *out and *in point to "length" allocated bytes at least
   xor_buf(std::span{out, length}, std::span{in, length});
}

/**
* XOR arrays. Postcondition out[i] = in[i] ^ in2[i] forall i = 0...length
* @param out the output buffer
* @param in the first input buffer
* @param in2 the second input buffer
* @param length the length of the three buffers
*/
inline void xor_buf(uint8_t out[], const uint8_t in[], const uint8_t in2[], size_t length) {
   // simply assumes that *out, *in, and *in2 point to "length" allocated bytes at least
   xor_buf(std::span{out, length}, std::span{in, length}, std::span{in2, length});
}

// TODO: deprecate and replace, use .subspan()
inline void xor_buf(std::span<uint8_t> out, std::span<const uint8_t> in, size_t n) {
   BOTAN_ARG_CHECK(out.size() >= n, "output span is too small");
   BOTAN_ARG_CHECK(in.size() >= n, "input span is too small");
   xor_buf(out.first(n), in.first(n));
}

// TODO: deprecate and replace, use .subspan()
template <typename Alloc>
void xor_buf(std::vector<uint8_t, Alloc>& out, const uint8_t* in, size_t n) {
   BOTAN_ARG_CHECK(out.size() >= n, "output vector is too small");
   // simply assumes that *in points to "n" allocated bytes at least
   xor_buf(std::span{out}.first(n), std::span{in, n});
}

// TODO: deprecate and replace
template <typename Alloc, typename Alloc2>
void xor_buf(std::vector<uint8_t, Alloc>& out, const uint8_t* in, const std::vector<uint8_t, Alloc2>& in2, size_t n) {
   BOTAN_ARG_CHECK(out.size() >= n, "output vector is too small");
   BOTAN_ARG_CHECK(in2.size() >= n, "input vector is too small");
   // simply assumes that *in points to "n" allocated bytes at least
   xor_buf(std::span{out}.first(n), std::span{in, n}, std::span{in2}.first(n));
}

template <typename Alloc, typename Alloc2>
std::vector<uint8_t, Alloc>& operator^=(std::vector<uint8_t, Alloc>& out, const std::vector<uint8_t, Alloc2>& in) {
   if(out.size() < in.size()) {
      out.resize(in.size());
   }

   xor_buf(std::span{out}.first(in.size()), in);
   return out;
}

}  // namespace Botan

BOTAN_FUTURE_INTERNAL_HEADER(numthry.h)

namespace Botan {

class RandomNumberGenerator;

/**
* Return the absolute value
* @param n an integer
* @return absolute value of n
*/
inline BigInt abs(const BigInt& n) {
   return n.abs();
}

/**
* Compute the greatest common divisor
* @param x a positive integer
* @param y a positive integer
* @return gcd(x,y)
*/
BigInt BOTAN_PUBLIC_API(2, 0) gcd(const BigInt& x, const BigInt& y);

/**
* Least common multiple
* @param x a positive integer
* @param y a positive integer
* @return z, smallest integer such that z % x == 0 and z % y == 0
*/
BigInt BOTAN_PUBLIC_API(2, 0) lcm(const BigInt& x, const BigInt& y);

/**
* @param x an integer
* @return (x*x)
*/
BigInt BOTAN_PUBLIC_API(2, 0) square(const BigInt& x);

/**
* Modular inversion. This algorithm is const time with respect to x,
* as long as x is less than modulus. It also avoids leaking
* information about the modulus, except that it does leak which of 3
* categories the modulus is in: an odd integer, a power of 2, or some
* other even number, and if the modulus is even, leaks the power of 2
* which divides the modulus.
*
* @param x a positive integer
* @param modulus a positive integer
* @return y st (x*y) % modulus == 1 or 0 if no such value
*/
BigInt BOTAN_PUBLIC_API(2, 0) inverse_mod(const BigInt& x, const BigInt& modulus);

/**
* Compute the Jacobi symbol. If n is prime, this is equivalent
* to the Legendre symbol.
* @see http://mathworld.wolfram.com/JacobiSymbol.html
*
* @param a is a non-negative integer
* @param n is an odd integer > 1
* @return (n / m)
*/
int32_t BOTAN_PUBLIC_API(2, 0) jacobi(BigInt a, BigInt n);

/**
* Modular exponentiation
* @param b an integer base
* @param x a positive exponent
* @param m a positive modulus
* @return (b^x) % m
*/
BigInt BOTAN_PUBLIC_API(2, 0) power_mod(const BigInt& b, const BigInt& x, const BigInt& m);

/**
* Compute the square root of x modulo a prime using the Tonelli-Shanks
* algorithm. This algorithm is primarily used for EC point
* decompression which takes only public inputs, as a consequence it is
* not written to be constant-time and may leak side-channel information
* about its arguments.
*
* @param x the input
* @param p the prime modulus
* @return y such that (y*y)%p == x, or -1 if no such integer
*/
BigInt BOTAN_PUBLIC_API(3, 0) sqrt_modulo_prime(const BigInt& x, const BigInt& p);

/**
* @param x an integer
* @return count of the low zero bits in x, or, equivalently, the
*         largest value of n such that 2^n divides x evenly. Returns
*         zero if x is equal to zero.
*/
size_t BOTAN_PUBLIC_API(2, 0) low_zero_bits(const BigInt& x);

/**
* Check for primality
*
* This uses probabilistic algorithms - there is some non-zero (but very low)
* probability that this function will return true even if *n* is actually
* composite.
*
* @param n a positive integer to test for primality
* @param rng a random number generator
* @param prob chance of false positive is bounded by 1/2**prob
* @param is_random true if n was randomly chosen by us
* @return true if all primality tests passed, otherwise false
*/
bool BOTAN_PUBLIC_API(2, 0)
   is_prime(const BigInt& n, RandomNumberGenerator& rng, size_t prob = 64, bool is_random = false);

/**
* Test if the positive integer x is a perfect square ie if there
* exists some positive integer y st y*y == x
* See FIPS 186-4 sec C.4
* @return 0 if the integer is not a perfect square, otherwise
*         returns the positive y st y*y == x
*/
BigInt BOTAN_PUBLIC_API(2, 8) is_perfect_square(const BigInt& x);

/**
* Randomly generate a prime suitable for discrete logarithm parameters
* @param rng a random number generator
* @param bits how large the resulting prime should be in bits
* @param coprime a positive integer that (prime - 1) should be coprime to
* @param equiv a non-negative number that the result should be
               equivalent to modulo equiv_mod
* @param equiv_mod the modulus equiv should be checked against
* @param prob use test so false positive is bounded by 1/2**prob
* @return random prime with the specified criteria
*/
BigInt BOTAN_PUBLIC_API(2, 0) random_prime(RandomNumberGenerator& rng,
                                           size_t bits,
                                           const BigInt& coprime = BigInt::from_u64(0),
                                           size_t equiv = 1,
                                           size_t equiv_mod = 2,
                                           size_t prob = 128);

/**
* Generate a prime suitable for RSA p/q
* @param keygen_rng a random number generator
* @param prime_test_rng a random number generator
* @param bits how large the resulting prime should be in bits (must be >= 512)
* @param coprime a positive integer that (prime - 1) should be coprime to
* @param prob use test so false positive is bounded by 1/2**prob
* @return random prime with the specified criteria
*/
BigInt BOTAN_PUBLIC_API(2, 7) generate_rsa_prime(RandomNumberGenerator& keygen_rng,
                                                 RandomNumberGenerator& prime_test_rng,
                                                 size_t bits,
                                                 const BigInt& coprime,
                                                 size_t prob = 128);

/**
* Return a 'safe' prime, of the form p=2*q+1 with q prime
* @param rng a random number generator
* @param bits is how long the resulting prime should be
* @return prime randomly chosen from safe primes of length bits
*/
BigInt BOTAN_PUBLIC_API(2, 0) random_safe_prime(RandomNumberGenerator& rng, size_t bits);

/**
* The size of the PRIMES[] array
*/
const size_t PRIME_TABLE_SIZE = 6541;

/**
* A const array of all odd primes less than 65535
*/
extern const uint16_t BOTAN_PUBLIC_API(2, 0) PRIMES[];

}  // namespace Botan

BOTAN_DEPRECATED_HEADER("oids.h")

namespace Botan::OIDS {

/**
* Register an OID to string mapping.
* @param oid the oid to register
* @param name the name to be associated with the oid
*/
BOTAN_DEPRECATED("Use OID::register_oid") inline void add_oid(const OID& oid, std::string_view name) {
   OID::register_oid(oid, name);
}

BOTAN_DEPRECATED("Use OID::register_oid") BOTAN_UNSTABLE_API void add_oid2str(const OID& oid, std::string_view name);

BOTAN_DEPRECATED("Use OID::register_oid") BOTAN_UNSTABLE_API void add_str2oid(const OID& oid, std::string_view name);

BOTAN_DEPRECATED("Use OID::register_oid") inline void add_oidstr(const char* oidstr, const char* name) {
   OID::register_oid(OID(oidstr), name);
}

/**
* Resolve an OID
* @param oid the OID to look up
* @return name associated with this OID, or an empty string
*/
BOTAN_DEPRECATED("Use OID::human_name_or_empty") inline std::string oid2str_or_empty(const OID& oid) {
   return oid.human_name_or_empty();
}

/**
* Find the OID to a name. The lookup will be performed in the
* general OID section of the configuration.
* @param name the name to resolve
* @return OID associated with the specified name
*/
BOTAN_DEPRECATED("Use OID::from_name") inline OID str2oid_or_empty(std::string_view name) {
   return OID::from_name(name).value_or(OID());
}

BOTAN_DEPRECATED("Use OID::human_name_or_empty") inline std::string oid2str_or_throw(const OID& oid) {
   std::string s = oid.human_name_or_empty();
   if(s.empty()) {
      throw Lookup_Error("No name associated with OID " + oid.to_string());
   }
   return s;
}

BOTAN_DEPRECATED("Use OID::human_name_or_empty") inline std::string lookup(const OID& oid) {
   return oid.human_name_or_empty();
}

BOTAN_DEPRECATED("Use OID::from_name") inline OID lookup(std::string_view name) {
   return OID::from_name(name).value_or(OID());
}

}  // namespace Botan::OIDS

/*
* This entire interface is deprecated. Use the interface in pwdhash.h
*/
BOTAN_DEPRECATED_HEADER("pbkdf.h")

namespace Botan {

/**
* Base class for PBKDF (password based key derivation function)
* implementations. Converts a password into a key using a salt
* and iterated hashing to make brute force attacks harder.
*
* Starting in 2.8 this functionality is also offered by PasswordHash.
*
* @warning
* This class will be removed in a future major release. Use PasswordHash
*/
class BOTAN_PUBLIC_API(2, 0) PBKDF /* NOLINT(*-special-member-functions) */ {
   public:
      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to choose
      * @return a null pointer if the algo/provider combination cannot be found
      */
      BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
      static std::unique_ptr<PBKDF> create(std::string_view algo_spec, std::string_view provider = "");

      /**
      * Create an instance based on a name, or throw if the
      * algo/provider combination cannot be found. If provider is
      * empty then best available is chosen.
      */
      BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
      static std::unique_ptr<PBKDF> create_or_throw(std::string_view algo_spec, std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * @return new instance of this same algorithm
      */
      virtual std::unique_ptr<PBKDF> new_object() const = 0;

      /**
      * @return new instance of this same algorithm
      */
      PBKDF* clone() const { return this->new_object().release(); }

      /**
      * @return name of this PBKDF
      */
      virtual std::string name() const = 0;

      virtual ~PBKDF() = default;

      /**
      * Derive a key from a passphrase for a number of iterations
      * specified by either iterations or if iterations == 0 then
      * running until msec time has elapsed.
      *
      * @param out buffer to store the derived key, must be of out_len bytes
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param iterations the number of iterations to use (use 10K or more)
      * @param msec if iterations is zero, then instead the PBKDF is
      *        run until msec milliseconds has passed.
      * @return the number of iterations performed
      */
      virtual size_t pbkdf(uint8_t out[],
                           size_t out_len,
                           std::string_view passphrase,
                           const uint8_t salt[],
                           size_t salt_len,
                           size_t iterations,
                           std::chrono::milliseconds msec) const = 0;

      /**
      * Derive a key from a passphrase for a number of iterations.
      *
      * @param out buffer to store the derived key, must be of out_len bytes
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param iterations the number of iterations to use (use 10K or more)
      */
      void pbkdf_iterations(uint8_t out[],
                            size_t out_len,
                            std::string_view passphrase,
                            const uint8_t salt[],
                            size_t salt_len,
                            size_t iterations) const;

      /**
      * Derive a key from a passphrase, running until msec time has elapsed.
      *
      * @param out buffer to store the derived key, must be of out_len bytes
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param msec if iterations is zero, then instead the PBKDF is
      *        run until msec milliseconds has passed.
      * @param iterations set to the number iterations executed
      */
      void pbkdf_timed(uint8_t out[],
                       size_t out_len,
                       std::string_view passphrase,
                       const uint8_t salt[],
                       size_t salt_len,
                       std::chrono::milliseconds msec,
                       size_t& iterations) const;

      /**
      * Derive a key from a passphrase for a number of iterations.
      *
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param iterations the number of iterations to use (use 10K or more)
      * @return the derived key
      */
      secure_vector<uint8_t> pbkdf_iterations(
         size_t out_len, std::string_view passphrase, const uint8_t salt[], size_t salt_len, size_t iterations) const;

      /**
      * Derive a key from a passphrase, running until msec time has elapsed.
      *
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param msec if iterations is zero, then instead the PBKDF is
      *        run until msec milliseconds has passed.
      * @param iterations set to the number iterations executed
      * @return the derived key
      */
      secure_vector<uint8_t> pbkdf_timed(size_t out_len,
                                         std::string_view passphrase,
                                         const uint8_t salt[],
                                         size_t salt_len,
                                         std::chrono::milliseconds msec,
                                         size_t& iterations) const;

      // Following kept for compat with 1.10:

      /**
      * Derive a key from a passphrase
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param iterations the number of iterations to use (use 10K or more)
      */
      OctetString derive_key(
         size_t out_len, std::string_view passphrase, const uint8_t salt[], size_t salt_len, size_t iterations) const {
         return OctetString(pbkdf_iterations(out_len, passphrase, salt, salt_len, iterations));
      }

      /**
      * Derive a key from a passphrase
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param iterations the number of iterations to use (use 10K or more)
      */
      template <typename Alloc>
      OctetString derive_key(size_t out_len,
                             std::string_view passphrase,
                             const std::vector<uint8_t, Alloc>& salt,
                             size_t iterations) const {
         return OctetString(pbkdf_iterations(out_len, passphrase, salt.data(), salt.size(), iterations));
      }

      /**
      * Derive a key from a passphrase
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param msec is how long to run the PBKDF
      * @param iterations is set to the number of iterations used
      */
      OctetString derive_key(size_t out_len,
                             std::string_view passphrase,
                             const uint8_t salt[],
                             size_t salt_len,
                             std::chrono::milliseconds msec,
                             size_t& iterations) const {
         return OctetString(pbkdf_timed(out_len, passphrase, salt, salt_len, msec, iterations));
      }

      /**
      * Derive a key from a passphrase using a certain amount of time
      * @param out_len the desired length of the key to produce
      * @param passphrase the password to derive the key from
      * @param salt a randomly chosen salt
      * @param msec is how long to run the PBKDF
      * @param iterations is set to the number of iterations used
      */
      template <typename Alloc>
      OctetString derive_key(size_t out_len,
                             std::string_view passphrase,
                             const std::vector<uint8_t, Alloc>& salt,
                             std::chrono::milliseconds msec,
                             size_t& iterations) const {
         return OctetString(pbkdf_timed(out_len, passphrase, salt.data(), salt.size(), msec, iterations));
      }
};

/*
* Compatibility typedef
*/
typedef PBKDF S2K;

/**
* Password based key derivation function factory method
* @param algo_spec the name of the desired PBKDF algorithm
* @param provider the provider to use
* @return pointer to newly allocated object of that type
*/
BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
inline PBKDF* get_pbkdf(std::string_view algo_spec, std::string_view provider = "") {
   return PBKDF::create_or_throw(algo_spec, provider).release();
}

BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash") inline PBKDF* get_s2k(std::string_view algo_spec) {
   return PBKDF::create_or_throw(algo_spec).release();
}

}  // namespace Botan

namespace Botan {

/**
* Base class for password based key derivation functions.
*
* Converts a password into a key using a salt and iterated hashing to
* make brute force attacks harder.
*/
class BOTAN_PUBLIC_API(2, 8) PasswordHash /* NOLINT(*-special-member-functions) */ {
   public:
      virtual ~PasswordHash() = default;

      virtual std::string to_string() const = 0;

      /**
      * Most password hashes have some notion of iterations.
      */
      virtual size_t iterations() const = 0;

      /**
      * Some password hashing algorithms have a parameter which controls how
      * much memory is used. If not supported by some algorithm, returns 0.
      */
      virtual size_t memory_param() const { return 0; }

      /**
      * Some password hashing algorithms have a parallelism parameter.
      * If the algorithm does not support this notion, then the
      * function returns zero. This allows distinguishing between a
      * password hash which just does not support parallel operation,
      * vs one that does support parallel operation but which has been
      * configured to use a single lane.
      */
      virtual size_t parallelism() const { return 0; }

      /**
      * Returns an estimate of the total number of bytes required to perform this
      * key derivation.
      *
      * If this algorithm uses a small and constant amount of memory, with no
      * effort made towards being memory hard, this function returns 0.
      */
      virtual size_t total_memory_usage() const { return 0; }

      /**
      * @returns true if this password hash supports supplying a key
      */
      virtual bool supports_keyed_operation() const { return false; }

      /**
      * @returns true if this password hash supports supplying associated data
      */
      virtual bool supports_associated_data() const { return false; }

      /**
      * Hash a password into a bitstring
      *
      * Derive a key from the specified @p password and  @p salt, placing it into
      * @p out.
      *
      * @param out a span where the derived key will be placed
      * @param password the password to derive the key from
      * @param salt a randomly chosen salt
      *
      * This function is const, but is not thread safe. Different threads should
      * either use unique objects, or serialize all access.
      */
      void hash(std::span<uint8_t> out, std::string_view password, std::span<const uint8_t> salt) const {
         this->derive_key(out.data(), out.size(), password.data(), password.size(), salt.data(), salt.size());
      }

      /**
      * Hash a password into a bitstring
      *
      * Derive a key from the specified @p password, @p salt, @p
      * associated_data, and secret @p key, placing it into @p out. The
      * @p associated_data and @p key are both allowed to be empty. Currently
      * non-empty AD/key is only supported with Argon2.
      *
      * @param out a span where the derived key will be placed
      * @param password the password to derive the key from
      * @param salt a randomly chosen salt
      * @param associated_data some additional data
      * @param key a secret key
      *
      * This function is const, but is not thread safe. Different threads should
      * either use unique objects, or serialize all access.
      */
      void hash(std::span<uint8_t> out,
                std::string_view password,
                std::span<const uint8_t> salt,
                std::span<const uint8_t> associated_data,
                std::span<const uint8_t> key) const {
         this->derive_key(out.data(),
                          out.size(),
                          password.data(),
                          password.size(),
                          salt.data(),
                          salt.size(),
                          associated_data.data(),
                          associated_data.size(),
                          key.data(),
                          key.size());
      }

      /**
      * Derive a key from a password
      *
      * @param out buffer to store the derived key, must be of out_len bytes
      * @param out_len the desired length of the key to produce
      * @param password the password to derive the key from
      * @param password_len the length of password in bytes
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      *
      * This function is const, but is not thread safe. Different threads should
      * either use unique objects, or serialize all access.
      */
      virtual void derive_key(uint8_t out[],
                              size_t out_len,
                              const char* password,
                              size_t password_len,
                              const uint8_t salt[],
                              size_t salt_len) const = 0;

      /**
      * Derive a key from a password plus additional data and/or a secret key
      *
      * Currently this is only supported for Argon2. Using a non-empty AD or key
      * with other algorithms will cause a Not_Implemented exception.
      *
      * @param out buffer to store the derived key, must be of out_len bytes
      * @param out_len the desired length of the key to produce
      * @param password the password to derive the key from
      * @param password_len the length of password in bytes
      * @param salt a randomly chosen salt
      * @param salt_len length of salt in bytes
      * @param ad some additional data
      * @param ad_len length of ad in bytes
      * @param key a secret key
      * @param key_len length of key in bytes
      *
      * This function is const, but is not thread safe. Different threads should
      * either use unique objects, or serialize all access.
      */
      virtual void derive_key(uint8_t out[],
                              size_t out_len,
                              const char* password,
                              size_t password_len,
                              const uint8_t salt[],
                              size_t salt_len,
                              const uint8_t ad[],
                              size_t ad_len,
                              const uint8_t key[],
                              size_t key_len) const;
};

class BOTAN_PUBLIC_API(2, 8) PasswordHashFamily /* NOLINT(*-special-member-functions) */ {
   public:
      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to choose
      * @return a null pointer if the algo/provider combination cannot be found
      */
      static std::unique_ptr<PasswordHashFamily> create(std::string_view algo_spec, std::string_view provider = "");

      /**
      * Create an instance based on a name, or throw if the
      * algo/provider combination cannot be found. If provider is
      * empty then best available is chosen.
      */
      static std::unique_ptr<PasswordHashFamily> create_or_throw(std::string_view algo_spec,
                                                                 std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      virtual ~PasswordHashFamily() = default;

      /**
      * @return name of this PasswordHash
      */
      virtual std::string name() const = 0;

      /**
      * Return a new parameter set tuned for this machine
      *
      * Return a password hash instance tuned to run for approximately @p msec
      * milliseconds when producing an output of length @p output_length.
      * (Accuracy may vary, use the command line utility ``botan pbkdf_tune`` to
      * check.)
      *
      * The parameters will be selected to use at most @p max_memory_usage_mb
      * megabytes of memory, or if left as zero any size is allowed.
      *
      * This function works by running a short tuning loop to estimate the
      * performance of the algorithm, then scaling the parameters appropriately
      * to hit the target size. The length of time the tuning loop runs can be
      * controlled using the @p tuning_msec parameter.
      *
      * @param output_length how long the output length will be
      * @param msec the desired execution time in milliseconds
      *
      * @param max_memory_usage_mb some password hash functions can use a
      * tunable amount of memory, in this case max_memory_usage limits the
      * amount of RAM the returned parameters will require, in mebibytes (2**20
      * bytes). It may require some small amount above the request. Set to zero
      * to place no limit at all.
      * @param tuning_msec how long to run the tuning loop
      */
      virtual std::unique_ptr<PasswordHash> tune(
         size_t output_length,
         std::chrono::milliseconds msec,
         size_t max_memory_usage_mb = 0,
         std::chrono::milliseconds tuning_msec = std::chrono::milliseconds(10)) const = 0;

      /**
      * Return some default parameter set for this PBKDF that should be good
      * enough for most users. The value returned may change over time as
      * processing power and attacks improve.
      */
      virtual std::unique_ptr<PasswordHash> default_params() const = 0;

      /**
      * Return a parameter chosen based on a rough approximation with the
      * specified iteration count. The exact value this returns for a particular
      * algorithm may change from over time. Think of it as an alternative to
      * tune, where time is expressed in terms of PBKDF2 iterations rather than
      * milliseconds.
      */
      virtual std::unique_ptr<PasswordHash> from_iterations(size_t iterations) const = 0;

      /**
      * Create a password hash using some scheme specific format. Parameters are as follows:
      * - For PBKDF2, PGP-S2K, and Bcrypt-PBKDF, i1 is iterations
      * - Scrypt uses N, r, p for i{1-3}
      * - Argon2 family uses memory (in KB), iterations, and parallelism for i{1-3}
      *
      * All unneeded parameters should be set to 0 or left blank.
      */
      virtual std::unique_ptr<PasswordHash> from_params(size_t i1, size_t i2 = 0, size_t i3 = 0) const = 0;
};

}  // namespace Botan

// Use pwdhash.h
BOTAN_FUTURE_INTERNAL_HEADER(pbkdf2.h)

namespace Botan {

BOTAN_PUBLIC_API(2, 0)
size_t pbkdf2(MessageAuthenticationCode& prf,
              uint8_t out[],
              size_t out_len,
              std::string_view passphrase,
              const uint8_t salt[],
              size_t salt_len,
              size_t iterations,
              std::chrono::milliseconds msec);

/**
* Perform PBKDF2. The prf is assumed to be keyed already.
*/
BOTAN_PUBLIC_API(2, 8)
void pbkdf2(MessageAuthenticationCode& prf,
            uint8_t out[],
            size_t out_len,
            const uint8_t salt[],
            size_t salt_len,
            size_t iterations);

/**
* PBKDF2
*/
class BOTAN_PUBLIC_API(2, 8) PBKDF2 final : public PasswordHash {
   public:
      PBKDF2(const MessageAuthenticationCode& prf, size_t iter) : m_prf(prf.new_object()), m_iterations(iter) {}

      BOTAN_DEPRECATED("For runtime tuning use PBKDF2_Family::tune")
      PBKDF2(const MessageAuthenticationCode& prf, size_t olen, std::chrono::milliseconds msec);

      size_t iterations() const override { return m_iterations; }

      std::string to_string() const override;

      void derive_key(uint8_t out[],
                      size_t out_len,
                      const char* password,
                      size_t password_len,
                      const uint8_t salt[],
                      size_t salt_len) const override;

   private:
      std::unique_ptr<MessageAuthenticationCode> m_prf;
      size_t m_iterations;
};

/**
* Family of PKCS #5 PBKDF2 operations
*/
class BOTAN_PUBLIC_API(2, 8) PBKDF2_Family final : public PasswordHashFamily {
   public:
      BOTAN_FUTURE_EXPLICIT PBKDF2_Family(std::unique_ptr<MessageAuthenticationCode> prf) : m_prf(std::move(prf)) {}

      std::string name() const override;

      std::unique_ptr<PasswordHash> tune(size_t output_len,
                                         std::chrono::milliseconds msec,
                                         size_t max_memory,
                                         std::chrono::milliseconds tune_msec) const override;

      /**
      * Return some default parameter set for this PBKDF that should be good
      * enough for most users. The value returned may change over time as
      * processing power and attacks improve.
      */
      std::unique_ptr<PasswordHash> default_params() const override;

      std::unique_ptr<PasswordHash> from_iterations(size_t iter) const override;

      std::unique_ptr<PasswordHash> from_params(size_t iter, size_t /*unused*/, size_t /*unused*/) const override;

   private:
      std::unique_ptr<MessageAuthenticationCode> m_prf;
};

/**
* PKCS #5 PBKDF2 (old interface)
*/
class BOTAN_PUBLIC_API(2, 0) PKCS5_PBKDF2 final : public PBKDF {
   public:
      std::string name() const override;

      std::unique_ptr<PBKDF> new_object() const override;

      size_t pbkdf(uint8_t output_buf[],
                   size_t output_len,
                   std::string_view passphrase,
                   const uint8_t salt[],
                   size_t salt_len,
                   size_t iterations,
                   std::chrono::milliseconds msec) const override;

      /**
      * Create a PKCS #5 instance using the specified message auth code
      * @param mac_fn the MAC object to use as PRF
      */
      BOTAN_DEPRECATED("Use version taking unique_ptr")
      explicit PKCS5_PBKDF2(MessageAuthenticationCode* mac_fn) : m_mac(mac_fn) {}

      /**
      * Create a PKCS #5 instance using the specified message auth code
      * @param mac_fn the MAC object to use as PRF
      */
      BOTAN_DEPRECATED("Use PasswordHashFamily + PasswordHash")
      explicit PKCS5_PBKDF2(std::unique_ptr<MessageAuthenticationCode> mac_fn) : m_mac(std::move(mac_fn)) {}

   private:
      std::unique_ptr<MessageAuthenticationCode> m_mac;
};

}  // namespace Botan

namespace Botan {

class DataSource;

namespace PEM_Code {

/**
* Encode some binary data in PEM format
* @param data binary data to encode
* @param data_len length of binary data in bytes
* @param label PEM label put after BEGIN and END
* @param line_width after this many characters, a new line is inserted
*/
BOTAN_PUBLIC_API(2, 0)
std::string encode(const uint8_t data[], size_t data_len, std::string_view label, size_t line_width = 64);

/**
* Encode some binary data in PEM format
* @param data binary data to encode
* @param label PEM label
* @param line_width after this many characters, a new line is inserted
*/
template <typename Alloc>
std::string encode(const std::vector<uint8_t, Alloc>& data, std::string_view label, size_t line_width = 64) {
   return encode(data.data(), data.size(), label, line_width);
}

/**
* Decode PEM data
* @param pem a datasource containing PEM encoded data
* @param label is set to the PEM label found for later inspection
*/
BOTAN_PUBLIC_API(2, 0) secure_vector<uint8_t> decode(DataSource& pem, std::string& label);

/**
* Decode PEM data
* @param pem a string containing PEM encoded data
* @param label is set to the PEM label found for later inspection
*/
BOTAN_PUBLIC_API(2, 0) secure_vector<uint8_t> decode(std::string_view pem, std::string& label);

/**
* Decode PEM data
* @param pem a datasource containing PEM encoded data
* @param label is what we expect the label to be
*/
BOTAN_PUBLIC_API(2, 0)
secure_vector<uint8_t> decode_check_label(DataSource& pem, std::string_view label);

/**
* Decode PEM data
* @param pem a string containing PEM encoded data
* @param label is what we expect the label to be
*/
BOTAN_PUBLIC_API(2, 0)
secure_vector<uint8_t> decode_check_label(std::string_view pem, std::string_view label);

/**
* Heuristic test for PEM data.
*/
BOTAN_PUBLIC_API(2, 0) bool matches(DataSource& source, std::string_view extra = "", size_t search_range = 4096);

}  // namespace PEM_Code

}  // namespace Botan
namespace Botan::PK_Ops {

class Encryption;
class Decryption;
class Verification;
class Signature;
class Key_Agreement;
class KEM_Encryption;
class KEM_Decryption;

}  // namespace Botan::PK_Ops


namespace Botan {

class BigInt;
class RandomNumberGenerator;

/**
* Enumeration specifying the signature format.
*
* This is mostly used for requesting DER encoding of ECDSA signatures;
* most other algorithms only support "standard".
*/
enum class Signature_Format : uint8_t {
   Standard = 0,
   DerSequence = 1,

   IEEE_1363 BOTAN_DEPRECATED("Use Standard") = Standard,
   DER_SEQUENCE BOTAN_DEPRECATED("Use DerSequence") = DerSequence,
};

/**
* Enumeration of possible operations a public key could be used for.
*
* It is possible to query if a key supports a particular operation
* type using Asymmetric_Key::supports_operation()
*/
enum class PublicKeyOperation : uint8_t {
   Encryption,
   Signature,
   KeyEncapsulation,
   KeyAgreement,
};

class Private_Key;

/**
* An interface for objects that are keys in public key algorithms
*
* This is derived for both public and private keys
*/
class BOTAN_PUBLIC_API(3, 0) Asymmetric_Key /* NOLINT(*special-member-functions) */ {
   public:
      virtual ~Asymmetric_Key() = default;

      /**
      * Get the name of the underlying public key scheme.
      * @return name of the public key scheme
      */
      virtual std::string algo_name() const = 0;

      /**
      * Return the estimated strength of the underlying key against
      * the best currently known attack. Note that this ignores anything
      * but pure attacks against the key itself and do not take into
      * account padding schemes, usage mistakes, etc which might reduce
      * the strength. However it does suffice to provide an upper bound.
      *
      * @return estimated strength in bits
      */
      virtual size_t estimated_strength() const = 0;

      /**
      * Get the OID of the underlying public key scheme.
      * @return OID of the public key scheme
      */
      virtual OID object_identifier() const;

      /**
      * Access an algorithm specific field
      *
      * If the field is not known for this algorithm, an Invalid_Argument is
      * thrown. The interpretation of the result requires knowledge of which
      * algorithm is involved. For instance for RSA "p" represents one of the
      * secret primes, while for DSA "p" is the public prime.
      *
      * Some algorithms may not implement this method at all.
      *
      * This is primarily used to implement the FFI botan_pubkey_get_field
      * and botan_privkey_get_field functions.
      *
      * TODO(Botan4) Change this to return by value
      */
      virtual const BigInt& get_int_field(std::string_view field) const;

      /**
      * Return true if this key could be used for the specified type
      * of operation.
      */
      virtual bool supports_operation(PublicKeyOperation op) const = 0;

      /**
       * Generate another (cryptographically independent) key pair using the
       * same algorithm parameters as this key. This is most useful for algorithms
       * that support PublicKeyOperation::KeyAgreement to generate a fitting ephemeral
       * key pair. For other key types it might throw Not_Implemented.
       */
      virtual std::unique_ptr<Private_Key> generate_another(RandomNumberGenerator& rng) const = 0;

      /*
      * Test the key values for consistency.
      * @param rng rng to use
      * @param strong whether to perform strong and lengthy version of the test
      * @return true if the test is passed
      */
      virtual bool check_key(RandomNumberGenerator& rng, bool strong) const = 0;

      // Declarations for internal library functions not covered by SemVer follow

      /**
      * Certain signatures schemes such as ECDSA have more than
      * one element, and certain unfortunate protocols decided the
      * thing to do was not concatenate them as normally done, but
      * instead DER encode each of the elements as independent values.
      *
      * If this returns a value x then the signature is checked to
      * be exactly 2*x bytes and split in half for DER encoding.
      */
      virtual std::optional<size_t> _signature_element_size_for_DER_encoding() const { return {}; }

      /*
      * Return the format normally used by this algorithm for X.509 signatures
      */
      virtual Signature_Format _default_x509_signature_format() const;
};

/*
* Public Key Base Class.
*/
class BOTAN_PUBLIC_API(2, 0) Public_Key : public virtual Asymmetric_Key {
   public:
      /**
      * Return an integer value best approximating the length of the
      * primary security parameter. For example for RSA this will be
      * the size of the modulus, for ECDSA the size of the ECC group,
      * and for McEliece the size of the code will be returned.
      */
      virtual size_t key_length() const = 0;

      /**
      * Deprecated version of object_identifier
      */
      BOTAN_DEPRECATED("Use object_identifier") OID get_oid() const { return this->object_identifier(); }

      /**
      * @return X.509 AlgorithmIdentifier for this key
      */
      virtual AlgorithmIdentifier algorithm_identifier() const = 0;

      /**
      * @return binary public key bits, with no additional encoding
      *
      * For key agreements this is an alias for PK_Key_Agreement_Key::public_value.
      *
      * Note: some algorithms (for example RSA) do not have an obvious encoding
      * for this value due to having many different values, and thus throw
      * Not_Implemented when invoking this method.
      */
      virtual std::vector<uint8_t> raw_public_key_bits() const = 0;

      /**
      * @return BER encoded public key bits
      */
      virtual std::vector<uint8_t> public_key_bits() const = 0;

      /**
      * @return X.509 subject key encoding for this key object
      */
      std::vector<uint8_t> subject_public_key() const;

      /**
       * @return Hash of the subject public key
       */
      std::string fingerprint_public(std::string_view alg = "SHA-256") const;

      // Declarations for internal library functions not covered by SemVer follow

      /**
      * Returns more than 1 if the output of this algorithm
      * (ciphertext, signature) should be treated as more than one
      * value. This is used for algorithms like DSA and ECDSA, where
      * the (r,s) output pair can be encoded as either a plain binary
      * list or a TLV tagged DER encoding depending on the protocol.
      *
      * This function is public but applications should have few
      * reasons to ever call this.
      *
      * @return number of message parts
      */
      BOTAN_DEPRECATED("Deprecated no replacement") size_t message_parts() const {
         return _signature_element_size_for_DER_encoding() ? 2 : 1;
      }

      /**
      * Returns how large each of the message parts referred to
      * by message_parts() is
      *
      * This function is public but applications should have few
      * reasons to ever call this.
      *
      * @return size of the message parts in bits
      */
      BOTAN_DEPRECATED("Deprecated no replacement") size_t message_part_size() const {
         return _signature_element_size_for_DER_encoding().value_or(0);
      }

      /*
      * Return the format normally used by this algorithm for X.509 signatures
      */
      BOTAN_DEPRECATED("Deprecated no replacement")
      Signature_Format default_x509_signature_format() const /* NOLINT(bugprone-virtual-near-miss) */ {
         return _default_x509_signature_format();
      }

      /**
      * This is an internal library function exposed on key types.
      * In almost all cases applications should use wrappers in pubkey.h
      *
      * Return an encryption operation for this key/params or throw
      *
      * @param rng a random number generator. The PK_Op may maintain a
      * reference to the RNG and use it many times. The rng must outlive
      * any operations which reference it.
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::Encryption> create_encryption_op(RandomNumberGenerator& rng,
                                                                       std::string_view params,
                                                                       std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In almost all cases applications should use wrappers in pubkey.h
      *
      * Return a KEM encryption operation for this key/params or throw
      *
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::KEM_Encryption> create_kem_encryption_op(std::string_view params,
                                                                               std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return a verification operation for this key/params or throw
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::Verification> create_verification_op(std::string_view params,
                                                                           std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return a verification operation for this combination of key and
      * signature algorithm or throw.
      *
      * @param signature_algorithm is the X.509 algorithm identifier encoding the padding
      * scheme and hash hash function used in the signature if applicable.
      *
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::Verification> create_x509_verification_op(
         const AlgorithmIdentifier& signature_algorithm, std::string_view provider) const;
};

/**
* Private Key Base Class
*/
class BOTAN_PUBLIC_API(2, 0) Private_Key : public virtual Public_Key {
   public:
      /**
      * @return BER encoded private key bits
      */
      virtual secure_vector<uint8_t> private_key_bits() const = 0;

      /**
      * @return binary private key bits, with no additional encoding
      *
      * Note: some algorithms (for example RSA) do not have an obvious encoding
      * for this value due to having many different values, and thus not implement
      * this function. The default implementation throws Not_Implemented
      */
      virtual secure_vector<uint8_t> raw_private_key_bits() const;

      /**
      * Allocate a new object for the public key associated with this
      * private key.
      *
      * @return public key
      */
      virtual std::unique_ptr<Public_Key> public_key() const = 0;

      /**
      * @return PKCS #8 private key encoding for this key object
      */
      secure_vector<uint8_t> private_key_info() const;

      /**
      * @return PKCS #8 AlgorithmIdentifier for this key
      * Might be different from the X.509 identifier, but normally is not
      */
      virtual AlgorithmIdentifier pkcs8_algorithm_identifier() const { return algorithm_identifier(); }

      /**
      * Indicates if this key is stateful, ie that performing a private
      * key operation requires updating the key storage.
      */
      virtual bool stateful_operation() const { return false; }

      /**
       * @brief Retrieves the number of remaining operations if this is a stateful private key.
       *
       * @returns the number of remaining operations or std::nullopt if not applicable.
       */
      virtual std::optional<uint64_t> remaining_operations() const { return std::nullopt; }

      // Declarations for internal library functions not covered by SemVer follow

      /**
       * @return Hash of the PKCS #8 encoding for this key object
       */
      std::string fingerprint_private(std::string_view alg) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return an decryption operation for this key/params or throw
      *
      * @param rng a random number generator. The PK_Op may maintain a
      * reference to the RNG and use it many times. The rng must outlive
      * any operations which reference it.
      * @param params additional parameters
      * @param provider the provider to use
      *
      */
      virtual std::unique_ptr<PK_Ops::Decryption> create_decryption_op(RandomNumberGenerator& rng,
                                                                       std::string_view params,
                                                                       std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return a KEM decryption operation for this key/params or throw
      *
      * @param rng a random number generator. The PK_Op may maintain a
      * reference to the RNG and use it many times. The rng must outlive
      * any operations which reference it.
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::KEM_Decryption> create_kem_decryption_op(RandomNumberGenerator& rng,
                                                                               std::string_view params,
                                                                               std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return a signature operation for this key/params or throw
      *
      * @param rng a random number generator. The PK_Op may maintain a
      * reference to the RNG and use it many times. The rng must outlive
      * any operations which reference it.
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::Signature> create_signature_op(RandomNumberGenerator& rng,
                                                                     std::string_view params,
                                                                     std::string_view provider) const;

      /**
      * This is an internal library function exposed on key types.
      * In all cases applications should use wrappers in pubkey.h
      *
      * Return a key agreement operation for this key/params or throw
      *
      * @param rng a random number generator. The PK_Op may maintain a
      * reference to the RNG and use it many times. The rng must outlive
      * any operations which reference it.
      * @param params additional parameters
      * @param provider the provider to use
      */
      virtual std::unique_ptr<PK_Ops::Key_Agreement> create_key_agreement_op(RandomNumberGenerator& rng,
                                                                             std::string_view params,
                                                                             std::string_view provider) const;
};

/**
* PK Secret Value Derivation Key
*/
class BOTAN_PUBLIC_API(2, 0) PK_Key_Agreement_Key : public virtual Private_Key {
   public:
      /*
      * @return public component of this key
      */
      virtual std::vector<uint8_t> public_value() const = 0;
};

std::string BOTAN_PUBLIC_API(2, 4) create_hex_fingerprint(const uint8_t bits[], size_t len, std::string_view hash_name);

inline std::string create_hex_fingerprint(std::span<const uint8_t> vec, std::string_view hash_name) {
   return create_hex_fingerprint(vec.data(), vec.size(), hash_name);
}

}  // namespace Botan

namespace Botan {

BOTAN_PUBLIC_API(2, 0)
std::unique_ptr<Public_Key> load_public_key(const AlgorithmIdentifier& alg_id, std::span<const uint8_t> key_bits);

BOTAN_PUBLIC_API(2, 0)
std::unique_ptr<Private_Key> load_private_key(const AlgorithmIdentifier& alg_id, std::span<const uint8_t> key_bits);

/**
* Create a new key
* For ECC keys, algo_params specifies EC group (eg, "secp256r1")
* For DH/DSA/ElGamal keys, algo_params is DL group (eg, "modp/ietf/2048")
* For RSA, algo_params is integer keylength
* For McEliece, algo_params is n,t
* If algo_params is left empty, suitable default parameters are chosen.
*/
BOTAN_PUBLIC_API(2, 0)
std::unique_ptr<Private_Key> create_private_key(std::string_view algo_name,
                                                RandomNumberGenerator& rng,
                                                std::string_view algo_params = "",
                                                std::string_view provider = "");

class EC_Group;

/**
* Create a new ECC key
*/
BOTAN_PUBLIC_API(3, 0)
std::unique_ptr<Private_Key> create_ec_private_key(std::string_view algo_name,
                                                   const EC_Group& group,
                                                   RandomNumberGenerator& rng);

BOTAN_PUBLIC_API(2, 2)
std::vector<std::string> probe_provider_private_key(std::string_view algo_name,
                                                    const std::vector<std::string>& possible);

}  // namespace Botan
/**
* Ordinary applications should never need to include or use this
* header. It is exposed only for specialized applications which want
* to implement new versions of public key crypto without merging them
* as changes to the library. One actual example of such usage is an
* application which creates RSA signatures using a custom TPM library.
* Unless you're doing something like that, you don't need anything
* here. Instead use pubkey.h which wraps these types safely and
* provides a stable application-oriented API.
*
* Note: This header was accidentally pulled from the public API between
*       Botan 3.0.0 and 3.2.0, and then restored in 3.3.0. If you are
*       maintaining an application which used this header in Botan 2.x,
*       you should make sure to use Botan 3.3.0 or later when migrating.
*/


namespace Botan {

class RandomNumberGenerator;

}  // namespace Botan

namespace Botan::PK_Ops {

/**
* Public key encryption interface
*/
class BOTAN_UNSTABLE_API Encryption /* NOLINT(*special-member-functions) */ {
   public:
      /**
      * Encrypt a message returning the ciphertext
      */
      virtual std::vector<uint8_t> encrypt(std::span<const uint8_t> msg, RandomNumberGenerator& rng) = 0;

      /**
      * Return the maximum input size for this key
      */
      virtual size_t max_input_bits() const = 0;

      /**
      * Given the plaintext length, return an upper bound of the ciphertext
      * length for this key and padding.
      */
      virtual size_t ciphertext_length(size_t ptext_len) const = 0;

      virtual ~Encryption() = default;
};

/**
* Public key decryption interface
*/
class BOTAN_UNSTABLE_API Decryption /* NOLINT(*special-member-functions) */ {
   public:
      virtual secure_vector<uint8_t> decrypt(uint8_t& valid_mask, std::span<const uint8_t> ctext) = 0;

      virtual size_t plaintext_length(size_t ctext_len) const = 0;

      virtual ~Decryption() = default;
};

/**
* Public key signature verification interface
*/
class BOTAN_UNSTABLE_API Verification /* NOLINT(*special-member-functions) */ {
   public:
      /**
      * Add more data to the message currently being signed
      * @param input the input to be hashed/verified
      */
      virtual void update(std::span<const uint8_t> input) = 0;

      /**
      * Perform a verification operation
      * @param sig the signature to be checked with respect to the input
      */
      virtual bool is_valid_signature(std::span<const uint8_t> sig) = 0;

      /**
      * Return the hash function being used by this signer
      */
      virtual std::string hash_function() const = 0;

      virtual ~Verification() = default;
};

/**
* Public key signature creation interface
*/
class BOTAN_UNSTABLE_API Signature /* NOLINT(*special-member-functions) */ {
   public:
      /**
      * Add more data to the message currently being signed
      * @param input the input to be hashed/signed
      */
      virtual void update(std::span<const uint8_t> input) = 0;

      /**
      * Perform a signature operation
      * @param rng a random number generator
      */
      virtual std::vector<uint8_t> sign(RandomNumberGenerator& rng) = 0;

      /**
      * Return an upper bound on the length of the output signature
      */
      virtual size_t signature_length() const = 0;

      /**
      * Return an algorithm identifier associated with this signature scheme.
      *
      * Default implementation throws an exception
      */
      virtual AlgorithmIdentifier algorithm_identifier() const;

      /**
      * Return the hash function being used by this signer
      */
      virtual std::string hash_function() const = 0;

      virtual ~Signature() = default;
};

/**
* A generic key agreement operation (eg DH or ECDH)
*/
class BOTAN_UNSTABLE_API Key_Agreement /* NOLINT(*special-member-functions) */ {
   public:
      virtual secure_vector<uint8_t> agree(size_t key_len,
                                           std::span<const uint8_t> other_key,
                                           std::span<const uint8_t> salt) = 0;

      virtual size_t agreed_value_size() const = 0;

      virtual ~Key_Agreement() = default;
};

/**
* KEM (key encapsulation)
*/
class BOTAN_UNSTABLE_API KEM_Encryption /* NOLINT(*special-member-functions) */ {
   public:
      virtual void kem_encrypt(std::span<uint8_t> out_encapsulated_key,
                               std::span<uint8_t> out_shared_key,
                               RandomNumberGenerator& rng,
                               size_t desired_shared_key_len,
                               std::span<const uint8_t> salt) = 0;

      virtual size_t shared_key_length(size_t desired_shared_key_len) const = 0;

      virtual size_t encapsulated_key_length() const = 0;

      virtual ~KEM_Encryption() = default;
};

class BOTAN_UNSTABLE_API KEM_Decryption /* NOLINT(*special-member-functions) */ {
   public:
      virtual void kem_decrypt(std::span<uint8_t> out_shared_key,
                               std::span<const uint8_t> encapsulated_key,
                               size_t desired_shared_key_len,
                               std::span<const uint8_t> salt) = 0;

      virtual size_t shared_key_length(size_t desired_shared_key_len) const = 0;

      virtual size_t encapsulated_key_length() const = 0;

      virtual ~KEM_Decryption() = default;
};

}  // namespace Botan::PK_Ops

namespace Botan {

class RandomNumberGenerator;

/**
* PKCS #8 General Exception
*/
class BOTAN_PUBLIC_API(2, 0) PKCS8_Exception final : public Decoding_Error {
   public:
      explicit PKCS8_Exception(std::string_view error) : Decoding_Error("PKCS #8", error) {}
};

/**
* This namespace contains functions for handling PKCS #8 private keys
*/
namespace PKCS8 {

/**
* BER encode a private key
* @param key the private key to encode
* @return BER encoded key
*/
inline secure_vector<uint8_t> BER_encode(const Private_Key& key) {
   return key.private_key_info();
}

/**
* Get a string containing a PEM encoded private key.
* @param key the key to encode
* @return encoded key
*/
BOTAN_PUBLIC_API(2, 0) std::string PEM_encode(const Private_Key& key);

/**
* Encrypt a key using PKCS #8 encryption
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param msec number of milliseconds to run the password derivation
* @param pbe_algo the name of the desired password-based encryption
*        algorithm; if empty ("") a reasonable (portable/secure)
*        default will be chosen.
* @return encrypted key in binary BER form
*/
BOTAN_PUBLIC_API(2, 0)
std::vector<uint8_t> BER_encode(const Private_Key& key,
                                RandomNumberGenerator& rng,
                                std::string_view pass,
                                std::chrono::milliseconds msec = std::chrono::milliseconds(300),
                                std::string_view pbe_algo = "");

/**
* Get a string containing a PEM encoded private key, encrypting it with a
* password.
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param msec number of milliseconds to run the password derivation
* @param pbe_algo the name of the desired password-based encryption
*        algorithm; if empty ("") a reasonable (portable/secure)
*        default will be chosen.
* @return encrypted key in PEM form
*/
BOTAN_PUBLIC_API(2, 0)
std::string PEM_encode(const Private_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view pass,
                       std::chrono::milliseconds msec = std::chrono::milliseconds(300),
                       std::string_view pbe_algo = "");

/**
* Encrypt a key using PKCS #8 encryption and a fixed iteration count
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param pbkdf_iter number of iterations to run PBKDF2
* @param cipher if non-empty specifies the cipher to use. CBC and GCM modes
*   are supported, for example "AES-128/CBC", "AES-256/GCM", "Serpent/CBC".
*   If empty a suitable default is chosen.
* @param pbkdf_hash if non-empty specifies the PBKDF hash function to use.
*   For example "SHA-256" or "SHA-384". If empty a suitable default is chosen.
* @return encrypted key in binary BER form
*/
BOTAN_PUBLIC_API(2, 1)
std::vector<uint8_t> BER_encode_encrypted_pbkdf_iter(const Private_Key& key,
                                                     RandomNumberGenerator& rng,
                                                     std::string_view pass,
                                                     size_t pbkdf_iter,
                                                     std::string_view cipher = "",
                                                     std::string_view pbkdf_hash = "");

/**
* Get a string containing a PEM encoded private key, encrypting it with a
* password.
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param pbkdf_iter number of iterations to run PBKDF
* @param cipher if non-empty specifies the cipher to use. CBC and GCM modes
*   are supported, for example "AES-128/CBC", "AES-256/GCM", "Serpent/CBC".
*   If empty a suitable default is chosen.
* @param pbkdf_hash if non-empty specifies the PBKDF hash function to use.
*   For example "SHA-256" or "SHA-384". If empty a suitable default is chosen.
* @return encrypted key in PEM form
*/
BOTAN_PUBLIC_API(2, 1)
std::string PEM_encode_encrypted_pbkdf_iter(const Private_Key& key,
                                            RandomNumberGenerator& rng,
                                            std::string_view pass,
                                            size_t pbkdf_iter,
                                            std::string_view cipher = "",
                                            std::string_view pbkdf_hash = "");

/**
* Encrypt a key using PKCS #8 encryption and a variable iteration count
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param pbkdf_msec how long to run PBKDF2
* @param pbkdf_iterations if non-null, set to the number of iterations used
* @param cipher if non-empty specifies the cipher to use. CBC and GCM modes
*   are supported, for example "AES-128/CBC", "AES-256/GCM", "Serpent/CBC".
*   If empty a suitable default is chosen.
* @param pbkdf_hash if non-empty specifies the PBKDF hash function to use.
*   For example "SHA-256" or "SHA-384". If empty a suitable default is chosen.
* @return encrypted key in binary BER form
*/
BOTAN_PUBLIC_API(2, 1)
std::vector<uint8_t> BER_encode_encrypted_pbkdf_msec(const Private_Key& key,
                                                     RandomNumberGenerator& rng,
                                                     std::string_view pass,
                                                     std::chrono::milliseconds pbkdf_msec,
                                                     size_t* pbkdf_iterations,
                                                     std::string_view cipher = "",
                                                     std::string_view pbkdf_hash = "");

/**
* Get a string containing a PEM encoded private key, encrypting it with a
* password.
* @param key the key to encode
* @param rng the rng to use
* @param pass the password to use for encryption
* @param pbkdf_msec how long in milliseconds to run PBKDF2
* @param pbkdf_iterations (output argument) number of iterations of PBKDF
*  that ended up being used
* @param cipher if non-empty specifies the cipher to use. CBC and GCM modes
*   are supported, for example "AES-128/CBC", "AES-256/GCM", "Serpent/CBC".
*   If empty a suitable default is chosen.
* @param pbkdf_hash if non-empty specifies the PBKDF hash function to use.
*   For example "SHA-256" or "SHA-384". If empty a suitable default is chosen.
* @return encrypted key in PEM form
*/
BOTAN_PUBLIC_API(2, 1)
std::string PEM_encode_encrypted_pbkdf_msec(const Private_Key& key,
                                            RandomNumberGenerator& rng,
                                            std::string_view pass,
                                            std::chrono::milliseconds pbkdf_msec,
                                            size_t* pbkdf_iterations,
                                            std::string_view cipher = "",
                                            std::string_view pbkdf_hash = "");

/**
* Load an encrypted key from a data source.
* @param source the data source providing the encoded key
* @param get_passphrase a function that returns passphrases
* @return loaded private key object
*/
BOTAN_PUBLIC_API(2, 3)
std::unique_ptr<Private_Key> load_key(DataSource& source, const std::function<std::string()>& get_passphrase);

/** Load an encrypted key from a data source.
* @param source the data source providing the encoded key
* @param pass the passphrase to decrypt the key
* @return loaded private key object
*/
BOTAN_PUBLIC_API(2, 3)
std::unique_ptr<Private_Key> load_key(DataSource& source, std::string_view pass);

/** Load an unencrypted key from a data source.
* @param source the data source providing the encoded key
* @return loaded private key object
*/
BOTAN_PUBLIC_API(2, 3)
std::unique_ptr<Private_Key> load_key(DataSource& source);

/**
* Load an encrypted key from memory.
* @param source the byte buffer containing the encoded key
* @param get_passphrase a function that returns passphrases
* @return loaded private key object
*/
BOTAN_PUBLIC_API(3, 0)
std::unique_ptr<Private_Key> load_key(std::span<const uint8_t> source,
                                      const std::function<std::string()>& get_passphrase);

/** Load an encrypted key from memory.
* @param source the byte buffer containing the encoded key
* @param pass the passphrase to decrypt the key
* @return loaded private key object
*/
BOTAN_PUBLIC_API(3, 0)
std::unique_ptr<Private_Key> load_key(std::span<const uint8_t> source, std::string_view pass);

/** Load an unencrypted key from memory.
* @param source the byte buffer containing the encoded key
* @return loaded private key object
*/
BOTAN_PUBLIC_API(3, 0)
std::unique_ptr<Private_Key> load_key(std::span<const uint8_t> source);

/**
* Copy an existing encoded key object.
* @param key the key to copy
* @return new copy of the key
*/
inline std::unique_ptr<Private_Key> copy_key(const Private_Key& key) {
   DataSource_Memory source(key.private_key_info());
   return PKCS8::load_key(source);
}

}  // namespace PKCS8

}  // namespace Botan

namespace Botan {

/**
* PSS parameters type
*
* Handles encoding/decoding of RSASSA-PSS-params from RFC 3447
*
* Only MGF1 is supported, and the trailer field must 1 (ie the variant
* from IEEE 1363a using a hash identifier is not supported)
*/
class BOTAN_PUBLIC_API(3, 7) PSS_Params final : public ASN1_Object {
   public:
      /**
      * Note that the only valid strings you can pass to this function
      * are values returned by SignaturePaddingScheme::name() and
      * these may change in a minor release.
      */
      static PSS_Params from_padding_name(std::string_view padding_name);

      /**
      * Note that the only valid strings you can pass to this function
      * are values returned by SignaturePaddingScheme::name() and
      * these may change in a minor release.
      */
      BOTAN_DEPRECATED("Use PSS_Params::from_padding_name")
      static PSS_Params from_emsa_name(std::string_view padding_name) {
         return PSS_Params::from_padding_name(padding_name);
      }

      PSS_Params(std::string_view hash_fn, size_t salt_len);

      /**
      * Decode an encoded RSASSA-PSS-params
      */
      BOTAN_FUTURE_EXPLICIT PSS_Params(std::span<const uint8_t> der);

      const AlgorithmIdentifier& hash_algid() const { return m_hash; }

      const AlgorithmIdentifier& mgf_algid() const { return m_mgf; }

      const AlgorithmIdentifier& mgf_hash_algid() const { return m_mgf_hash; }

      size_t salt_length() const { return m_salt_len; }

      size_t trailer_field() const { return m_trailer_field; }

      std::string hash_function() const { return hash_algid().oid().to_formatted_string(); }

      std::string mgf_function() const { return mgf_algid().oid().to_formatted_string(); }

      std::vector<uint8_t> serialize() const;

      void encode_into(DER_Encoder& to) const override;

   private:
      // We don't currently support uninitialized PSS_Params
      void decode_from(BER_Decoder& from) override;

      AlgorithmIdentifier m_hash;
      AlgorithmIdentifier m_mgf;
      AlgorithmIdentifier m_mgf_hash;
      size_t m_salt_len;
      size_t m_trailer_field;
};

}  // namespace Botan

namespace Botan {

class RandomNumberGenerator;

/**
* Public Key Encryptor
* This is the primary interface for public key encryption
*/
class BOTAN_PUBLIC_API(2, 0) PK_Encryptor {
   public:
      /**
      * Encrypt a message.
      * @param in the message as a byte array
      * @param length the length of the above byte array
      * @param rng the random number source to use
      * @return encrypted message
      */
      std::vector<uint8_t> encrypt(const uint8_t in[], size_t length, RandomNumberGenerator& rng) const {
         return enc(in, length, rng);
      }

      /**
      * Encrypt a message.
      * @param in the message
      * @param rng the random number source to use
      * @return encrypted message
      */
      std::vector<uint8_t> encrypt(std::span<const uint8_t> in, RandomNumberGenerator& rng) const {
         return enc(in.data(), in.size(), rng);
      }

      /**
      * Return the maximum allowed message size in bytes.
      * @return maximum message size in bytes
      */
      virtual size_t maximum_input_size() const = 0;

      /**
      * Return an upper bound on the ciphertext length
      */
      virtual size_t ciphertext_length(size_t ctext_len) const = 0;

      PK_Encryptor() = default;
      virtual ~PK_Encryptor() = default;

      PK_Encryptor(const PK_Encryptor&) = delete;
      PK_Encryptor& operator=(const PK_Encryptor&) = delete;

      PK_Encryptor(PK_Encryptor&&) noexcept = default;
      PK_Encryptor& operator=(PK_Encryptor&&) noexcept = default;

   private:
      virtual std::vector<uint8_t> enc(const uint8_t[], size_t, RandomNumberGenerator&) const = 0;
};

/**
* Public Key Decryptor
*/
class BOTAN_PUBLIC_API(2, 0) PK_Decryptor {
   public:
      /**
      * Decrypt a ciphertext, throwing an exception if the input
      * seems to be invalid (eg due to an accidental or malicious
      * error in the ciphertext).
      *
      * @param in the ciphertext as a byte array
      * @param length the length of the above byte array
      * @return decrypted message
      */
      secure_vector<uint8_t> decrypt(const uint8_t in[], size_t length) const;

      /**
      * Same as above, but taking a vector
      * @param in the ciphertext
      * @return decrypted message
      */
      secure_vector<uint8_t> decrypt(std::span<const uint8_t> in) const { return decrypt(in.data(), in.size()); }

      /**
      * Decrypt a ciphertext. If the ciphertext is invalid (eg due to
      * invalid padding) or is not the expected length, instead
      * returns a random string of the expected length. Use to avoid
      * oracle attacks, especially against PKCS #1 v1.5 decryption.
      */
      secure_vector<uint8_t> decrypt_or_random(const uint8_t in[],
                                               size_t length,
                                               size_t expected_pt_len,
                                               RandomNumberGenerator& rng) const;

      /**
      * Decrypt a ciphertext. If the ciphertext is invalid (eg due to
      * invalid padding) or is not the expected length, instead
      * returns a random string of the expected length. Use to avoid
      * oracle attacks, especially against PKCS #1 v1.5 decryption.
      *
      * Additionally checks (also in const time) that:
      *    contents[required_content_offsets[i]] == required_content_bytes[i]
      * for 0 <= i < required_contents
      *
      * Used for example in TLS, which encodes the client version in
      * the content bytes: if there is any timing variation the version
      * check can be used as an oracle to recover the key.
      */
      secure_vector<uint8_t> decrypt_or_random(const uint8_t in[],
                                               size_t length,
                                               size_t expected_pt_len,
                                               RandomNumberGenerator& rng,
                                               const uint8_t required_content_bytes[],
                                               const uint8_t required_content_offsets[],
                                               size_t required_contents) const;

      /**
      * Return an upper bound on the plaintext length for a particular
      * ciphertext input length
      */
      virtual size_t plaintext_length(size_t ctext_len) const = 0;

      PK_Decryptor() = default;
      virtual ~PK_Decryptor() = default;

      PK_Decryptor(const PK_Decryptor&) = delete;
      PK_Decryptor& operator=(const PK_Decryptor&) = delete;

      PK_Decryptor(PK_Decryptor&&) noexcept = default;
      PK_Decryptor& operator=(PK_Decryptor&&) noexcept = default;

   private:
      virtual secure_vector<uint8_t> do_decrypt(uint8_t& valid_mask, const uint8_t in[], size_t in_len) const = 0;
};

/**
* Public Key Signer. Use the sign_message() functions for small
* messages. Use multiple calls update() to process large messages and
* generate the signature by finally calling signature().
*/
class BOTAN_PUBLIC_API(2, 0) PK_Signer final {
   public:
      /**
      * Construct a PK Signer.
      * @param key the key to use inside this signer
      * @param rng the random generator to use
      * @param padding the padding/hash to use, eg "SHA-512" or "PSS(SHA-256)"
      * @param format the signature format to use
      * @param provider the provider to use
      */
      PK_Signer(const Private_Key& key,
                RandomNumberGenerator& rng,
                std::string_view padding,
                Signature_Format format = Signature_Format::Standard,
                std::string_view provider = "");

      ~PK_Signer();

      PK_Signer(const PK_Signer&) = delete;
      PK_Signer& operator=(const PK_Signer&) = delete;

      PK_Signer(PK_Signer&&) noexcept;
      PK_Signer& operator=(PK_Signer&&) noexcept;

      /**
      * Sign a message all in one go
      * @param in the message to sign as a byte array
      * @param length the length of the above byte array
      * @param rng the rng to use
      * @return signature
      */
      std::vector<uint8_t> sign_message(const uint8_t in[], size_t length, RandomNumberGenerator& rng) {
         this->update(in, length);
         return this->signature(rng);
      }

      /**
      * Sign a message.
      * @param in the message to sign
      * @param rng the rng to use
      * @return signature
      */
      std::vector<uint8_t> sign_message(std::span<const uint8_t> in, RandomNumberGenerator& rng) {
         return sign_message(in.data(), in.size(), rng);
      }

      /**
      * Add a message part (single byte).
      * @param in the byte to add
      */
      void update(uint8_t in) { update(&in, 1); }

      /**
      * Add a message part.
      * @param in the message part to add as a byte array
      * @param length the length of the above byte array
      */
      void update(const uint8_t in[], size_t length);

      /**
      * Add a message part.
      * @param in the message part to add
      */
      void update(std::span<const uint8_t> in) { update(in.data(), in.size()); }

      /**
      * Add a message part.
      * @param in the message part to add
      */
      void update(std::string_view in);

      /**
      * Get the signature of the so far processed message (provided by the
      * calls to update()).
      * @param rng the rng to use
      * @return signature of the total message
      */
      std::vector<uint8_t> signature(RandomNumberGenerator& rng);

      /**
      * Set the output format of the signature.
      * @param format the signature format to use
      */
      void set_output_format(Signature_Format format) { m_sig_format = format; }

      /**
      * Return an upper bound on the length of the signatures this
      * PK_Signer will produce
      */
      size_t signature_length() const;

      /**
      * Return an AlgorithmIdentifier appropriate for identifying the signature
      * method being generated by this PK_Signer. Throws an exception if this
      * is not available for the current signature scheme.
      */
      AlgorithmIdentifier algorithm_identifier() const;

      /**
      * Return the hash function which is being used to create signatures.
      * This should never return an empty string however it may return a string
      * which does not map directly to a hash function, in particular if "Raw"
      * (unhashed) encoding is being used.
      */
      std::string hash_function() const;

   private:
      std::unique_ptr<PK_Ops::Signature> m_op;
      Signature_Format m_sig_format;
      std::optional<size_t> m_sig_element_size;
};

/**
* Public Key Verifier. Use the verify_message() functions for small
* messages. Use multiple calls update() to process large messages and
* verify the signature by finally calling check_signature().
*/
class BOTAN_PUBLIC_API(2, 0) PK_Verifier final {
   public:
      /**
      * Construct a PK Verifier.
      * @param pub_key the public key to verify against
      * @param padding the padding/hash to use (eg "SHA-512" or "PSS(SHA-256)")
      * @param format the signature format to use
      * @param provider the provider to use
      */
      PK_Verifier(const Public_Key& pub_key,
                  std::string_view padding,
                  Signature_Format format = Signature_Format::Standard,
                  std::string_view provider = "");

      /**
      * Construct a PK Verifier (X.509 specific)
      *
      * This constructor will attempt to decode signature_format relative
      * to the public key provided. If they seem to be inconsistent or
      * otherwise unsupported, a Decoding_Error is thrown.
      *
      * @param pub_key the public key to verify against
      * @param signature_algorithm the supposed signature algorithm
      * @param provider the provider to use
      */
      PK_Verifier(const Public_Key& pub_key,
                  const AlgorithmIdentifier& signature_algorithm,
                  std::string_view provider = "");

      ~PK_Verifier();

      PK_Verifier(const PK_Verifier&) = delete;
      PK_Verifier& operator=(const PK_Verifier&) = delete;

      PK_Verifier(PK_Verifier&&) noexcept;
      PK_Verifier& operator=(PK_Verifier&&) noexcept;

      /**
      * Verify a signature.
      * @param msg the message that the signature belongs to, as a byte array
      * @param msg_length the length of the above byte array msg
      * @param sig the signature as a byte array
      * @param sig_length the length of the above byte array sig
      * @return true if the signature is valid
      */
      bool verify_message(const uint8_t msg[], size_t msg_length, const uint8_t sig[], size_t sig_length);

      /**
      * Verify a signature.
      * @param msg the message that the signature belongs to
      * @param sig the signature
      * @return true if the signature is valid
      */
      bool verify_message(std::span<const uint8_t> msg, std::span<const uint8_t> sig) {
         return verify_message(msg.data(), msg.size(), sig.data(), sig.size());
      }

      /**
      * Add a message part (single byte) of the message corresponding to the
      * signature to be verified.
      * @param in the byte to add
      */
      void update(uint8_t in) { update(&in, 1); }

      /**
      * Add a message part of the message corresponding to the
      * signature to be verified.
      * @param msg_part the new message part as a byte array
      * @param length the length of the above byte array
      */
      void update(const uint8_t msg_part[], size_t length);

      /**
      * Add a message part of the message corresponding to the
      * signature to be verified.
      * @param in the new message part
      */
      void update(std::span<const uint8_t> in) { update(in.data(), in.size()); }

      /**
      * Add a message part of the message corresponding to the
      * signature to be verified.
      */
      void update(std::string_view in);

      /**
      * Check the signature of the buffered message, i.e. the one build
      * by successive calls to update.
      * @param sig the signature to be verified as a byte array
      * @param length the length of the above byte array
      * @return true if the signature is valid, false otherwise
      */
      bool check_signature(const uint8_t sig[], size_t length);

      /**
      * Check the signature of the buffered message, i.e. the one build
      * by successive calls to update.
      * @param sig the signature to be verified
      * @return true if the signature is valid, false otherwise
      */
      bool check_signature(std::span<const uint8_t> sig) { return check_signature(sig.data(), sig.size()); }

      /**
      * Set the format of the signatures fed to this verifier.
      * @param format the signature format to use
      */
      BOTAN_DEPRECATED("Provide Signature_Format to the constructor") void set_input_format(Signature_Format format);

      /**
      * Return the hash function which is being used to verify signatures.
      * This should never return an empty string however it may return a string
      * which does not map directly to a hash function, in particular if "Raw"
      * (unhashed) encoding is being used.
      */
      std::string hash_function() const;

   private:
      std::unique_ptr<PK_Ops::Verification> m_op;
      Signature_Format m_sig_format;
      std::optional<size_t> m_sig_element_size;
};

/**
* Object used for key agreement
*/
class BOTAN_PUBLIC_API(2, 0) PK_Key_Agreement final {
   public:
      /**
      * Construct a PK Key Agreement.
      * @param key the key to use
      * @param rng the random generator to use
      * @param kdf name of the KDF to use (or 'Raw' for no KDF)
      * @param provider the algo provider to use (or empty for default)
      */
      PK_Key_Agreement(const Private_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view kdf,
                       std::string_view provider = "");

      ~PK_Key_Agreement();

      PK_Key_Agreement(const PK_Key_Agreement&) = delete;
      PK_Key_Agreement& operator=(const PK_Key_Agreement&) = delete;

      PK_Key_Agreement(PK_Key_Agreement&&) noexcept;
      PK_Key_Agreement& operator=(PK_Key_Agreement&&) noexcept;

      /**
      * Perform Key Agreement Operation
      * @param key_len the desired key output size (ignored if "Raw" KDF is used)
      * @param peer_key the other parties key
      * @param salt extra derivation salt
      */
      SymmetricKey derive_key(size_t key_len, std::span<const uint8_t> peer_key, std::span<const uint8_t> salt) const;

      /**
      * Perform Key Agreement Operation
      * @param key_len the desired key output size (ignored if "Raw" KDF is used)
      * @param peer_key the other parties key
      * @param peer_key_len the length of peer_key in bytes
      * @param salt extra derivation salt
      * @param salt_len the length of salt in bytes
      */
      SymmetricKey derive_key(
         size_t key_len, const uint8_t peer_key[], size_t peer_key_len, const uint8_t salt[], size_t salt_len) const {
         return this->derive_key(key_len, {peer_key, peer_key_len}, {salt, salt_len});
      }

      /**
      * Perform Key Agreement Operation
      * @param key_len the desired key output size (ignored if "Raw" KDF is used)
      * @param peer_key the other parties key
      * @param salt extra derivation salt
      * @param salt_len the length of salt in bytes
      */
      SymmetricKey derive_key(size_t key_len,
                              std::span<const uint8_t> peer_key,
                              const uint8_t salt[],
                              size_t salt_len) const {
         return derive_key(key_len, peer_key.data(), peer_key.size(), salt, salt_len);
      }

      /**
      * Perform Key Agreement Operation
      * @param key_len the desired key output size (ignored if "Raw" KDF is used)
      * @param peer_key the other parties key
      * @param peer_key_len the length of peer_key in bytes
      * @param salt extra derivation info
      */
      SymmetricKey derive_key(size_t key_len,
                              const uint8_t peer_key[],
                              size_t peer_key_len,
                              std::string_view salt = "") const;

      /**
      * Perform Key Agreement Operation
      * @param key_len the desired key output size (ignored if "Raw" KDF is used)
      * @param peer_key the other parties key
      * @param salt extra derivation info
      */
      SymmetricKey derive_key(size_t key_len, std::span<const uint8_t> peer_key, std::string_view salt = "") const;

      /**
      * Return the underlying size of the value that is agreed.
      * If derive_key is called with a length of 0 with a "Raw"
      * KDF, it will return a value of this size.
      */
      size_t agreed_value_size() const;

   private:
      std::unique_ptr<PK_Ops::Key_Agreement> m_op;
};

/**
* Encryption using a standard message recovery algorithm like RSA or
* ElGamal, paired with an encoding scheme like OAEP.
*/
class BOTAN_PUBLIC_API(2, 0) PK_Encryptor_EME final : public PK_Encryptor {
   public:
      size_t maximum_input_size() const override;

      /**
      * Construct an instance.
      * @param key the key to use inside the encryptor
      * @param rng the RNG to use
      * @param padding the message encoding scheme to use (eg "OAEP(SHA-256)")
      * @param provider the provider to use
      */
      PK_Encryptor_EME(const Public_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view padding,
                       std::string_view provider = "");

      ~PK_Encryptor_EME() override;

      PK_Encryptor_EME(const PK_Encryptor_EME&) = delete;
      PK_Encryptor_EME& operator=(const PK_Encryptor_EME&) = delete;

      PK_Encryptor_EME(PK_Encryptor_EME&&) noexcept;
      PK_Encryptor_EME& operator=(PK_Encryptor_EME&&) noexcept;

      /**
      * Return an upper bound on the ciphertext length for a particular
      * plaintext input length
      */
      size_t ciphertext_length(size_t ptext_len) const override;

   private:
      std::vector<uint8_t> enc(const uint8_t ptext[], size_t len, RandomNumberGenerator& rng) const override;

      std::unique_ptr<PK_Ops::Encryption> m_op;
};

/**
* Decryption with a padding scheme.
*
* This is typically only used with RSA
*/
class BOTAN_PUBLIC_API(2, 0) PK_Decryptor_EME final : public PK_Decryptor {
   public:
      /**
      * Construct an instance.
      * @param key the key to use inside the decryptor
      * @param rng the random generator to use
      * @param padding the padding scheme to use
      * @param provider the provider to use
      */
      PK_Decryptor_EME(const Private_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view padding,
                       std::string_view provider = "");

      size_t plaintext_length(size_t ptext_len) const override;

      ~PK_Decryptor_EME() override;

      PK_Decryptor_EME(const PK_Decryptor_EME&) = delete;
      PK_Decryptor_EME& operator=(const PK_Decryptor_EME&) = delete;

      PK_Decryptor_EME(PK_Decryptor_EME&&) noexcept;
      PK_Decryptor_EME& operator=(PK_Decryptor_EME&&) noexcept;

   private:
      secure_vector<uint8_t> do_decrypt(uint8_t& valid_mask, const uint8_t in[], size_t in_len) const override;

      std::unique_ptr<PK_Ops::Decryption> m_op;
};

/**
* Result of a key encapsulation operation.
*/
class KEM_Encapsulation final {
   public:
      KEM_Encapsulation(std::vector<uint8_t> encapsulated_shared_key, secure_vector<uint8_t> shared_key) :
            m_encapsulated_shared_key(std::move(encapsulated_shared_key)), m_shared_key(std::move(shared_key)) {}

      /**
      * @returns the encapsulated shared secret (encrypted with the public key)
      */
      const std::vector<uint8_t>& encapsulated_shared_key() const { return m_encapsulated_shared_key; }

      /**
      * @returns the plaintext shared secret
      */
      const secure_vector<uint8_t>& shared_key() const { return m_shared_key; }

      /**
       * @returns the pair (encapsulated key, key) extracted from @p kem
       */
      static std::pair<std::vector<uint8_t>, secure_vector<uint8_t>> destructure(
         KEM_Encapsulation&& kem) /* NOLINT(*param-not-moved*) */ {
         return std::make_pair(std::exchange(kem.m_encapsulated_shared_key, {}), std::exchange(kem.m_shared_key, {}));
      }

   private:
      friend class PK_KEM_Encryptor;

      KEM_Encapsulation(size_t encapsulated_size, size_t shared_key_size) :
            m_encapsulated_shared_key(encapsulated_size), m_shared_key(shared_key_size) {}

   private:
      std::vector<uint8_t> m_encapsulated_shared_key;
      secure_vector<uint8_t> m_shared_key;
};

/**
* Public Key Key Encapsulation Mechanism Encryption.
*/
class BOTAN_PUBLIC_API(2, 0) PK_KEM_Encryptor final {
   public:
      /**
      * Construct an instance.
      * @param key the key to encrypt to
      * @param kem_param additional KEM parameters
      * @param provider the provider to use
      */
      BOTAN_FUTURE_EXPLICIT PK_KEM_Encryptor(const Public_Key& key,
                                             std::string_view kem_param = "",
                                             std::string_view provider = "");

      /**
      * Construct an instance.
      * @param key the key to encrypt to
      * @param rng the RNG to use
      * @param kem_param additional KEM parameters
      * @param provider the provider to use
      */
      BOTAN_DEPRECATED("Use constructor that does not take RNG")
      PK_KEM_Encryptor(const Public_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view kem_param = "",
                       std::string_view provider = "");

      ~PK_KEM_Encryptor();

      PK_KEM_Encryptor(const PK_KEM_Encryptor&) = delete;
      PK_KEM_Encryptor& operator=(const PK_KEM_Encryptor&) = delete;

      PK_KEM_Encryptor(PK_KEM_Encryptor&&) noexcept;
      PK_KEM_Encryptor& operator=(PK_KEM_Encryptor&&) noexcept;

      /**
      * Return the length of the shared key returned by this KEM
      *
      * If this KEM was used with a KDF, then it will always return
      * exactly the desired key length, because the output of the KEM
      * will be hashed by the KDF.
      *
      * However if the KEM was used with "Raw" kdf, to request the
      * algorithmic output of the KEM directly, then the desired key
      * length will be ignored and a bytestring that depends on the
      * algorithm is returned
      *
      * @param desired_shared_key_len is the requested length
      */
      size_t shared_key_length(size_t desired_shared_key_len) const;

      /**
      * Return the length in bytes of encapsulated keys returned by this KEM
      */
      size_t encapsulated_key_length() const;

      /**
      * Generate a shared key for data encryption.
      *
      * @param rng                    the RNG to use
      * @param desired_shared_key_len desired size of the shared key in bytes for the KDF
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      *
      * @returns a struct with both the shared secret and its encapsulation
      */
      KEM_Encapsulation encrypt(RandomNumberGenerator& rng,
                                size_t desired_shared_key_len = 32,
                                std::span<const uint8_t> salt = {}) {
         std::vector<uint8_t> encapsulated_shared_key(encapsulated_key_length());
         secure_vector<uint8_t> shared_key(shared_key_length(desired_shared_key_len));

         encrypt(std::span{encapsulated_shared_key}, std::span{shared_key}, rng, desired_shared_key_len, salt);
         return KEM_Encapsulation(std::move(encapsulated_shared_key), std::move(shared_key));
      }

      /**
      * Generate a shared key for data encryption.
      * @param out_encapsulated_key   the generated encapsulated key
      * @param out_shared_key         the generated shared key
      * @param rng                    the RNG to use
      * @param desired_shared_key_len desired size of the shared key in bytes
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      */
      void encrypt(secure_vector<uint8_t>& out_encapsulated_key,
                   secure_vector<uint8_t>& out_shared_key,
                   RandomNumberGenerator& rng,
                   size_t desired_shared_key_len = 32,
                   std::span<const uint8_t> salt = {}) {
         out_encapsulated_key.resize(encapsulated_key_length());
         out_shared_key.resize(shared_key_length(desired_shared_key_len));
         encrypt(std::span{out_encapsulated_key}, std::span{out_shared_key}, rng, desired_shared_key_len, salt);
      }

      /**
      * Generate a shared key for data encryption.
      * @param out_encapsulated_key   the generated encapsulated key
      * @param out_shared_key         the generated shared key
      * @param rng                    the RNG to use
      * @param desired_shared_key_len desired size of the shared key in bytes
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      */
      void encrypt(std::span<uint8_t> out_encapsulated_key,
                   std::span<uint8_t> out_shared_key,
                   RandomNumberGenerator& rng,
                   size_t desired_shared_key_len = 32,
                   std::span<const uint8_t> salt = {});

      BOTAN_DEPRECATED("use overload with salt as std::span<>")
      void encrypt(secure_vector<uint8_t>& out_encapsulated_key,
                   secure_vector<uint8_t>& out_shared_key,
                   size_t desired_shared_key_len,
                   RandomNumberGenerator& rng,
                   const uint8_t salt[],
                   size_t salt_len) {
         this->encrypt(out_encapsulated_key, out_shared_key, rng, desired_shared_key_len, {salt, salt_len});
      }

      BOTAN_DEPRECATED("use overload where rng comes after the out-parameters")
      void encrypt(secure_vector<uint8_t>& out_encapsulated_key,
                   secure_vector<uint8_t>& out_shared_key,
                   size_t desired_shared_key_len,
                   RandomNumberGenerator& rng,
                   std::span<const uint8_t> salt = {}) {
         out_encapsulated_key.resize(encapsulated_key_length());
         out_shared_key.resize(shared_key_length(desired_shared_key_len));
         encrypt(out_encapsulated_key, out_shared_key, rng, desired_shared_key_len, salt);
      }

   private:
      std::unique_ptr<PK_Ops::KEM_Encryption> m_op;
};

/**
* Public Key Key Encapsulation Mechanism Decryption.
*/
class BOTAN_PUBLIC_API(2, 0) PK_KEM_Decryptor final {
   public:
      /**
      * Construct an instance.
      * @param key the key to use inside the decryptor
      * @param rng the RNG to use
      * @param kem_param additional KEM parameters
      * @param provider the provider to use
      */
      PK_KEM_Decryptor(const Private_Key& key,
                       RandomNumberGenerator& rng,
                       std::string_view kem_param = "",
                       std::string_view provider = "");

      ~PK_KEM_Decryptor();
      PK_KEM_Decryptor(const PK_KEM_Decryptor&) = delete;
      PK_KEM_Decryptor& operator=(const PK_KEM_Decryptor&) = delete;

      PK_KEM_Decryptor(PK_KEM_Decryptor&&) noexcept;
      PK_KEM_Decryptor& operator=(PK_KEM_Decryptor&&) noexcept;

      /**
      * Return the length of the shared key returned by this KEM
      *
      * If this KEM was used with a KDF, then it will always return
      * exactly the desired key length, because the output of the KEM
      * will be hashed by the KDF.
      *
      * However if the KEM was used with "Raw" kdf, to request the
      * algorithmic output of the KEM directly, then the desired key
      * length will be ignored and a bytestring that depends on the
      * algorithm is returned
      *
      * @param desired_shared_key_len is the requested length.
      */
      size_t shared_key_length(size_t desired_shared_key_len) const;

      /**
      * Return the length of the encapsulated key expected by this KEM
      */
      size_t encapsulated_key_length() const;

      /**
      * Decrypts the shared key for data encryption.
      *
      * @param out_shared_key         the generated shared key
      * @param encap_key              the encapsulated key
      * @param desired_shared_key_len desired size of the shared key in bytes
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      */
      void decrypt(std::span<uint8_t> out_shared_key,
                   std::span<const uint8_t> encap_key,
                   size_t desired_shared_key_len = 32,
                   std::span<const uint8_t> salt = {});

      /**
      * Decrypts the shared key for data encryption.
      *
      * @param encap_key              the encapsulated key
      * @param encap_key_len          size of the encapsulated key in bytes
      * @param desired_shared_key_len desired size of the shared key in bytes
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      * @param salt_len               size of the salt value in bytes
      *                               (ignored if no KDF is used)
      *
      * @return the shared data encryption key
      */
      secure_vector<uint8_t> decrypt(const uint8_t encap_key[],
                                     size_t encap_key_len,
                                     size_t desired_shared_key_len,
                                     const uint8_t salt[] = nullptr,
                                     size_t salt_len = 0) {
         secure_vector<uint8_t> shared_key(shared_key_length(desired_shared_key_len));
         decrypt(shared_key, {encap_key, encap_key_len}, desired_shared_key_len, {salt, salt_len});
         return shared_key;
      }

      /**
      * Decrypts the shared key for data encryption.
      *
      * @param encap_key              the encapsulated key
      * @param desired_shared_key_len desired size of the shared key in bytes
      *                               (ignored if no KDF is used)
      * @param salt                   a salt value used in the KDF
      *                               (ignored if no KDF is used)
      *
      * @return the shared data encryption key
      */
      secure_vector<uint8_t> decrypt(std::span<const uint8_t> encap_key,
                                     size_t desired_shared_key_len = 32,
                                     std::span<const uint8_t> salt = {}) {
         secure_vector<uint8_t> shared_key(shared_key_length(desired_shared_key_len));
         decrypt(shared_key, encap_key, desired_shared_key_len, salt);
         return shared_key;
      }

   private:
      std::unique_ptr<PK_Ops::KEM_Decryption> m_op;
};

}  // namespace Botan

BOTAN_DEPRECATED_HEADER("reducer.h")

namespace Botan {

/**
* Modular Reducer
*
* This class is deprecated without replacement
*/
class BOTAN_PUBLIC_API(2, 0) Modular_Reducer final {
   public:
      const BigInt& get_modulus() const { return m_modulus; }

      BigInt reduce(const BigInt& x) const;

      /**
      * Multiply mod p
      * @param x the first operand
      * @param y the second operand
      * @return (x * y) % p
      */
      BigInt multiply(const BigInt& x, const BigInt& y) const { return reduce(x * y); }

      /**
      * Multiply mod p
      * @return (x * y * z) % p
      */
      BigInt multiply(const BigInt& x, const BigInt& y, const BigInt& z) const { return multiply(x, multiply(y, z)); }

      /**
      * Square mod p
      * @param x the value to square
      * @return (x * x) % p
      */
      BigInt square(const BigInt& x) const { return reduce(x * x); }

      /**
      * Cube mod p
      * @param x the value to cube
      * @return (x * x * x) % p
      */
      BigInt cube(const BigInt& x) const { return multiply(x, this->square(x)); }

      /**
      * Low level reduction function. Mostly for internal use.
      * Sometimes useful for performance by reducing temporaries
      * Reduce x mod p and place the output in out.
      *
      * @warning X and out must not reference each other
      *
      * ws is an (ignored) a temporary workspace.
      */
      void reduce(BigInt& out, const BigInt& x, secure_vector<word>& /*ws*/) const { out = reduce(x); }

      bool initialized() const { return (m_mod_words != 0); }

      BOTAN_DEPRECATED("Use for_public_modulus or for_secret_modulus") Modular_Reducer() : m_mod_words(0) {}

      /**
      * Accepts m == 0 and leaves the Modular_Reducer in an uninitialized state
      */
      explicit Modular_Reducer(const BigInt& mod);

      /**
      * Requires that m > 0
      */
      static Modular_Reducer for_public_modulus(const BigInt& m) { return Modular_Reducer(m); }

      /**
      * Requires that m > 0
      */
      static Modular_Reducer for_secret_modulus(const BigInt& m) { return Modular_Reducer(m); }

   private:
      Modular_Reducer(const BigInt& m, BigInt mu, size_t mw) : m_modulus(m), m_mu(std::move(mu)), m_mod_words(mw) {}

      BigInt m_modulus, m_mu;
      size_t m_mod_words;
};

}  // namespace Botan

namespace Botan {

/**
* Base class for all stream ciphers
*/
class BOTAN_PUBLIC_API(2, 0) StreamCipher : public SymmetricAlgorithm {
   public:
      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to use
      * @return a null pointer if the algo/provider combination cannot be found
      */
      static std::unique_ptr<StreamCipher> create(std::string_view algo_spec, std::string_view provider = "");

      /**
      * Create an instance based on a name
      * If provider is empty then best available is chosen.
      * @param algo_spec algorithm name
      * @param provider provider implementation to use
      * Throws a Lookup_Error if the algo/provider combination cannot be found
      */
      static std::unique_ptr<StreamCipher> create_or_throw(std::string_view algo_spec, std::string_view provider = "");

      /**
      * @return list of available providers for this algorithm, empty if not available
      */
      static std::vector<std::string> providers(std::string_view algo_spec);

      /**
      * Encrypt or decrypt a message
      *
      * Processes all bytes plain/ciphertext from @p in and writes the result to
      * @p out.
      *
      * @param in the plaintext
      * @param out the byte array to hold the output, i.e. the ciphertext
      * @param len the length of both in and out in bytes
      */
      void cipher(const uint8_t in[], uint8_t out[], size_t len) { cipher_bytes(in, out, len); }

      /**
      * Encrypt or decrypt a message
      * @param in the plaintext
      * @param out the byte array to hold the output, i.e. the ciphertext
      *            with at least the same size as @p in
      */
      void cipher(std::span<const uint8_t> in, std::span<uint8_t> out);

      /**
      * Write keystream bytes to a buffer
      *
      * The contents of @p out are ignored/overwritten
      *
      * @param out the byte array to hold the keystream
      * @param len the length of out in bytes
      */
      void write_keystream(uint8_t out[], size_t len) { generate_keystream(out, len); }

      /**
      * Fill a given buffer with keystream bytes
      *
      * The contents of @p out are ignored/overwritten
      *
      * @param out the byte array to hold the keystream
      */
      void write_keystream(std::span<uint8_t> out) { generate_keystream(out.data(), out.size()); }

      /**
      * Get @p bytes from the keystream
      *
      * The bytes are written into a continuous byte buffer of your choosing.
      *
      * @param bytes The number of bytes to be produced
      */
      template <concepts::resizable_byte_buffer T = secure_vector<uint8_t>>
      T keystream_bytes(size_t bytes) {
         T out(bytes);
         write_keystream(out);
         return out;
      }

      /**
      * Encrypt or decrypt a message
      * The message is encrypted/decrypted in place.
      * @param buf the plaintext / ciphertext
      * @param len the length of buf in bytes
      */
      void cipher1(uint8_t buf[], size_t len) { cipher(buf, buf, len); }

      /**
      * Encrypt or decrypt a message
      * The message is encrypted/decrypted in place.
      * @param buf the plaintext / ciphertext
      */
      void cipher1(std::span<uint8_t> buf) { cipher(buf, buf); }

      /**
      * Encrypt a message
      * The message is encrypted/decrypted in place.
      * @param inout the plaintext / ciphertext
      */
      void encipher(std::span<uint8_t> inout) { cipher(inout.data(), inout.data(), inout.size()); }

      /**
      * Encrypt a message
      * The message is encrypted in place.
      * @param inout the plaintext / ciphertext
      */
      void encrypt(std::span<uint8_t> inout) { cipher(inout.data(), inout.data(), inout.size()); }

      /**
      * Decrypt a message in place
      * The message is decrypted in place.
      * @param inout the plaintext / ciphertext
      */
      void decrypt(std::span<uint8_t> inout) { cipher(inout.data(), inout.data(), inout.size()); }

      /**
      * Return the optimium buffer size to use with this cipher
      *
      * Most stream ciphers internally produce blocks of bytes.  This function
      * returns that block size. Aligning buffer sizes to a multiple of this
      * size may improve performance by reducing internal buffering overhead.
      *
      * Note the return value of this function may change for any particular
      * algorithm due to changes in the implementation from release to release,
      * or changes in the runtime environment (such as CPUID indicating
      * availability of an optimized implementation). It is not intrinsic to
      * the algorithm; it is just a suggestion for gaining best performance.
      */
      virtual size_t buffer_size() const = 0;

      /**
      * Resync the cipher using the IV
      *
      * Load @p IV into the stream cipher state. This should happen after the
      * key is set (set_key()) and before any operation (encrypt(), decrypt() or
      * seek()) is called.
      *
      * If the cipher does not support IVs, then a call with an empty IV will be
      * accepted and any other length will cause an Invalid_IV_Length exception.
      *
      * @param iv the initialization vector
      * @param iv_len the length of the IV in bytes
      */
      void set_iv(const uint8_t iv[], size_t iv_len) { set_iv_bytes(iv, iv_len); }

      /**
      * Resync the cipher using the IV
      * @param iv the initialization vector
      * @throws Invalid_IV_Length if an incompatible IV was passed.
      */
      void set_iv(std::span<const uint8_t> iv) { set_iv_bytes(iv.data(), iv.size()); }

      /**
      * Return the default (preferred) nonce length
      *
      * If this function returns zero, then this cipher does not support nonces;
      * in this case any call to set_iv with a (non-empty) value will fail.
      *
      * Default implementation returns 0
      */
      virtual size_t default_iv_length() const;

      /**
      * @param iv_len the length of the IV in bytes
      * @return if the length is valid for this algorithm
      */
      virtual bool valid_iv_length(size_t iv_len) const { return (iv_len == 0); }

      /**
      * @return a new object representing the same algorithm as *this
      */
      StreamCipher* clone() const { return this->new_object().release(); }

      /**
      * @return new object representing the same algorithm as *this
      */
      virtual std::unique_ptr<StreamCipher> new_object() const = 0;

      /**
      * Set the offset and the state used later to generate the keystream
      *
      * Sets the state of the stream cipher and keystream according to the
      * passed @p offset, exactly as if @p offset bytes had first been
      * encrypted. The key and (if required) the IV have to be set before this
      * can be called.
      *
      * @note Not all ciphers support seeking; such objects will throw
      *       Not_Implemented in this case.
      *
      * @param offset the offset where we begin to generate the keystream
      */
      virtual void seek(uint64_t offset) = 0;

      /**
      * @return provider information about this implementation. Default is "base",
      * might also return "sse2", "avx2" or some other arbitrary string.
      */
      virtual std::string provider() const { return "base"; }

   protected:
      /**
      * Encrypt or decrypt a message
      */
      virtual void cipher_bytes(const uint8_t in[], uint8_t out[], size_t len) = 0;

      /**
      * Write keystream bytes to a buffer
      */
      virtual void generate_keystream(uint8_t out[], size_t len);

      /**
      * Resync the cipher using the IV
      */
      virtual void set_iv_bytes(const uint8_t iv[], size_t iv_len) = 0;
};

}  // namespace Botan


namespace Botan {

/**
 * Added as an additional "capability tag" to enable arithmetic operators with
 * plain numbers for Strong<> types that wrap a number.
 */
struct EnableArithmeticWithPlainNumber {};

namespace detail {

/**
 * Checks whether the @p CapabilityT is included in the @p Tags type pack.
 */
template <typename CapabilityT, typename... Tags>
constexpr bool has_capability = (std::is_same_v<CapabilityT, Tags> || ...);

template <typename T>
class Strong_Base {
   private:
      T m_value;

   public:
      using wrapped_type = T;

   public:
      Strong_Base() = default;
      Strong_Base(const Strong_Base&) = default;
      Strong_Base(Strong_Base&&) noexcept = default;
      Strong_Base& operator=(const Strong_Base&) = default;
      Strong_Base& operator=(Strong_Base&&) noexcept = default;
      ~Strong_Base() = default;

      constexpr explicit Strong_Base(T v) : m_value(std::move(v)) {}

      constexpr T& get() & { return m_value; }

      constexpr const T& get() const& { return m_value; }

      constexpr T&& get() && { return std::move(m_value); }

      constexpr const T&& get() const&& { return std::move(m_value); }
};

template <typename T>
class Strong_Adapter : public Strong_Base<T> {
   public:
      using Strong_Base<T>::Strong_Base;
};

template <std::integral T>
class Strong_Adapter<T> : public Strong_Base<T> {
   public:
      using Strong_Base<T>::Strong_Base;
};

template <concepts::container T>
class Container_Strong_Adapter_Base : public Strong_Base<T> {
   public:
      using value_type = typename T::value_type;
      using size_type = typename T::size_type;
      using iterator = typename T::iterator;
      using const_iterator = typename T::const_iterator;

   public:
      using Strong_Base<T>::Strong_Base;

      explicit Container_Strong_Adapter_Base(size_t size)
         requires(concepts::resizable_container<T>)
            : Container_Strong_Adapter_Base(T(size)) {}

      template <typename InputIt>
      Container_Strong_Adapter_Base(InputIt begin, InputIt end) : Container_Strong_Adapter_Base(T(begin, end)) {}

   public:
      decltype(auto) begin() noexcept(noexcept(this->get().begin())) { return this->get().begin(); }

      decltype(auto) begin() const noexcept(noexcept(this->get().begin())) { return this->get().begin(); }

      decltype(auto) end() noexcept(noexcept(this->get().end())) { return this->get().end(); }

      decltype(auto) end() const noexcept(noexcept(this->get().end())) { return this->get().end(); }

      decltype(auto) cbegin() noexcept(noexcept(this->get().cbegin())) { return this->get().cbegin(); }

      decltype(auto) cbegin() const noexcept(noexcept(this->get().cbegin())) { return this->get().cbegin(); }

      decltype(auto) cend() noexcept(noexcept(this->get().cend())) { return this->get().cend(); }

      decltype(auto) cend() const noexcept(noexcept(this->get().cend())) { return this->get().cend(); }

      size_type size() const noexcept(noexcept(this->get().size())) { return this->get().size(); }

      bool empty() const noexcept(noexcept(this->get().empty()))
         requires(concepts::has_empty<T>)
      {
         return this->get().empty();
      }

      void resize(size_type size) noexcept(noexcept(this->get().resize(size)))
         requires(concepts::resizable_container<T>)
      {
         this->get().resize(size);
      }

      void reserve(size_type size) noexcept(noexcept(this->get().reserve(size)))
         requires(concepts::reservable_container<T>)
      {
         this->get().reserve(size);
      }

      template <typename U>
      decltype(auto) operator[](U&& i) const noexcept(noexcept(this->get().operator[](i))) {
         return this->get()[std::forward<U>(i)];
      }

      template <typename U>
      decltype(auto) operator[](U&& i) noexcept(noexcept(this->get().operator[](i))) {
         return this->get()[std::forward<U>(i)];
      }

      template <typename U>
      decltype(auto) at(U&& i) const noexcept(noexcept(this->get().at(i)))
         requires(concepts::has_bounds_checked_accessors<T>)
      {
         return this->get().at(std::forward<U>(i));
      }

      template <typename U>
      decltype(auto) at(U&& i) noexcept(noexcept(this->get().at(i)))
         requires(concepts::has_bounds_checked_accessors<T>)
      {
         return this->get().at(std::forward<U>(i));
      }
};

template <concepts::container T>
class Strong_Adapter<T> : public Container_Strong_Adapter_Base<T> {
   public:
      using Container_Strong_Adapter_Base<T>::Container_Strong_Adapter_Base;
};

template <concepts::contiguous_container T>
class Strong_Adapter<T> : public Container_Strong_Adapter_Base<T> {
   public:
      using pointer = typename T::pointer;
      using const_pointer = typename T::const_pointer;

   public:
      using Container_Strong_Adapter_Base<T>::Container_Strong_Adapter_Base;

      explicit Strong_Adapter(std::span<const typename Container_Strong_Adapter_Base<T>::value_type> span) :
            Strong_Adapter(T(span.begin(), span.end())) {}

      // Disambiguates the usage of string literals, otherwise:
      // Strong_Adapter(std::span<>) and Strong_Adapter(const char*)
      // would be ambiguous.
      explicit Strong_Adapter(const char* str)
         requires(std::same_as<T, std::string>)
            : Strong_Adapter(std::string(str)) {}

   public:
      decltype(auto) data() noexcept(noexcept(this->get().data())) { return this->get().data(); }

      decltype(auto) data() const noexcept(noexcept(this->get().data())) { return this->get().data(); }
};

}  // namespace detail

/**
 * Strong types can be used as wrappers around common types to provide
 * compile time semantics. They usually contribute to more maintainable and
 * less error-prone code especially when dealing with function parameters.
 *
 * Internally, this provides adapters so that the wrapping strong type behaves
 * as much as the underlying type as possible and desirable.
 *
 * This implementation was inspired by:
 *   https://stackoverflow.com/a/69030899
 */
template <typename T, typename TagTypeT, typename... Capabilities>
class Strong : public detail::Strong_Adapter<T> {
   public:
      using detail::Strong_Adapter<T>::Strong_Adapter;

      template <typename CapabilityT>
      constexpr static bool has_capability() {
         return (std::is_same_v<CapabilityT, Capabilities> || ...);
      }

   private:
      using Tag = TagTypeT;
};

/**
 * @brief Generically unwraps a strong type to its underlying type.
 *
 * If the provided type is not a strong type, it is returned as is.
 *
 * @note This is meant as a helper for generic code that needs to deal with both
 *       wrapped strong types and bare objects. Use the ordinary `get()` method
 *       if you know that you are dealing with a strong type.
 *
 * @param t  value to be unwrapped
 * @return   the unwrapped value
 */
template <typename T>
[[nodiscard]] constexpr decltype(auto) unwrap_strong_type(T&& t) {
   if constexpr(!concepts::strong_type<std::remove_cvref_t<T>>) {
      // If the parameter type isn't a strong type, return it as is.
      return std::forward<T>(t);
   } else {
      // Unwrap the strong type and return the underlying value.
      return std::forward<T>(t).get();
   }
}

/**
 * @brief Wraps a value into a caller-defined (strong) type.
 *
 * If the provided object @p t is already of type @p T, it is returned as is.
 *
 * @note This is meant as a helper for generic code that needs to deal with both
 *       wrapped strong types and bare objects. Use the ordinary constructor if
 *       you know that you are dealing with a bare value type.
 *
 * @param t  value to be wrapped
 * @return   the wrapped value
 */
template <typename T, typename ParamT>
   requires std::constructible_from<T, ParamT> ||
            (concepts::strong_type<T> && std::constructible_from<typename T::wrapped_type, ParamT>)
[[nodiscard]] constexpr decltype(auto) wrap_strong_type(ParamT&& t) {
   if constexpr(std::same_as<std::remove_cvref_t<ParamT>, T>) {
      // Noop, if the parameter type already is the desired return type.
      return std::forward<ParamT>(t);
   } else if constexpr(std::constructible_from<T, ParamT>) {
      // Implicit conversion from the parameter type to the return type.
      return T{std::forward<ParamT>(t)};
   } else {
      // Explicitly calling the wrapped type's constructor to support
      // implicit conversions on types that mark their constructors as explicit.
      static_assert(concepts::strong_type<T> && std::constructible_from<typename T::wrapped_type, ParamT>);
      return T{typename T::wrapped_type{std::forward<ParamT>(t)}};
   }
}

namespace detail {

template <typename T>
struct wrapped_type_helper {
      using type = T;
};

template <concepts::strong_type T>
struct wrapped_type_helper<T> {
      using type = typename T::wrapped_type;
};

}  // namespace detail

/**
 * @brief Extracts the wrapped type from a strong type.
 *
 * If the provided type is not a strong type, it is returned as is.
 *
 * @note This is meant as a helper for generic code that needs to deal with both
 *       wrapped strong types and bare objects. Use the ordinary `::wrapped_type`
 *       declaration if you know that you are dealing with a strong type.
 */
template <typename T>
using strong_type_wrapped_type = typename detail::wrapped_type_helper<std::remove_cvref_t<T>>::type;

template <typename T, typename... Tags>
   requires(concepts::streamable<T>)
decltype(auto) operator<<(std::ostream& os, const Strong<T, Tags...>& v) {
   return os << v.get();
}

template <typename T, typename... Tags>
   requires(std::equality_comparable<T>)
bool operator==(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs) {
   return lhs.get() == rhs.get();
}

template <typename T, typename... Tags>
   requires(std::three_way_comparable<T>)
auto operator<=>(const Strong<T, Tags...>& lhs, const Strong<T, Tags...>& rhs) {
   return lhs.get() <=> rhs.get();
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator<=>(T1 a, Strong<T2, Tags...> b) {
   return a <=> b.get();
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator<=>(Strong<T1, Tags...> a, T2 b) {
   return a.get() <=> b;
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator==(T1 a, Strong<T2, Tags...> b) {
   return a == b.get();
}

template <std::integral T1, std::integral T2, typename... Tags>
auto operator==(Strong<T1, Tags...> a, T2 b) {
   return a.get() == b;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator+(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a + b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator+(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() + b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator+(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() + b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator-(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a - b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator-(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() - b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator-(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() - b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator*(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a * b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator*(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() * b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator*(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() * b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator/(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a / b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator/(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() / b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator/(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() / b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator^(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a ^ b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator^(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() ^ b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator^(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() ^ b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator&(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a & b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator&(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() & b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator&(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() & b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator|(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a | b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator|(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() | b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator|(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() | b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator>>(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a >> b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator>>(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() >> b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator>>(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() >> b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator<<(T1 a, Strong<T2, Tags...> b) {
   return Strong<T2, Tags...>(a << b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr decltype(auto) operator<<(Strong<T1, Tags...> a, T2 b) {
   return Strong<T1, Tags...>(a.get() << b);
}

template <std::integral T, typename... Tags>
constexpr decltype(auto) operator<<(Strong<T, Tags...> a, Strong<T, Tags...> b) {
   return Strong<T, Tags...>(a.get() << b.get());
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator+=(Strong<T1, Tags...>& a, T2 b) {
   a.get() += b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator+=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() += b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator-=(Strong<T1, Tags...>& a, T2 b) {
   a.get() -= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator-=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() -= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator*=(Strong<T1, Tags...>& a, T2 b) {
   a.get() *= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator*=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() *= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator/=(Strong<T1, Tags...>& a, T2 b) {
   a.get() /= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator/=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() /= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator^=(Strong<T1, Tags...>& a, T2 b) {
   a.get() ^= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator^=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() ^= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator&=(Strong<T1, Tags...>& a, T2 b) {
   a.get() &= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator&=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() &= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator|=(Strong<T1, Tags...>& a, T2 b) {
   a.get() |= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator|=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() |= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator>>=(Strong<T1, Tags...>& a, T2 b) {
   a.get() >>= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator>>=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() >>= b.get();
   return a;
}

template <std::integral T1, std::integral T2, typename... Tags>
   requires(detail::has_capability<EnableArithmeticWithPlainNumber, Tags...>)
constexpr auto operator<<=(Strong<T1, Tags...>& a, T2 b) {
   a.get() <<= b;
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator<<=(Strong<T, Tags...>& a, Strong<T, Tags...> b) {
   a.get() <<= b.get();
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator++(Strong<T, Tags...>& a, int) {
   auto tmp = a;
   ++a.get();
   return tmp;
}

template <std::integral T, typename... Tags>
constexpr auto operator++(Strong<T, Tags...>& a) {
   ++a.get();
   return a;
}

template <std::integral T, typename... Tags>
constexpr auto operator--(Strong<T, Tags...>& a, int) {
   auto tmp = a;
   --a.get();
   return tmp;
}

template <std::integral T, typename... Tags>
constexpr auto operator--(Strong<T, Tags...>& a) {
   --a.get();
   return a;
}

/**
 * This mimics a std::span but keeps track of the strong-type information. Use
 * this when you would want to use `const Strong<...>&` as a parameter
 * declaration. In particular this allows assigning strong-type information to
 * slices of a bigger buffer without copying the bytes. E.g:
 *
 *    using Foo = Strong<std::vector<uint8_t>, Foo_>;
 *
 *    void bar(StrongSpan<Foo> foo) { ... }
 *
 *    std::vector<uint8_t> buffer;
 *    BufferSlicer slicer(buffer);
 *    bar(slicer.take<Foo>());  // This does not copy the data from buffer but
 *                              // just annotates the 'Foo' strong-type info.
 */
template <concepts::contiguous_strong_type T>
class StrongSpan {
      using underlying_span = std::
         conditional_t<std::is_const_v<T>, std::span<const typename T::value_type>, std::span<typename T::value_type>>;

   public:
      using value_type = typename underlying_span::value_type;
      using size_type = typename underlying_span::size_type;
      using iterator = typename underlying_span::iterator;
      using pointer = typename underlying_span::pointer;
      using const_pointer = typename underlying_span::const_pointer;

      StrongSpan() = default;

      explicit StrongSpan(underlying_span span) : m_span(span) {}

      // NOLINTNEXTLINE(*-explicit-conversions)
      StrongSpan(T& strong) : m_span(strong) {}

      // Allows implicit conversion from `StrongSpan<T>` to `StrongSpan<const T>`.
      // Note that this is not bi-directional. Conversion from `StrongSpan<const T>`
      // to `StrongSpan<T>` is not allowed.
      //
      // TODO: Technically, we should be able to phrase this with a `requires std::is_const_v<T>`
      //       instead of the `std::enable_if` constructions. clang-tidy (14 or 15) doesn't seem
      //       to pick up on that (yet?). As a result, for a non-const T it assumes this to be
      //       a declaration of an ordinary copy constructor. The existence of a copy constructor
      //       is interpreted as "not cheap to copy", setting off the `performance-unnecessary-value-param` check.
      //       See also: https://github.com/randombit/botan/issues/3591
      template <concepts::contiguous_strong_type T2>
      // NOLINTNEXTLINE(*-explicit-conversions)
      StrongSpan(const StrongSpan<T2>& other)
         requires(std::is_same_v<T2, std::remove_const_t<T>>)
            : m_span(other.get()) {}

      StrongSpan(const StrongSpan& other) = default;
      StrongSpan(StrongSpan&& other) = default;
      StrongSpan& operator=(const StrongSpan& other) = default;
      StrongSpan& operator=(StrongSpan&& other) = default;

      ~StrongSpan() = default;

      /**
       * @returns the underlying std::span without any type constraints
       */
      underlying_span get() const { return m_span; }

      /**
       * @returns the underlying std::span without any type constraints
       */
      underlying_span get() { return m_span; }

      decltype(auto) data() noexcept(noexcept(this->m_span.data())) { return this->m_span.data(); }

      decltype(auto) data() const noexcept(noexcept(this->m_span.data())) { return this->m_span.data(); }

      decltype(auto) size() const noexcept(noexcept(this->m_span.size())) { return this->m_span.size(); }

      bool empty() const noexcept(noexcept(this->m_span.empty())) { return this->m_span.empty(); }

      decltype(auto) begin() noexcept(noexcept(this->m_span.begin())) { return this->m_span.begin(); }

      decltype(auto) begin() const noexcept(noexcept(this->m_span.begin())) { return this->m_span.begin(); }

      decltype(auto) end() noexcept(noexcept(this->m_span.end())) { return this->m_span.end(); }

      decltype(auto) end() const noexcept(noexcept(this->m_span.end())) { return this->m_span.end(); }

      decltype(auto) operator[](typename underlying_span::size_type i) const noexcept { return this->m_span[i]; }

   private:
      underlying_span m_span;
};

template <typename>
struct is_strong_span : std::false_type {};

template <typename T>
struct is_strong_span<StrongSpan<T>> : std::true_type {};

template <typename T>
constexpr bool is_strong_span_v = is_strong_span<T>::value;

}  // namespace Botan

namespace Botan {

/**
* Return a shared reference to a global PRNG instance provided by the
* operating system. For instance might be instantiated by /dev/urandom
* or CryptGenRandom.
*/
BOTAN_PUBLIC_API(2, 0) RandomNumberGenerator& system_rng();

/*
* Instantiable reference to the system RNG.
*/
class BOTAN_PUBLIC_API(2, 0) System_RNG final : public RandomNumberGenerator {
   public:
      std::string name() const override { return system_rng().name(); }

      bool is_seeded() const override { return system_rng().is_seeded(); }

      bool accepts_input() const override { return system_rng().accepts_input(); }

      void clear() override { system_rng().clear(); }

   protected:
      void fill_bytes_with_input(std::span<uint8_t> out, std::span<const uint8_t> in) override {
         system_rng().randomize_with_input(out, in);
      }
};

}  // namespace Botan

namespace Botan {

/*
* Get information describing the version
*/

/**
* Get a human-readable single-line string identifying the version of Botan.
* No particular format should be assumed.
* @return version string
*/
BOTAN_PUBLIC_API(2, 0) std::string version_string();

/**
* Same as version_string() except returning a pointer to a statically
* allocated string.
* @return version string
*/
BOTAN_PUBLIC_API(2, 0) const char* version_cstr();

/**
* Return a version string of the form "MAJOR.MINOR.PATCH" where
* each of the values is an integer.
*/
BOTAN_PUBLIC_API(2, 4) std::string short_version_string();

/**
* Same as version_short_string except returning a pointer to the string.
*/
BOTAN_PUBLIC_API(2, 4) const char* short_version_cstr();

/**
* Return the date this version of botan was released, in an integer of
* the form YYYYMMDD. For instance a version released on May 21, 2013
* would return the integer 20130521. If the currently running version
* is not an official release, this function will return 0 instead.
*
* @return release date, or zero if unreleased
*/
BOTAN_PUBLIC_API(2, 0) uint32_t version_datestamp();

/**
* Get the major version number.
* @return major version number
*/
BOTAN_PUBLIC_API(2, 0) uint32_t version_major();

/**
* Get the minor version number.
* @return minor version number
*/
BOTAN_PUBLIC_API(2, 0) uint32_t version_minor();

/**
* Get the patch number.
* @return patch number
*/
BOTAN_PUBLIC_API(2, 0) uint32_t version_patch();

/**
* Returns a string that is set to a revision identifier corresponding to the
* source, or `nullopt` if this could not be determined. It is set for all
* official releases, and for builds that originated from within a git checkout.
*
* @return VC revision
*/
BOTAN_PUBLIC_API(3, 8) std::optional<std::string> version_vc_revision();

/**
* Return any string that is set at build time using the `--distribution-info`
* option. It allows a packager of the library to specify any distribution-specific
* patches. If no value is given at build time, returns `nullopt`.
*
* @return distribution info
*/
BOTAN_PUBLIC_API(3, 8) std::optional<std::string> version_distribution_info();

/**
* Usable for checking that the DLL version loaded at runtime exactly matches the
* compile-time version. Call using BOTAN_VERSION_* macro values, like so:
*
* ```
* Botan::runtime_version_check(BOTAN_VERSION_MAJOR, BOTAN_VERSION_MINOR, BOTAN_VERSION_PATCH);
* ```
*
* It will return an empty string if the versions match, or otherwise an error
* message indicating the discrepancy. This only is useful in dynamic libraries,
* where it is possible to compile and run against different versions.
*/
BOTAN_PUBLIC_API(2, 0) std::string runtime_version_check(uint32_t major, uint32_t minor, uint32_t patch);

/**
* Certain build-time options, used for testing, result in a binary which is not
* safe for use in a production system. This function can be used to test for such
* a configuration at runtime.
*
* Currently these unsafe conditions include:
*
* - Unsafe fuzzer mode (--unsafe-fuzzer-mode) which intentionally disables various
*   checks in order to improve the effectiveness of fuzzing.
* - Terminate on asserts (--unsafe-terminate-on-asserts) which intentionally aborts
*   if any internal assertion failure occurs, rather than throwing an exception.
*/
BOTAN_PUBLIC_API(3, 8) bool unsafe_for_production_build();

// NOLINTBEGIN(*-macro-usage)

/*
* Macros for compile-time version checks
*
* Return a value that can be used to compare versions. The current
* (compile-time) version is available as the macro BOTAN_VERSION_CODE. For
* instance, to choose one code path for version 3.1.0 and later, and another
* code path for older releases:
*
* ```
* #if BOTAN_VERSION_CODE >= BOTAN_VERSION_CODE_FOR(3,1,0)
*    // 3.1+ code path
* #else
*    // code path for older versions
* #endif
* ```
*/
#define BOTAN_VERSION_CODE_FOR(a, b, c) (((a) << 16) | ((b) << 8) | (c))

/**
* Compare using BOTAN_VERSION_CODE_FOR, as in
*  # if BOTAN_VERSION_CODE < BOTAN_VERSION_CODE_FOR(1,8,0)
*  #    error "Botan version too old"
*  # endif
*/
#define BOTAN_VERSION_CODE BOTAN_VERSION_CODE_FOR(BOTAN_VERSION_MAJOR, BOTAN_VERSION_MINOR, BOTAN_VERSION_PATCH)

// NOLINTEND(*-macro-usage)

}  // namespace Botan

namespace Botan::X509 {

/**
* BER encode a key
* @param key the public key to encode
* @return BER encoding of this key
*/
inline std::vector<uint8_t> BER_encode(const Public_Key& key) {
   return key.subject_public_key();
}

/**
* PEM encode a public key into a string.
* @param key the key to encode
* @return PEM encoded key
*/
BOTAN_PUBLIC_API(2, 0) std::string PEM_encode(const Public_Key& key);

/**
* Create a public key from a data source.
* @param source the source providing the DER or PEM encoded key
* @return new public key object
*/
BOTAN_PUBLIC_API(3, 0) std::unique_ptr<Public_Key> load_key(DataSource& source);

#if defined(BOTAN_TARGET_OS_HAS_FILESYSTEM)
/**
* Create a public key from a file
* @param filename pathname to the file to load
* @return new public key object
*/
inline std::unique_ptr<Public_Key> load_key(std::string_view filename) {
   DataSource_Stream source(filename, true);
   return X509::load_key(source);
}
#endif

/**
* Create a public key from a memory region.
* @param enc the memory region containing the DER or PEM encoded key
* @return new public key object
*/
inline std::unique_ptr<Public_Key> load_key(const std::vector<uint8_t>& enc) {
   DataSource_Memory source(enc);
   return X509::load_key(source);
}

/**
* Create a public key from a memory region.
* @param enc the memory region containing the DER or PEM encoded key
* @return new public key object
*/
inline std::unique_ptr<Public_Key> load_key(std::span<const uint8_t> enc) {
   DataSource_Memory source(enc);
   return X509::load_key(source);
}

/**
* Copy a key.
* @param key the public key to copy
* @return new public key object
*/
inline std::unique_ptr<Public_Key> copy_key(const Public_Key& key) {
   DataSource_Memory source(PEM_encode(key));
   return X509::load_key(source);
}

}  // namespace Botan::X509

#endif // BOTAN_AMALGAMATION_H_
