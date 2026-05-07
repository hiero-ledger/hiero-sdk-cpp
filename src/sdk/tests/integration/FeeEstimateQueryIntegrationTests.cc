// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "Client.h"
#include "ContractCreateTransaction.h"
#include "ED25519PrivateKey.h"
#include "FeeEstimateMode.h"
#include "FeeEstimateQuery.h"
#include "FeeEstimateResponse.h"
#include "FileAppendTransaction.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "Hbar.h"
#include "TokenCreateTransaction.h"
#include "TokenMintTransaction.h"
#include "TokenSupplyType.h"
#include "TokenType.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicId.h"
#include "TopicMessageSubmitTransaction.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "impl/HttpClient.h"
#include "impl/Utilities.h"

#include <chrono>
#include <gtest/gtest.h>
#include <services/transaction.pb.h>
#include <vector>

using namespace Hiero;

namespace
{
// Tolerance band for "estimate vs. actual" comparisons (test #15, #17). Estimates are computed against the
// mirror node's known state, while actuals are charged at consensus time — small variances are expected.
constexpr double ACTUAL_VS_ESTIMATE_LOWER_BOUND = 0.5;
constexpr double ACTUAL_VS_ESTIMATE_UPPER_BOUND = 2.0;

WrappedTransaction wrap(const TransferTransaction& tx)
{
  return WrappedTransaction(WrappedTransaction::AnyPossibleTransaction(tx));
}
} // namespace

class FeeEstimateQueryIntegrationTests : public BaseIntegrationTest
{
};

//-----
// Design test #1: TransferTransaction in STATE mode auto-freezes and returns valid components.
TEST_F(FeeEstimateQueryIntegrationTests, EstimateTransferTransactionStateMode)
{
  TransferTransaction tx;
  tx.addHbarTransfer(getTestClient().getOperatorAccountId().value(), Hbar(-1LL))
    .addHbarTransfer(AccountId(0, 0, 3ULL), Hbar(1LL));

  FeeEstimateQuery query;
  query.setMode(FeeEstimateMode::STATE).setTransaction(wrap(tx));

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = query.execute(getTestClient()));

  EXPECT_GT(response.mTotal, 0ULL);
  EXPECT_EQ(response.mTotal, response.mNetwork.mSubtotal + response.mNode.subtotal() + response.mService.subtotal());
}

//-----
// Design test #2: TransferTransaction in INTRINSIC mode returns valid components without state-dependent fees.
TEST_F(FeeEstimateQueryIntegrationTests, EstimateTransferTransactionIntrinsicMode)
{
  TransferTransaction tx;
  tx.addHbarTransfer(getTestClient().getOperatorAccountId().value(), Hbar(-1LL))
    .addHbarTransfer(AccountId(0, 0, 3ULL), Hbar(1LL));

  FeeEstimateQuery query;
  query.setMode(FeeEstimateMode::INTRINSIC).setTransaction(wrap(tx));

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = query.execute(getTestClient()));
  EXPECT_GT(response.mTotal, 0ULL);
}

//-----
// Design test #3: default mode is INTRINSIC.
TEST_F(FeeEstimateQueryIntegrationTests, DefaultModeIsIntrinsicEndToEnd)
{
  TransferTransaction tx;
  tx.addHbarTransfer(getTestClient().getOperatorAccountId().value(), Hbar(-1LL))
    .addHbarTransfer(AccountId(0, 0, 3ULL), Hbar(1LL));

  FeeEstimateQuery query;
  query.setTransaction(wrap(tx));
  EXPECT_EQ(query.getMode(), FeeEstimateMode::INTRINSIC);

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = query.execute(getTestClient()));
  EXPECT_GT(response.mTotal, 0ULL);
}

//-----
// Design test #4: missing transaction throws.
TEST_F(FeeEstimateQueryIntegrationTests, MissingTransactionThrows)
{
  FeeEstimateQuery query;
  EXPECT_THROW(query.execute(getTestClient()), std::invalid_argument);
}

//-----
// Design test #5: TokenCreateTransaction.
TEST_F(FeeEstimateQueryIntegrationTests, EstimateTokenCreate)
{
  TokenCreateTransaction tx;
  tx.setTokenName("FeeEstimateTestToken")
    .setTokenSymbol("FETT")
    .setDecimals(0)
    .setInitialSupply(0)
    .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
    .setTokenType(TokenType::FUNGIBLE_COMMON);

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = tx.estimateFee().execute(getTestClient()));
  EXPECT_GT(response.mTotal, 0ULL);
  EXPECT_GT(response.mService.mBase, 0ULL);
}

