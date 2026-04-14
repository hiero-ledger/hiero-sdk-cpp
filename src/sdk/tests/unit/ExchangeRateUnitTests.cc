// SPDX-License-Identifier: Apache-2.0
#include "ExchangeRate.h"
#include "ExchangeRates.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <gtest/gtest.h>
#include <services/exchange_rate.pb.h>

using namespace Hiero;

class ExchangeRateUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline int32_t getTestCents() const { return mTestCents; }
  [[nodiscard]] inline int32_t getTestHbar() const { return mTestHbar; }
  [[nodiscard]] inline const std::chrono::system_clock::time_point& getTestExpirationTime() const
  {
    return mTestExpirationTime;
  }

private:
  const int32_t mTestCents = 2;
  const int32_t mTestHbar = 1;
  const std::chrono::system_clock::time_point mTestExpirationTime = std::chrono::system_clock::now();
};

//-----
TEST_F(ExchangeRateUnitTests, ExchangeRateConstructor)
{
  // Given / When
  const ExchangeRate exchangeRate(getTestHbar(), getTestCents(), getTestExpirationTime());

  // Then
  EXPECT_EQ(exchangeRate.mHbars, getTestHbar());
  EXPECT_EQ(exchangeRate.mCents, getTestCents());
  EXPECT_EQ(exchangeRate.mExpirationTime, getTestExpirationTime());
}

//-----
TEST_F(ExchangeRateUnitTests, ExchangeRateFromProtobuf)
{
  // Given
  proto::ExchangeRate protoExchangeRate;
  protoExchangeRate.set_hbarequiv(getTestHbar());
  protoExchangeRate.set_centequiv(getTestCents());
  protoExchangeRate.set_allocated_expirationtime(
    internal::TimestampConverter::toSecondsProtobuf(getTestExpirationTime()));

  // When
  const ExchangeRate exchangeRate = ExchangeRate::fromProtobuf(protoExchangeRate);

  // Then
  EXPECT_EQ(exchangeRate.mHbars, getTestHbar());
  EXPECT_EQ(exchangeRate.mCents, getTestCents());
  EXPECT_EQ(std::chrono::duration_cast<std::chrono::seconds>(exchangeRate.mExpirationTime.time_since_epoch()),
            std::chrono::duration_cast<std::chrono::seconds>(getTestExpirationTime().time_since_epoch()));
  EXPECT_EQ(exchangeRate.mExchangeRateInCents, getTestCents() / getTestHbar());
}

//-----
TEST_F(ExchangeRateUnitTests, ExchangeRatesConstructor)
{
  // Given / When
  const ExchangeRate exchangeRate(getTestHbar(), getTestCents(), getTestExpirationTime());
  const ExchangeRates exchangeRates(exchangeRate, exchangeRate);

  // Then
  EXPECT_EQ(exchangeRates.mCurrentRate.mHbars, getTestHbar());
  EXPECT_EQ(exchangeRates.mCurrentRate.mCents, getTestCents());
  EXPECT_EQ(exchangeRates.mCurrentRate.mExpirationTime, getTestExpirationTime());
  EXPECT_EQ(exchangeRates.mNextRate.mHbars, getTestHbar());
  EXPECT_EQ(exchangeRates.mNextRate.mCents, getTestCents());
  EXPECT_EQ(exchangeRates.mNextRate.mExpirationTime, getTestExpirationTime());
}

//-----
TEST_F(ExchangeRateUnitTests, ExchangeRatesFromProtobuf)
{
  // Given
  proto::ExchangeRate protoExchangeRate;
  protoExchangeRate.set_hbarequiv(getTestHbar());
  protoExchangeRate.set_centequiv(getTestCents());
  protoExchangeRate.set_allocated_expirationtime(
    internal::TimestampConverter::toSecondsProtobuf(getTestExpirationTime()));

  proto::ExchangeRateSet protoExchangeRateSet;
  *protoExchangeRateSet.mutable_currentrate() = protoExchangeRate;
  *protoExchangeRateSet.mutable_nextrate() = protoExchangeRate;

  // When
  const ExchangeRates exchangeRates = ExchangeRates::fromProtobuf(protoExchangeRateSet);

  // Then
  EXPECT_EQ(exchangeRates.mCurrentRate.mHbars, getTestHbar());
  EXPECT_EQ(exchangeRates.mCurrentRate.mCents, getTestCents());
  EXPECT_EQ(
    std::chrono::duration_cast<std::chrono::seconds>(exchangeRates.mCurrentRate.mExpirationTime.time_since_epoch()),
    std::chrono::duration_cast<std::chrono::seconds>(getTestExpirationTime().time_since_epoch()));
  EXPECT_EQ(exchangeRates.mCurrentRate.mExchangeRateInCents, getTestCents() / getTestHbar());
  EXPECT_EQ(exchangeRates.mNextRate.mHbars, getTestHbar());
  EXPECT_EQ(exchangeRates.mNextRate.mCents, getTestCents());
  EXPECT_EQ(
    std::chrono::duration_cast<std::chrono::seconds>(exchangeRates.mNextRate.mExpirationTime.time_since_epoch()),
    std::chrono::duration_cast<std::chrono::seconds>(getTestExpirationTime().time_since_epoch()));
  EXPECT_EQ(exchangeRates.mNextRate.mExchangeRateInCents, getTestCents() / getTestHbar());
}

