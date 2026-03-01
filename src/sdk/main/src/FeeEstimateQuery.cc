// SPDX-License-Identifier: Apache-2.0
#include "FeeEstimateQuery.h"
#include "Client.h"
#include "FileAppendTransaction.h"
#include "TopicMessageSubmitTransaction.h"
#include "exceptions/IllegalStateException.h"
#include "impl/HttpClient.h"
#include "impl/MirrorNetwork.h"
#include "impl/Utilities.h"

#include <chrono>
#include <thread>

#include <nlohmann/json.hpp>
#include <services/transaction.pb.h>

namespace Hiero
{
//-----
FeeEstimateResponse FeeEstimateQuery::execute(const Client& client)
{
  // Validate mirror network is set
  const auto mirrorNetwork = client.getMirrorNetwork();
  if (mirrorNetwork.empty())
  {
    throw IllegalStateException("Mirror network is not set on the client");
  }

  // Get the wrapped transaction and freeze it if needed
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
const WrappedTransaction& FeeEstimateQuery::getTransaction() const
{
  return mTransaction;
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
  std::string lastError;

  // Reset attempt counter for each call
  mAttempt = 0;

  while (mAttempt < mMaxAttempts)
  {
    try
    {
      response = internal::HttpClient::invokeRESTWithStatus(url, "POST", txBytes, "application/protobuf", statusCode);

      if (statusCode == 200)
      {
        break;
      }

      if (!shouldRetry(statusCode))
      {
        throw IllegalStateException("Fee estimate API returned status " + std::to_string(statusCode) + ": " + response);
      }

      lastError = "Received status " + std::to_string(statusCode);
    }
    catch (const std::exception& e)
    {
      lastError = e.what();
      if (!shouldRetry(statusCode))
      {
        throw;
      }
    }

    // Calculate delay with exponential backoff: 250ms, 500ms, 1000ms, etc.
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

  // Parse JSON response
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
  // Build the protobuf Transaction from the WrappedTransaction
  std::unique_ptr<proto::Transaction> protoTx = mTransaction.toProtobufTransaction();

  if (!protoTx)
  {
    throw IllegalStateException("Failed to build protobuf transaction");
  }

  return callGetFeeEstimate(client, *protoTx);
}

//-----
FeeEstimateResponse FeeEstimateQuery::executeChunkedTransaction(const Client& client)
{
  // For chunked transactions, we need to estimate each chunk and aggregate the results
  // This is a simplified implementation - the full implementation would need to handle
  // FileAppendTransaction and TopicMessageSubmitTransaction separately

  FeeEstimateResponse aggregatedResponse;
  aggregatedResponse.mNodeFee = FeeEstimate{};
  aggregatedResponse.mServiceFee = FeeEstimate{};
  aggregatedResponse.mNetworkFee = NetworkFee{};
  aggregatedResponse.mNotes = {};

  // For now, just estimate as a single transaction
  // A full implementation would iterate through chunks
  return estimateSingleTransaction(client);
}

//-----
bool FeeEstimateQuery::shouldRetry(int statusCode) const
{
  // Retry on server errors (5xx) or rate limiting (429)
  if (statusCode >= 500 || statusCode == 429)
  {
    return true;
  }

  // Don't retry on client errors (4xx except 429)
  if (statusCode >= 400 && statusCode < 500)
  {
    return false;
  }

  // Retry on connection errors (represented by -1 or 0)
  if (statusCode <= 0)
  {
    return true;
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

  // Check if it's localhost
  const bool isLocalHost = (mirrorUrl.find("localhost") != std::string::npos) ||
                           (mirrorUrl.find("127.0.0.1") != std::string::npos);

  // Add http/https prefix if not present
  if (mirrorUrl.compare(0, 7, "http://") != 0 && mirrorUrl.compare(0, 8, "https://") != 0)
  {
    if (isLocalHost)
    {
      mirrorUrl = "http://" + mirrorUrl;
    }
    else
    {
      mirrorUrl = "https://" + mirrorUrl;
    }
  }

  // For localhost, use port 8084
  if (isLocalHost)
  {
    // Replace port if present or append default port
    const size_t portPos = mirrorUrl.rfind(':');
    if (portPos != std::string::npos && portPos > 7)
    {
      mirrorUrl = mirrorUrl.substr(0, portPos) + ":8084";
    }
  }

  // Build the full URL with mode parameter
  const std::string modeStr = gFeeEstimateModeToString.at(mMode);
  return mirrorUrl + "/api/v1/network/fees?mode=" + modeStr;
}

} // namespace Hiero
