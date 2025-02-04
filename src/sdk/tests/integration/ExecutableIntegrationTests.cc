// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceApproveTransaction.h"
#include "AccountAllowanceDeleteTransaction.h"
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "AccountRecords.h"
#include "AccountRecordsQuery.h"
#include "AccountStakersQuery.h"
#include "AccountUpdateTransaction.h"
#include "BaseIntegrationTest.h"
#include "ContractByteCodeQuery.h"
#include "ContractCallQuery.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ContractExecuteTransaction.h"
#include "ContractFunctionResult.h"
#include "ContractInfo.h"
#include "ContractInfoQuery.h"
#include "ContractUpdateTransaction.h"
#include "EthereumTransaction.h"
#include "FileContentsQuery.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileInfo.h"
#include "FileInfoQuery.h"
#include "FileUpdateTransaction.h"
#include "FreezeTransaction.h"
#include "NetworkVersionInfo.h"
#include "NetworkVersionInfoQuery.h"
#include "PrngTransaction.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleDeleteTransaction.h"
#include "ScheduleInfo.h"
#include "ScheduleInfoQuery.h"
#include "ScheduleSignTransaction.h"
#include "SystemDeleteTransaction.h"
#include "SystemUndeleteTransaction.h"
#include "TokenAssociateTransaction.h"
#include "TokenBurnTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenDissociateTransaction.h"
#include "TokenFeeScheduleUpdateTransaction.h"
#include "TokenFreezeTransaction.h"
#include "TokenGrantKycTransaction.h"
#include "TokenInfo.h"
#include "TokenInfoQuery.h"
#include "TokenMintTransaction.h"
#include "TokenNftInfo.h"
#include "TokenNftInfoQuery.h"
#include "TokenPauseTransaction.h"
#include "TokenRevokeKycTransaction.h"
#include "TokenUnfreezeTransaction.h"
#include "TokenUnpauseTransaction.h"
#include "TokenUpdateTransaction.h"
#include "TokenWipeTransaction.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicInfo.h"
#include "TopicInfoQuery.h"
#include "TopicUpdateTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptQuery.h"
#include "TransactionRecord.h"
#include "TransactionRecordQuery.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"

#include <chrono>
#include <functional>
#include <future>
#include <stdexcept>
#include <thread>

using namespace Hiero;

template<typename T>
class ExecutableIntegrationTests : public BaseIntegrationTest
{
};

using ExecutableTypes = ::testing::Types<std::tuple<AccountAllowanceApproveTransaction, TransactionResponse>,
                                         std::tuple<AccountAllowanceDeleteTransaction, TransactionResponse>,
                                         std::tuple<AccountBalanceQuery, AccountBalance>,
                                         std::tuple<AccountCreateTransaction, TransactionResponse>,
                                         std::tuple<AccountDeleteTransaction, TransactionResponse>,
                                         std::tuple<AccountInfoQuery, AccountInfo>,
                                         std::tuple<AccountRecordsQuery, AccountRecords>,
                                         std::tuple<AccountStakersQuery, AccountStakers>,
                                         std::tuple<AccountUpdateTransaction, TransactionResponse>,
                                         std::tuple<ContractByteCodeQuery, ContractByteCode>,
                                         std::tuple<ContractCallQuery, ContractFunctionResult>,
                                         std::tuple<ContractCreateTransaction, TransactionResponse>,
                                         std::tuple<ContractDeleteTransaction, TransactionResponse>,
                                         std::tuple<ContractExecuteTransaction, TransactionResponse>,
                                         std::tuple<ContractInfoQuery, ContractInfo>,
                                         std::tuple<ContractUpdateTransaction, TransactionResponse>,
                                         std::tuple<EthereumTransaction, TransactionResponse>,
                                         std::tuple<FileContentsQuery, FileContents>,
                                         std::tuple<FileCreateTransaction, TransactionResponse>,
                                         std::tuple<FileDeleteTransaction, TransactionResponse>,
                                         std::tuple<FileInfoQuery, FileInfo>,
                                         std::tuple<FileUpdateTransaction, TransactionResponse>,
                                         std::tuple<FreezeTransaction, TransactionResponse>,
                                         std::tuple<NetworkVersionInfoQuery, NetworkVersionInfo>,
                                         std::tuple<PrngTransaction, TransactionResponse>,
                                         std::tuple<ScheduleCreateTransaction, TransactionResponse>,
                                         std::tuple<ScheduleDeleteTransaction, TransactionResponse>,
                                         std::tuple<ScheduleInfoQuery, ScheduleInfo>,
                                         std::tuple<ScheduleSignTransaction, TransactionResponse>,
                                         std::tuple<SystemDeleteTransaction, TransactionResponse>,
                                         std::tuple<SystemUndeleteTransaction, TransactionResponse>,
                                         std::tuple<TokenAssociateTransaction, TransactionResponse>,
                                         std::tuple<TokenBurnTransaction, TransactionResponse>,
                                         std::tuple<TokenCreateTransaction, TransactionResponse>,
                                         std::tuple<TokenDeleteTransaction, TransactionResponse>,
                                         std::tuple<TokenDissociateTransaction, TransactionResponse>,
                                         std::tuple<TokenFeeScheduleUpdateTransaction, TransactionResponse>,
                                         std::tuple<TokenFreezeTransaction, TransactionResponse>,
                                         std::tuple<TokenGrantKycTransaction, TransactionResponse>,
                                         std::tuple<TokenInfoQuery, TokenInfo>,
                                         std::tuple<TokenMintTransaction, TransactionResponse>,
                                         std::tuple<TokenNftInfoQuery, TokenNftInfo>,
                                         std::tuple<TokenPauseTransaction, TransactionResponse>,
                                         std::tuple<TokenRevokeKycTransaction, TransactionResponse>,
                                         std::tuple<TokenUnfreezeTransaction, TransactionResponse>,
                                         std::tuple<TokenUnpauseTransaction, TransactionResponse>,
                                         std::tuple<TokenUpdateTransaction, TransactionResponse>,
                                         std::tuple<TokenWipeTransaction, TransactionResponse>,
                                         std::tuple<TopicCreateTransaction, TransactionResponse>,
                                         std::tuple<TopicDeleteTransaction, TransactionResponse>,
                                         std::tuple<TopicInfoQuery, TopicInfo>,
                                         std::tuple<TopicUpdateTransaction, TransactionResponse>,
                                         std::tuple<TransactionReceiptQuery, TransactionReceipt>,
                                         std::tuple<TransactionRecordQuery, TransactionRecord>,
                                         std::tuple<TransferTransaction, TransactionResponse>>;
