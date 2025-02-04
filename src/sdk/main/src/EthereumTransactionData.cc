// SPDX-License-Identifier: Apache-2.0
#include "EthereumTransactionData.h"
#include "EthereumTransactionDataEip1559.h"
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
  RLPItem rlpItem;
  rlpItem.read(bytes);

  if (rlpItem.isType(RLPItem::RLPType::LIST_TYPE))
  {
    return std::make_unique<EthereumTransactionDataLegacy>(EthereumTransactionDataLegacy::fromBytes(bytes));
  }
  else
  {
    return std::make_unique<EthereumTransactionDataEip1559>(EthereumTransactionDataEip1559::fromBytes(bytes));
  }
}

} // namespace Hiero
