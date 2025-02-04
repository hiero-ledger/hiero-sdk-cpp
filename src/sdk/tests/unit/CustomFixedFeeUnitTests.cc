// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "CustomFixedFee.h"
#include "TokenId.h"
#include "impl/Utilities.h"

#include <custom_fees.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class CustomFixedFeeUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestFeeCollectorAccountId() const { return mTestFeeCollectorAccountId; }
  [[nodiscard]] inline bool getTestAllCollectorsAreExempt() const { return mTestAllCollectorsAreExempt; }
  [[nodiscard]] inline const uint64_t& getTestAmount() const { return mTestAmount; }
  [[nodiscard]] inline const TokenId& getTestTokenId() const { return mTestTokenId; }

private:
  const AccountId mTestFeeCollectorAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const bool mTestAllCollectorsAreExempt = true;
  const uint64_t mTestAmount = 4ULL;
  const TokenId mTestTokenId = TokenId(5ULL, 6ULL, 7ULL);
};

//-----
TEST_F(CustomFixedFeeUnitTests, FromProtobuf)
{
  // Given
  proto::FixedFee protoFee;
  protoFee.set_amount(static_cast<int64_t>(getTestAmount()));
  protoFee.set_allocated_denominating_token_id(getTestTokenId().toProtobuf().release());

  // When
  const CustomFixedFee customFixedFee = CustomFixedFee::fromProtobuf(protoFee);

  // Then
  EXPECT_EQ(customFixedFee.getAmount(), getTestAmount());
  EXPECT_EQ(customFixedFee.getDenominatingTokenId(), getTestTokenId());
}

//-----
TEST_F(CustomFixedFeeUnitTests, FromBytes)
{
  // Given
  proto::CustomFee protoFee;
  protoFee.set_allocated_fee_collector_account_id(getTestFeeCollectorAccountId().toProtobuf().release());
  protoFee.set_all_collectors_are_exempt(getTestAllCollectorsAreExempt());
  protoFee.mutable_fixed_fee()->set_amount(static_cast<int64_t>(getTestAmount()));
  protoFee.mutable_fixed_fee()->set_allocated_denominating_token_id(getTestTokenId().toProtobuf().release());

  // When
  std::unique_ptr<CustomFee> customFee =
    CustomFee::fromBytes(internal::Utilities::stringToByteVector(protoFee.SerializeAsString()));

  // Then
  ASSERT_NE(dynamic_cast<CustomFixedFee*>(customFee.get()), nullptr);

  const std::unique_ptr<CustomFixedFee> customFixedFee(dynamic_cast<CustomFixedFee*>(customFee.release()));
  EXPECT_EQ(customFixedFee->getFeeCollectorAccountId(), getTestFeeCollectorAccountId());
  EXPECT_EQ(customFixedFee->getAllCollectorsAreExempt(), getTestAllCollectorsAreExempt());
  EXPECT_EQ(customFixedFee->getAmount(), getTestAmount());
  EXPECT_EQ(customFixedFee->getDenominatingTokenId(), getTestTokenId());
}

//-----
TEST_F(CustomFixedFeeUnitTests, Clone)
{
  // Given
  CustomFixedFee customFixedFee;
  customFixedFee.setFeeCollectorAccountId(getTestFeeCollectorAccountId());
  customFixedFee.setAllCollectorsAreExempt(getTestAllCollectorsAreExempt());

  // When
  const std::unique_ptr<CustomFee> clonedCustomFixedFee = customFixedFee.clone();

  // Then
  EXPECT_EQ(clonedCustomFixedFee->getFeeCollectorAccountId(), getTestFeeCollectorAccountId());
  EXPECT_EQ(clonedCustomFixedFee->getAllCollectorsAreExempt(), getTestAllCollectorsAreExempt());
}

