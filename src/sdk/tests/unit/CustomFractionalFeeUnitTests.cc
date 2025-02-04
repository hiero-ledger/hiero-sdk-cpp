// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "CustomFractionalFee.h"
#include "FeeAssessmentMethod.h"
#include "impl/Utilities.h"

#include <custom_fees.pb.h>
#include <gtest/gtest.h>
#include <stdexcept>

using namespace Hiero;

class CustomFractionalFeeUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestFeeCollectorAccountId() const { return mTestFeeCollectorAccountId; }
  [[nodiscard]] inline bool getTestAllCollectorsAreExempt() const { return mTestAllCollectorsAreExempt; }
  [[nodiscard]] inline const int64_t& getTestNumerator() const { return mTestNumerator; }
  [[nodiscard]] inline const int64_t& getTestDenominator() const { return mTestDenominator; }
  [[nodiscard]] inline const uint64_t& getTestMinAmount() const { return mTestMinAmount; }
  [[nodiscard]] inline const uint64_t& getTestMaxAmount() const { return mTestMaxAmount; }
  [[nodiscard]] inline FeeAssessmentMethod getTestAssessmentMethod() const { return mTestAssessmentMethod; }

private:
  const AccountId mTestFeeCollectorAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const bool mTestAllCollectorsAreExempt = true;
  const int64_t mTestNumerator = 4ULL;
  const int64_t mTestDenominator = 5ULL;
  const uint64_t mTestMinAmount = 6ULL;
  const uint64_t mTestMaxAmount = 7ULL;
  const FeeAssessmentMethod mTestAssessmentMethod = FeeAssessmentMethod::EXCLUSIVE;
};

//-----
TEST_F(CustomFractionalFeeUnitTests, FromProtobuf)
{
  // Given
  proto::FractionalFee protoFee;
  protoFee.mutable_fractional_amount()->set_numerator(getTestNumerator());
  protoFee.mutable_fractional_amount()->set_denominator(getTestDenominator());
  protoFee.set_minimum_amount(static_cast<int64_t>(getTestMinAmount()));
  protoFee.set_maximum_amount(static_cast<int64_t>(getTestMaxAmount()));
  protoFee.set_net_of_transfers(getTestAssessmentMethod() == FeeAssessmentMethod::EXCLUSIVE);

  // When
  const CustomFractionalFee customFractionalFee = CustomFractionalFee::fromProtobuf(protoFee);

  // Then
  EXPECT_EQ(customFractionalFee.getNumerator(), getTestNumerator());
  EXPECT_EQ(customFractionalFee.getDenominator(), getTestDenominator());
  EXPECT_EQ(customFractionalFee.getMinimumAmount(), getTestMinAmount());
  EXPECT_EQ(customFractionalFee.getMaximumAmount(), getTestMaxAmount());
  EXPECT_EQ(customFractionalFee.getAssessmentMethod(), getTestAssessmentMethod());
}

//-----
TEST_F(CustomFractionalFeeUnitTests, FromBytes)
{
  // Given
  proto::CustomFee protoFee;
  protoFee.set_allocated_fee_collector_account_id(getTestFeeCollectorAccountId().toProtobuf().release());
  protoFee.set_all_collectors_are_exempt(getTestAllCollectorsAreExempt());
  protoFee.mutable_fractional_fee()->mutable_fractional_amount()->set_numerator(getTestNumerator());
  protoFee.mutable_fractional_fee()->mutable_fractional_amount()->set_denominator(getTestDenominator());
  protoFee.mutable_fractional_fee()->set_minimum_amount(static_cast<int64_t>(getTestMinAmount()));
  protoFee.mutable_fractional_fee()->set_maximum_amount(static_cast<int64_t>(getTestMaxAmount()));
  protoFee.mutable_fractional_fee()->set_net_of_transfers(getTestAssessmentMethod() == FeeAssessmentMethod::EXCLUSIVE);

  // When
  std::unique_ptr<CustomFee> customFee =
    CustomFee::fromBytes(internal::Utilities::stringToByteVector(protoFee.SerializeAsString()));

  // Then
  ASSERT_NE(dynamic_cast<CustomFractionalFee*>(customFee.get()), nullptr);

  const std::unique_ptr<CustomFractionalFee> customFractionalFee(
    dynamic_cast<CustomFractionalFee*>(customFee.release()));
  EXPECT_EQ(customFractionalFee->getFeeCollectorAccountId(), getTestFeeCollectorAccountId());
  EXPECT_EQ(customFractionalFee->getAllCollectorsAreExempt(), getTestAllCollectorsAreExempt());
  EXPECT_EQ(customFractionalFee->getNumerator(), getTestNumerator());
  EXPECT_EQ(customFractionalFee->getDenominator(), getTestDenominator());
  EXPECT_EQ(customFractionalFee->getMinimumAmount(), getTestMinAmount());
  EXPECT_EQ(customFractionalFee->getMaximumAmount(), getTestMaxAmount());
  EXPECT_EQ(customFractionalFee->getAssessmentMethod(), getTestAssessmentMethod());
}

