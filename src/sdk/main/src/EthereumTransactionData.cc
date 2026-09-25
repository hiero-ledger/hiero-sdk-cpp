// SPDX-License-Identifier: Apache-2.0
#include "EthereumTransactionData.h"
#include "EthereumTransactionDataEip1559.h"
#include "EthereumTransactionDataEip7702.h"
#include "EthereumTransactionDataLegacy.h"
#include "impl/RLPItem.h"

namespace Hiero
{
//-----
EthereumTransactionData::EthereumTransactionData(std::vector<std::byte> callData)
  : mCallData(std::move(callData))
{
}

//-----
std::unique_ptr<EthereumTransactionData> EthereumTransactionData::fromBytes(const std::vector<std::byte>& bytes)
{
  if (bytes.empty())
  {
    throw std::invalid_argument("Input byte array is empty");
  }

  // Check the transaction type prefix
  const std::byte typePrefix = bytes.at(0);

  // EIP-7702 transactions start with 0x04
  if (typePrefix == std::byte(0x04))
  {
    return std::make_unique<EthereumTransactionDataEip7702>(EthereumTransactionDataEip7702::fromBytes(bytes));
  }

  // EIP-1559 transactions start with 0x02
  if (typePrefix == std::byte(0x02))
  {
    return std::make_unique<EthereumTransactionDataEip1559>(EthereumTransactionDataEip1559::fromBytes(bytes));
  }

  // Legacy transactions are RLP-encoded lists (no type prefix)
  RLPItem rlpItem;
  rlpItem.read(bytes);

  if (rlpItem.isType(RLPItem::RLPType::LIST_TYPE))
  {
    return std::make_unique<EthereumTransactionDataLegacy>(EthereumTransactionDataLegacy::fromBytes(bytes));
  }

  throw std::invalid_argument("Unknown Ethereum transaction type");
}

} // namespace Hiero
