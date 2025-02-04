// SPDX-License-Identifier: Apache-2.0
#include "ScheduleId.h"
#include "Client.h"
#include "LedgerId.h"
#include "impl/EntityIdHelper.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <limits>

namespace Hiero
{
//-----
ScheduleId::ScheduleId(uint64_t num)
  : mScheduleNum(num)
{
}

//-----
ScheduleId::ScheduleId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum)
  : mShardNum(shard)
  , mRealmNum(realm)
  , mScheduleNum(num)
  , mChecksum(checksum)
{
}

//-----
bool ScheduleId::operator==(const ScheduleId& other) const
{
  return (mShardNum == other.mShardNum) && (mRealmNum == other.mRealmNum) && (mScheduleNum == other.mScheduleNum);
}

//-----
ScheduleId ScheduleId::fromString(std::string_view id)
{
  return ScheduleId(internal::EntityIdHelper::getShardNum(id),
                    internal::EntityIdHelper::getRealmNum(id),
                    internal::EntityIdHelper::getEntityNum(id),
                    internal::EntityIdHelper::getChecksum(id));
}

//-----
ScheduleId ScheduleId::fromProtobuf(const proto::ScheduleID& proto)
{
  return ScheduleId(static_cast<uint64_t>(proto.shardnum()),
                    static_cast<uint64_t>(proto.realmnum()),
                    static_cast<uint64_t>(proto.schedulenum()));
}

//-----
ScheduleId ScheduleId::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ScheduleID proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
void ScheduleId::validateChecksum(const Client& client) const
{
  if (!mChecksum.empty())
  {
    internal::EntityIdHelper::validate(mShardNum, mRealmNum, mScheduleNum, client, mChecksum);
  }
}

//-----
std::unique_ptr<proto::ScheduleID> ScheduleId::toProtobuf() const
{
  auto body = std::make_unique<proto::ScheduleID>();
  body->set_shardnum(static_cast<int64_t>(mShardNum));
  body->set_realmnum(static_cast<int64_t>(mRealmNum));
  body->set_schedulenum(static_cast<int64_t>(mScheduleNum));
  return body;
}

//-----
std::string ScheduleId::toString() const
{
  return internal::EntityIdHelper::toString(mShardNum, mRealmNum, mScheduleNum);
}

//-----
std::string ScheduleId::toStringWithChecksum(const Client& client) const
{
  if (mChecksum.empty())
  {
    mChecksum = internal::EntityIdHelper::checksum(
      internal::EntityIdHelper::toString(mShardNum, mRealmNum, mScheduleNum), client.getLedgerId());
  }

  return internal::EntityIdHelper::toString(mShardNum, mRealmNum, mScheduleNum, mChecksum);
}

//-----
std::vector<std::byte> ScheduleId::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

} // namespace Hiero