//-----
TEST_F(CustomFractionalFeeUnitTests, Clone)
{
  // Given
  CustomFractionalFee customFractionalFee;
  customFractionalFee.setFeeCollectorAccountId(getTestFeeCollectorAccountId());
  customFractionalFee.setAllCollectorsAreExempt(getTestAllCollectorsAreExempt());

  // When
  const std::unique_ptr<CustomFee> clonedCustomFractionalFee = customFractionalFee.clone();

  // Then
  EXPECT_EQ(clonedCustomFractionalFee->getFeeCollectorAccountId(), getTestFeeCollectorAccountId());
  EXPECT_EQ(clonedCustomFractionalFee->getAllCollectorsAreExempt(), getTestAllCollectorsAreExempt());
}

//-----
TEST_F(CustomFractionalFeeUnitTests, ToProtobuf)
{
  // Given
  CustomFractionalFee customFractionalFee;
  customFractionalFee.setFeeCollectorAccountId(getTestFeeCollectorAccountId());
  customFractionalFee.setAllCollectorsAreExempt(getTestAllCollectorsAreExempt());
  customFractionalFee.setNumerator(getTestNumerator());
  customFractionalFee.setDenominator(getTestDenominator());
  customFractionalFee.setMinimumAmount(getTestMinAmount());
  customFractionalFee.setMaximumAmount(getTestMaxAmount());
  customFractionalFee.setAssessmentMethod(getTestAssessmentMethod());

  // When
  const std::unique_ptr<proto::CustomFee> protoCustomFee = customFractionalFee.toProtobuf();

  // Then
  EXPECT_EQ(protoCustomFee->fee_collector_account_id().shardnum(), getTestFeeCollectorAccountId().mShardNum);
  EXPECT_EQ(protoCustomFee->fee_collector_account_id().realmnum(), getTestFeeCollectorAccountId().mRealmNum);
  EXPECT_EQ(protoCustomFee->fee_collector_account_id().accountnum(), getTestFeeCollectorAccountId().mAccountNum);
  EXPECT_EQ(protoCustomFee->all_collectors_are_exempt(), getTestAllCollectorsAreExempt());

  ASSERT_EQ(protoCustomFee->fee_case(), proto::CustomFee::FeeCase::kFractionalFee);
  EXPECT_EQ(protoCustomFee->fractional_fee().fractional_amount().numerator(), getTestNumerator());
  EXPECT_EQ(protoCustomFee->fractional_fee().fractional_amount().denominator(), getTestDenominator());
  EXPECT_EQ(protoCustomFee->fractional_fee().minimum_amount(), getTestMinAmount());
  EXPECT_EQ(protoCustomFee->fractional_fee().maximum_amount(), getTestMaxAmount());
  EXPECT_EQ(protoCustomFee->fractional_fee().net_of_transfers(),
            getTestAssessmentMethod() == FeeAssessmentMethod::EXCLUSIVE);
}

//-----
TEST_F(CustomFractionalFeeUnitTests, GetSetNumerator)
{
  // Given
  CustomFractionalFee customFractionalFee;

  // When
  customFractionalFee.setNumerator(getTestNumerator());

  // Then
  EXPECT_EQ(customFractionalFee.getNumerator(), getTestNumerator());
}

//-----
TEST_F(CustomFractionalFeeUnitTests, GetSetDenominator)
{
  // Given
  CustomFractionalFee customFractionalFee;

  // When
  customFractionalFee.setDenominator(getTestDenominator());

  // Then
  EXPECT_EQ(customFractionalFee.getDenominator(), getTestDenominator());
}

//-----
TEST_F(CustomFractionalFeeUnitTests, GetSetMinimumAmount)
{
  // Given
  CustomFractionalFee customFractionalFee;

  // When
  customFractionalFee.setMinimumAmount(getTestMinAmount());

  // Then
  EXPECT_EQ(customFractionalFee.getMinimumAmount(), getTestMinAmount());
}

//-----
TEST_F(CustomFractionalFeeUnitTests, GetSetMaximumAmount)
{
  // Given
  CustomFractionalFee customFractionalFee;

  // When
  customFractionalFee.setMaximumAmount(getTestMaxAmount());

  // Then
  EXPECT_EQ(customFractionalFee.getMaximumAmount(), getTestMaxAmount());
}

//-----
TEST_F(CustomFractionalFeeUnitTests, GetSetAssessmentMethod)
{
  // Given
  CustomFractionalFee customFractionalFee;

  // When
  customFractionalFee.setAssessmentMethod(getTestAssessmentMethod());

  // Then
  EXPECT_EQ(customFractionalFee.getAssessmentMethod(), getTestAssessmentMethod());
}
