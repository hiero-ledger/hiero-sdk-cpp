// SPDX-License-Identifier: Apache-2.0
#include "TokenNftTransfer.h"
#include "hooks/NftHookCall.h"
#include "hooks/NftHookType.h"

#include <gtest/gtest.h>
#include <services/basic_types.pb.h>

using namespace Hiero;

class TokenNftTransferHookUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const NftId& getTestNftId() const { return mNftId; }
  [[nodiscard]] inline const AccountId& getTestSenderAccountId() const { return mSenderAccountId; }
  [[nodiscard]] inline const AccountId& getTestReceiverAccountId() const { return mReceiverAccountId; }
  [[nodiscard]] inline bool getTestApproval() const { return mApproval; }
  [[nodiscard]] inline const NftHookCall& getTestSenderHookCall() const { return mSenderHookCall; }
  [[nodiscard]] inline const NftHookCall& getTestReceiverHookCall() const { return mReceiverHookCall; }
  [[nodiscard]] inline int64_t getTestHookId1() const { return mHookId1; }
  [[nodiscard]] inline int64_t getTestHookId2() const { return mHookId2; }
  [[nodiscard]] inline int64_t getTestHookId3() const { return mHookId3; }
  [[nodiscard]] inline int64_t getTestHookId4() const { return mHookId4; }
  [[nodiscard]] inline int64_t getTestHookId5() const { return mHookId5; }
  [[nodiscard]] inline int64_t getTestHookId6() const { return mHookId6; }
  [[nodiscard]] inline int64_t getTestHookId7() const { return mHookId7; }
  [[nodiscard]] inline int64_t getTestHookId8() const { return mHookId8; }
  [[nodiscard]] inline int64_t getTestHookId9() const { return mHookId9; }
  [[nodiscard]] inline int64_t getTestHookId10() const { return mHookId10; }
  [[nodiscard]] inline int64_t getTestHookId11() const { return mHookId11; }
  [[nodiscard]] inline int64_t getTestHookId12() const { return mHookId12; }
  [[nodiscard]] inline int64_t getTestZeroHookId() const { return mZeroHookId; }

private:
  const TokenId mTokenId = TokenId(1ULL, 2ULL, 3ULL);
  const NftId mNftId = NftId(mTokenId, 4ULL);
  const AccountId mSenderAccountId = AccountId(5ULL, 6ULL, 7ULL);
  const AccountId mReceiverAccountId = AccountId(8ULL, 9ULL, 10ULL);
  const bool mApproval = true;
  const int64_t mHookId1 = 123LL;
  const int64_t mHookId2 = 456LL;
  const int64_t mHookId3 = 789LL;
  const int64_t mHookId4 = getTestHookId4();
  const int64_t mHookId5 = getTestHookId5();
  const int64_t mHookId6 = getTestHookId6();
  const int64_t mHookId7 = getTestHookId7();
  const int64_t mHookId8 = getTestHookId8();
  const int64_t mHookId9 = getTestHookId9();
  const int64_t mHookId10 = getTestHookId10();
  const int64_t mHookId11 = getTestHookId11();
  const int64_t mHookId12 = getTestHookId12();
  const int64_t mZeroHookId = getTestZeroHookId();
  const NftHookCall mSenderHookCall = []()
  {
    NftHookCall hookCall;
    hookCall.setHookType(NftHookType::PRE_HOOK);
    hookCall.setHookId(123LL);
    return hookCall;
  }();
  const NftHookCall mReceiverHookCall = []()
  {
    NftHookCall hookCall;
    hookCall.setHookType(NftHookType::PRE_POST_HOOK);
    hookCall.setHookId(456LL);
    return hookCall;
  }();
};