//-----
TEST_F(CustomFixedFeeUnitTests, ToProtobuf)
{
  // Given
  CustomFixedFee customFixedFee;
  customFixedFee.setFeeCollectorAccountId(getTestFeeCollectorAccountId());
  customFixedFee.setAllCollectorsAreExempt(getTestAllCollectorsAreExempt());
  customFixedFee.setAmount(getTestAmount());
  customFixedFee.setDenominatingTokenId(getTestTokenId());

  // When
  const std::unique_ptr<proto::CustomFee> protoCustomFee = customFixedFee.toProtobuf();

  // Then
  EXPECT_EQ(protoCustomFee->fee_collector_account_id().shardnum(), getTestFeeCollectorAccountId().mShardNum);
  EXPECT_EQ(protoCustomFee->fee_collector_account_id().realmnum(), getTestFeeCollectorAccountId().mRealmNum);
  EXPECT_EQ(protoCustomFee->fee_collector_account_id().accountnum(), getTestFeeCollectorAccountId().mAccountNum);
  EXPECT_EQ(protoCustomFee->all_collectors_are_exempt(), getTestAllCollectorsAreExempt());

  ASSERT_EQ(protoCustomFee->fee_case(), proto::CustomFee::FeeCase::kFixedFee);
  EXPECT_EQ(protoCustomFee->fixed_fee().amount(), getTestAmount());
  EXPECT_EQ(protoCustomFee->fixed_fee().denominating_token_id().shardnum(), getTestTokenId().mShardNum);
  EXPECT_EQ(protoCustomFee->fixed_fee().denominating_token_id().realmnum(), getTestTokenId().mRealmNum);
  EXPECT_EQ(protoCustomFee->fixed_fee().denominating_token_id().tokennum(), getTestTokenId().mTokenNum);
}

//-----
TEST_F(CustomFixedFeeUnitTests, ToFixedFeeProtobuf)
{
  // Given
  CustomFixedFee customFixedFee;
  customFixedFee.setAmount(getTestAmount());
  customFixedFee.setDenominatingTokenId(getTestTokenId());

  // When
  std::unique_ptr<proto::FixedFee> protoFixedFee = customFixedFee.toFixedFeeProtobuf();

  // Then
  EXPECT_EQ(protoFixedFee->amount(), getTestAmount());
  EXPECT_EQ(protoFixedFee->denominating_token_id().shardnum(), getTestTokenId().mShardNum);
  EXPECT_EQ(protoFixedFee->denominating_token_id().realmnum(), getTestTokenId().mRealmNum);
  EXPECT_EQ(protoFixedFee->denominating_token_id().tokennum(), getTestTokenId().mTokenNum);
}

//-----
TEST_F(CustomFixedFeeUnitTests, GetSetAmount)
{
  // Given
  CustomFixedFee customFixedFeeInt64;
  CustomFixedFee customFixedFeeHbar;

  // When
  customFixedFeeInt64.setAmount(getTestAmount());
  customFixedFeeHbar.setHbarAmount(Hbar(static_cast<int64_t>(getTestAmount()), HbarUnit::TINYBAR()));

  // Then
  EXPECT_EQ(customFixedFeeInt64.getAmount(), getTestAmount());
  EXPECT_EQ(customFixedFeeHbar.getHbarAmount().toTinybars(), getTestAmount());
  EXPECT_EQ(customFixedFeeInt64.getHbarAmount().toTinybars(), customFixedFeeHbar.getAmount());
}

//-----
TEST_F(CustomFixedFeeUnitTests, GetSetDenominatingToken)
{
  // Given
  CustomFixedFee customFixedFee;

  // When
  customFixedFee.setDenominatingTokenId(getTestTokenId());

  // Then
  EXPECT_EQ(customFixedFee.getDenominatingTokenId(), getTestTokenId());
}

//-----
TEST_F(CustomFixedFeeUnitTests, SetSentinelValueToken)
{
  // Given
  CustomFixedFee customFixedFee;

  // When
  customFixedFee.setDenominatingTokenToSameToken();

  // Then
  EXPECT_EQ(customFixedFee.getDenominatingTokenId(), TokenId(0ULL, 0ULL, 0ULL));
}