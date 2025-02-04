// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "AccountRecords.h"
#include "AccountRecordsQuery.h"
#include "AccountStakersQuery.h"
#include "BaseIntegrationTest.h"
#include "ContractByteCodeQuery.h"
#include "ContractCallQuery.h"
#include "ContractFunctionResult.h"
#include "ContractInfo.h"
#include "ContractInfoQuery.h"
#include "FileContentsQuery.h"
#include "FileInfo.h"
#include "FileInfoQuery.h"
#include "Hbar.h"
#include "NetworkVersionInfo.h"
#include "NetworkVersionInfoQuery.h"
#include "ScheduleInfo.h"
#include "ScheduleInfoQuery.h"
#include "TokenInfo.h"
#include "TokenInfoQuery.h"
#include "TokenNftInfo.h"
#include "TokenNftInfoQuery.h"
#include "TopicInfo.h"
#include "TopicInfoQuery.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptQuery.h"
#include "TransactionRecord.h"
#include "TransactionRecordQuery.h"
#include "exceptions/PrecheckStatusException.h"

#include <chrono>
#include <functional>
#include <future>
#include <stdexcept>
#include <thread>

using namespace Hiero;

template<typename T>
class QueryIntegrationTests : public BaseIntegrationTest
{
};

using QueryTypes = ::testing::Types<AccountBalanceQuery,
                                    AccountInfoQuery,
                                    AccountRecordsQuery,
                                    AccountStakersQuery,
                                    ContractByteCodeQuery,
                                    ContractCallQuery,
                                    ContractInfoQuery,
                                    FileContentsQuery,
                                    FileInfoQuery,
                                    NetworkVersionInfoQuery,
                                    ScheduleInfoQuery,
                                    TokenInfoQuery,
                                    TokenNftInfoQuery,
                                    TopicInfoQuery,
                                    TransactionReceiptQuery,
                                    TransactionRecordQuery>;
TYPED_TEST_SUITE(QueryIntegrationTests, QueryTypes);

//-----
TYPED_TEST(QueryIntegrationTests, GetCostAsync)
{
  // Given
  TypeParam query;
  std::future<Hbar> cost;

  // When
  EXPECT_NO_THROW(cost = query.getCostAsync(BaseIntegrationTest::getTestClient()));

  // Then
  try
  {
    cost.get();
    EXPECT_TRUE(true);
  }
  catch (const PrecheckStatusException&)
  {
    // PrecheckStatusExceptions are fine since the Query is not being filled with any data.
    EXPECT_TRUE(true);
  }
  catch (...)
  {
    // Other exceptions constitute a failure.
    EXPECT_TRUE(false);
  }
}

//-----
TYPED_TEST(QueryIntegrationTests, GetCostAsyncWithSingleCallback)
{
  // Given
  bool completed = false;
  const std::function<void(const Hbar&, const std::exception&)> callback =
    [&completed](const Hbar&, const std::exception&) { completed = true; };

  TypeParam query;

  // When
  EXPECT_NO_THROW(query.getCostAsync(BaseIntegrationTest::getTestClient(), callback));

  // Then
  const std::chrono::system_clock::time_point timeout = std::chrono::system_clock::now() + std::chrono::seconds(10);
  while (!completed && std::chrono::system_clock::now() < timeout)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  EXPECT_TRUE(completed);
}

//-----
TYPED_TEST(QueryIntegrationTests, GetCostAsyncWithCallbacks)
{
  // Given
  bool completed = false;
  const std::function<void(const Hbar&)> responseCallback = [&completed](const Hbar&) { completed = true; };
  const std::function<void(const std::exception&)> exceptionCallback = [&completed](const std::exception&)
  { completed = true; };

  TypeParam query;

  // When
  EXPECT_NO_THROW(query.getCostAsync(BaseIntegrationTest::getTestClient(), responseCallback, exceptionCallback));

  // Then
  const std::chrono::system_clock::time_point timeout = std::chrono::system_clock::now() + std::chrono::seconds(10);
  while (!completed && std::chrono::system_clock::now() < timeout)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  EXPECT_TRUE(completed);
}