//-----
TEST_F(ExchangeRateUnitTests, ToString)
{
  // Given
  const ExchangeRate exchangeRate(getTestHbar(), getTestCents(), getTestExpirationTime());

  // When
  const std::string result = exchangeRate.toString();

  // Then
  EXPECT_FALSE(result.empty());
  EXPECT_NE(result.find(std::to_string(getTestHbar())), std::string::npos);
  EXPECT_NE(result.find(std::to_string(getTestCents())), std::string::npos);
}

//-----
TEST_F(ExchangeRateUnitTests, ExchangeRatesFromBytes)
{
  // Given
  proto::ExchangeRate protoExchangeRate;
  protoExchangeRate.set_hbarequiv(getTestHbar());
  protoExchangeRate.set_centequiv(getTestCents());
  protoExchangeRate.set_allocated_expirationtime(
    internal::TimestampConverter::toSecondsProtobuf(getTestExpirationTime()));

  proto::ExchangeRateSet protoExchangeRateSet;
  *protoExchangeRateSet.mutable_currentrate() = protoExchangeRate;
  *protoExchangeRateSet.mutable_nextrate() = protoExchangeRate;

  // When
  const ExchangeRates exchangeRates =
    ExchangeRates::fromBytes(internal::Utilities::stringToByteVector(protoExchangeRateSet.SerializeAsString()));

  // Then
  EXPECT_EQ(exchangeRates.mCurrentRate.mHbars, getTestHbar());
  EXPECT_EQ(exchangeRates.mCurrentRate.mCents, getTestCents());
  EXPECT_EQ(
    std::chrono::duration_cast<std::chrono::seconds>(exchangeRates.mCurrentRate.mExpirationTime.time_since_epoch()),
    std::chrono::duration_cast<std::chrono::seconds>(getTestExpirationTime().time_since_epoch()));
  EXPECT_EQ(exchangeRates.mCurrentRate.mExchangeRateInCents, getTestCents() / getTestHbar());
  EXPECT_EQ(exchangeRates.mNextRate.mHbars, getTestHbar());
  EXPECT_EQ(exchangeRates.mNextRate.mCents, getTestCents());
  EXPECT_EQ(
    std::chrono::duration_cast<std::chrono::seconds>(exchangeRates.mNextRate.mExpirationTime.time_since_epoch()),
    std::chrono::duration_cast<std::chrono::seconds>(getTestExpirationTime().time_since_epoch()));
  EXPECT_EQ(exchangeRates.mNextRate.mExchangeRateInCents, getTestCents() / getTestHbar());
}

//-----
TEST_F(ExchangeRateUnitTests, OperatorEqualsDefaults)
{
  // Given
  ExchangeRate lhs;
  ExchangeRate rhs;

  // Then
  EXPECT_TRUE(lhs == rhs);
}

//-----
TEST_F(ExchangeRateUnitTests, OperatorEqualsSimilar)
{
  // Given
  auto testExpirationTime = getTestExpirationTime();
  ExchangeRate lhs(getTestHbar(), getTestCents(), testExpirationTime);
  ExchangeRate rhs(getTestHbar(), getTestCents(), testExpirationTime);

  // Then
  EXPECT_TRUE(lhs == rhs);
}

//-----
TEST_F(ExchangeRateUnitTests, OperatorEqualsDiff)
{
  // Given
  ExchangeRate lhs(getTestHbar(), getTestCents(), getTestExpirationTime());
  ExchangeRate diffHbar(3, getTestCents(), getTestExpirationTime());
  ExchangeRate diffCents(getTestHbar(), 3, getTestExpirationTime());
  ExchangeRate diffExpirationTime(getTestHbar(), getTestCents(), getTestExpirationTime() + std::chrono::seconds(1));

  // Then
  EXPECT_FALSE(lhs == diffHbar);
  EXPECT_FALSE(lhs == diffCents);
  EXPECT_FALSE(lhs == diffExpirationTime);
}

//-----
TEST_F(ExchangeRateUnitTests, ExchangeRatesOperatorEqualsDefaults)
{
  // Given
  ExchangeRates lhs;
  ExchangeRates rhs;

  // Then
  EXPECT_TRUE(lhs == rhs);
}

//-----
TEST_F(ExchangeRateUnitTests, ExchangeRatesOperatorEqualsSimilar)
{
  // Given
  const ExchangeRate currentRate(getTestHbar(), getTestCents(), getTestExpirationTime());
  const ExchangeRate nextRate(getTestHbar(), getTestCents(), getTestExpirationTime());
  const ExchangeRates lhs(currentRate, nextRate);
  const ExchangeRates rhs(currentRate, nextRate);

  // Then
  EXPECT_TRUE(lhs == rhs);
}

//-----
TEST_F(ExchangeRateUnitTests, ExchangeRatesOperatorEqualsDiff)
{
  // Given
  const ExchangeRate currentRate(getTestHbar(), getTestCents(), getTestExpirationTime());
  const ExchangeRate nextRate(getTestHbar(), getTestCents(), getTestExpirationTime());

  const ExchangeRates lhs(currentRate, nextRate);
  const ExchangeRates diffCurrentRate(ExchangeRate(3, getTestCents(), getTestExpirationTime()), nextRate);
  const ExchangeRates diffNextRate(currentRate, ExchangeRate(getTestHbar(), 3, getTestExpirationTime()));
  const ExchangeRates diffNextRateExpiration(
    currentRate, ExchangeRate(getTestHbar(), getTestCents(), getTestExpirationTime() + std::chrono::seconds(1)));

  // Then
  EXPECT_FALSE(lhs == diffCurrentRate);
  EXPECT_FALSE(lhs == diffNextRate);
  EXPECT_FALSE(lhs == diffNextRateExpiration);
}
