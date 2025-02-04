// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_EVM_ADDRESS_H_
#define HIERO_SDK_CPP_EVM_ADDRESS_H_

#include <string>
#include <string_view>
#include <vector>

namespace Hiero
{
/**
 * A wrapper class for a 20-byte ethereum account address.
 */
class EvmAddress
{
public:
  /**
   * The number of bytes an EVM address should be.
   */
  static constexpr const size_t NUM_BYTES = 20ULL;

  /**
   * Construct an EvmAddress from a hex-encoded string that represents the 20-byte EVM address.
   *
   * @param address The hex-encoded string representing the EvmAddress.
   * @throws std::invalid_argument If the input string is malformed.
   * @throws OpenSSLException If OpenSSL is unable to convert the string to a byte array.
   */
  [[nodiscard]] static EvmAddress fromString(std::string_view address);

  /**
   * Construct an EvmAddress from a 20-byte buffer.
   *
   * @param bytes The bytes of the EvmAddress.
   * @throws std::invalid_argument If the input byte buffer is not 20 bytes long.
   */
  [[nodiscard]] static EvmAddress fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Get the hex-encoded string representation of this EVM address.
   *
   * @return The hex-encoded string representation of this EVM address.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Get the bytes of this EVM address.
   *
   * @return The bytes of this EVM address.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

private:
  EvmAddress() = default;

  /**
   * Helper function for making sure this EVM address is correctly formed.
   *
   * @throws std::invalid_argument If this EVM address is malformed.
   */
  void checkEvmAddress() const;

  /**
   * The 20-byte buffer that represents the address for an ethereum account.
   */
  std::vector<std::byte> mEvmAddress;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_EVM_ADDRESS_H_
