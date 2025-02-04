// SPDX-License-Identifier: Apache-2.0
#include "AccountRecords.h"

#include <crypto_get_account_records.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class AccountRecordsUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const AccountId& getTestAccountId() const { return mTestAccountId; }

private:
  const AccountId mTestAccountId = AccountId(3ULL);
};

//-----
TEST_F(AccountRecordsUnitTests, DeserializeAccountRecordsFromProtobuf)
{
  // Given
  proto::CryptoGetAccountRecordsResponse accountRecordsResponse;
  accountRecordsResponse.set_allocated_accountid(getTestAccountId().toProtobuf().release());
  accountRecordsResponse.add_records();
  accountRecordsResponse.add_records();

  // When
  const AccountRecords accountRecords = AccountRecords::fromProtobuf(accountRecordsResponse);

  // Then
  EXPECT_EQ(accountRecords.mAccountId, getTestAccountId());
  // Don't bother testing records data as that's already tested in TransactionRecordTest
  EXPECT_EQ(accountRecords.mRecords.size(), 2);
}