// SPDX-License-Identifier: Apache-2.0
#include "ContractFunctionResult.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <contract_call_local.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
ContractFunctionResult ContractFunctionResult::fromProtobuf(const proto::ContractFunctionResult& proto)
{
  ContractFunctionResult contractFunctionResult;

  if (proto.has_contractid())
  {
    contractFunctionResult.mContractId = ContractId::fromProtobuf(proto.contractid());
  }

  contractFunctionResult.mContractCallResult = internal::Utilities::stringToByteVector(proto.contractcallresult());
  contractFunctionResult.mErrorMessage = proto.errormessage();

  // Remove the function selector bytes if an exception was thrown
  if (!contractFunctionResult.mErrorMessage.empty() &&
      internal::Utilities::isPrefixOf(contractFunctionResult.mContractCallResult, ERROR_PREFIX))
  {
    contractFunctionResult.mContractCallResult = internal::Utilities::removePrefix(
      contractFunctionResult.mContractCallResult, static_cast<long>(ERROR_PREFIX.size()));
  }

  contractFunctionResult.mBloom = internal::Utilities::stringToByteVector(proto.bloom());
  contractFunctionResult.mGasUsed = proto.gasused();

  for (int i = 0; i < proto.loginfo_size(); ++i)
  {
    contractFunctionResult.mLogs.push_back(ContractLogInfo::fromProtobuf(proto.loginfo(i)));
  }

  if (proto.has_evm_address())
  {
    contractFunctionResult.mEvmAddress =
      EvmAddress::fromBytes(internal::Utilities::stringToByteVector(proto.evm_address().value()));
  }

  contractFunctionResult.mGas = static_cast<uint64_t>(proto.gas());
  contractFunctionResult.mHbarAmount = Hbar(proto.amount(), HbarUnit::TINYBAR());
  contractFunctionResult.mFunctionParameters = internal::Utilities::stringToByteVector(proto.functionparameters());

  if (proto.has_sender_id())
  {
    contractFunctionResult.mSenderAccountId = AccountId::fromProtobuf(proto.sender_id());
  }

  for (int i = 0; i < proto.contract_nonces_size(); ++i)
  {
    contractFunctionResult.mContractNonces.push_back(ContractNonceInfo::fromProtobuf(proto.contract_nonces(i)));
  }

  if (proto.has_signer_nonce())
  {
    contractFunctionResult.mSignerNonce = static_cast<int64_t>(proto.signer_nonce().value());
  }

  return contractFunctionResult;
}

//-----
ContractFunctionResult ContractFunctionResult::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ContractFunctionResult proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::ContractFunctionResult> ContractFunctionResult::toProtobuf() const
{
  auto proto = std::make_unique<proto::ContractFunctionResult>();
  proto->set_allocated_contractid(mContractId.toProtobuf().release());
  proto->set_contractcallresult(internal::Utilities::byteVectorToString(mContractCallResult));
  proto->set_errormessage(mErrorMessage);
  proto->set_bloom(internal::Utilities::byteVectorToString(mBloom));
  proto->set_gasused(mGasUsed);
  std::for_each(
    mLogs.cbegin(), mLogs.cend(), [&proto](const ContractLogInfo& log) { *proto->add_loginfo() = *log.toProtobuf(); });

  if (mEvmAddress.has_value())
  {
    proto->mutable_evm_address()->set_value(internal::Utilities::byteVectorToString(mEvmAddress->toBytes()));
  }

  proto->set_gas(static_cast<int64_t>(mGas));
  proto->set_amount(mHbarAmount.toTinybars());
  proto->set_functionparameters(internal::Utilities::byteVectorToString(mFunctionParameters));
  proto->set_allocated_sender_id(mSenderAccountId.toProtobuf().release());
  std::for_each(mContractNonces.cbegin(),
                mContractNonces.cend(),
                [&proto](const ContractNonceInfo& nonceInfo)
                { *proto->add_contract_nonces() = *nonceInfo.toProtobuf(); });

  if (mSignerNonce.has_value())
  {
    proto->mutable_signer_nonce()->set_value(mSignerNonce.value());
  }

  return proto;
}

