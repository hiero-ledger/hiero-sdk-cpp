// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ETHEREUM_TRANSACTION_DATA_EIP1559_H_
#define HIERO_SDK_CPP_ETHEREUM_TRANSACTION_DATA_EIP1559_H_

#include "EthereumTransactionData.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace Hiero
{
/**
 * Wrapper class for EIP1559 Ethereum data.
 */
class EthereumTransactionDataEip1559 : public EthereumTransactionData
{
public:
  EthereumTransactionDataEip1559() = default;

  /**
   * Construct with all fields of Ethereum EIP1559 data.
   *
   * @param chainId  The chain ID.
   * @param nonce    The transaction nonce.
   * @param gasPrice The price of one gas.
   * @param gasLimit The amount of gas available for the transaction.
   * @param to       The receiver of the transaction.
   * @param value    The value of the transaction.
   * @param callData The call data.
   * @param v        The v value of the signature.
   * @param r        The r value of the signature.
   * @param s        The s value of the signature.
   */
  explicit EthereumTransactionDataEip1559(std::vector<std::byte> chainId,
                                          std::vector<std::byte> nonce,
                                          std::vector<std::byte> maxPriorityGas,
                                          std::vector<std::byte> maxGas,
                                          std::vector<std::byte> gasLimit,
                                          std::vector<std::byte> to,
                                          std::vector<std::byte> value,
                                          std::vector<std::byte> callData,
                                          std::vector<std::byte> accessList,
                                          std::vector<std::byte> recoveryId,
                                          std::vector<std::byte> r,
                                          std::vector<std::byte> s);

  /**
   * Construct an EthereumTransactionDataEip1559 object from raw bytes.
   *
   * @param bytes The raw bytes of the ethereum data.
   * @return The constructed EthereumTransactionDataEip1559.
   * @throws std::invalid_argument If the input byte array doesn't follow EIP1559 standards.
   */
  [[nodiscard]] static EthereumTransactionDataEip1559 fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Derived from EthereumTransactionData. Serialize this EthereumTransactionDataEip1559 object into RLP-encoded bytes.
   *
   * @return The RLP-encoded serialized EthereumTransactionDataEip1559.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const override;

  /**
   * Derived from EthereumTransactionData. Serialize this EthereumTransactionDataEip1559 object into a hex-encoded,
   * RLP-encoded string.
   *
   * @return The hex-encoded, RLP-encoded EthereumTransactionDataEip1559.
   */
  [[nodiscard]] std::string toString() const override;

  /**
   * The ID of the chain.
   */
  std::vector<std::byte> mChainId;

  /**
   * The transaction's nonce.
   */
  std::vector<std::byte> mNonce;

  /**
   * An 'optional' additional fee in Ethereum that is paid directly to miners in order to incentivize them to include
   * your transaction in a block. Not used in Hiero
   */
  std::vector<std::byte> mMaxPriorityGas;

  /**
   * The maximum amount, in tinybars, that the payer of the Hiero transaction is willing to pay to complete the
   * transaction.
   */
  std::vector<std::byte> mMaxGas;

  /**
   * The amount of gas available for the transaction.
   */
  std::vector<std::byte> mGasLimit;

  /**
   * The receiver of the transaction.
   */
  std::vector<std::byte> mTo;

  /**
   * The transaction value.
   */
  std::vector<std::byte> mValue;

  /**
   * The array of addresses and storage keys that the transaction plans to access.
   */
  std::vector<std::byte> mAccessList;

  /**
   * The recovery parameter used to ease the signature verification.
   */
  std::vector<std::byte> mRecoveryId;

  /**
   * The R value of the signature.
   */
  std::vector<std::byte> mR;

  /**
   * The S value of the signature.
   */
  std::vector<std::byte> mS;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ETHEREUM_TRANSACTION_DATA_EIP1559_H_
