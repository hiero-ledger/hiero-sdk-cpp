// SPDX-License-Identifier: Apache-2.0
#include "FeeAssessmentMethod.h"

#include <gtest/gtest.h>

using namespace Hiero;

class FeeAssessmentMethodUnitTests : public ::testing::Test
{
};

//-----
TEST_F(FeeAssessmentMethodUnitTests, FeeAssessmentMethodToString)
{
  // Given / When / Then
  ASSERT_TRUE(gFeeAssessmentMethodToString.find(FeeAssessmentMethod::INCLUSIVE) != gFeeAssessmentMethodToString.end());
  ASSERT_TRUE(gFeeAssessmentMethodToString.find(FeeAssessmentMethod::EXCLUSIVE) != gFeeAssessmentMethodToString.end());
  EXPECT_EQ(gFeeAssessmentMethodToString.at(FeeAssessmentMethod::INCLUSIVE), "INCLUSIVE");
  EXPECT_EQ(gFeeAssessmentMethodToString.at(FeeAssessmentMethod::EXCLUSIVE), "EXCLUSIVE");
}