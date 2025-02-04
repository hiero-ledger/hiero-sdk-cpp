// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "PendingAirdropId.h"
#include "TokenCancelAirdropTransaction.h"
#include "TokenId.h"

#include <gtest/gtest.h>
#include <token_cancel_airdrop.pb.h>
#include <transaction_body.pb.h>

using namespace Hiero;

class TokenCancelAirdropTransactionUnitTests : public ::testing::Test
{
protected:
  TokenCancelAirdropTransaction tokenCancelAirdropTransaction;
};

//-----
TEST_F(TokenCancelAirdropTransactionUnitTests, ConstructFromTransactionBody)
{
  // Given
  proto::TransactionBody transactionBody;
  proto::TokenCancelAirdropTransactionBody* cancelAirdropBody = transactionBody.mutable_tokencancelairdrop();

  proto::PendingAirdropId* pendingAirdrop = cancelAirdropBody->add_pending_airdrops();
  pendingAirdrop->mutable_sender_id()->set_shardnum(1);
  pendingAirdrop->mutable_sender_id()->set_realmnum(2);
  pendingAirdrop->mutable_sender_id()->set_accountnum(3);
  pendingAirdrop->mutable_receiver_id()->set_shardnum(4);
  pendingAirdrop->mutable_receiver_id()->set_realmnum(5);
  pendingAirdrop->mutable_receiver_id()->set_accountnum(6);

  // When
  TokenCancelAirdropTransaction transaction(transactionBody);

  // Then
  ASSERT_EQ(transaction.getPendingAirdrops().size(), 1);
  EXPECT_EQ(transaction.getPendingAirdrops()[0].mSender, AccountId(1, 2, 3));
  EXPECT_EQ(transaction.getPendingAirdrops()[0].mReceiver, AccountId(4, 5, 6));
}

//-----
TEST_F(TokenCancelAirdropTransactionUnitTests, SetPendingAirdrops)
{
  // Given
  std::vector<PendingAirdropId> pendingAirdrops;
  pendingAirdrops.emplace_back(AccountId(1, 2, 3), AccountId(4, 5, 6), TokenId(7, 8, 9));

  // When
  tokenCancelAirdropTransaction.setPendingAirdrops(pendingAirdrops);

  // Then
  ASSERT_EQ(tokenCancelAirdropTransaction.getPendingAirdrops().size(), 1);
  EXPECT_EQ(tokenCancelAirdropTransaction.getPendingAirdrops()[0].mSender, AccountId(1, 2, 3));
  EXPECT_EQ(tokenCancelAirdropTransaction.getPendingAirdrops()[0].mReceiver, AccountId(4, 5, 6));
  EXPECT_EQ(tokenCancelAirdropTransaction.getPendingAirdrops()[0].mFt.value(), TokenId(7, 8, 9));
}

//-----
TEST_F(TokenCancelAirdropTransactionUnitTests, InitFromSourceTransactionBody)
{
  // Given
  proto::TransactionBody transactionBody;
  proto::TokenCancelAirdropTransactionBody* cancelAirdropBody = transactionBody.mutable_tokencancelairdrop();
  proto::PendingAirdropId* pendingAirdrop = cancelAirdropBody->add_pending_airdrops();

  pendingAirdrop->mutable_sender_id()->set_shardnum(1);
  pendingAirdrop->mutable_sender_id()->set_realmnum(2);
  pendingAirdrop->mutable_sender_id()->set_accountnum(3);
  pendingAirdrop->mutable_receiver_id()->set_shardnum(4);
  pendingAirdrop->mutable_receiver_id()->set_realmnum(5);
  pendingAirdrop->mutable_receiver_id()->set_accountnum(6);

  // When
  TokenCancelAirdropTransaction transaction(transactionBody);

  // Then
  ASSERT_EQ(transaction.getPendingAirdrops().size(), 1);
  EXPECT_EQ(transaction.getPendingAirdrops()[0].mSender, AccountId(1, 2, 3));
  EXPECT_EQ(transaction.getPendingAirdrops()[0].mReceiver, AccountId(4, 5, 6));
}
