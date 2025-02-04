// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "Defaults.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"

#include <gtest/gtest.h>

using namespace Hiero;

class ClientUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mAccountId; }
  [[nodiscard]] inline const std::shared_ptr<ED25519PrivateKey>& getTestPrivateKey() const { return mPrivateKey; }
  [[nodiscard]] inline const std::chrono::system_clock::duration& getTestNetworkUpdatePeriod() const
  {
    return mTestNetworkUpdatePeriod;
  }

  [[nodiscard]] inline const std::chrono::milliseconds& getNegativeBackoffTime() const { return mNegativeBackoffTime; }
  [[nodiscard]] inline const std::chrono::milliseconds& getZeroBackoffTime() const { return mZeroBackoffTime; }
  [[nodiscard]] inline const std::chrono::milliseconds& getBelowMinBackoffTime() const { return mBelowMinBackoffTime; }
  [[nodiscard]] inline const std::chrono::milliseconds& getAboveMaxBackoffTime() const { return mAboveMaxBackoffTime; }

private:
  const AccountId mAccountId = AccountId(10ULL);
  const std::shared_ptr<ED25519PrivateKey> mPrivateKey = ED25519PrivateKey::generatePrivateKey();
  const std::chrono::system_clock::duration mTestNetworkUpdatePeriod = std::chrono::seconds(2);

  const std::chrono::milliseconds mNegativeBackoffTime = std::chrono::milliseconds(-1);
  const std::chrono::milliseconds mZeroBackoffTime = std::chrono::milliseconds(0);
  const std::chrono::milliseconds mBelowMinBackoffTime = DEFAULT_MIN_BACKOFF - std::chrono::milliseconds(1);
  const std::chrono::milliseconds mAboveMaxBackoffTime = DEFAULT_MAX_BACKOFF + std::chrono::milliseconds(1);
};

//-----
TEST_F(ClientUnitTests, ConstructClient)
{
  Client client;
  EXPECT_FALSE(client.getOperatorAccountId());
  EXPECT_EQ(client.getOperatorPublicKey(), nullptr);
  EXPECT_FALSE(client.getMaxTransactionFee());
  EXPECT_EQ(client.getRequestTimeout(), std::chrono::minutes(2));
}

//-----
TEST_F(ClientUnitTests, MoveClient)
{
  Client client;
  client.setOperator(getTestAccountId(), getTestPrivateKey());

  Client client2 = std::move(client);
  EXPECT_EQ(*client2.getOperatorAccountId(), getTestAccountId());
  EXPECT_EQ(client2.getOperatorPublicKey()->toStringDer(), getTestPrivateKey()->getPublicKey()->toStringDer());
}

//-----
TEST_F(ClientUnitTests, SetOperator)
{
  Client client;
  client.setOperator(getTestAccountId(), getTestPrivateKey());

  EXPECT_EQ(*client.getOperatorAccountId(), getTestAccountId());
  EXPECT_EQ(client.getOperatorPublicKey()->toStringDer(), getTestPrivateKey()->getPublicKey()->toStringDer());

  client.setOperator(getTestAccountId(), ED25519PrivateKey::generatePrivateKey());

  // No way to grab the string value of the rvalue, just make it's not empty
  EXPECT_FALSE(client.getOperatorPublicKey()->toStringDer().empty());
}

//-----
TEST_F(ClientUnitTests, SetDefaultMaxTransactionFee)
{
  Client client;
  const auto fee = Hbar(1ULL);
  client.setMaxTransactionFee(fee);
  EXPECT_EQ(*client.getMaxTransactionFee(), fee);

  // Negative value should throw
  EXPECT_THROW(client.setMaxTransactionFee(fee.negated()), std::invalid_argument);
}

//-----
TEST_F(ClientUnitTests, SetNetworkUpdatePeriod)
{
  // Given
  Client client;

  // When
  client.setNetworkUpdatePeriod(getTestNetworkUpdatePeriod());

  // Then
  EXPECT_EQ(client.getNetworkUpdatePeriod(), getTestNetworkUpdatePeriod());
}

//-----
TEST_F(ClientUnitTests, SetInvalidMinBackoff)
{
  // Given
  std::unordered_map<std::string, AccountId> networkMap;
  Client client = Client::forNetwork(networkMap);

  // When / Then
  EXPECT_THROW(client.setMinBackoff(getNegativeBackoffTime()), std::invalid_argument); // INVALID_ARGUMENT
  EXPECT_THROW(client.setMinBackoff(getAboveMaxBackoffTime()), std::invalid_argument); // INVALID_ARGUMENT
}

//-----
TEST_F(ClientUnitTests, SetValidMinBackoff)
{
  // Given
  std::unordered_map<std::string, AccountId> networkMap;
  Client client = Client::forNetwork(networkMap);

  // When / Then
  EXPECT_NO_THROW(client.setMinBackoff(getZeroBackoffTime()));
  EXPECT_NO_THROW(client.setMinBackoff(DEFAULT_MIN_BACKOFF));
  EXPECT_NO_THROW(client.setMinBackoff(DEFAULT_MAX_BACKOFF));
}

//-----
TEST_F(ClientUnitTests, SetInvalidMaxBackoff)
{
  // Given
  std::unordered_map<std::string, AccountId> networkMap;
  Client client = Client::forNetwork(networkMap);

  // When / Then
  EXPECT_THROW(client.setMaxBackoff(getNegativeBackoffTime()), std::invalid_argument); // INVALID_ARGUMENT
  EXPECT_THROW(client.setMaxBackoff(getZeroBackoffTime()), std::invalid_argument);     // INVALID_ARGUMENT
  EXPECT_THROW(client.setMaxBackoff(getBelowMinBackoffTime()), std::invalid_argument); // INVALID_ARGUMENT
  EXPECT_THROW(client.setMaxBackoff(getAboveMaxBackoffTime()), std::invalid_argument); // INVALID_ARGUMENT
}

//-----
TEST_F(ClientUnitTests, SetValidMaxBackoff)
{
  // Given
  std::unordered_map<std::string, AccountId> networkMap;
  Client client = Client::forNetwork(networkMap);

  // When / Then
  EXPECT_NO_THROW(client.setMaxBackoff(DEFAULT_MIN_BACKOFF));
  EXPECT_NO_THROW(client.setMaxBackoff(DEFAULT_MAX_BACKOFF));
}