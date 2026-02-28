// SPDX-License-Identifier: Apache-2.0
#include "EthereumTransactionDataEip7702.h"
#include "impl/HexConverter.h"
#include "impl/RLPItem.h"
#include "impl/Utilities.h"

#include <stdexcept>

namespace Hiero
{
//-----
EthereumTransactionDataEip7702::EthereumTransactionDataEip7702(std::vector<std::byte> chainId,
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
  , mAuthorizationList(std::move(authorizationList))
  , mRecoveryId(std::move(recoveryId))
  , mR(std::move(r))
  , mS(std::move(s))
{
}

//-----
EthereumTransactionDataEip7702 EthereumTransactionDataEip7702::fromBytes(const std::vector<std::byte>& bytes)
{
  if (bytes.empty() || bytes.at(0) != std::byte(0x04))
  {
    throw std::invalid_argument(
      "Input byte array is malformed. It should start with 0x04 followed by 13 RLP-encoded elements as a list");
  }

  RLPItem item;
  item.read(internal::Utilities::removePrefix(bytes, 1));

  if (!item.isType(RLPItem::RLPType::LIST_TYPE) || item.getValues().size() != 13)
  {
    throw std::invalid_argument(
      "Input byte array is malformed. It should be 0x04 followed by 13 RLP-encoded elements as a list");
  }

  const std::vector<RLPItem>& values = item.getValues();

  // Parse access list (index 8)
  std::vector<RLPItem> accessList;
  if (values.at(8).isType(RLPItem::RLPType::LIST_TYPE))
  {
    accessList = values.at(8).getValues();
  }

  // Parse authorization list (index 9) - array of [chainId, contractAddress, nonce, yParity, r, s] tuples
  std::vector<AuthorizationTuple> authorizationList;
  if (values.at(9).isType(RLPItem::RLPType::LIST_TYPE))
  {
    for (const RLPItem& authTupleItem : values.at(9).getValues())
    {
      if (!authTupleItem.isType(RLPItem::RLPType::LIST_TYPE))
      {
        throw std::invalid_argument("Invalid authorization list entry: must be a list");
      }

      const std::vector<RLPItem>& tupleValues = authTupleItem.getValues();
      if (tupleValues.size() != 6)
      {
        throw std::invalid_argument(
          "Invalid authorization list entry: must be [chainId, contractAddress, nonce, yParity, r, s]");
      }

      AuthorizationTuple tuple;
      tuple.mChainId = tupleValues.at(0).getValue();
      tuple.mContractAddress = tupleValues.at(1).getValue();
      tuple.mNonce = tupleValues.at(2).getValue();
      tuple.mYParity = tupleValues.at(3).getValue();
      tuple.mR = tupleValues.at(4).getValue();
      tuple.mS = tupleValues.at(5).getValue();
      authorizationList.push_back(tuple);
    }
  }

  return EthereumTransactionDataEip7702(values.at(0).getValue(),
                                        values.at(1).getValue(),
                                        values.at(2).getValue(),
                                        values.at(3).getValue(),
                                        values.at(4).getValue(),
                                        values.at(5).getValue(),
                                        values.at(6).getValue(),
                                        values.at(7).getValue(),
                                        accessList,
                                        authorizationList,
                                        values.at(10).getValue(),
                                        values.at(11).getValue(),
                                        values.at(12).getValue());
}

//-----
std::vector<std::byte> EthereumTransactionDataEip7702::toBytes() const
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

  // Add access list
  RLPItem accessListItem(RLPItem::RLPType::LIST_TYPE);
  for (const RLPItem& item : mAccessList)
  {
    accessListItem.pushBack(item);
  }
  list.pushBack(accessListItem);

  // Add authorization list: array of [chainId, contractAddress, nonce, yParity, r, s] tuples
  RLPItem authorizationListItem(RLPItem::RLPType::LIST_TYPE);
  for (const AuthorizationTuple& authTuple : mAuthorizationList)
  {
    RLPItem tupleItem(RLPItem::RLPType::LIST_TYPE);
    tupleItem.pushBack(authTuple.mChainId);
    tupleItem.pushBack(authTuple.mContractAddress);
    tupleItem.pushBack(authTuple.mNonce);
    tupleItem.pushBack(authTuple.mYParity);
    tupleItem.pushBack(authTuple.mR);
    tupleItem.pushBack(authTuple.mS);
    authorizationListItem.pushBack(tupleItem);
  }
  list.pushBack(authorizationListItem);

  list.pushBack(mRecoveryId);
  list.pushBack(mR);
  list.pushBack(mS);

  // Prepend 0x04 as per EIP-7702 standard
  return internal::Utilities::concatenateVectors({ { std::byte(0x04) }, list.write() });
}

//-----
std::string EthereumTransactionDataEip7702::toString() const
{
  // Format access list
  std::string accessListStr = "[";
  for (size_t i = 0; i < mAccessList.size(); ++i)
  {
    if (i > 0)
    {
      accessListStr += ", ";
    }
    accessListStr += internal::HexConverter::bytesToHex(mAccessList[i].getValue());
  }
  accessListStr += "]";

  // Format authorization list
  std::string authorizationListStr = "[";
  for (size_t i = 0; i < mAuthorizationList.size(); ++i)
  {
    if (i > 0)
    {
      authorizationListStr += ", ";
    }
    authorizationListStr += "[";
    authorizationListStr += internal::HexConverter::bytesToHex(mAuthorizationList[i].mChainId) + ", ";
    authorizationListStr += internal::HexConverter::bytesToHex(mAuthorizationList[i].mContractAddress) + ", ";
    authorizationListStr += internal::HexConverter::bytesToHex(mAuthorizationList[i].mNonce) + ", ";
    authorizationListStr += internal::HexConverter::bytesToHex(mAuthorizationList[i].mYParity) + ", ";
    authorizationListStr += internal::HexConverter::bytesToHex(mAuthorizationList[i].mR) + ", ";
    authorizationListStr += internal::HexConverter::bytesToHex(mAuthorizationList[i].mS);
    authorizationListStr += "]";
  }
  authorizationListStr += "]";

  return "mChainId: " + internal::HexConverter::bytesToHex(mChainId) +
         "\nmNonce: " + internal::HexConverter::bytesToHex(mNonce) +
         "\nmMaxPriorityGas: " + internal::HexConverter::bytesToHex(mMaxPriorityGas) +
         "\nmMaxGas: " + internal::HexConverter::bytesToHex(mMaxGas) +
         "\nmGasLimit: " + internal::HexConverter::bytesToHex(mGasLimit) +
         "\nmTo: " + internal::HexConverter::bytesToHex(mTo) +
         "\nmValue: " + internal::HexConverter::bytesToHex(mValue) +
         "\nmCallData: " + internal::HexConverter::bytesToHex(mCallData) + "\nmAccessList: " + accessListStr +
         "\nmAuthorizationList: " + authorizationListStr +
         "\nmRecoveryId: " + internal::HexConverter::bytesToHex(mRecoveryId) +
         "\nmR: " + internal::HexConverter::bytesToHex(mR) + "\nmS: " + internal::HexConverter::bytesToHex(mS);
}

} // namespace Hiero
