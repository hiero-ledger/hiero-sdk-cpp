// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "Defaults.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"

#include <gtest/gtest.h>

#include <thread>

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

//-----
TEST_F(ClientUnitTests, SetValidGrpcDeadline)
{
  // Given
  Client client;

  // When / Then
  EXPECT_NO_THROW(client.setGrpcDeadline(std::chrono::seconds(5)));
  EXPECT_EQ(client.getGrpcDeadline().value(), std::chrono::seconds(5));
  EXPECT_NO_THROW(client.setGrpcDeadline(std::chrono::seconds(30)));
  EXPECT_EQ(client.getGrpcDeadline().value(), std::chrono::seconds(30));
}

//-----
TEST_F(ClientUnitTests, SetInvalidGrpcDeadline)
{
  // Given
  Client client;
  client.setRequestTimeout(std::chrono::seconds(30));

  // When / Then - gRPC deadline cannot be greater than request timeout
  EXPECT_THROW(client.setGrpcDeadline(std::chrono::seconds(60)), std::invalid_argument);
}

//-----
TEST_F(ClientUnitTests, SetValidRequestTimeout)
{
  // Given
  Client client;
  client.setGrpcDeadline(std::chrono::seconds(10));

  // When / Then
  EXPECT_NO_THROW(client.setRequestTimeout(std::chrono::seconds(30)));
  EXPECT_EQ(client.getRequestTimeout(), std::chrono::seconds(30));
  EXPECT_NO_THROW(client.setRequestTimeout(std::chrono::minutes(2)));
  EXPECT_EQ(client.getRequestTimeout(), std::chrono::minutes(2));
}

//-----
TEST_F(ClientUnitTests, SetInvalidRequestTimeout)
{
  // Given
  Client client;
  client.setGrpcDeadline(std::chrono::seconds(30));

  // When / Then - request timeout cannot be less than gRPC deadline
  EXPECT_THROW(client.setRequestTimeout(std::chrono::seconds(10)), std::invalid_argument);
}

//-----
TEST_F(ClientUnitTests, SetGrpcDeadlineAndRequestTimeoutInCorrectOrder)
{
  // Given
  Client client;

  // When / Then - setting grpcDeadline first, then requestTimeout (should succeed)
  EXPECT_NO_THROW(client.setGrpcDeadline(std::chrono::seconds(10)));
  EXPECT_NO_THROW(client.setRequestTimeout(std::chrono::seconds(30)));

  // When / Then - setting requestTimeout first, then grpcDeadline (should succeed)
  Client client2;
  EXPECT_NO_THROW(client2.setRequestTimeout(std::chrono::seconds(30)));
  EXPECT_NO_THROW(client2.setGrpcDeadline(std::chrono::seconds(10)));
}

//-----
TEST_F(ClientUnitTests, SetGrpcDeadlineEqualToRequestTimeout)
{
  // Given
  Client client;

  // When / Then - grpcDeadline can equal requestTimeout
  EXPECT_NO_THROW(client.setRequestTimeout(std::chrono::seconds(30)));
  EXPECT_NO_THROW(client.setGrpcDeadline(std::chrono::seconds(30)));
}

//-----
TEST_F(ClientUnitTests, DefaultValuesRespectConstraint)
{
  // Given
  Client client;

  // When / Then - defaults should already satisfy requestTimeout >= grpcDeadline
  EXPECT_EQ(client.getRequestTimeout(), DEFAULT_REQUEST_TIMEOUT); // 2 minutes
  EXPECT_FALSE(client.getGrpcDeadline().has_value()); // Not set, uses DEFAULT_GRPC_DEADLINE (10s) internally

  // Setting them explicitly with defaults should work
  EXPECT_NO_THROW(client.setGrpcDeadline(DEFAULT_GRPC_DEADLINE));
  EXPECT_NO_THROW(client.setRequestTimeout(DEFAULT_REQUEST_TIMEOUT));
}

//-----
TEST_F(ClientUnitTests, AllowReceiptNodeFailoverDefaultIsFalse)
{
  Client client;
  EXPECT_FALSE(client.getAllowReceiptNodeFailover());
}

//-----
TEST_F(ClientUnitTests, SetAllowReceiptNodeFailover)
{
  Client client;
  client.setAllowReceiptNodeFailover(true);
  EXPECT_TRUE(client.getAllowReceiptNodeFailover());

  client.setAllowReceiptNodeFailover(false);
  EXPECT_FALSE(client.getAllowReceiptNodeFailover());
}

