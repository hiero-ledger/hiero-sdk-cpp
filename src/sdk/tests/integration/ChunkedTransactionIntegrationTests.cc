// SPDX-License-Identifier: Apache-2.0
#include "BaseIntegrationTest.h"
#include "FileAppendTransaction.h"
#include "TopicMessageSubmitTransaction.h"
#include "TransactionResponse.h"
#include "exceptions/PrecheckStatusException.h"

#include <chrono>
#include <functional>
#include <future>
#include <stdexcept>
#include <thread>

using namespace Hiero;

template<typename T>
class ChunkedTransactionIntegrationTests : public BaseIntegrationTest
{
};

using ChunkedTransactionTypes = ::testing::Types<FileAppendTransaction, TopicMessageSubmitTransaction>;
TYPED_TEST_SUITE(ChunkedTransactionIntegrationTests, ChunkedTransactionTypes);

//-----
TYPED_TEST(ChunkedTransactionIntegrationTests, ExecuteAllAsync)
{
  // Given
  TypeParam chunkedTransaction;
  std::future<std::vector<TransactionResponse>> response;

  // When
  EXPECT_NO_THROW(response = chunkedTransaction.executeAllAsync(BaseIntegrationTest::getTestClient()));

  // Then
  try
  {
    response.get();
  }
  catch (const PrecheckStatusException&)
  {
    // PrecheckStatusExceptions are fine since the ChunkedTransaction is not being filled with any data.
    EXPECT_TRUE(true);
  }
  catch (...)
  {
    // Other exceptions constitute a failure.
    EXPECT_TRUE(false);
  }

  // No exception is fine.
  EXPECT_TRUE(true);
}

//-----
TYPED_TEST(ChunkedTransactionIntegrationTests, ExecuteAllAsyncWithSingleCallback)
{
  // Given
  bool completed = false;
  const std::function<void(const std::vector<TransactionResponse>&, const std::exception&)> callback =
    [&completed](const std::vector<TransactionResponse>&, const std::exception&) { completed = true; };

  TypeParam chunkedTransaction;

  // When
  EXPECT_NO_THROW(chunkedTransaction.executeAllAsync(BaseIntegrationTest::getTestClient(), callback));

  // Then
  const std::chrono::system_clock::time_point timeout = std::chrono::system_clock::now() + std::chrono::seconds(10);
  while (!completed && std::chrono::system_clock::now() < timeout)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  EXPECT_TRUE(completed);
}

//-----
TYPED_TEST(ChunkedTransactionIntegrationTests, ExecuteAllAsyncWithCallbacks)
{
  // Given
  bool completed = false;
  const std::function<void(const std::vector<TransactionResponse>&)> responseCallback =
    [&completed](const std::vector<TransactionResponse>&) { completed = true; };
  const std::function<void(const std::exception&)> exceptionCallback = [&completed](const std::exception&)
  { completed = true; };

  TypeParam chunkedTransaction;

  // When
  EXPECT_NO_THROW(
    chunkedTransaction.executeAllAsync(BaseIntegrationTest::getTestClient(), responseCallback, exceptionCallback));

  // Then
  const std::chrono::system_clock::time_point timeout = std::chrono::system_clock::now() + std::chrono::seconds(10);
  while (!completed && std::chrono::system_clock::now() < timeout)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  EXPECT_TRUE(completed);
}
