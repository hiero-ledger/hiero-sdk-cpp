// SPDX-License-Identifier: Apache-2.0
#include "TokenNftInfo.h"
#include "impl/HexConverter.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>
#include <token_get_nft_info.pb.h>

namespace Hiero
{
//-----
TokenNftInfo TokenNftInfo::fromProtobuf(const proto::TokenNftInfo& proto)
{
  TokenNftInfo tokenNftInfo;

  if (proto.has_nftid())
  {
    tokenNftInfo.mNftId = NftId::fromProtobuf(proto.nftid());
  }

  if (proto.has_accountid())
  {
    tokenNftInfo.mAccountId = AccountId::fromProtobuf(proto.accountid());
  }

  if (proto.has_creationtime())
  {
    tokenNftInfo.mCreationTime = internal::TimestampConverter::fromProtobuf(proto.creationtime());
  }

  tokenNftInfo.mMetadata = internal::Utilities::stringToByteVector(proto.metadata());
  tokenNftInfo.mLedgerId = LedgerId(internal::Utilities::stringToByteVector(proto.ledger_id()));

  if (proto.has_spender_id())
  {
    tokenNftInfo.mSpenderId = AccountId::fromProtobuf(proto.spender_id());
  }

  return tokenNftInfo;
}

//-----
TokenNftInfo TokenNftInfo::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::TokenNftInfo proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<proto::TokenNftInfo> TokenNftInfo::toProtobuf() const
{
  auto protoTokenNftInfo = std::make_unique<proto::TokenNftInfo>();
  protoTokenNftInfo->set_allocated_nftid(mNftId.toProtobuf().release());
  protoTokenNftInfo->set_allocated_accountid(mAccountId.toProtobuf().release());
  protoTokenNftInfo->set_allocated_creationtime(internal::TimestampConverter::toProtobuf(mCreationTime));
  protoTokenNftInfo->set_metadata(internal::Utilities::byteVectorToString(mMetadata));
  protoTokenNftInfo->set_ledger_id(internal::Utilities::byteVectorToString(mLedgerId.toBytes()));

  if (mSpenderId.has_value())
  {
    protoTokenNftInfo->set_allocated_spender_id(mSpenderId->toProtobuf().release());
  }

  return protoTokenNftInfo;
}

//-----
std::vector<std::byte> TokenNftInfo::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string TokenNftInfo::toString() const
{
  nlohmann::json json;
  json["mNftId"] = mNftId.toString();
  json["mAccountId"] = mAccountId.toString();
  json["mCreationTime"] = internal::TimestampConverter::toString(mCreationTime);
  json["mMetadata"] = internal::HexConverter::bytesToHex(mMetadata);
  json["mLedgerId"] = mLedgerId.toString();

  if (mSpenderId.has_value())
  {
    json["mSpenderId"] = mSpenderId->toString();
  }

  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const TokenNftInfo& info)
{
  os << info.toString();
  return os;
}

} // namespace Hiero
