// SPDX-License-Identifier: Apache-2.0
#include "FeeComponents.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
FeeComponents FeeComponents::fromProtobuf(const proto::FeeComponents& proto)
{
  return FeeComponents()
    .setMin(proto.min())
    .setMax(proto.max())
    .setConstant(proto.constant())
    .setTransactionBandwidthBytes(proto.bpt())
    .setTransactionVerification(proto.vpt())
    .setTransactionRamByteHour(proto.rbh())
    .setTransactionStorageByteHour(proto.sbh())
    .setContractTransactionGas(proto.gas())
    .setTransferVolumeHbar(proto.tv())
    .setResponseMemoryByte(proto.bpr())
    .setResponseDiskByte(proto.sbpr());
}

//-----
FeeComponents FeeComponents::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::FeeComponents proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::FeeComponents> FeeComponents::toProtobuf() const
{
  auto proto = std::make_unique<proto::FeeComponents>();
  proto->set_min(mMin);
  proto->set_max(mMax);
  proto->set_constant(mConstant);
  proto->set_bpt(mTransactionBandwidthBytes);
  proto->set_vpt(mTransactionVerification);
  proto->set_rbh(mTransactionRamByteHour);
  proto->set_sbh(mTransactionStorageByteHour);
  proto->set_gas(mContractTransactionGas);
  proto->set_tv(mTransferVolumeHbar);
  proto->set_bpr(mResponseMemoryByte);
  proto->set_sbpr(mResponseDiskByte);
  return proto;
}

//-----
std::vector<std::byte> FeeComponents::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string FeeComponents::toString() const
{
  nlohmann::json json;
  json["mMin"] = mMin;
  json["mMax"] = mMax;
  json["mConstant"] = mConstant;
  json["mTransactionBandwidthBytes"] = mTransactionBandwidthBytes;
  json["mTransactionVerification"] = mTransactionVerification;
  json["mTransactionRamByteHour"] = mTransactionRamByteHour;
  json["mTransactionStorageByteHour"] = mTransactionStorageByteHour;
  json["mContractTransactionGas"] = mContractTransactionGas;
  json["mTransferVolumeHbar"] = mTransferVolumeHbar;
  json["mResponseMemoryByte"] = mResponseMemoryByte;
  json["mResponseDiskByte"] = mResponseDiskByte;
  return json.dump();
}

} // namespace Hiero
