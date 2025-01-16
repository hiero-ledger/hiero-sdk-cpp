// SPDX-License-Identifier: Apache-2.0
#include "impl/MirrorNodeContractQuery.h"

namespace Hiero
{
//------
MirrorNodeContractQuery& MirrorNodeContractQuery::setContractId(const std::string& id)
{
  mContractId = id;
  return *this;
}

//------
MirrorNodeContractQuery& MirrorNodeContractQuery::setContractEvmAddress(const std::string& address)
{
  mContractEvmAddress = address;
  return *this;
}

//------
MirrorNodeContractQuery& MirrorNodeContractQuery::setSender(const std::string& id)
{
  mSender = id;
  return *this;
}

//------
MirrorNodeContractQuery& MirrorNodeContractQuery::setSenderEvmAddress(const std::string& address)
{
  mSenderEvmAddress = address;
  return *this;
}

//------
MirrorNodeContractQuery& MirrorNodeContractQuery::setCallData(const std::vector<std::byte>& data)
{
  mCallData = data;
  return *this;
}

//------
MirrorNodeContractQuery& MirrorNodeContractQuery::setValue(int64_t val)
{
  mValue = val;
  return *this;
}

//------
MirrorNodeContractQuery& MirrorNodeContractQuery::setGasLimit(int64_t limit)
{
  mGasLimit = limit;
  return *this;
}

//------
MirrorNodeContractQuery& MirrorNodeContractQuery::setGasPrice(int64_t price)
{
  mGasPrice = price;
  return *this;
}

//------
MirrorNodeContractQuery& MirrorNodeContractQuery::setBlockNumber(int64_t number)
{
  mBlockNumber = number;
  return *this;
}

//------
json MirrorNodeContractQuery::toJson() const
{
  json j;

  j["data"] = internal::HexConverter::bytesToHex(mCallData);
  j["to"] = mContractEvmAddress;
  j["estimate"] = mEstimate;
  j["blockNumber"] = mBlockNumber;

  if (mSenderEvmAddress.has_value())
  {
    j["from"] = mSenderEvmAddress.value();
  }

  if (mGasLimit > 0)
  {
    j["gas"] = mGasLimit;
  }

  if (mGasPrice > 0)
  {
    j["gasPrice"] = mGasPrice;
  }

  if (mValue > 0)
  {
    j["value"] = mValue;
  }

  return j;
}

}