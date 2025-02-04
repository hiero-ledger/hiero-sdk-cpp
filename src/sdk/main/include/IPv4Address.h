// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IPV4_ADDRESS_H_
#define HIERO_SDK_CPP_IPV4_ADDRESS_H_

#include <array>
#include <cstddef>
#include <string>
#include <vector>

namespace Hiero
{
/**
 * An IPv4 address (without port).
 */
class IPv4Address
{
public:
  /**
   * Construct an IPv4Address object from a byte array.
   *
   * @param bytes The byte array from which to construct an IPv4Address.
   * @return The constructed IPv4Address object.
   * @throws std::invalid_argument If an IPv4Address could not be realized from the input bytes.
   */
  [[nodiscard]] static IPv4Address fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Get the byte array representation of this IPv4Address.
   *
   * @return The byte array representation of this IPv4Address.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Get the string representation of this IPv4Address.
   *
   * @return The string representation of this IPv4Address.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Check if the IPV4Address is empty.
   *
   * @return The boolean representation of an IPV4Address empty state.
   */
  [[nodiscard]] bool isEmpty() const;

private:
  /**
   * The four octets of the address.
   */
  std::vector<std::byte> mAddress;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_IPV4_ADDRESS_H_