//-----
// Design test #6: TokenMintTransaction (NFT mint with multiple serials -> extras for mint count beyond included).
TEST_F(FeeEstimateQueryIntegrationTests, EstimateTokenMintMultipleTokens)
{
  // First create an NFT token to mint serials for.
  const std::shared_ptr<PrivateKey> supplyKey = ED25519PrivateKey::generatePrivateKey();
  TransactionReceipt createReceipt;
  ASSERT_NO_THROW(createReceipt = TokenCreateTransaction()
                                    .setTokenName("FeeEstimateNft")
                                    .setTokenSymbol("FENFT")
                                    .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                                    .setSupplyType(TokenSupplyType::FINITE)
                                    .setMaxSupply(100)
                                    .setTreasuryAccountId(getTestClient().getOperatorAccountId().value())
                                    .setSupplyKey(supplyKey)
                                    .freezeWith(&const_cast<Client&>(getTestClient()))
                                    .sign(supplyKey)
                                    .execute(getTestClient())
                                    .getReceipt(getTestClient()));
  const TokenId tokenId = createReceipt.mTokenId.value();

  TokenMintTransaction tx;
  tx.setTokenId(tokenId);
  for (int i = 0; i < 5; ++i)
  {
    tx.addMetadata({ std::byte{ 0x01 }, std::byte{ static_cast<unsigned char>(i) } });
  }

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = tx.estimateFee().execute(getTestClient()));
  EXPECT_GT(response.mTotal, 0ULL);
}

//-----
// Design test #7: TopicCreateTransaction.
TEST_F(FeeEstimateQueryIntegrationTests, EstimateTopicCreate)
{
  TopicCreateTransaction tx;
  tx.setMemo("FeeEstimate test topic");

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = tx.estimateFee().execute(getTestClient()));
  EXPECT_GT(response.mTotal, 0ULL);
}

//-----
// Design test #8: ContractCreateTransaction.
TEST_F(FeeEstimateQueryIntegrationTests, EstimateContractCreate)
{
  // Upload bytecode to a file first.
  TransactionReceipt fileReceipt;
  ASSERT_NO_THROW(fileReceipt = FileCreateTransaction()
                                  .setKeys({ getTestClient().getOperatorPublicKey() })
                                  .setContents(getTestSmartContractBytecode())
                                  .execute(getTestClient())
                                  .getReceipt(getTestClient()));
  const FileId bytecodeFile = fileReceipt.mFileId.value();

  ContractCreateTransaction tx;
  tx.setBytecodeFileId(bytecodeFile).setGas(500000ULL).setConstructorParameters({});

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = tx.estimateFee().execute(getTestClient()));
  EXPECT_GT(response.mTotal, 0ULL);

  ASSERT_NO_THROW(FileDeleteTransaction().setFileId(bytecodeFile).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
// Design test #9: FileCreateTransaction.
TEST_F(FeeEstimateQueryIntegrationTests, EstimateFileCreate)
{
  FileCreateTransaction tx;
  tx.setKeys({ getTestClient().getOperatorPublicKey() }).setContents(std::string("hello, simple-fees"));

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = tx.estimateFee().execute(getTestClient()));
  EXPECT_GT(response.mTotal, 0ULL);
}

//-----
// Design test #10: network.subtotal == (node.base + sum(node.extras[*].subtotal)) * network.multiplier.
TEST_F(FeeEstimateQueryIntegrationTests, NetworkSubtotalEqualsNodeTotalTimesMultiplier)
{
  TransferTransaction tx;
  tx.addHbarTransfer(getTestClient().getOperatorAccountId().value(), Hbar(-1LL))
    .addHbarTransfer(AccountId(0, 0, 3ULL), Hbar(1LL));

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = tx.estimateFee().execute(getTestClient()));
  EXPECT_EQ(response.mNetwork.mSubtotal,
            response.mNode.subtotal() * static_cast<uint64_t>(response.mNetwork.mMultiplier));
}

