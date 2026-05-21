// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ETHEREUM_TRANSACTION_DATA_EIP7702_H_
#define HIERO_SDK_CPP_ETHEREUM_TRANSACTION_DATA_EIP7702_H_

#include "EthereumTransactionData.h"
#include "impl/RLPItem.h"

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

namespace Hiero
{
/**
 * Represents a single authorization entry for EIP-7702: [chainId, contractAddress, nonce, yParity, r, s]
 */
struct AuthorizationTuple
{
  std::vector<std::byte> mChainId;
  std::vector<std::byte> mContractAddress;
  std::vector<std::byte> mNonce;
  std::vector<std::byte> mYParity;
  std::vector<std::byte> mR;
  std::vector<std::byte> mS;
};

/**
 * Wrapper class for EIP7702 Ethereum data.
 * EIP-7702 introduces account abstraction by allowing EOAs to delegate their code to a contract.
 */
class EthereumTransactionDataEip7702 : public EthereumTransactionData
{
public:
  EthereumTransactionDataEip7702() = default;

  /**
   * Construct with all fields of Ethereum EIP7702 data.
   *
   * @param chainId           The chain ID.
   * @param nonce             The transaction nonce.
   * @param maxPriorityGas    The max priority gas (tip).
   * @param maxGas            The max gas price.
   * @param gasLimit          The amount of gas available for the transaction.
   * @param to                The receiver of the transaction.
   * @param value             The value of the transaction.
   * @param callData          The call data.
   * @param accessList        The access list.
   * @param authorizationList The authorization list for EIP-7702.
   * @param recoveryId        The recovery ID (y parity).
   * @param r                 The r value of the signature.
   * @param s                 The s value of the signature.
   */
  explicit EthereumTransactionDataEip7702(std::vector<std::byte> chainId,
                                          std::vector<std::byte> nonce,
                                          std::vector<std::byte> maxPriorityGas,
                                          std::vector<std::byte> maxGas,
                                          std::vector<std::byte> gasLimit,
                                          std::vector<std::byte> to,
                                          std::vector<std::byte> value,
                                          std::vector<std::byte> callData,
                                          std::vector<RLPItem> accessList,
                                          std::vector<AuthorizationTuple> authorizationList,
                                          std::vector<std::byte> recoveryId,
                                          std::vector<std::byte> r,
                                          std::vector<std::byte> s);

  /**
   * Construct an EthereumTransactionDataEip7702 object from raw bytes.
   *
   * @param bytes The raw bytes of the ethereum data.
   * @return The constructed EthereumTransactionDataEip7702.
   * @throws std::invalid_argument If the input byte array doesn't follow EIP7702 standards.
   */
  [[nodiscard]] static EthereumTransactionDataEip7702 fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Derived from EthereumTransactionData. Serialize this EthereumTransactionDataEip7702 object into RLP-encoded bytes.
   *
   * @return The RLP-encoded serialized EthereumTransactionDataEip7702.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const override;

  /**
   * Derived from EthereumTransactionData. Serialize this EthereumTransactionDataEip7702 object into a hex-encoded,
   * RLP-encoded string.
   *
   * @return The hex-encoded, RLP-encoded EthereumTransactionDataEip7702.
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
   * your transaction in a block. Not used in Hiero.
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
  std::vector<RLPItem> mAccessList;

  /**
   * The authorization list for EIP-7702. Each entry is a tuple: [chainId, contractAddress, nonce, yParity, r, s]
   * This allows an EOA to delegate its code to a contract.
   */
  std::vector<AuthorizationTuple> mAuthorizationList;

  /**
   * The recovery parameter used to ease the signature verification (y parity).
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

#endif // HIERO_SDK_CPP_ETHEREUM_TRANSACTION_DATA_EIP7702_H_
