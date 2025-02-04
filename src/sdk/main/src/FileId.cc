// SPDX-License-Identifier: Apache-2.0
#include "FileId.h"
#include "Client.h"
#include "LedgerId.h"
#include "impl/EntityIdHelper.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <limits>

namespace Hiero
{
//-----
const FileId FileId::ADDRESS_BOOK = FileId(0ULL, 0ULL, 102ULL);

//-----
const FileId FileId::FEE_SCHEDULE = FileId(0ULL, 0ULL, 111ULL);

//-----
const FileId FileId::EXCHANGE_RATES = FileId(0ULL, 0ULL, 112ULL);

//-----
FileId::FileId(uint64_t num)
  : mFileNum(num)
{
}

//-----
FileId::FileId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum)
  : mShardNum(shard)
  , mRealmNum(realm)
  , mFileNum(num)
  , mChecksum(checksum)
{
}

//-----
bool FileId::operator==(const FileId& other) const
{
  return (mShardNum == other.mShardNum) && (mRealmNum == other.mRealmNum) && (mFileNum == other.mFileNum);
}

//-----
FileId FileId::fromString(std::string_view id)
{
  return FileId(internal::EntityIdHelper::getShardNum(id),
                internal::EntityIdHelper::getRealmNum(id),
                internal::EntityIdHelper::getEntityNum(id),
                internal::EntityIdHelper::getChecksum(id));
}

//-----
FileId FileId::fromSolidityAddress(std::string_view address)
{
  return internal::EntityIdHelper::fromSolidityAddress<FileId>(
    internal::EntityIdHelper::decodeSolidityAddress(address));
}

//-----
FileId FileId::fromProtobuf(const proto::FileID& proto)
{
  return FileId(static_cast<uint64_t>(proto.shardnum()),
                static_cast<uint64_t>(proto.realmnum()),
                static_cast<uint64_t>(proto.filenum()));
}

//-----
FileId FileId::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::FileID proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
void FileId::validateChecksum(const Client& client) const
{
  if (!mChecksum.empty())
  {
    internal::EntityIdHelper::validate(mShardNum, mRealmNum, mFileNum, client, mChecksum);
  }
}

//-----
std::unique_ptr<proto::FileID> FileId::toProtobuf() const
{
  auto body = std::make_unique<proto::FileID>();
  body->set_shardnum(static_cast<int64_t>(mShardNum));
  body->set_realmnum(static_cast<int64_t>(mRealmNum));
  body->set_filenum(static_cast<int64_t>(mFileNum));
  return body;
}

//-----
std::string FileId::toSolidityAddress() const
{
  return internal::EntityIdHelper::toSolidityAddress(mShardNum, mRealmNum, mFileNum);
}

//-----
std::string FileId::toString() const
{
  return internal::EntityIdHelper::toString(mShardNum, mRealmNum, mFileNum);
}

//-----
std::string FileId::toStringWithChecksum(const Client& client) const
{
  if (mChecksum.empty())
  {
    mChecksum = internal::EntityIdHelper::checksum(internal::EntityIdHelper::toString(mShardNum, mRealmNum, mFileNum),
                                                   client.getLedgerId());
  }

  return internal::EntityIdHelper::toString(mShardNum, mRealmNum, mFileNum, mChecksum);
}

//-----
std::vector<std::byte> FileId::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

} // namespace Hiero