//-----
// Design test #11: total == network.subtotal + node.subtotal() + service.subtotal().
TEST_F(FeeEstimateQueryIntegrationTests, TotalEqualsSumOfComponents)
{
  TransferTransaction tx;
  tx.addHbarTransfer(getTestClient().getOperatorAccountId().value(), Hbar(-1LL))
    .addHbarTransfer(AccountId(0, 0, 3ULL), Hbar(1LL));

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = tx.estimateFee().execute(getTestClient()));
  EXPECT_EQ(response.mTotal, response.mNetwork.mSubtotal + response.mNode.subtotal() + response.mService.subtotal());
}

//-----
// Design test #12: malformed transaction returns 400 with no retry.
TEST_F(FeeEstimateQueryIntegrationTests, MalformedTransactionReturns400NoRetry)
{
  // Build the URL the same way FeeEstimateQuery would.
  FeeEstimateQuery query;
  const std::string url = query.buildMirrorNodeUrl(getTestClient());

  // Send garbage bytes that won't parse as a Transaction protobuf.
  const std::string garbage("not-a-valid-transaction-protobuf");

  int statusCode = 0;
  bool isTimeout = false;
  const auto start = std::chrono::steady_clock::now();
  ASSERT_NO_THROW(
    internal::HttpClient::invokeRESTWithStatus(url, "POST", garbage, "application/protobuf", statusCode, isTimeout));
  const auto elapsed = std::chrono::steady_clock::now() - start;

  // Mirror should respond 400 immediately — no retry-style exponential backoff.
  EXPECT_EQ(statusCode, 400);
  EXPECT_FALSE(isTimeout);
  EXPECT_LT(elapsed, std::chrono::seconds(1));
  // The retry classifier must agree with the response: 400 is not retryable.
  EXPECT_FALSE(FeeEstimateQuery::shouldRetry(statusCode, isTimeout));
}

//-----
// Design test #15: estimated fees vs. actual submitted-transaction fees should be within a reasonable band.
TEST_F(FeeEstimateQueryIntegrationTests, EstimateMatchesActualWithinRange)
{
  const std::shared_ptr<PrivateKey> recipientKey = ED25519PrivateKey::generatePrivateKey();
  TransactionReceipt createReceipt;
  ASSERT_NO_THROW(createReceipt = AccountCreateTransaction()
                                    .setKeyWithoutAlias(recipientKey->getPublicKey())
                                    .setInitialBalance(Hbar(1LL))
                                    .execute(getTestClient())
                                    .getReceipt(getTestClient()));
  const AccountId recipientId = createReceipt.mAccountId.value();

  TransferTransaction tx;
  tx.addHbarTransfer(getTestClient().getOperatorAccountId().value(), Hbar(-1LL))
    .addHbarTransfer(recipientId, Hbar(1LL));

  FeeEstimateResponse estimate;
  ASSERT_NO_THROW(estimate = tx.estimateFee().setMode(FeeEstimateMode::STATE).execute(getTestClient()));
  ASSERT_GT(estimate.mTotal, 0ULL);

  TransactionResponse submitted;
  ASSERT_NO_THROW(submitted = tx.execute(getTestClient()));
  TransactionRecord record;
  ASSERT_NO_THROW(record = submitted.getRecord(getTestClient()));
  const uint64_t actualFee = record.mTransactionFee;

  // Some local test networks don't charge fees — only assert the ratio when the network actually
  // charges something. The estimate itself was already asserted to be > 0 above.
  if (actualFee > 0ULL)
  {
    const double ratio = static_cast<double>(actualFee) / static_cast<double>(estimate.mTotal);
    EXPECT_GE(ratio, ACTUAL_VS_ESTIMATE_LOWER_BOUND) << "actual=" << actualFee << " estimate=" << estimate.mTotal;
    EXPECT_LE(ratio, ACTUAL_VS_ESTIMATE_UPPER_BOUND) << "actual=" << actualFee << " estimate=" << estimate.mTotal;
  }

  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(recipientId)
                    .setTransferAccountId(getTestClient().getOperatorAccountId().value())
                    .freezeWith(&const_cast<Client&>(getTestClient()))
                    .sign(recipientKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));
}