//-----
TEST_F(TokenNftTransferHookUnitTests, ConstructWithoutHookCalls)
{
  // Given / When
  const TokenNftTransfer transfer(
    getTestNftId(), getTestSenderAccountId(), getTestReceiverAccountId(), getTestApproval());

  // Then
  EXPECT_EQ(transfer.mNftId, getTestNftId());
  EXPECT_EQ(transfer.mSenderAccountId, getTestSenderAccountId());
  EXPECT_EQ(transfer.mReceiverAccountId, getTestReceiverAccountId());
  EXPECT_EQ(transfer.mIsApproval, getTestApproval());
  EXPECT_EQ(transfer.mSenderHookCall.getHookType(), NftHookType::UNINITIALIZED);
  EXPECT_EQ(transfer.mSenderHookCall.getHookId(), getTestZeroHookId());
  EXPECT_EQ(transfer.mReceiverHookCall.getHookType(), NftHookType::UNINITIALIZED);
  EXPECT_EQ(transfer.mReceiverHookCall.getHookId(), getTestZeroHookId());
}

//-----
TEST_F(TokenNftTransferHookUnitTests, ConstructWithSenderAndReceiverHookCalls)
{
  // Given / When
  const TokenNftTransfer transfer(getTestNftId(),
                                  getTestSenderAccountId(),
                                  getTestReceiverAccountId(),
                                  getTestApproval(),
                                  getTestSenderHookCall(),
                                  getTestReceiverHookCall());

  // Then
  EXPECT_EQ(transfer.mNftId, getTestNftId());
  EXPECT_EQ(transfer.mSenderAccountId, getTestSenderAccountId());
  EXPECT_EQ(transfer.mReceiverAccountId, getTestReceiverAccountId());
  EXPECT_EQ(transfer.mIsApproval, getTestApproval());
  EXPECT_EQ(transfer.mSenderHookCall.getHookType(), getTestSenderHookCall().getHookType());
  EXPECT_EQ(transfer.mSenderHookCall.getHookId(), getTestSenderHookCall().getHookId());
  EXPECT_EQ(transfer.mReceiverHookCall.getHookType(), getTestReceiverHookCall().getHookType());
  EXPECT_EQ(transfer.mReceiverHookCall.getHookId(), getTestReceiverHookCall().getHookId());
}

//-----
TEST_F(TokenNftTransferHookUnitTests, FromProtobufWithSenderPreTxAllowanceHook)
{
  // Given
  proto::NftTransfer proto;
  proto.set_allocated_senderaccountid(getTestSenderAccountId().toProtobuf().release());
  proto.set_allocated_receiveraccountid(getTestReceiverAccountId().toProtobuf().release());
  proto.set_serialnumber(static_cast<int64_t>(getTestNftId().mSerialNum));
  proto.set_is_approval(getTestApproval());

  proto::HookCall* hookCallProto = new proto::HookCall();
  hookCallProto->set_hook_id(getTestHookId3());
  proto.set_allocated_pre_tx_sender_allowance_hook(hookCallProto);

  // When
  const TokenNftTransfer transfer = TokenNftTransfer::fromProtobuf(proto, getTestNftId().mTokenId);

  // Then
  EXPECT_EQ(transfer.mNftId, getTestNftId());
  EXPECT_EQ(transfer.mSenderAccountId, getTestSenderAccountId());
  EXPECT_EQ(transfer.mReceiverAccountId, getTestReceiverAccountId());
  EXPECT_EQ(transfer.mIsApproval, getTestApproval());
  EXPECT_EQ(transfer.mSenderHookCall.getHookType(), NftHookType::PRE_HOOK);
  EXPECT_EQ(transfer.mSenderHookCall.getHookId(), getTestHookId3());
  EXPECT_EQ(transfer.mReceiverHookCall.getHookType(), NftHookType::UNINITIALIZED);
  EXPECT_EQ(transfer.mReceiverHookCall.getHookId(), getTestZeroHookId());
}

