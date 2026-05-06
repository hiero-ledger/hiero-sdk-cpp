// SPDX-License-Identifier: Apache-2.0
#include "FeeEstimateQuery.h"
#include "Client.h"
#include "FileAppendTransaction.h"
#include "TopicMessageSubmitTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "impl/HttpClient.h"
#include "impl/MirrorNetwork.h"

#include <algorithm>
#include <chrono>
#include <stdexcept>
#include <thread>

#include <nlohmann/json.hpp>
#include <services/transaction.pb.h>

namespace Hiero
{
namespace
{
constexpr uint16_t MAX_HIGH_VOLUME_THROTTLE = 10000;

//-----
// Generic auto-freeze: visit the variant and call freezeWith() on whichever transaction is held.
void freezeIfNeeded(WrappedTransaction& wrapped, const Client& client)
{
  std::visit(
    [&client](auto& tx)
    {
      if (!tx.isFrozen())
      {
        tx.freezeWith(&client);
      }
    },
    wrapped.getVariant());
}

} // namespace

//-----
FeeEstimateResponse FeeEstimateQuery::execute(const Client& client)
{
  if (!mTransaction.has_value())
  {
    throw std::invalid_argument("FeeEstimateQuery requires a transaction to be set via setTransaction()");
  }

  if (client.getMirrorNetwork().empty())
  {
    throw IllegalStateException("Mirror network is not set on the client");
  }

  freezeIfNeeded(*mTransaction, client);

  // Detect chunked transactions and dispatch to the per-chunk aggregation path when more than one chunk is
  // required.
  if (auto* fileAppend = mTransaction->getTransaction<FileAppendTransaction>())
  {
    if (fileAppend->getNumberOfChunksRequired() > 1)
    {
      return estimateChunkedTransaction(client, fileAppend->getChunkedTransactionProtobufObjects());
    }
  }
  else if (auto* topicSubmit = mTransaction->getTransaction<TopicMessageSubmitTransaction>())
  {
    if (topicSubmit->getNumberOfChunksRequired() > 1)
    {
      return estimateChunkedTransaction(client, topicSubmit->getChunkedTransactionProtobufObjects());
    }
  }

  return estimateSingleTransaction(client);
}

//-----
FeeEstimateQuery& FeeEstimateQuery::setMode(FeeEstimateMode mode)
{
  mMode = mode;
  return *this;
}

//-----
FeeEstimateMode FeeEstimateQuery::getMode() const
{
  return mMode;
}

//-----
FeeEstimateQuery& FeeEstimateQuery::setTransaction(const WrappedTransaction& transaction)
{
  mTransaction = transaction;
  return *this;
}

//-----
const WrappedTransaction* FeeEstimateQuery::getTransaction() const
{
  return mTransaction.has_value() ? &mTransaction.value() : nullptr;
}

//-----
FeeEstimateQuery& FeeEstimateQuery::setHighVolumeThrottle(uint16_t throttle)
{
  if (throttle > MAX_HIGH_VOLUME_THROTTLE)
  {
    throw std::invalid_argument("highVolumeThrottle must be in [0, 10000] basis points");
  }
  mHighVolumeThrottle = throttle;
  return *this;
}

//-----
uint16_t FeeEstimateQuery::getHighVolumeThrottle() const
{
  return mHighVolumeThrottle;
}

//-----
FeeEstimateQuery& FeeEstimateQuery::setMaxAttempts(uint64_t maxAttempts)
{
  mMaxAttempts = maxAttempts;
  return *this;
}

//-----
uint64_t FeeEstimateQuery::getMaxAttempts() const
{
  return mMaxAttempts;
}

//-----
FeeEstimateResponse FeeEstimateQuery::callGetFeeEstimate(const Client& client, const proto::Transaction& protoTx)
{
  const std::string url = buildMirrorNodeUrl(client);
  const std::string txBytes = protoTx.SerializeAsString();

  std::string response;
  int statusCode = 0;
  bool isTimeout = false;
  std::string lastError;

  mAttempt = 0;

  while (mAttempt < mMaxAttempts)
  {
    try
    {
      response =
        internal::HttpClient::invokeRESTWithStatus(url, "POST", txBytes, "application/protobuf", statusCode, isTimeout);

      if (statusCode == 200)
      {
        break;
      }

      if (!shouldRetry(statusCode, /*isTimeout=*/false))
      {
        throw IllegalStateException("Fee estimate API returned status " + std::to_string(statusCode) + ": " + response);
      }

      lastError = "Received status " + std::to_string(statusCode);
    }
    catch (const IllegalStateException&)
    {
      throw;
    }
    catch (const std::exception& e)
    {
      lastError = e.what();
      if (!shouldRetry(statusCode, isTimeout))
      {
        throw IllegalStateException(std::string("Fee estimate API failed: ") + e.what());
      }
    }

    // Exponential backoff capped at 8 seconds.
    auto delayMs = static_cast<uint64_t>(250.0 * static_cast<double>(1ULL << mAttempt));
    if (delayMs > 8000)
    {
      delayMs = 8000;
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
    ++mAttempt;
  }

  if (statusCode != 200)
  {
    throw IllegalStateException("Failed to call fee estimate API after " + std::to_string(mMaxAttempts) +
                                " attempts: " + lastError);
  }

  try
  {
    const nlohmann::json jsonResponse = nlohmann::json::parse(response);
    return FeeEstimateResponse::fromJson(jsonResponse);
  }
  catch (const nlohmann::json::exception& e)
  {
    throw IllegalStateException(std::string("Failed to parse fee estimate response: ") + e.what());
  }
}

//-----
FeeEstimateResponse FeeEstimateQuery::estimateSingleTransaction(const Client& client)
{
  std::unique_ptr<proto::Transaction> protoTx = mTransaction->toProtobufTransaction();
  if (!protoTx)
  {
    throw IllegalStateException("Failed to build protobuf transaction");
  }
  return callGetFeeEstimate(client, *protoTx);
}

//-----
FeeEstimateResponse FeeEstimateQuery::estimateChunkedTransaction(const Client& client,
                                                                 std::vector<proto::Transaction> chunkProtos)
{
  std::vector<FeeEstimateResponse> perChunk;
  perChunk.reserve(chunkProtos.size());
  for (const auto& chunk : chunkProtos)
  {
    perChunk.push_back(callGetFeeEstimate(client, chunk));
  }
  return aggregateChunkResponses(perChunk);
}

//-----
FeeEstimateResponse FeeEstimateQuery::aggregateChunkResponses(const std::vector<FeeEstimateResponse>& chunks)
{
  FeeEstimateResponse aggregated;
  if (chunks.empty())
  {
    return aggregated;
  }

  uint64_t aggregatedNodeTotal = 0;
  uint64_t aggregatedServiceTotal = 0;
  uint64_t maxHighVolumeMultiplier = 0;

  for (const auto& chunk : chunks)
  {
    aggregatedNodeTotal += chunk.mNode.subtotal();
    aggregatedServiceTotal += chunk.mService.subtotal();
    maxHighVolumeMultiplier = std::max(maxHighVolumeMultiplier, chunk.mHighVolumeMultiplier);

    aggregated.mNode.mExtras.insert(
      aggregated.mNode.mExtras.end(), chunk.mNode.mExtras.cbegin(), chunk.mNode.mExtras.cend());
    aggregated.mService.mExtras.insert(
      aggregated.mService.mExtras.end(), chunk.mService.mExtras.cbegin(), chunk.mService.mExtras.cend());
    aggregated.mNode.mBase += chunk.mNode.mBase;
    aggregated.mService.mBase += chunk.mService.mBase;
  }

  aggregated.mNetwork.mMultiplier = chunks.front().mNetwork.mMultiplier;
  aggregated.mNetwork.mSubtotal = aggregatedNodeTotal * aggregated.mNetwork.mMultiplier;
  aggregated.mHighVolumeMultiplier = maxHighVolumeMultiplier;
  aggregated.mTotal = aggregated.mNetwork.mSubtotal + aggregatedNodeTotal + aggregatedServiceTotal;

  return aggregated;
}

//-----
bool FeeEstimateQuery::shouldRetry(int statusCode, bool isTimeout)
{
  if (isTimeout)
  {
    return true;
  }

  // Retry only on 500 / 503 per the HIP-1261 retry policy.
  if (statusCode == 500 || statusCode == 503)
  {
    return true;
  }

  // Do not retry on 400 (or any other 4xx — malformed transaction body).
  if (statusCode >= 400 && statusCode < 500)
  {
    return false;
  }

  return false;
}

//-----
std::string FeeEstimateQuery::buildMirrorNodeUrl(const Client& client) const
{
  const auto mirrorNetwork = client.getMirrorNetwork();
  if (mirrorNetwork.empty())
  {
    throw IllegalStateException("Mirror network is not set");
  }

  std::string mirrorUrl = mirrorNetwork[0];

  const bool isLocalHost =
    (mirrorUrl.find("localhost") != std::string::npos) || (mirrorUrl.find("127.0.0.1") != std::string::npos);

  if (mirrorUrl.compare(0, 7, "http://") != 0 && mirrorUrl.compare(0, 8, "https://") != 0)
  {
    mirrorUrl = (isLocalHost ? "http://" : "https://") + mirrorUrl;
  }

  // For local solo networks the mirror node REST API listens on port 8084.
  if (isLocalHost)
  {
    const size_t portPos = mirrorUrl.rfind(':');
    if (portPos != std::string::npos && portPos > 7)
    {
      mirrorUrl = mirrorUrl.substr(0, portPos) + ":8084";
    }
  }

  std::string url = mirrorUrl + "/api/v1/network/fees?mode=" + gFeeEstimateModeToString.at(mMode);
  if (mHighVolumeThrottle != 0)
  {
    url += "&high_volume_throttle=" + std::to_string(mHighVolumeThrottle);
  }
  return url;
}

} // namespace Hiero