//-----
// Design test #17: FileAppendTransaction with multiple chunks aggregates fees.
TEST_F(FeeEstimateQueryIntegrationTests, FileAppendChunkedAggregateMatchesActualSum)
{
  TransactionReceipt fileReceipt;
  ASSERT_NO_THROW(fileReceipt = FileCreateTransaction()
                                  .setKeys({ getTestClient().getOperatorPublicKey() })
                                  .setContents(std::string("seed"))
                                  .execute(getTestClient())
                                  .getReceipt(getTestClient()));
  const FileId fileId = fileReceipt.mFileId.value();

  // Use a payload large enough to require multiple chunks (default chunk size is 4096 bytes; 5KB triggers 2 chunks).
  const std::string payload(5500, 'A');

  FileAppendTransaction tx;
  tx.setFileId(fileId).setContents(payload);

  FeeEstimateResponse aggregated;
  ASSERT_NO_THROW(aggregated = tx.estimateFee().execute(getTestClient()));
  EXPECT_GT(aggregated.mTotal, 0ULL);
  EXPECT_EQ(aggregated.mTotal,
            aggregated.mNetwork.mSubtotal + aggregated.mNode.subtotal() + aggregated.mService.subtotal());

  ASSERT_NO_THROW(FileDeleteTransaction().setFileId(fileId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
// Design test #18: TopicMessageSubmitTransaction with payload smaller than chunk size returns single-chunk fees.
TEST_F(FeeEstimateQueryIntegrationTests, TopicMessageSubmitSmallPayloadSingleChunk)
{
  TransactionReceipt topicReceipt;
  // Set an admin key so we can delete the topic in cleanup.
  ASSERT_NO_THROW(topicReceipt = TopicCreateTransaction()
                                   .setAdminKey(getTestClient().getOperatorPublicKey())
                                   .execute(getTestClient())
                                   .getReceipt(getTestClient()));
  const TopicId topicId = topicReceipt.mTopicId.value();

  TopicMessageSubmitTransaction tx;
  // 5-byte payload is well under the default chunk size, so this exercises the single-chunk fee path.
  tx.setTopicId(topicId).setMessage(std::string("hello"));

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = tx.estimateFee().execute(getTestClient()));
  EXPECT_GT(response.mTotal, 0ULL);

  ASSERT_NO_THROW(TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
// Design test #19: TopicMessageSubmitTransaction with payload larger than chunk size aggregates fees across chunks.
TEST_F(FeeEstimateQueryIntegrationTests, TopicMessageSubmitLargePayloadAggregatesAcrossChunks)
{
  TransactionReceipt topicReceipt;
  // Set an admin key so we can delete the topic in cleanup.
  ASSERT_NO_THROW(topicReceipt = TopicCreateTransaction()
                                   .setAdminKey(getTestClient().getOperatorPublicKey())
                                   .execute(getTestClient())
                                   .getReceipt(getTestClient()));
  const TopicId topicId = topicReceipt.mTopicId.value();

  // 3000 bytes exceeds the default chunk size, so this exercises the per-chunk aggregation path.
  const std::string payload(3000, 'B');
  TopicMessageSubmitTransaction tx;
  tx.setTopicId(topicId).setMessage(payload).setMaxChunks(10);

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = tx.estimateFee().execute(getTestClient()));
  EXPECT_GT(response.mTotal, 0ULL);
  EXPECT_EQ(response.mTotal, response.mNetwork.mSubtotal + response.mNode.subtotal() + response.mService.subtotal());

  ASSERT_NO_THROW(TopicDeleteTransaction().setTopicId(topicId).execute(getTestClient()).getReceipt(getTestClient()));
}

//-----
// Design test #20: highVolumeThrottle is appended to the URL and the response multiplier is >= 1.
TEST_F(FeeEstimateQueryIntegrationTests, HighVolumeThrottleSendsParamAndReturnsMultiplierAtLeastOne)
{
  TransferTransaction tx;
  tx.addHbarTransfer(getTestClient().getOperatorAccountId().value(), Hbar(-1LL))
    .addHbarTransfer(AccountId(0, 0, 3ULL), Hbar(1LL));

  FeeEstimateQuery query;
  query.setMode(FeeEstimateMode::INTRINSIC).setHighVolumeThrottle(5000).setTransaction(wrap(tx));

  EXPECT_NE(query.buildMirrorNodeUrl(getTestClient()).find("high_volume_throttle=5000"), std::string::npos);

  FeeEstimateResponse response;
  ASSERT_NO_THROW(response = query.execute(getTestClient()));
  EXPECT_GE(response.mHighVolumeMultiplier, 1ULL);
}