//-----
TEST_F(TokenNftTransferHookUnitTests, FromProtobufWithSenderPrePostTxAllowanceHook)
{
  // Given
  proto::NftTransfer proto;
  proto.set_allocated_senderaccountid(getTestSenderAccountId().toProtobuf().release());
  proto.set_allocated_receiveraccountid(getTestReceiverAccountId().toProtobuf().release());
  proto.set_serialnumber(static_cast<int64_t>(getTestNftId().mSerialNum));
  proto.set_is_approval(getTestApproval());

  proto::HookCall* hookCallProto = new proto::HookCall();
  hookCallProto->set_hook_id(getTestHookId4());
  proto.set_allocated_pre_post_tx_sender_allowance_hook(hookCallProto);

  // When
  const TokenNftTransfer transfer = TokenNftTransfer::fromProtobuf(proto, getTestNftId().mTokenId);

  // Then
  EXPECT_EQ(transfer.mNftId, getTestNftId());
  EXPECT_EQ(transfer.mSenderAccountId, getTestSenderAccountId());
  EXPECT_EQ(transfer.mReceiverAccountId, getTestReceiverAccountId());
  EXPECT_EQ(transfer.mIsApproval, getTestApproval());
  EXPECT_EQ(transfer.mSenderHookCall.getHookType(), NftHookType::PRE_POST_HOOK);
  EXPECT_EQ(transfer.mSenderHookCall.getHookId(), getTestHookId4());
  EXPECT_EQ(transfer.mReceiverHookCall.getHookType(), NftHookType::UNINITIALIZED);
  EXPECT_EQ(transfer.mReceiverHookCall.getHookId(), getTestZeroHookId());
}

//-----
TEST_F(TokenNftTransferHookUnitTests, FromProtobufWithReceiverPreTxAllowanceHook)
{
  // Given
  proto::NftTransfer proto;
  proto.set_allocated_senderaccountid(getTestSenderAccountId().toProtobuf().release());
  proto.set_allocated_receiveraccountid(getTestReceiverAccountId().toProtobuf().release());
  proto.set_serialnumber(static_cast<int64_t>(getTestNftId().mSerialNum));
  proto.set_is_approval(getTestApproval());

  proto::HookCall* hookCallProto = new proto::HookCall();
  hookCallProto->set_hook_id(getTestHookId5());
  proto.set_allocated_pre_tx_receiver_allowance_hook(hookCallProto);

  // When
  const TokenNftTransfer transfer = TokenNftTransfer::fromProtobuf(proto, getTestNftId().mTokenId);

  // Then
  EXPECT_EQ(transfer.mNftId, getTestNftId());
  EXPECT_EQ(transfer.mSenderAccountId, getTestSenderAccountId());
  EXPECT_EQ(transfer.mReceiverAccountId, getTestReceiverAccountId());
  EXPECT_EQ(transfer.mIsApproval, getTestApproval());
  EXPECT_EQ(transfer.mSenderHookCall.getHookType(), NftHookType::UNINITIALIZED);
  EXPECT_EQ(transfer.mSenderHookCall.getHookId(), getTestZeroHookId());
  EXPECT_EQ(transfer.mReceiverHookCall.getHookType(), NftHookType::PRE_HOOK);
  EXPECT_EQ(transfer.mReceiverHookCall.getHookId(), getTestHookId5());
}

//-----
TEST_F(TokenNftTransferHookUnitTests, FromProtobufWithReceiverPrePostTxAllowanceHook)
{
  // Given
  proto::NftTransfer proto;
  proto.set_allocated_senderaccountid(getTestSenderAccountId().toProtobuf().release());
  proto.set_allocated_receiveraccountid(getTestReceiverAccountId().toProtobuf().release());
  proto.set_serialnumber(static_cast<int64_t>(getTestNftId().mSerialNum));
  proto.set_is_approval(getTestApproval());

  proto::HookCall* hookCallProto = new proto::HookCall();
  hookCallProto->set_hook_id(getTestHookId6());
  proto.set_allocated_pre_post_tx_receiver_allowance_hook(hookCallProto);

  // When
  const TokenNftTransfer transfer = TokenNftTransfer::fromProtobuf(proto, getTestNftId().mTokenId);

  // Then
  EXPECT_EQ(transfer.mNftId, getTestNftId());
  EXPECT_EQ(transfer.mSenderAccountId, getTestSenderAccountId());
  EXPECT_EQ(transfer.mReceiverAccountId, getTestReceiverAccountId());
  EXPECT_EQ(transfer.mIsApproval, getTestApproval());
  EXPECT_EQ(transfer.mSenderHookCall.getHookType(), NftHookType::UNINITIALIZED);
  EXPECT_EQ(transfer.mSenderHookCall.getHookId(), getTestZeroHookId());
  EXPECT_EQ(transfer.mReceiverHookCall.getHookType(), NftHookType::PRE_POST_HOOK);
  EXPECT_EQ(transfer.mReceiverHookCall.getHookId(), getTestHookId6());
}

