// SPDX-License-Identifier: Apache-2.0
#include "TokenNftTransfer.h"
#include "hooks/NftHookType.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>

#include <services/basic_types.pb.h>

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
TokenNftTransfer::TokenNftTransfer(NftId nftId,
                                   AccountId sender,
                                   AccountId receiver,
                                   bool approved,
                                   const NftHookCall& senderHookCall,
                                   const NftHookCall& receiverHookCall)
  : mNftId(std::move(nftId))
  , mSenderAccountId(std::move(sender))
  , mReceiverAccountId(std::move(receiver))
  , mIsApproval(approved)
  , mSenderHookCall(senderHookCall)
  , mReceiverHookCall(receiverHookCall)
{
}

//-----
TokenNftTransfer TokenNftTransfer::fromProtobuf(const proto::NftTransfer& proto, const TokenId& tokenId)
{
  TokenNftTransfer transfer(NftId(tokenId, static_cast<uint64_t>(proto.serialnumber())),
                            AccountId::fromProtobuf(proto.senderaccountid()),
                            AccountId::fromProtobuf(proto.receiveraccountid()),
                            proto.is_approval());

  if (proto.has_pre_tx_sender_allowance_hook())
  {
    transfer.mSenderHookCall = NftHookCall::fromProtobuf(proto.pre_tx_sender_allowance_hook(), NftHookType::PRE_HOOK);
  }
  else if (proto.has_pre_post_tx_sender_allowance_hook())
  {
    transfer.mSenderHookCall =
      NftHookCall::fromProtobuf(proto.pre_post_tx_sender_allowance_hook(), NftHookType::PRE_POST_HOOK);
  }

  if (proto.has_pre_tx_receiver_allowance_hook())
  {
    transfer.mReceiverHookCall =
      NftHookCall::fromProtobuf(proto.pre_tx_receiver_allowance_hook(), NftHookType::PRE_HOOK);
  }
  else if (proto.has_pre_post_tx_receiver_allowance_hook())
  {
    transfer.mReceiverHookCall =
      NftHookCall::fromProtobuf(proto.pre_post_tx_receiver_allowance_hook(), NftHookType::PRE_POST_HOOK);
  }

  return transfer;
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

  if (mSenderHookCall.getHookType() == NftHookType::PRE_HOOK)
  {
    proto->set_allocated_pre_tx_sender_allowance_hook(mSenderHookCall.toProtobuf().release());
  }
  else if (mSenderHookCall.getHookType() == NftHookType::PRE_POST_HOOK)
  {
    proto->set_allocated_pre_post_tx_sender_allowance_hook(mSenderHookCall.toProtobuf().release());
  }

  if (mReceiverHookCall.getHookType() == NftHookType::PRE_HOOK)
  {
    proto->set_allocated_pre_tx_receiver_allowance_hook(mReceiverHookCall.toProtobuf().release());
  }
  else if (mReceiverHookCall.getHookType() == NftHookType::PRE_POST_HOOK)
  {
    proto->set_allocated_pre_post_tx_receiver_allowance_hook(mReceiverHookCall.toProtobuf().release());
  }

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
  json["mSenderHookType"] = gNftHookTypeToString.at(mSenderHookCall.getHookType());
  json["mReceiverHookType"] = gNftHookTypeToString.at(mReceiverHookCall.getHookType());
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const TokenNftTransfer& transfer)
{
  os << transfer.toString();
  return os;
}

} // namespace Hiero
