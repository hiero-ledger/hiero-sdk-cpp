// SPDX-License-Identifier: Apache-2.0
#include "TokenNftTransfer.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
TokenNftTransfer::TokenNftTransfer(NftId nftId, AccountId sender, AccountId receiver, bool approved)
  : mNftId(std::move(nftId))
  , mSenderAccountId(std::move(sender))
  , mReceiverAccountId(std::move(receiver))
  , mIsApproval(approved)
{
}

//-----
TokenNftTransfer TokenNftTransfer::fromProtobuf(const proto::NftTransfer& proto, const TokenId& tokenId)
{
  return { NftId(tokenId, static_cast<uint64_t>(proto.serialnumber())),
           AccountId::fromProtobuf(proto.senderaccountid()),
           AccountId::fromProtobuf(proto.receiveraccountid()),
           proto.is_approval() };
}

//-----
TokenNftTransfer TokenNftTransfer::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::NftTransfer proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto, TokenId());
}

//-----
void TokenNftTransfer::validateChecksums(const Client& client) const
{
  mNftId.mTokenId.validateChecksum(client);
  mSenderAccountId.validateChecksum(client);
  mReceiverAccountId.validateChecksum(client);
}

//-----
std::unique_ptr<proto::NftTransfer> TokenNftTransfer::toProtobuf() const
{
  auto proto = std::make_unique<proto::NftTransfer>();
  proto->set_allocated_senderaccountid(mSenderAccountId.toProtobuf().release());
  proto->set_allocated_receiveraccountid(mReceiverAccountId.toProtobuf().release());
  proto->set_serialnumber(static_cast<int64_t>(mNftId.mSerialNum));
  proto->set_is_approval(mIsApproval);
  return proto;
}

//-----
std::vector<std::byte> TokenNftTransfer::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string TokenNftTransfer::toString() const
{
  nlohmann::json json;
  json["mNftId"] = mNftId.toString();
  json["mSenderAccountId"] = mSenderAccountId.toString();
  json["mReceiverAccountId"] = mReceiverAccountId.toString();
  json["mIsApproval"] = mIsApproval;
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const TokenNftTransfer& transfer)
{
  os << transfer.toString();
  return os;
}

} // namespace Hiero
