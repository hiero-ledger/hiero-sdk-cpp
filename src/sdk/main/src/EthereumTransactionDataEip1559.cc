// SPDX-License-Identifier: Apache-2.0
#include "EthereumTransactionDataEip1559.h"
#include "impl/HexConverter.h"
#include "impl/RLPItem.h"
#include "impl/Utilities.h"

#include <stdexcept>

namespace Hiero
{
//-----
EthereumTransactionDataEip1559::EthereumTransactionDataEip1559(std::vector<std::byte> chainId,
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
                                                               std::vector<std::byte> s)
  : EthereumTransactionData(std::move(callData))
  , mChainId(std::move(chainId))
  , mNonce(std::move(nonce))
  , mMaxPriorityGas(std::move(maxPriorityGas))
  , mMaxGas(std::move(maxGas))
  , mGasLimit(std::move(gasLimit))
  , mTo(std::move(to))
  , mValue(std::move(value))
  , mAccessList(std::move(accessList))
  , mRecoveryId(std::move(recoveryId))
  , mR(std::move(r))
  , mS(std::move(s))
{
}

//-----
EthereumTransactionDataEip1559 EthereumTransactionDataEip1559::fromBytes(const std::vector<std::byte>& bytes)
{
  if (bytes.empty() || bytes.at(0) != std::byte(0x02))
  {
    throw std::invalid_argument(
      "Input byte array is malformed, It should be 0x02 followed by 12 RLP-encoded elements as a list");
  }

  RLPItem item;
  item.read(internal::Utilities::removePrefix(bytes, 1));

  if (!item.isType(RLPItem::RLPType::LIST_TYPE) || item.getValues().size() != 12)
  {
    throw std::invalid_argument(
      "Input byte array is malformed. It should be 0x02 followed by 12 RLP-encoded elements as a list");
  }

  return EthereumTransactionDataEip1559(item.getValues().at(0).getValue(),
                                        item.getValues().at(1).getValue(),
                                        item.getValues().at(2).getValue(),
                                        item.getValues().at(3).getValue(),
                                        item.getValues().at(4).getValue(),
                                        item.getValues().at(5).getValue(),
                                        item.getValues().at(6).getValue(),
                                        item.getValues().at(7).getValue(),
                                        item.getValues().at(8).getValue(),
                                        item.getValues().at(9).getValue(),
                                        item.getValues().at(10).getValue(),
                                        item.getValues().at(11).getValue());
}

//-----
std::vector<std::byte> EthereumTransactionDataEip1559::toBytes() const
{
  RLPItem list(RLPItem::RLPType::LIST_TYPE);
  list.pushBack(mChainId);
  list.pushBack(mNonce);
  list.pushBack(mMaxPriorityGas);
  list.pushBack(mMaxGas);
  list.pushBack(mGasLimit);
  list.pushBack(mTo);
  list.pushBack(mValue);
  list.pushBack(mCallData);
  list.pushBack(RLPItem());
  list.pushBack(mRecoveryId);
  list.pushBack(mR);
  list.pushBack(mS);

  return internal::Utilities::concatenateVectors({ { std::byte(0x02) }, list.write() });
}

//-----
std::string EthereumTransactionDataEip1559::toString() const
{
  return "mChainId: " + internal::HexConverter::bytesToHex(mChainId) +
         "\nmNonce: " + internal::HexConverter::bytesToHex(mNonce) +
         "\nmMaxPriorityGas: " + internal::HexConverter::bytesToHex(mMaxPriorityGas) +
         "\nmMaxGas: " + internal::HexConverter::bytesToHex(mMaxGas) +
         "\nmGasLimit: " + internal::HexConverter::bytesToHex(mGasLimit) +
         "\nmTo: " + internal::HexConverter::bytesToHex(mTo) +
         "\nmValue: " + internal::HexConverter::bytesToHex(mValue) +
         "\nmCallData: " + internal::HexConverter::bytesToHex(mCallData) +
         "\nmAccessList: " + internal::HexConverter::bytesToHex(mAccessList) +
         "\nmRecoveryId: " + internal::HexConverter::bytesToHex(mRecoveryId) +
         "\nmR: " + internal::HexConverter::bytesToHex(mR) + "\nmS: " + internal::HexConverter::bytesToHex(mS);
}

} // namespace Hiero
