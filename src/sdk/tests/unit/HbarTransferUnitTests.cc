// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Hbar.h"
#include "HbarTransfer.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class HbarTransferUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const int64_t& getTestAmount() const { return mAmount; }
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }

private:
  const int64_t mAmount = 10LL;
  const AccountId mAccountId = AccountId(10ULL);
};

// Tests serialization of Hiero::HbarTransfer -> proto::AccountAmount.
TEST_F(HbarTransferUnitTests, SerializeTransferToProtobuf)
{
  // Given
  const HbarTransfer testTransfer(getTestAccountId(), Hbar(getTestAmount(), HbarUnit::TINYBAR()), false);

  // When
  const auto protoAccountAmountPtr = std::unique_ptr<proto::AccountAmount>(testTransfer.toProtobuf());

  // Then
  EXPECT_EQ(protoAccountAmountPtr->accountid().accountnum(), getTestAccountId().mAccountNum);
  EXPECT_EQ(protoAccountAmountPtr->amount(), getTestAmount());
  EXPECT_FALSE(protoAccountAmountPtr->is_approval());
}

// Tests deserialization of proto::AccountAmount -> Hiero::HbarTransfer.
TEST_F(HbarTransferUnitTests, DeserializeTransferFromProtobuf)
{
  // Given
  const AccountId testAccountId = getTestAccountId();
  const int64_t testAmount = getTestAmount();
  proto::AccountAmount testProtoAccountAmount;
  testProtoAccountAmount.set_allocated_accountid(testAccountId.toProtobuf().release());
  testProtoAccountAmount.set_amount(testAmount);
  testProtoAccountAmount.set_is_approval(true);

  // When
  const HbarTransfer transfer = HbarTransfer::fromProtobuf(testProtoAccountAmount);

  // Then
  EXPECT_EQ(transfer.mAccountId, testAccountId);
  EXPECT_EQ(transfer.mAmount.toTinybars(), testAmount);
  EXPECT_TRUE(transfer.mIsApproved);
}

// Tests serialization & deserialization of Hiero::HbarTransfer -> proto::AccountAmount -> Hiero::HbarTransfer.
TEST_F(HbarTransferUnitTests, ProtoTransfer)
{
  AccountId accountId(10ULL);
  int64_t amount = 10LL;

  proto::AccountAmount protoAccountAmount;
  protoAccountAmount.set_allocated_accountid(accountId.toProtobuf().release());
  protoAccountAmount.set_amount(amount);
  protoAccountAmount.set_is_approval(true);

  HbarTransfer transfer = HbarTransfer::fromProtobuf(protoAccountAmount);
  EXPECT_EQ(transfer.mAccountId, accountId);
  EXPECT_EQ(transfer.mAmount.toTinybars(), amount);
  EXPECT_TRUE(transfer.mIsApproved);

  accountId.mAccountNum = 15ULL;
  amount = 15LL;

  transfer.mAccountId = accountId;
  transfer.mAmount = Hbar(amount, HbarUnit::TINYBAR());
  transfer.mIsApproved = false;

  const auto protoAccountAmountPtr = std::unique_ptr<proto::AccountAmount>(transfer.toProtobuf());
  EXPECT_EQ(protoAccountAmountPtr->accountid().accountnum(), accountId.mAccountNum);
  EXPECT_EQ(protoAccountAmountPtr->amount(), amount);
  EXPECT_FALSE(protoAccountAmountPtr->is_approval());
}
