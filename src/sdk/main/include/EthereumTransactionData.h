// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ETHEREUM_TRANSACTION_DATA_H_
#define HIERO_SDK_CPP_ETHEREUM_TRANSACTION_DATA_H_

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace Hiero
{
/**
 * Wrapper class for Ethereum data.
 */
class EthereumTransactionData
{
public:
  EthereumTransactionData() = default;
  virtual ~EthereumTransactionData() = default;

  /**
   * Construct with call data.
   *
   * @param callData The call data.
   */
  explicit EthereumTransactionData(std::vector<std::byte> callData);

  /**
   * Construct an EthereumTransactionData object from raw bytes.
   *
   * @param bytes The raw bytes of the ethereum data.
   * @return A pointer to the constructed EthereumTransactionData.
   */
  [[nodiscard]] static std::unique_ptr<EthereumTransactionData> fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Serialize this EthereumTransactionData object into RLP-encoded bytes.
   *
   * @return The RLP-encoded serialized EthereumTransactionData.
   */
  [[nodiscard]] virtual std::vector<std::byte> toBytes() const = 0;

  /**
   * Serialize this EthereumTransactionData object into a hex-encoded, RLP-encoded string.
   *
   * @return The hex-encoded, RLP-encoded EthereumTransactionData.
   */
  [[nodiscard]] virtual std::string toString() const = 0;

  /**
   * The call data.
   */
  std::vector<std::byte> mCallData;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ETHEREUM_TRANSACTION_DATA_H_
