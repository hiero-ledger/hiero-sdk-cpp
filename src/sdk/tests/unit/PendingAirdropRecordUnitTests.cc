// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "NftId.h"
#include "PendingAirdropId.h"
#include "PendingAirdropRecord.h"
#include "TokenId.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>
#include <transaction_record.pb.h>

using namespace Hiero;

class PendingAirdropRecordUnitTests : public ::testing::Test
{
protected:
  PendingAirdropRecord pendingAirdropRecord;
};

//-----
TEST_F(PendingAirdropRecordUnitTests, ConstructPendingAirdropRecord)
{
  // Given
  AccountId sender(1, 2, 3);
  AccountId receiver(4, 5, 6);
  TokenId tokenId(7, 8, 9);
  PendingAirdropId pendingAirdropId(sender, receiver, tokenId);
  uint64_t amount = 1000;

  // When
  PendingAirdropRecord pendingAirdropRecord(pendingAirdropId, amount);

  // Then
  EXPECT_EQ(pendingAirdropRecord.mPendingAirdropId.mSender, sender);
  EXPECT_EQ(pendingAirdropRecord.mPendingAirdropId.mReceiver, receiver);
  EXPECT_EQ(pendingAirdropRecord.mPendingAirdropId.mFt.value(), tokenId);
  EXPECT_EQ(pendingAirdropRecord.mAmount, amount);
}

//-----
TEST_F(PendingAirdropRecordUnitTests, FromProtobuf)
{
  // Given
  proto::PendingAirdropRecord proto;
  proto.mutable_pending_airdrop_id()->mutable_sender_id()->set_shardnum(1);
  proto.mutable_pending_airdrop_id()->mutable_sender_id()->set_realmnum(2);
  proto.mutable_pending_airdrop_id()->mutable_sender_id()->set_accountnum(3);

  proto.mutable_pending_airdrop_id()->mutable_receiver_id()->set_shardnum(4);
  proto.mutable_pending_airdrop_id()->mutable_receiver_id()->set_realmnum(5);
  proto.mutable_pending_airdrop_id()->mutable_receiver_id()->set_accountnum(6);

  proto.mutable_pending_airdrop_id()->mutable_fungible_token_type()->set_shardnum(7);
  proto.mutable_pending_airdrop_id()->mutable_fungible_token_type()->set_realmnum(8);
  proto.mutable_pending_airdrop_id()->mutable_fungible_token_type()->set_tokennum(9);

  proto.mutable_pending_airdrop_value()->set_amount(1000);

  // When
  PendingAirdropRecord pendingAirdropRecord = PendingAirdropRecord::fromProtobuf(proto);

  // Then
  EXPECT_EQ(pendingAirdropRecord.mPendingAirdropId.mSender, AccountId(1, 2, 3));
  EXPECT_EQ(pendingAirdropRecord.mPendingAirdropId.mReceiver, AccountId(4, 5, 6));
  EXPECT_EQ(pendingAirdropRecord.mPendingAirdropId.mFt.value(), TokenId(7, 8, 9));
  EXPECT_EQ(pendingAirdropRecord.mAmount, 1000);
}

//-----
TEST_F(PendingAirdropRecordUnitTests, ToProtobuf)
{
  // Given
  AccountId sender(1, 2, 3);
  AccountId receiver(4, 5, 6);
  TokenId tokenId(7, 8, 9);
  PendingAirdropId pendingAirdropId(sender, receiver, tokenId);
  uint64_t amount = 1000;
  PendingAirdropRecord pendingAirdropRecord(pendingAirdropId, amount);

  // When
  auto proto = pendingAirdropRecord.toProtobuf();

  // Then
  EXPECT_EQ(proto->pending_airdrop_id().sender_id().shardnum(), 1);
  EXPECT_EQ(proto->pending_airdrop_id().sender_id().realmnum(), 2);
  EXPECT_EQ(proto->pending_airdrop_id().sender_id().accountnum(), 3);

  EXPECT_EQ(proto->pending_airdrop_id().receiver_id().shardnum(), 4);
  EXPECT_EQ(proto->pending_airdrop_id().receiver_id().realmnum(), 5);
  EXPECT_EQ(proto->pending_airdrop_id().receiver_id().accountnum(), 6);

  EXPECT_EQ(proto->pending_airdrop_id().fungible_token_type().shardnum(), 7);
  EXPECT_EQ(proto->pending_airdrop_id().fungible_token_type().realmnum(), 8);
  EXPECT_EQ(proto->pending_airdrop_id().fungible_token_type().tokennum(), 9);

  EXPECT_EQ(proto->pending_airdrop_value().amount(), 1000);
}

//-----
TEST_F(PendingAirdropRecordUnitTests, ToString)
{
  // Given
  AccountId sender(1, 2, 3);
  AccountId receiver(4, 5, 6);
  TokenId tokenId(7, 8, 9);
  PendingAirdropId pendingAirdropId(sender, receiver, tokenId);
  uint64_t amount = 1000;
  PendingAirdropRecord pendingAirdropRecord(pendingAirdropId, amount);

  // When
  std::string result = pendingAirdropRecord.toString();

  // Then
  EXPECT_EQ(result, "1.2.3 4.5.6 7.8.9 1000");
}
