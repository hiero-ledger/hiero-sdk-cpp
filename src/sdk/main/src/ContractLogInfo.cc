// SPDX-License-Identifier: Apache-2.0
#include "ContractLogInfo.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <contract_call_local.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
ContractLogInfo ContractLogInfo::fromProtobuf(const proto::ContractLoginfo& proto)
{
  ContractLogInfo contractLogInfo;
  contractLogInfo.mContractId = ContractId::fromProtobuf(proto.contractid());
  contractLogInfo.mBloom = internal::Utilities::stringToByteVector(proto.bloom());

  for (int i = 0; i < proto.topic_size(); ++i)
  {
    contractLogInfo.mTopics.push_back(internal::Utilities::stringToByteVector(proto.topic(i)));
  }

  contractLogInfo.mData = internal::Utilities::stringToByteVector(proto.data());
  return contractLogInfo;
}

//-----
ContractLogInfo ContractLogInfo::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ContractLoginfo proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::ContractLoginfo> ContractLogInfo::toProtobuf() const
{
  auto proto = std::make_unique<proto::ContractLoginfo>();
  proto->set_allocated_contractid(mContractId.toProtobuf().release());
  proto->set_bloom(internal::Utilities::byteVectorToString(mBloom));
  std::for_each(mTopics.cbegin(),
                mTopics.cend(),
                [&proto](const std::vector<std::byte>& topic)
                { proto->add_topic(internal::Utilities::byteVectorToString(topic)); });
  proto->set_data(internal::Utilities::byteVectorToString(mData));
  return proto;
}

//-----
std::vector<std::byte> ContractLogInfo::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string ContractLogInfo::toString() const
{
  nlohmann::json json;
  json["mContractId"] = mContractId.toString();
  json["mBloom"] = internal::HexConverter::bytesToHex(mBloom);
  std::for_each(mTopics.cbegin(),
                mTopics.cend(),
                [&json](const std::vector<std::byte>& topic)
                { json["mTopics"].push_back(internal::HexConverter::bytesToHex(topic)); });
  json["mData"] = internal::HexConverter::bytesToHex(mData);

  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const ContractLogInfo& logInfo)
{
  os << logInfo.toString();
  return os;
}

} // namespace Hiero
