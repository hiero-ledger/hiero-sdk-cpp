// SPDX-License-Identifier: Apache-2.0
#include "impl/MirrorNodeContractQuery.h"
#include "impl/MirrorNetwork.h"
#include "impl/MirrorNodeGateway.h"

#include "string"

namespace Hiero
{
//-----
MirrorNodeContractQuery& MirrorNodeContractQuery::setContractId(const ContractId& id)
{
  mContractId = id;
  return *this;
}

//-----
MirrorNodeContractQuery& MirrorNodeContractQuery::setContractEvmAddress(const std::string& address)
{
  mContractEvmAddress = address;
  return *this;
}

//-----
MirrorNodeContractQuery& MirrorNodeContractQuery::setSender(const AccountId& id)
{
  mSender = id;
  return *this;
}

//-----
MirrorNodeContractQuery& MirrorNodeContractQuery::setSenderEvmAddress(const std::string& address)
{
  mSenderEvmAddress = address;
  return *this;
}

//-----
MirrorNodeContractQuery& MirrorNodeContractQuery::setFunction(const std::string& functionName,
                                                              std::optional<ContractFunctionParameters>& parameters)
{
  mCallData = parameters.value_or(ContractFunctionParameters()).toBytes(functionName);
  return *this;
}

//-----
MirrorNodeContractQuery& MirrorNodeContractQuery::setValue(int64_t val)
{
  mValue = val;
  return *this;
}

//-----
MirrorNodeContractQuery& MirrorNodeContractQuery::setGasLimit(int64_t limit)
{
  mGasLimit = limit;
  return *this;
}

//-----
MirrorNodeContractQuery& MirrorNodeContractQuery::setGasPrice(int64_t price)
{
  mGasPrice = price;
  return *this;
}

//-----
MirrorNodeContractQuery& MirrorNodeContractQuery::setBlockNumber(uint64_t number)
{
  mBlockNumber = number;
  return *this;
}

//-----
MirrorNodeContractQuery& MirrorNodeContractQuery::setEstimate(bool estimate)
{
  mEstimate = estimate;
  return *this;
}

//-----
void MirrorNodeContractQuery::populateContractEvmAddress(const Client& client)
{
  json contractInfo = internal::MirrorNodeGateway::MirrorNodeQuery(client.getClientMirrorNetwork()->getNetwork()[0],
                                                                   { this->getContractId().value().toString() },
                                                                   internal::MirrorNodeGateway::CONTRACT_INFO_QUERY);

  if (!contractInfo["evm_address"].empty())
  {
    std::string evmAddress = contractInfo["evm_address"].dump();
    // json dump returns strings in dquotes, so we need to trim first and last characters
    evmAddress = evmAddress.substr(1, evmAddress.length() - 2);
    setContractEvmAddress(evmAddress);
  }
}

//-----
json MirrorNodeContractQuery::toJson() const
{
  json obj;

  obj["data"] = internal::HexConverter::bytesToHex(mCallData);

  if (mContractEvmAddress.has_value())
  {
    obj["to"] = mContractEvmAddress.value();
  }

  obj["estimate"] = mEstimate;

  std::string blockNumberString = "latest";
  if (mBlockNumber != 0)
  {
    obj["blockNumber"] = std::to_string(mBlockNumber);
  }

  if (mSenderEvmAddress.has_value())
  {
    obj["from"] = mSenderEvmAddress.value();
  }
  else if (mSender.has_value())
  {
    obj["from"] = mSender.value().toSolidityAddress();
  }

  if (mGasLimit > 0)
  {
    obj["gas"] = mGasLimit;
  }

  if (mGasPrice > 0)
  {
    obj["gasPrice"] = mGasPrice;
  }

  if (mValue > 0)
  {
    obj["value"] = mValue;
  }

  return obj;
}

} // namespace Hiero