//-----
TEST_F(TokenNftTransferHookUnitTests, ToProtobufWithSenderPreHook)
{
  // Given
  NftHookCall senderHookCall;
  senderHookCall.setHookType(NftHookType::PRE_HOOK);
  senderHookCall.setHookId(getTestHookId7());
  NftHookCall receiverHookCall;
  receiverHookCall.setHookType(NftHookType::UNINITIALIZED);
  receiverHookCall.setHookId(getTestZeroHookId());
  const TokenNftTransfer transfer(getTestNftId(),
                                  getTestSenderAccountId(),
                                  getTestReceiverAccountId(),
                                  getTestApproval(),
                                  senderHookCall,
                                  receiverHookCall);

  // When
  const std::unique_ptr<proto::NftTransfer> proto = transfer.toProtobuf();

  // Then
  EXPECT_EQ(proto->senderaccountid().shardnum(), getTestSenderAccountId().mShardNum);
  EXPECT_EQ(proto->senderaccountid().realmnum(), getTestSenderAccountId().mRealmNum);
  EXPECT_EQ(proto->senderaccountid().accountnum(), getTestSenderAccountId().mAccountNum);
  EXPECT_EQ(proto->receiveraccountid().shardnum(), getTestReceiverAccountId().mShardNum);
  EXPECT_EQ(proto->receiveraccountid().realmnum(), getTestReceiverAccountId().mRealmNum);
  EXPECT_EQ(proto->receiveraccountid().accountnum(), getTestReceiverAccountId().mAccountNum);
  EXPECT_EQ(proto->serialnumber(), static_cast<int64_t>(getTestNftId().mSerialNum));
  EXPECT_EQ(proto->is_approval(), getTestApproval());
  EXPECT_TRUE(proto->has_pre_tx_sender_allowance_hook());
  EXPECT_EQ(proto->pre_tx_sender_allowance_hook().hook_id(), getTestHookId7());
  EXPECT_FALSE(proto->has_pre_post_tx_sender_allowance_hook());
  EXPECT_FALSE(proto->has_pre_tx_receiver_allowance_hook());
  EXPECT_FALSE(proto->has_pre_post_tx_receiver_allowance_hook());
}

//-----
TEST_F(TokenNftTransferHookUnitTests, ToProtobufWithSenderPrePostHook)
{
  // Given
  NftHookCall senderHookCall;
  senderHookCall.setHookType(NftHookType::PRE_POST_HOOK);
  senderHookCall.setHookId(getTestHookId8());
  NftHookCall receiverHookCall;
  receiverHookCall.setHookType(NftHookType::UNINITIALIZED);
  receiverHookCall.setHookId(getTestZeroHookId());
  const TokenNftTransfer transfer(getTestNftId(),
                                  getTestSenderAccountId(),
                                  getTestReceiverAccountId(),
                                  getTestApproval(),
                                  senderHookCall,
                                  receiverHookCall);

  // When
  const std::unique_ptr<proto::NftTransfer> proto = transfer.toProtobuf();

  // Then
  EXPECT_TRUE(proto->has_pre_post_tx_sender_allowance_hook());
  EXPECT_EQ(proto->pre_post_tx_sender_allowance_hook().hook_id(), getTestHookId8());
  EXPECT_FALSE(proto->has_pre_tx_sender_allowance_hook());
  EXPECT_FALSE(proto->has_pre_tx_receiver_allowance_hook());
  EXPECT_FALSE(proto->has_pre_post_tx_receiver_allowance_hook());
}