//-----
// Regression tests for Issue #931: Network update thread deadlock.
// The network update thread calls back into Client getters (getRequestTimeout,
// getClientMirrorNetwork, etc.) which all acquire mMutex. If the thread held
// mMutex during the query, it would self-deadlock. These tests verify that
// Client lifecycle operations complete without hanging.

TEST_F(ClientUnitTests, NetworkUpdateThreadDoesNotDeadlockOnDestruction)
{
  // Given / When
  // Creating and immediately destroying a Client exercises the full lifecycle:
  // constructor starts the bg thread, destructor cancels and joins it.
  // This test will hang/timeout if the deadlock is present.
  {
    Client client;
    // Give the thread a moment to start and enter its wait state.
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
  }
  // If we get here, no deadlock occurred.

  // Then
  SUCCEED();
}

//-----
TEST_F(ClientUnitTests, NetworkUpdateThreadDoesNotDeadlockWithConcurrentGetters)
{
  // Given
  Client client;
  client.setOperator(getTestAccountId(), getTestPrivateKey());

  // When — call multiple getters concurrently while the bg thread is alive.
  // These getters all acquire mMutex. If the bg thread holds mMutex during its
  // query, this would deadlock or contend indefinitely.
  for (int i = 0; i < 10; ++i)
  {
    EXPECT_NO_THROW(client.getRequestTimeout());
    EXPECT_NO_THROW(client.getOperatorAccountId());
    EXPECT_NO_THROW(client.getNetworkUpdatePeriod());
    EXPECT_NO_THROW(client.getMaxTransactionFee());
    EXPECT_NO_THROW(client.getMaxQueryPayment());
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  // Then — destruction also must not deadlock.
  SUCCEED();
}

//-----
TEST_F(ClientUnitTests, NetworkUpdateThreadDoesNotDeadlockOnSetNetworkUpdatePeriod)
{
  // Given
  Client client;

  // When — setNetworkUpdatePeriod cancels the old thread and starts a new one.
  // This exercises cancel + join + restart while the bg thread may be waiting.
  EXPECT_NO_THROW(client.setNetworkUpdatePeriod(std::chrono::seconds(30)));
  EXPECT_EQ(client.getNetworkUpdatePeriod(), std::chrono::seconds(30));

  // When — do it again to exercise the cycle a second time.
  EXPECT_NO_THROW(client.setNetworkUpdatePeriod(std::chrono::seconds(60)));
  EXPECT_EQ(client.getNetworkUpdatePeriod(), std::chrono::seconds(60));

  // Then
  SUCCEED();
}

//-----
TEST_F(ClientUnitTests, NetworkUpdateThreadDoesNotDeadlockOnMove)
{
  // Given
  Client client;
  client.setOperator(getTestAccountId(), getTestPrivateKey());

  // Give the thread a moment to start.
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // When — moving a Client cancels the bg thread on the source and restarts
  // it on the destination.
  Client client2 = std::move(client);

  // Then — the moved-to client should be functional and destroyable.
  EXPECT_EQ(*client2.getOperatorAccountId(), getTestAccountId());
  SUCCEED();
}

//-----
TEST_F(ClientUnitTests, NetworkUpdateThreadDoesNotDeadlockOnClose)
{
  // Given
  Client client;

  // Give the thread a moment to start.
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  // When — close() cancels the bg thread and joins it.
  EXPECT_NO_THROW(client.close());

  // Then — if we reach here without hanging, no deadlock occurred.
  SUCCEED();
}

//-----
TEST_F(ClientUnitTests, NetworkUpdateThreadSkipsUpdateWhenNoNetworkConfigured)
{
  // Given — a Client with no mirror/consensus network and a very short update
  // period. This forces the bg thread past wait_for and into the body of
  // scheduleNetworkUpdate(), exercising the null-network continue guard.
  Client client;
  client.setNetworkUpdatePeriod(std::chrono::milliseconds(50));

  // When — let several update cycles fire.
  std::this_thread::sleep_for(std::chrono::milliseconds(300));

  // Then — the update period is preserved and the mutex is not stuck.
  // getNetworkUpdatePeriod() acquires mMutex, so it would hang if the bg
  // thread were holding the lock.
  EXPECT_EQ(client.getNetworkUpdatePeriod(), std::chrono::milliseconds(50));
}
