// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "CustomFeeLimit.h"
#include "CustomFixedFee.h"
#include <gtest/gtest.h>
#include <services/transaction.pb.h>

using namespace Hiero;

class CustomFeeLimitUnitTests : public ::testing::Test
{
};

//-----
TEST(CustomFeeLimitUnitTests, DefaultConstructor)
{
  CustomFeeLimit feeLimit;

  EXPECT_FALSE(feeLimit.getPayerId().has_value()); // PayerId should be empty
  EXPECT_TRUE(feeLimit.getCustomFees().empty());   // Custom fees should be empty
}

//-----
TEST(CustomFeeLimitUnitTests, SetAndGetPayerId)
{
  CustomFeeLimit feeLimit;
  AccountId payerId(1, 2, 3);

  feeLimit.setPayerId(payerId);

  ASSERT_TRUE(feeLimit.getPayerId().has_value());
  EXPECT_EQ(feeLimit.getPayerId().value(), payerId);
}

//-----
TEST(CustomFeeLimitUnitTests, SetAndGetCustomFees)
{
  CustomFeeLimit feeLimit;
  CustomFixedFee fee1;
  fee1.setAmount(10);
  CustomFixedFee fee2;
  fee2.setAmount(20);

  std::vector<CustomFixedFee> fees = { fee1, fee2 };
  feeLimit.setCustomFees(fees);

  std::vector<CustomFixedFee> retrievedFees = feeLimit.getCustomFees();
  ASSERT_EQ(retrievedFees.size(), 2);
  EXPECT_EQ(retrievedFees[0].getAmount(), 10);
  EXPECT_EQ(retrievedFees[1].getAmount(), 20);
}

//-----
TEST(CustomFeeLimitTest, AddCustomFee)
{
  CustomFeeLimit feeLimit;
  CustomFixedFee fee;
  fee.setAmount(15);

  feeLimit.addCustomFee(fee);

  std::vector<CustomFixedFee> retrievedFees = feeLimit.getCustomFees();
  ASSERT_EQ(retrievedFees.size(), 1);
  EXPECT_EQ(retrievedFees[0].getAmount(), 15);
}

//-----
TEST(CustomFeeLimitUnitTests, ProtobufConversion)
{
  // Set up the protobuf message
  proto::CustomFeeLimit protoFeeLimit;
  protoFeeLimit.mutable_account_id()->set_shardnum(1);
  protoFeeLimit.mutable_account_id()->set_realmnum(2);
  protoFeeLimit.mutable_account_id()->set_accountnum(3);

  auto* protoFee = protoFeeLimit.add_fees();
  protoFee->set_amount(25);

  // Convert from protobuf
  CustomFeeLimit feeLimit = CustomFeeLimit::fromProtobuf(protoFeeLimit);

  // Validate the conversion
  ASSERT_TRUE(feeLimit.getPayerId().has_value());
  EXPECT_EQ(feeLimit.getPayerId()->toString(), "1.2.3");

  std::vector<CustomFixedFee> fees = feeLimit.getCustomFees();
  ASSERT_EQ(fees.size(), 1);
  EXPECT_EQ(fees[0].getAmount(), 25);

  // Convert back to protobuf and check values
  std::unique_ptr<proto::CustomFeeLimit> convertedProto = feeLimit.toProtobuf();
  EXPECT_EQ(convertedProto->account_id().shardnum(), 1);
  EXPECT_EQ(convertedProto->account_id().realmnum(), 2);
  EXPECT_EQ(convertedProto->account_id().accountnum(), 3);
  ASSERT_EQ(convertedProto->fees_size(), 1);

  // Test the scenario with no fees
  proto::CustomFeeLimit protoFeeLimitEmpty;
  CustomFeeLimit feeLimitEmpty = CustomFeeLimit::fromProtobuf(protoFeeLimitEmpty);
  EXPECT_FALSE(feeLimitEmpty.getPayerId().has_value());
  EXPECT_TRUE(feeLimitEmpty.getCustomFees().empty());
}
