// SPDX-License-Identifier: Apache-2.0
#include "TokenId.h"
#include "Client.h"
#include "LedgerId.h"
#include "NftId.h"
#include "impl/EntityIdHelper.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <limits>

namespace Hiero
{
//-----
TokenId::TokenId(uint64_t num)
  : mTokenNum(num)
{
}

//-----
TokenId::TokenId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum)
  : mShardNum(shard)
  , mRealmNum(realm)
  , mTokenNum(num)
  , mChecksum(checksum)
{
}

//-----
bool TokenId::operator==(const TokenId& other) const
{
  return (mShardNum == other.mShardNum) && (mRealmNum == other.mRealmNum) && (mTokenNum == other.mTokenNum);
}

//-----
TokenId TokenId::fromString(std::string_view id)
{
  return TokenId(internal::EntityIdHelper::getShardNum(id),
                 internal::EntityIdHelper::getRealmNum(id),
                 internal::EntityIdHelper::getEntityNum(id),
                 internal::EntityIdHelper::getChecksum(id));
}

//-----
TokenId TokenId::fromSolidityAddress(std::string_view address)
{
  return internal::EntityIdHelper::fromSolidityAddress<TokenId>(
    internal::EntityIdHelper::decodeSolidityAddress(address));
}

//-----
TokenId TokenId::fromProtobuf(const proto::TokenID& proto)
{
  return TokenId(static_cast<uint64_t>(proto.shardnum()),
                 static_cast<uint64_t>(proto.realmnum()),
                 static_cast<uint64_t>(proto.tokennum()));
}

//-----
TokenId TokenId::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::TokenID proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
void TokenId::validateChecksum(const Client& client) const
{
  if (!mChecksum.empty())
  {
    internal::EntityIdHelper::validate(mShardNum, mRealmNum, mTokenNum, client, mChecksum);
  }
}

//-----
NftId TokenId::nft(uint64_t serial) const
{
  return NftId(*this, serial);
}

//-----
std::unique_ptr<proto::TokenID> TokenId::toProtobuf() const
{
  auto body = std::make_unique<proto::TokenID>();
  body->set_shardnum(static_cast<int64_t>(mShardNum));
  body->set_realmnum(static_cast<int64_t>(mRealmNum));
  body->set_tokennum(static_cast<int64_t>(mTokenNum));
  return body;
}

//-----
std::string TokenId::toString() const
{
  return internal::EntityIdHelper::toString(mShardNum, mRealmNum, mTokenNum);
}

//-----
std::string TokenId::toStringWithChecksum(const Client& client) const
{
  if (mChecksum.empty())
  {
    mChecksum = internal::EntityIdHelper::checksum(internal::EntityIdHelper::toString(mShardNum, mRealmNum, mTokenNum),
                                                   client.getLedgerId());
  }

  return internal::EntityIdHelper::toString(mShardNum, mRealmNum, mTokenNum, mChecksum);
}

//-----
std::vector<std::byte> TokenId::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

} // namespace Hiero
