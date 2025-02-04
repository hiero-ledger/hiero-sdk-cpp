// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_HEX_CONVERTER_H_
#define HIERO_SDK_CPP_IMPL_HEX_CONVERTER_H_

#include <string>
#include <string_view>
#include <vector>

namespace Hiero::internal::HexConverter
{
/**
 * Convert an array of bytes to a hex string.
 *
 * @param bytes The bytes from which to get the hex string.
 * @return A string containing the hex values of the input byte array.
 * @throws OpenSSLException If OpenSSL is unable to convert the input bytes to hex.
 */
std::string bytesToHex(const std::vector<std::byte>& bytes);

/**
 * Convert a hex string to the array of bytes it represents.
 *
 * @param hex The hex string to convert to a byte array.
 * @return The byte array containing the values of the input hex string.
 * @throws OpenSSLException If OpenSSL is unable to convert the input string to a byte array.
 */
std::vector<std::byte> hexToBytes(std::string_view hex);

/**
 * Convert a Base64-encoded string to a hexadecimal string.
 *
 * This function takes a Base64-encoded string as input and returns a hexadecimal
 * string.
 *
 * @param base64 The Base64-encoded string to convert.
 * @return The hexadecimal representation of the input.
 */
std::string base64ToHex(std::string_view base64);

} // namespace Hiero::internal::HexConverter

#endif // HIERO_SDK_CPP_IMPL_HEX_CONVERTER_H_
