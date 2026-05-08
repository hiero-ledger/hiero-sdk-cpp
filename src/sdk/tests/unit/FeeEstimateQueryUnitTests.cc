// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "Client.h"
#include "FeeEstimateMode.h"
#include "FeeEstimateQuery.h"
#include "FeeEstimateResponse.h"
#include "TransferTransaction.h"
#include "WrappedTransaction.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <stdexcept>

using namespace Hiero;

namespace
{
Client makeClientWithMirror()
{
  Client client = Client::forNetwork({});
  client.setMirrorNetwork({ "127.0.0.1:5600" });
  return client;
}

WrappedTransaction makeTransferTransaction()
{
  TransferTransaction tx;
  tx.addHbarTransfer(AccountId(0, 0, 1ULL), Hbar(-1));
  tx.addHbarTransfer(AccountId(0, 0, 2ULL), Hbar(1));
  return WrappedTransaction(WrappedTransaction::AnyPossibleTransaction(tx));
}
} // namespace

//-----
TEST(FeeEstimateQueryUnitTests, DefaultModeIsIntrinsic)
{
  FeeEstimateQuery query;
  EXPECT_EQ(query.getMode(), FeeEstimateMode::INTRINSIC);
}

//-----
TEST(FeeEstimateQueryUnitTests, SetAndGetMode)
{
  FeeEstimateQuery query;
  query.setMode(FeeEstimateMode::STATE);
  EXPECT_EQ(query.getMode(), FeeEstimateMode::STATE);
  query.setMode(FeeEstimateMode::INTRINSIC);
  EXPECT_EQ(query.getMode(), FeeEstimateMode::INTRINSIC);
}

//-----
TEST(FeeEstimateQueryUnitTests, SetAndGetTransaction)
{
  FeeEstimateQuery query;
  EXPECT_EQ(query.getTransaction(), nullptr);

  const WrappedTransaction wrapped = makeTransferTransaction();
  query.setTransaction(wrapped);
  ASSERT_NE(query.getTransaction(), nullptr);
}

//-----
TEST(FeeEstimateQueryUnitTests, SetAndGetHighVolumeThrottle)
{
  FeeEstimateQuery query;
  EXPECT_EQ(query.getHighVolumeThrottle(), 0);

  query.setHighVolumeThrottle(0);
  EXPECT_EQ(query.getHighVolumeThrottle(), 0);

  query.setHighVolumeThrottle(5000);
  EXPECT_EQ(query.getHighVolumeThrottle(), 5000);

  query.setHighVolumeThrottle(10000);
  EXPECT_EQ(query.getHighVolumeThrottle(), 10000);

  EXPECT_THROW(query.setHighVolumeThrottle(10001), std::invalid_argument);
}

//-----
TEST(FeeEstimateQueryUnitTests, ExecuteWithoutTransactionThrows)
{
  FeeEstimateQuery query;
  Client client = makeClientWithMirror();
  EXPECT_THROW(query.execute(client), std::invalid_argument);
}

//-----
TEST(FeeEstimateQueryUnitTests, BuildUrlContainsModeIntrinsic)
{
  FeeEstimateQuery query;
  Client client = makeClientWithMirror();
  const std::string url = query.buildMirrorNodeUrl(client);
  EXPECT_NE(url.find("/api/v1/network/fees"), std::string::npos);
  EXPECT_NE(url.find("mode=INTRINSIC"), std::string::npos);
  EXPECT_EQ(url.find("high_volume_throttle"), std::string::npos);
}

//-----
TEST(FeeEstimateQueryUnitTests, BuildUrlContainsModeState)
{
  FeeEstimateQuery query;
  query.setMode(FeeEstimateMode::STATE);
  Client client = makeClientWithMirror();
  const std::string url = query.buildMirrorNodeUrl(client);
  EXPECT_NE(url.find("mode=STATE"), std::string::npos);
}

//-----
TEST(FeeEstimateQueryUnitTests, BuildUrlIncludesHighVolumeThrottleWhenNonZero)
{
  FeeEstimateQuery query;
  query.setHighVolumeThrottle(5000);
  Client client = makeClientWithMirror();
  const std::string url = query.buildMirrorNodeUrl(client);
  EXPECT_NE(url.find("high_volume_throttle=5000"), std::string::npos);
}

//-----
TEST(FeeEstimateQueryUnitTests, BuildUrlOmitsHighVolumeThrottleWhenZero)
{
  FeeEstimateQuery query;
  query.setHighVolumeThrottle(0);
  Client client = makeClientWithMirror();
  EXPECT_EQ(query.buildMirrorNodeUrl(client).find("high_volume_throttle"), std::string::npos);
}

//-----
TEST(FeeEstimateQueryUnitTests, ShouldRetryReturnsTrueFor500And503AndTimeout)
{
  EXPECT_TRUE(FeeEstimateQuery::shouldRetry(500, false));
  EXPECT_TRUE(FeeEstimateQuery::shouldRetry(503, false));
  EXPECT_TRUE(FeeEstimateQuery::shouldRetry(0, true));
  EXPECT_TRUE(FeeEstimateQuery::shouldRetry(-1, true));
}

//-----
TEST(FeeEstimateQueryUnitTests, ShouldRetryReturnsFalseFor400AndOther4xx)
{
  EXPECT_FALSE(FeeEstimateQuery::shouldRetry(400, false));
  EXPECT_FALSE(FeeEstimateQuery::shouldRetry(401, false));
  EXPECT_FALSE(FeeEstimateQuery::shouldRetry(404, false));
  EXPECT_FALSE(FeeEstimateQuery::shouldRetry(429, false));
}

