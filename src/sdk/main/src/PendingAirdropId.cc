// SPDX-License-Identifier: Apache-2.0
#include "PendingAirdropId.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
PendingAirdropId::PendingAirdropId(AccountId sender, AccountId receiver, TokenId tokenId)
  : mSender(sender)
  , mReceiver(receiver)
  , mFt(tokenId)
{
}

//-----
PendingAirdropId::PendingAirdropId(AccountId sender, AccountId receiver, NftId tokenId)
  : mSender(sender)
  , mReceiver(receiver)
  , mNft(tokenId)
{
}

//-----
PendingAirdropId PendingAirdropId::fromProtobuf(const proto::PendingAirdropId& proto)
{
  PendingAirdropId pendingAirdropId;
  pendingAirdropId.mSender = AccountId::fromProtobuf(proto.sender_id());
  pendingAirdropId.mReceiver = AccountId::fromProtobuf(proto.receiver_id());

  if (proto.has_fungible_token_type())
  {
    pendingAirdropId.mFt = TokenId::fromProtobuf(proto.fungible_token_type());
  }

  if (proto.has_non_fungible_token())
  {
    pendingAirdropId.mNft = NftId::fromProtobuf(proto.non_fungible_token());
  }

  return pendingAirdropId;
}

//-----
void PendingAirdropId::validateChecksum(const Client& client) const
{
  mSender.validateChecksum(client);
  mReceiver.validateChecksum(client);

  if (mFt.has_value())
  {
    mFt.value().validateChecksum(client);
  }

  if (mNft.has_value())
  {
    mNft.value().mTokenId.validateChecksum(client);
  }
}
//-----
std::unique_ptr<proto::PendingAirdropId> PendingAirdropId::toProtobuf() const
{
  auto proto = std::make_unique<proto::PendingAirdropId>();

  proto->set_allocated_sender_id(mSender.toProtobuf().release());
  proto->set_allocated_receiver_id(mReceiver.toProtobuf().release());

  if (mFt.has_value())
  {
    proto->set_allocated_fungible_token_type(mFt.value().toProtobuf().release());
  }

  if (mNft.has_value())
  {
    proto->set_allocated_non_fungible_token(mNft.value().toProtobuf().release());
  }

  return proto;
}
//-----
std::string PendingAirdropId::toString() const
{
  return mSender.toString() + " " + mReceiver.toString() + " " +
         (mFt.has_value() ? mFt.value().toString() : std::string("")) +
         (mNft.has_value() ? mNft.value().mTokenId.toString() : std::string(""));
}

} // namespace Hiero