//-----
std::vector<std::byte> ContractFunctionResult::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string ContractFunctionResult::toString() const
{
  nlohmann::json json;
  json["mContractId"] = mContractId.toString();
  json["mContractCallResult"] = internal::HexConverter::bytesToHex(mContractCallResult);
  json["mErrorMessage"] = mErrorMessage;
  json["mBloom"] = internal::HexConverter::bytesToHex(mBloom);
  json["mGasUsed"] = mGasUsed;
  std::for_each(mLogs.cbegin(),
                mLogs.cend(),
                [&json](const ContractLogInfo& logInfo) { json["mLogs"].push_back(logInfo.toString()); });

  if (mEvmAddress.has_value())
  {
    json["mEvmAddress"] = mEvmAddress->toString();
  }

  json["mGas"] = mGas;
  json["mHbarAmount"] = mHbarAmount.toString();
  json["mFunctionParameters"] = internal::HexConverter::bytesToHex(mFunctionParameters);
  json["mSenderAccountId"] = mSenderAccountId.toString();
  std::for_each(mContractNonces.cbegin(),
                mContractNonces.cend(),
                [&json](const ContractNonceInfo& nonceInfo)
                { json["mContractNonces"].push_back(nonceInfo.toString()); });

  if (mSignerNonce.has_value())
  {
    json["mSignerNonce"] = mSignerNonce.value();
  }

  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const ContractFunctionResult& result)
{
  os << result.toString();
  return os;
}

//-----
std::string ContractFunctionResult::getString(int index) const
{
  return internal::Utilities::byteVectorToString(getDynamicBytes(index));
}

//-----
std::vector<std::string> ContractFunctionResult::getStringArray(int index) const
{
  const int offset = getInt32(index);
  const int count = getIntValueAt(offset);
  std::vector<std::string> strings;

  for (int i = 0; i < count; ++i)
  {
    const int strOffset = getIntValueAt(offset + 32 + (i * 32));
    strings.push_back(internal::Utilities::byteVectorToString(
      getByteString(offset + strOffset + 64, offset + strOffset + 64 + getIntValueAt(offset + strOffset + 32))));
  }

  return strings;
}

//-----
std::vector<std::byte> ContractFunctionResult::getByteArray(int index) const
{
  return getDynamicBytes(index);
}

//-----
std::vector<std::byte> ContractFunctionResult::getBytes32(int index) const
{
  return getByteString(index * 32, (index + 1) * 32);
}

//-----
bool ContractFunctionResult::getBool(int index) const
{
  return getInt8(index) != 0;
}

//-----
int8_t ContractFunctionResult::getInt8(int index) const
{
  return static_cast<int8_t>(mContractCallResult.at((index * 32) + 31));
}

//-----
int32_t ContractFunctionResult::getInt32(int index) const
{
  return getIntValueAt(index * 32);
}

//-----
int64_t ContractFunctionResult::getInt64(int index) const
{
  const int64_t value = *internal::Utilities::toTypePtr<int64_t>(&mContractCallResult.at((index * 32) + 24));
  return internal::Utilities::swapEndianness(value);
}

//-----
uint8_t ContractFunctionResult::getUint8(int index) const
{
  return static_cast<uint8_t>(getInt8(index));
}

//-----
uint32_t ContractFunctionResult::getUint32(int index) const
{
  return static_cast<uint32_t>(getInt32(index));
}

//-----
uint64_t ContractFunctionResult::getUint64(int index) const
{
  return static_cast<uint64_t>(getInt64(index));
}

//-----
std::string ContractFunctionResult::getAddress(int index) const
{
  return internal::HexConverter::bytesToHex(getByteString(index * 32 + 12, (index + 1) * 32));
}

//-----
std::vector<std::byte> ContractFunctionResult::getDynamicBytes(int index) const
{
  const int offset = getInt32(index);
  return getByteString(offset + 32, offset + 32 + getIntValueAt(offset));
}

//-----
int ContractFunctionResult::getIntValueAt(int offset) const
{
  const int value = *internal::Utilities::toTypePtr<int>(&mContractCallResult.at(offset + 28));
  return internal::Utilities::swapEndianness(value);
}

//-----
std::vector<std::byte> ContractFunctionResult::getByteString(int start, int end) const
{
  return { mContractCallResult.cbegin() + start, mContractCallResult.cbegin() + end };
}

} // namespace Hiero
