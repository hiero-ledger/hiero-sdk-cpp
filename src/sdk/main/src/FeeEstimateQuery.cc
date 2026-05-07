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
#include <string_view>
#include <thread>

#include <nlohmann/json.hpp>
#include <services/transaction.pb.h>

namespace Hiero
{
namespace
{
constexpr uint16_t MAX_HIGH_VOLUME_THROTTLE = 10000;
constexpr int HTTP_OK = 200;
constexpr uint64_t BACKOFF_INITIAL_MS = 250;
constexpr uint64_t BACKOFF_CAP_MS = 8000;
constexpr uint16_t LOCAL_MIRROR_REST_PORT = 8084;
constexpr std::string_view HTTP_SCHEME = "http://";
constexpr std::string_view HTTPS_SCHEME = "https://";

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

//-----
// Result of a single mirror-node HTTP attempt. statusCode == 200 indicates success and `body` is the
// response body. Otherwise `errorMessage` describes why the attempt failed and `statusCode` /
// `isTimeout` drive the retry decision.
struct AttemptResult
{
  std::string body;
  int statusCode = 0;
  bool isTimeout = false;
  std::string errorMessage;
};

//-----
AttemptResult performSingleAttempt(const std::string& url, const std::string& txBytes)
{
  AttemptResult result;
  try
  {
    result.body = internal::HttpClient::invokeRESTWithStatus(
      url, "POST", txBytes, "application/protobuf", result.statusCode, result.isTimeout);
    if (result.statusCode != HTTP_OK)
    {
      result.errorMessage = "HTTP " + std::to_string(result.statusCode) + " - " + result.body;
    }
  }
  catch (const std::exception& e)
  {
    result.errorMessage = e.what();
  }
  return result;
}

//-----
FeeEstimateResponse parseFeeEstimateJson(const std::string& body)
{
  try
  {
    return FeeEstimateResponse::fromJson(nlohmann::json::parse(body));
  }
  catch (const nlohmann::json::exception& e)
  {
    throw IllegalStateException(std::string("Failed to parse fee estimate response: ") + e.what());
  }
}

//-----
void backoffSleep(uint64_t attempt)
{
  auto delayMs = static_cast<uint64_t>(static_cast<double>(BACKOFF_INITIAL_MS) * static_cast<double>(1ULL << attempt));
  if (delayMs > BACKOFF_CAP_MS)
  {
    delayMs = BACKOFF_CAP_MS;
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
}

//-----
bool hasHttpScheme(std::string_view url)
{
  // string_view::substr clamps n to the remaining length, so this is bounds-safe even when url is
  // shorter than the scheme prefix.
  return url.substr(0, HTTP_SCHEME.size()) == HTTP_SCHEME || url.substr(0, HTTPS_SCHEME.size()) == HTTPS_SCHEME;
}

//-----
bool isLocalHostUrl(std::string_view url)
{
  return url.find("localhost") != std::string_view::npos || url.find("127.0.0.1") != std::string_view::npos;
}

//-----
// Apply scheme prefix and (for local solo networks) rewrite the port to the mirror REST port 8084.
std::string normalizeMirrorBaseUrl(std::string_view mirrorUrl)
{
  constexpr std::string_view SCHEME_SEPARATOR = "://";
  const bool localHost = isLocalHostUrl(mirrorUrl);

  std::string url;
  if (!hasHttpScheme(mirrorUrl))
  {
    url.append(localHost ? HTTP_SCHEME : HTTPS_SCHEME);
  }
  url.append(mirrorUrl);

  if (localHost)
  {
    // Search for the host:port colon strictly after the scheme separator so we never accidentally
    // match the colon inside "://".
    const size_t schemeEnd = url.find(SCHEME_SEPARATOR);
    const size_t searchFrom = (schemeEnd == std::string::npos) ? 0 : schemeEnd + SCHEME_SEPARATOR.size();
    const size_t portPos = url.find(':', searchFrom);
    if (portPos != std::string::npos)
    {
      url.replace(portPos, std::string::npos, ":" + std::to_string(LOCAL_MIRROR_REST_PORT));
    }
  }
  return url;
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

  std::string lastError;
  for (mAttempt = 0; mAttempt < mMaxAttempts; ++mAttempt)
  {
    const AttemptResult attempt = performSingleAttempt(url, txBytes);
    if (attempt.statusCode == HTTP_OK)
    {
      return parseFeeEstimateJson(attempt.body);
    }
    if (!shouldRetry(attempt.statusCode, attempt.isTimeout))
    {
      throw IllegalStateException("Fee estimate API returned status " + std::to_string(attempt.statusCode) + ": " +
                                  attempt.errorMessage);
    }
    lastError = attempt.errorMessage;
    backoffSleep(mAttempt);
  }

  throw IllegalStateException("Failed to call fee estimate API after " + std::to_string(mMaxAttempts) +
                              " attempts: " + lastError);
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

  std::string url =
    normalizeMirrorBaseUrl(mirrorNetwork[0]) + "/api/v1/network/fees?mode=" + gFeeEstimateModeToString.at(mMode);
  if (mHighVolumeThrottle != 0)
  {
    url += "&high_volume_throttle=" + std::to_string(mHighVolumeThrottle);
  }
  return url;
}

} // namespace Hiero
