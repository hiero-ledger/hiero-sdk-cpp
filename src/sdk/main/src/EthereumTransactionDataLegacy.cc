// SPDX-License-Identifier: Apache-2.0
#include "EthereumTransactionDataLegacy.h"
#include "impl/HexConverter.h"
#include "impl/RLPItem.h"

#include <stdexcept>

namespace Hiero
{
//-----
EthereumTransactionDataLegacy::EthereumTransactionDataLegacy(std::vector<std::byte> nonce,
                                                             std::vector<std::byte> gasPrice,
                                                             std::vector<std::byte> gasLimit,
                                                             std::vector<std::byte> to,
                                                             std::vector<std::byte> value,
                                                             std::vector<std::byte> callData,
                                                             std::vector<std::byte> v,
                                                             std::vector<std::byte> r,
                                                             std::vector<std::byte> s)
  : EthereumTransactionData(std::move(callData))
  , mNonce(std::move(nonce))
  , mGasPrice(std::move(gasPrice))
  , mGasLimit(std::move(gasLimit))
  , mTo(std::move(to))
  , mValue(std::move(value))
  , mV(std::move(v))
  , mR(std::move(r))
  , mS(std::move(s))
{
}

//-----
EthereumTransactionDataLegacy EthereumTransactionDataLegacy::fromBytes(const std::vector<std::byte>& bytes)
{
  RLPItem item;
  item.read(bytes);

  if (!item.isType(RLPItem::RLPType::LIST_TYPE))
  {
    throw std::invalid_argument("Input byte array does not represent a list of RLP-encoded elements");
  }

  else if (item.getValues().size() != 9)
  {
    throw std::invalid_argument("Input byte array does not contain 9 RLP-encoded elements");
  }

  return EthereumTransactionDataLegacy(item.getValues().at(0).getValue(),
                                       item.getValues().at(1).getValue(),
                                       item.getValues().at(2).getValue(),
                                       item.getValues().at(3).getValue(),
                                       item.getValues().at(4).getValue(),
                                       item.getValues().at(5).getValue(),
                                       item.getValues().at(6).getValue(),
                                       item.getValues().at(7).getValue(),
                                       item.getValues().at(8).getValue());
}

//-----
std::vector<std::byte> EthereumTransactionDataLegacy::toBytes() const
{
  RLPItem item(RLPItem::RLPType::LIST_TYPE);
  item.pushBack(mNonce);
  item.pushBack(mGasPrice);
  item.pushBack(mGasLimit);
  item.pushBack(mTo);
  item.pushBack(mValue);
  item.pushBack(mCallData);
  item.pushBack(mV);
  item.pushBack(mR);
  item.pushBack(mS);

  return item.write();
}

//-----
std::string EthereumTransactionDataLegacy::toString() const
{
  return "mNonce: " + internal::HexConverter::bytesToHex(mNonce) +
         "\nmGasPrice: " + internal::HexConverter::bytesToHex(mGasPrice) +
         "\nmGasLimit: " + internal::HexConverter::bytesToHex(mGasLimit) +
         "\nmTo: " + internal::HexConverter::bytesToHex(mTo) +
         "\nmValue: " + internal::HexConverter::bytesToHex(mValue) +
         "\nmCallData: " + internal::HexConverter::bytesToHex(mCallData) +
         "\nmV: " + internal::HexConverter::bytesToHex(mV) + "\nmR: " + internal::HexConverter::bytesToHex(mR) +
         "\nmS: " + internal::HexConverter::bytesToHex(mS);
}

} // namespace Hiero
