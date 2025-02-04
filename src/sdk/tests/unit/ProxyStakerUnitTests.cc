// SPDX-License-Identifier: Apache-2.0
#include "ProxyStaker.h"

#include <crypto_get_stakers.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class ProxyStakerUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }
  [[nodiscard]] inline const Hbar& getTestAmount() const { return mTestAmount; }

private:
  const AccountId mTestAccountId = AccountId(1ULL, 2ULL, 3ULL);
  const Hbar mTestAmount = Hbar(4LL);
};

//-----
TEST_F(ProxyStakerUnitTests, ConstructWithValues)
{
  // Given / When
  const ProxyStaker proxyStaker(getTestAccountId(), getTestAmount().toTinybars());

  // Then
  EXPECT_EQ(proxyStaker.mAccountId, getTestAccountId());
  EXPECT_EQ(proxyStaker.mAmount, getTestAmount());
}

//-----
TEST_F(ProxyStakerUnitTests, FromProtobuf)
{
  // Given
  proto::ProxyStaker protoProxyStaker;
  protoProxyStaker.set_allocated_accountid(getTestAccountId().toProtobuf().release());
  protoProxyStaker.set_amount(getTestAmount().toTinybars());

  // When
  const ProxyStaker proxyStaker = ProxyStaker::fromProtobuf(protoProxyStaker);

  // Then
  EXPECT_EQ(proxyStaker.mAccountId, getTestAccountId());
  EXPECT_EQ(proxyStaker.mAmount, getTestAmount());
}
