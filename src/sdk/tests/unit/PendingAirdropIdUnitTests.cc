// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "NftId.h"
#include "PendingAirdropId.h"
#include "TokenId.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class PendingAirdropIdUnitTests : public ::testing::Test
{
protected:
  PendingAirdropId pendingAirdropId;
};

//-----
TEST_F(PendingAirdropIdUnitTests, ConstructPendingAirdropIdWithTokenId)
{
  // Given
  AccountId sender(1, 2, 3);
  AccountId receiver(4, 5, 6);
  TokenId tokenId(7, 8, 9);

  // When
  PendingAirdropId pendingAirdrop(sender, receiver, tokenId);

  // Then
  EXPECT_EQ(pendingAirdrop.mSender, sender);
  EXPECT_EQ(pendingAirdrop.mReceiver, receiver);
  EXPECT_TRUE(pendingAirdrop.mFt.has_value());
  EXPECT_EQ(pendingAirdrop.mFt.value(), tokenId);
  EXPECT_FALSE(pendingAirdrop.mNft.has_value());
}

//-----
TEST_F(PendingAirdropIdUnitTests, ConstructPendingAirdropIdWithNftId)
{
  // Given
  AccountId sender(1, 2, 3);
  AccountId receiver(4, 5, 6);
  NftId nftId(TokenId(7, 8, 9), 1);

  // When
  PendingAirdropId pendingAirdrop(sender, receiver, nftId);

  // Then
  EXPECT_EQ(pendingAirdrop.mSender, sender);
  EXPECT_EQ(pendingAirdrop.mReceiver, receiver);
  EXPECT_TRUE(pendingAirdrop.mNft.has_value());
  EXPECT_EQ(pendingAirdrop.mNft.value(), nftId);
  EXPECT_FALSE(pendingAirdrop.mFt.has_value());
}

//-----
TEST_F(PendingAirdropIdUnitTests, FromProtobuf)
{
  // Given
  proto::PendingAirdropId proto;
  proto.mutable_sender_id()->set_shardnum(1);
  proto.mutable_sender_id()->set_realmnum(2);
  proto.mutable_sender_id()->set_accountnum(3);

  proto.mutable_receiver_id()->set_shardnum(4);
  proto.mutable_receiver_id()->set_realmnum(5);
  proto.mutable_receiver_id()->set_accountnum(6);

  proto.mutable_fungible_token_type()->set_shardnum(7);
  proto.mutable_fungible_token_type()->set_realmnum(8);
  proto.mutable_fungible_token_type()->set_tokennum(9);

  // When
  PendingAirdropId pendingAirdrop = PendingAirdropId::fromProtobuf(proto);

  // Then
  EXPECT_EQ(pendingAirdrop.mSender, AccountId(1, 2, 3));
  EXPECT_EQ(pendingAirdrop.mReceiver, AccountId(4, 5, 6));
  EXPECT_TRUE(pendingAirdrop.mFt.has_value());
  EXPECT_EQ(pendingAirdrop.mFt.value(), TokenId(7, 8, 9));
  EXPECT_FALSE(pendingAirdrop.mNft.has_value());
}

//-----
TEST_F(PendingAirdropIdUnitTests, ToProtobuf)
{
  // Given
  AccountId sender(1, 2, 3);
  AccountId receiver(4, 5, 6);
  TokenId tokenId(7, 8, 9);
  PendingAirdropId pendingAirdrop(sender, receiver, tokenId);

  // When
  auto proto = pendingAirdrop.toProtobuf();

  // Then
  EXPECT_EQ(proto->sender_id().shardnum(), 1);
  EXPECT_EQ(proto->sender_id().realmnum(), 2);
  EXPECT_EQ(proto->sender_id().accountnum(), 3);

  EXPECT_EQ(proto->receiver_id().shardnum(), 4);
  EXPECT_EQ(proto->receiver_id().realmnum(), 5);
  EXPECT_EQ(proto->receiver_id().accountnum(), 6);

  EXPECT_EQ(proto->fungible_token_type().shardnum(), 7);
  EXPECT_EQ(proto->fungible_token_type().realmnum(), 8);
  EXPECT_EQ(proto->fungible_token_type().tokennum(), 9);
}

//-----
TEST_F(PendingAirdropIdUnitTests, ValidateChecksum)
{
  // Given
  AccountId sender(1, 2, 3);
  AccountId receiver(4, 5, 6);
  TokenId tokenId(7, 8, 9);
  PendingAirdropId pendingAirdrop(sender, receiver, tokenId);

  Client client; // Assuming you have a mock or real client setup

  // Expect no exceptions to be thrown
  EXPECT_NO_THROW(pendingAirdrop.validateChecksum(client));
}

//-----
TEST_F(PendingAirdropIdUnitTests, ToStringWithFt)
{
  // Given
  AccountId sender(1, 2, 3);
  AccountId receiver(4, 5, 6);
  TokenId tokenId(7, 8, 9);
  PendingAirdropId pendingAirdrop(sender, receiver, tokenId);

  // When
  std::string result = pendingAirdrop.toString();

  // Then
  EXPECT_EQ(result, "1.2.3 4.5.6 7.8.9");
}

//-----
TEST_F(PendingAirdropIdUnitTests, ToStringWithNft)
{
  // Given
  AccountId sender(1, 2, 3);
  AccountId receiver(4, 5, 6);
  NftId nftId(TokenId(7, 8, 9), 1);
  PendingAirdropId pendingAirdrop(sender, receiver, nftId);

  // When
  std::string result = pendingAirdrop.toString();

  // Then
  EXPECT_EQ(result, "1.2.3 4.5.6 7.8.9");
}