//-----
TEST(FeeEstimateQueryUnitTests, ParseFeeEstimateResponseFromJson)
{
  const auto json = nlohmann::json::parse(R"({
    "high_volume_multiplier": 1,
    "network": { "multiplier": 9, "subtotal": 900000 },
    "node": {
      "base": 100000,
      "extras": [
        { "name": "SIGNATURES", "included": 1, "count": 3, "charged": 2, "fee_per_unit": 1500, "subtotal": 3000 }
      ]
    },
    "service": {
      "base": 499000000,
      "extras": [
        { "name": "ACCOUNTS", "included": 0, "count": 2, "charged": 2, "fee_per_unit": 1000000, "subtotal": 2000000 }
      ]
    },
    "total": 501903000
  })");

  const FeeEstimateResponse response = FeeEstimateResponse::fromJson(json);

  EXPECT_EQ(response.mHighVolumeMultiplier, 1);
  EXPECT_EQ(response.mNetwork.mMultiplier, 9);
  EXPECT_EQ(response.mNetwork.mSubtotal, 900000);

  EXPECT_EQ(response.mNode.mBase, 100000);
  ASSERT_EQ(response.mNode.mExtras.size(), 1);
  EXPECT_EQ(response.mNode.mExtras[0].mName, "SIGNATURES");
  EXPECT_EQ(response.mNode.mExtras[0].mIncluded, 1);
  EXPECT_EQ(response.mNode.mExtras[0].mCount, 3);
  EXPECT_EQ(response.mNode.mExtras[0].mCharged, 2);
  EXPECT_EQ(response.mNode.mExtras[0].mFeePerUnit, 1500);
  EXPECT_EQ(response.mNode.mExtras[0].mSubtotal, 3000);

  EXPECT_EQ(response.mService.mBase, 499000000);
  ASSERT_EQ(response.mService.mExtras.size(), 1);
  EXPECT_EQ(response.mService.mExtras[0].mName, "ACCOUNTS");
  EXPECT_EQ(response.mService.mExtras[0].mSubtotal, 2000000);

  EXPECT_EQ(response.mTotal, 501903000);
}

//-----
TEST(FeeEstimateQueryUnitTests, FeeEstimateSubtotalSumsBaseAndExtras)
{
  FeeEstimate estimate;
  estimate.mBase = 1000;
  estimate.mExtras.push_back(FeeExtra{ "EXTRA_A", 0, 2, 2, 100, 200 });
  estimate.mExtras.push_back(FeeExtra{ "EXTRA_B", 0, 1, 1, 50, 50 });
  EXPECT_EQ(estimate.subtotal(), 1000 + 200 + 50);
}

//-----
TEST(FeeEstimateQueryUnitTests, ChunkAggregationFollowsHipFormula)
{
  // Build two synthetic per-chunk responses with simple, non-zero numbers.
  FeeEstimateResponse chunkA;
  chunkA.mHighVolumeMultiplier = 1;
  chunkA.mNetwork.mMultiplier = 9;
  chunkA.mNetwork.mSubtotal = 900; // ignored by aggregator (recomputed)
  chunkA.mNode.mBase = 100;
  chunkA.mNode.mExtras.push_back(FeeExtra{ "X", 0, 1, 1, 50, 50 });
  chunkA.mService.mBase = 200;
  chunkA.mService.mExtras.push_back(FeeExtra{ "Y", 0, 1, 1, 30, 30 });
  chunkA.mTotal = 0;

  FeeEstimateResponse chunkB = chunkA;
  chunkB.mNode.mBase = 80;
  chunkB.mService.mBase = 250;

  const FeeEstimateResponse aggregated = FeeEstimateQuery::aggregateChunkResponses({ chunkA, chunkB });

  const uint64_t aggregatedNodeTotal = chunkA.mNode.subtotal() + chunkB.mNode.subtotal();
  const uint64_t aggregatedServiceTotal = chunkA.mService.subtotal() + chunkB.mService.subtotal();

  EXPECT_EQ(aggregated.mNetwork.mMultiplier, 9);
  EXPECT_EQ(aggregated.mNetwork.mSubtotal, aggregatedNodeTotal * 9);
  EXPECT_EQ(aggregated.mNode.subtotal(), aggregatedNodeTotal);
  EXPECT_EQ(aggregated.mService.subtotal(), aggregatedServiceTotal);
  EXPECT_EQ(aggregated.mTotal, aggregated.mNetwork.mSubtotal + aggregatedNodeTotal + aggregatedServiceTotal);
  EXPECT_EQ(aggregated.mHighVolumeMultiplier, 1);
}

//-----
TEST(FeeEstimateQueryUnitTests, EstimateFeeOnTransactionReturnsQueryWithThatTransaction)
{
  TransferTransaction tx;
  tx.addHbarTransfer(AccountId(0, 0, 1ULL), Hbar(-1));
  tx.addHbarTransfer(AccountId(0, 0, 2ULL), Hbar(1));

  const FeeEstimateQuery query = tx.estimateFee();
  ASSERT_NE(query.getTransaction(), nullptr);
  EXPECT_NE(query.getTransaction()->getTransaction<TransferTransaction>(), nullptr);
  EXPECT_EQ(query.getMode(), FeeEstimateMode::INTRINSIC);
}
