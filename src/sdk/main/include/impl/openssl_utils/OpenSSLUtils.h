// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OPENSSL_UTILS_H_
#define HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OPENSSL_UTILS_H_

#include <string>
#include <string_view>
#include <vector>

namespace Hiero::internal::OpenSSLUtils
{
/**
 * The size of a SHA256 hash (in bytes).
 */
constexpr const size_t SHA256_HASH_SIZE = 32ULL;
/**
 * The size of a SHA384 hash (in bytes).
 */
constexpr const size_t SHA384_HASH_SIZE = 48ULL;
/**
 * The size of a KECCAK256 hash (in bytes)
 */
constexpr size_t KECCAK256_HASH_SIZE = 32ULL;
/**
 * The size of a SHA512 hash (in bytes).
 */
constexpr const size_t SHA512_HMAC_HASH_SIZE = 64ULL;
/**
 * The size of an OpenSSL error message.
 */
constexpr const size_t ERROR_MSG_SIZE = 256ULL;

/**
 * Computes the SHA256 hash of a byte array.
 *
 * @param data The byte array of which to compute the hash.
 * @return The SHA256 hash of the data.
 */
[[nodiscard]] std::vector<std::byte> computeSHA256(const std::vector<std::byte>& data);

/**
 * Compute the SHA384 hash of a byte array.
 *
 * @param data The byte array of which to compute the hash.
 * @return The SHA384 hash of the data.
 */
[[nodiscard]] std::vector<std::byte> computeSHA384(const std::vector<std::byte>& data);

/**
 * Compute the KECCAK256 hash of a byte array.
 *
 * @param data The byte array of which to compute the hash.
 * @return The KECCAK256 hash of the data.
 */
[[nodiscard]] std::vector<std::byte> computeKECCAK256(const std::vector<std::byte>& data);

/**
 * Compute the HMAC-SHA512 hash of a key and data.
 *
 * @param key  The key input to the hash function.
 * @param data The data input to the hash function.
 * @return The HMAC-SHA512 hash of the data and key.
 * @throws OpenSSLException If OpenSSL is unable to compute the HMAC-SHA512 hash of the given inputs.
 */
[[nodiscard]] std::vector<std::byte> computeSHA512HMAC(const std::vector<std::byte>& key,
                                                       const std::vector<std::byte>& data);

/**
 * Gets an error message for an OpenSSL error. Includes as much detail as possible.
 *
 * @param functionName The name of the OpenSSL function which caused the error.
 * @return An error string.
 */
[[nodiscard]] std::string getErrorMessage(std::string_view functionName);

/**
 * Get a vector of random bytes.
 *
 * @param count The number of random bytes to generate. Must be positive.
 * @return The vector of random bytes.
 * @throws std::invalid_argument If the input count is negative.
 * @throws OpenSSLException If OpenSSL is unable to generate random bytes.
 */
[[nodiscard]] std::vector<std::byte> getRandomBytes(int count);

} // namespace Hiero::internal::OpenSSLUtils

#endif // HIERO_SDK_CPP_IMPL_OPENSSL_UTILS_OPENSSL_UTILS_H_