//-----
TEST_F(TokenNftTransferHookUnitTests, ToProtobufWithReceiverPreHook)
{
  // Given
  NftHookCall senderHookCall;
  senderHookCall.setHookType(NftHookType::UNINITIALIZED);
  senderHookCall.setHookId(getTestZeroHookId());
  NftHookCall receiverHookCall;
  receiverHookCall.setHookType(NftHookType::PRE_HOOK);
  receiverHookCall.setHookId(getTestHookId9());
  const TokenNftTransfer transfer(getTestNftId(),
                                  getTestSenderAccountId(),
                                  getTestReceiverAccountId(),
                                  getTestApproval(),
                                  senderHookCall,
                                  receiverHookCall);

  // When
  const std::unique_ptr<proto::NftTransfer> proto = transfer.toProtobuf();

  // Then
  EXPECT_TRUE(proto->has_pre_tx_receiver_allowance_hook());
  EXPECT_EQ(proto->pre_tx_receiver_allowance_hook().hook_id(), getTestHookId9());
  EXPECT_FALSE(proto->has_pre_tx_sender_allowance_hook());
  EXPECT_FALSE(proto->has_pre_post_tx_sender_allowance_hook());
  EXPECT_FALSE(proto->has_pre_post_tx_receiver_allowance_hook());
}

//-----
TEST_F(TokenNftTransferHookUnitTests, ToProtobufWithReceiverPrePostHook)
{
  // Given
  NftHookCall senderHookCall;
  senderHookCall.setHookType(NftHookType::UNINITIALIZED);
  senderHookCall.setHookId(getTestZeroHookId());
  NftHookCall receiverHookCall;
  receiverHookCall.setHookType(NftHookType::PRE_POST_HOOK);
  receiverHookCall.setHookId(getTestHookId10());
  const TokenNftTransfer transfer(getTestNftId(),
                                  getTestSenderAccountId(),
                                  getTestReceiverAccountId(),
                                  getTestApproval(),
                                  senderHookCall,
                                  receiverHookCall);

  // When
  const std::unique_ptr<proto::NftTransfer> proto = transfer.toProtobuf();

  // Then
  EXPECT_TRUE(proto->has_pre_post_tx_receiver_allowance_hook());
  EXPECT_EQ(proto->pre_post_tx_receiver_allowance_hook().hook_id(), getTestHookId10());
  EXPECT_FALSE(proto->has_pre_tx_sender_allowance_hook());
  EXPECT_FALSE(proto->has_pre_post_tx_sender_allowance_hook());
  EXPECT_FALSE(proto->has_pre_tx_receiver_allowance_hook());
}

//-----
TEST_F(TokenNftTransferHookUnitTests, ToStringWithBothHookCalls)
{
  // Given
  NftHookCall senderHookCall;
  senderHookCall.setHookType(NftHookType::PRE_HOOK);
  senderHookCall.setHookId(getTestHookId11());
  NftHookCall receiverHookCall;
  receiverHookCall.setHookType(NftHookType::PRE_POST_HOOK);
  receiverHookCall.setHookId(getTestHookId12());
  const TokenNftTransfer transfer(getTestNftId(),
                                  getTestSenderAccountId(),
                                  getTestReceiverAccountId(),
                                  getTestApproval(),
                                  senderHookCall,
                                  receiverHookCall);

  // When
  const std::string result = transfer.toString();

  // Then
  EXPECT_TRUE(result.find("PRE_HOOK") != std::string::npos);
  EXPECT_TRUE(result.find("PRE_POST_HOOK") != std::string::npos);
  EXPECT_TRUE(result.find("mSenderHookType") != std::string::npos);
  EXPECT_TRUE(result.find("mReceiverHookType") != std::string::npos);
}