// SPDX-License-Identifier: Apache-2.0
#include "FeeData.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
FeeData FeeData::fromProtobuf(const proto::FeeData& proto)
{
  FeeData feeData;

  if (proto.has_nodedata())
  {
    feeData.mNodeData = FeeComponents::fromProtobuf(proto.nodedata());
  }

  if (proto.has_networkdata())
  {
    feeData.mNetworkData = FeeComponents::fromProtobuf(proto.networkdata());
  }

  if (proto.has_servicedata())
  {
    feeData.mServiceData = FeeComponents::fromProtobuf(proto.servicedata());
  }

  feeData.mType = gProtobufSubTypeToFeeDataType.at(proto.subtype());
  return feeData;
}

//-----
FeeData FeeData::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::FeeData feeData;
  feeData.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(feeData);
}

//-----
std::unique_ptr<proto::FeeData> FeeData::toProtobuf() const
{
  auto proto = std::make_unique<proto::FeeData>();
  proto->set_allocated_nodedata(mNodeData.toProtobuf().release());
  proto->set_allocated_networkdata(mNetworkData.toProtobuf().release());
  proto->set_allocated_servicedata(mServiceData.toProtobuf().release());
  proto->set_subtype(gFeeDataTypeToProtobufSubType.at(mType));
  return proto;
}

//-----
std::vector<std::byte> FeeData::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string FeeData::toString() const
{
  nlohmann::json json;
  json["mNodeData"] = mNodeData.toString();
  json["mNetworkData"] = mNetworkData.toString();
  json["mServiceData"] = mServiceData.toString();
  json["mType"] = gFeeDataTypeToString.at(mType);
  return json.dump();
}

} // namespace Hiero