TYPED_TEST_SUITE(ExecutableIntegrationTests, ExecutableTypes);

//-----
TYPED_TEST(ExecutableIntegrationTests, ExecuteAsync)
{
  // Given
  using ExecutableType = typename std::tuple_element<0, TypeParam>::type;
  using ResponseType = typename std::tuple_element<1, TypeParam>::type;

  ExecutableType executable;
  std::future<ResponseType> response;

  // When
  EXPECT_NO_THROW(response = executable.executeAsync(BaseIntegrationTest::getTestClient()));

  // Then
  try
  {
    response.get();
  }
  catch (const PrecheckStatusException&)
  {
    // PrecheckStatusExceptions are fine since the Executable is not being filled with any data.
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
TYPED_TEST(ExecutableIntegrationTests, ExecuteAsyncWithSingleCallback)
{
  // Given
  using ExecutableType = typename std::tuple_element<0, TypeParam>::type;
  using ResponseType = typename std::tuple_element<1, TypeParam>::type;

  bool completed = false;
  const std::function<void(const ResponseType&, const std::exception&)> callback =
    [&completed](const ResponseType&, const std::exception&) { completed = true; };

  ExecutableType executable;

  // When
  EXPECT_NO_THROW(executable.executeAsync(BaseIntegrationTest::getTestClient(), callback));

  // Then
  const std::chrono::system_clock::time_point timeout = std::chrono::system_clock::now() + std::chrono::seconds(10);
  while (!completed && std::chrono::system_clock::now() < timeout)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  EXPECT_TRUE(completed);
}

//-----
TYPED_TEST(ExecutableIntegrationTests, ExecuteAsyncWithCallbacks)
{
  // Given
  using ExecutableType = typename std::tuple_element<0, TypeParam>::type;
  using ResponseType = typename std::tuple_element<1, TypeParam>::type;

  bool completed = false;
  const std::function<void(const ResponseType&)> responseCallback = [&completed](const ResponseType&)
  { completed = true; };
  const std::function<void(const std::exception&)> exceptionCallback = [&completed](const std::exception&)
  { completed = true; };

  ExecutableType executable;

  // When
  EXPECT_NO_THROW(executable.executeAsync(BaseIntegrationTest::getTestClient(), responseCallback, exceptionCallback));

  // Then
  const std::chrono::system_clock::time_point timeout = std::chrono::system_clock::now() + std::chrono::seconds(10);
  while (!completed && std::chrono::system_clock::now() < timeout)
  {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  EXPECT_TRUE(completed);
}
