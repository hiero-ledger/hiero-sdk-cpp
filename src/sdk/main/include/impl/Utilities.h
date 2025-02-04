// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_UTILITIES_H_
#define HIERO_SDK_CPP_IMPL_UTILITIES_H_

#include <array>
#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>

namespace Hiero::internal::Utilities
{
/**
 * Reinterpret a pointer to a pointer of a different type.
 *
 * @param input The pointer to reinterpret.
 * @param A pointer to the same input, reinterpreted as a different type.
 */
template<typename ReturnType, typename InputType>
[[nodiscard]] ReturnType* toTypePtr(InputType* input)
{
  return reinterpret_cast<ReturnType*>(input);
}

template<typename ReturnType, typename InputType>
[[nodiscard]] const ReturnType* toTypePtr(const InputType* input)
{
  return reinterpret_cast<const ReturnType*>(input);
}

/**
 * Swap the endianness of an integral value.
 *
 * @param value The value of which to convert the endianness.
 * @return The value with the converted endianness.
 */
template<typename T>
[[nodiscard]] T swapEndianness(T value)
{
  static_assert(std::is_integral_v<T>, "swapEndianness works only with integral types");

  constexpr size_t numBytes = sizeof(T);
  union EndiannessUnion
  {
    T val;
    std::array<std::byte, numBytes> bytes;
  };

  EndiannessUnion src;
  EndiannessUnion dst;

  src.val = value;

  for (size_t i = 0; i < numBytes; ++i)
  {
    dst.bytes[i] = src.bytes[numBytes - i - 1];
  }

  return dst.val;
}

/**
 * Get the bytes (in big endian) that represent an integral type.
 *
 * @tparam T  The type of integer of which to get the bytes.
 * @param val The value of which to get the bytes.
 * @return An array of bytes that represents the input value.
 */
template<typename T>
[[nodiscard]] std::vector<std::byte> getBytes(const T& val)
{
  // Only allow integral types
  static_assert(std::is_integral_v<T>, "getBytes works only with integral types");

  std::vector<std::byte> bytes(sizeof(T));
  auto byte = internal::Utilities::toTypePtr<std::byte>(&val);
  for (size_t i = 0; i < sizeof(T); ++i)
  {
    bytes[sizeof(T) - i - 1] = *byte++;
  }

  return bytes;
}

/**
 * Determine if a byte array is the prefix of another.
 *
 * @param bytes  The bytes in which to look for the prefix.
 * @param prefix The prefix bytes to find in the byte array.
 * @return \c TRUE If prefix is a prefix of bytes, otherwise \c FALSE.
 */
[[nodiscard]] bool isPrefixOf(const std::vector<std::byte>& bytes, const std::vector<std::byte>& prefix);

/**
 * Remove a certain number of bytes from the beginning of a byte vector.
 *
 * @param bytes The bytes from which to remove the prefix.
 * @param num   The number of bytes to remove.
 * @return The byte vector with the removed prefix bytes.
 */
[[nodiscard]] std::vector<std::byte> removePrefix(const std::vector<std::byte>& bytes, long num);

/**
 * Concatenate byte vectors together (in the order of the arguments).
 *
 * @param vectors The byte vectors to concatenate together.
 * @return A byte vector containing the input byte vectors concatenated into one.
 */
[[nodiscard]] std::vector<std::byte> concatenateVectors(const std::vector<std::vector<std::byte>>& vectors);

/**
 * Convert a string to a byte vector.
 *
 * @param str The string to convert.
 * @return The byte vector representing the input string.
 */
[[nodiscard]] std::vector<std::byte> stringToByteVector(std::string_view str);

/**
 * Convert a string to a byte.
 *
 * @param str The string to convert.
 * @return The byte representing the string.
 */
[[nodiscard]] std::byte stringToByte(std::string_view str);

/**
 * Convert a byte vector to a string.
 *
 * @param bytes The byte vector to convert to a string.
 * @return The string that represent of the byte vector.
 */
[[nodiscard]] std::string byteVectorToString(const std::vector<std::byte>& bytes);

/**
 * Get a random number between the two input inclusive bounds.
 *
 * @param lowerBound The lower bound of the random number.
 * @param upperBound The upper bound of the random number.
 * @return A random number between the two bounds.
 * @throws std::invalid_argument If lowerBound is greater than or equal to upperBound.
 */
[[nodiscard]] unsigned int getRandomNumber(unsigned int lowerBound, unsigned int upperBound);

/**
 * Reads and parses a JSON file.
 *
 * This function reads the contents of a JSON file located at the specified path,
 * parses it, and returns a JSON object.
 *
 * @param path The path to the JSON file.
 * @return A JSON object representing the contents of the file.
 * @throws std::invalid_argument if the file cannot be found or if the contents
 *         are not valid JSON.
 */
[[nodiscard]] nlohmann::json fromConfigFile(std::string_view path);

} // namespace Hiero::internal::Utilities

#endif // HIERO_SDK_CPP_IMPL_UTILITIES_H_
