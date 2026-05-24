// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FEE_ESTIMATE_QUERY_H_
#define HIERO_SDK_CPP_FEE_ESTIMATE_QUERY_H_

#include "Defaults.h"
#include "FeeEstimateMode.h"
#include "FeeEstimateResponse.h"
#include "WrappedTransaction.h"

#include <cstdint>
#include <optional>
#include <string>

namespace proto
{
class Transaction;
}

namespace Hiero
{
class Client;

/**
 * FeeEstimateQuery allows users to estimate transaction fees by querying the mirror node REST endpoint
 * `POST /api/v1/network/fees` (HIP-1261). The transaction is automatically frozen if it is not already frozen
 * when execute() is called. For chunked transactions (FileAppendTransaction, TopicMessageSubmitTransaction),
 * fees are aggregated across all chunks per the HIP-1261 specification.
 */
class FeeEstimateQuery
{
public:
  FeeEstimateQuery() = default;

  /**
   * Execute the fee estimation query against the mirror node configured on the supplied client.
   *
   * @param client The Client to use for the query.
   * @return The FeeEstimateResponse containing the fee estimates.
   * @throws std::invalid_argument If no transaction has been set.
   * @throws IllegalStateException If the mirror network is unset or the request fails after the configured retries.
   */
  [[nodiscard]] FeeEstimateResponse execute(const Client& client);

  /**
   * Set the estimation mode (optional, defaults to INTRINSIC).
   */
  FeeEstimateQuery& setMode(FeeEstimateMode mode);

  /**
   * Get the current estimation mode.
   */
  [[nodiscard]] FeeEstimateMode getMode() const;

  /**
   * Set the transaction to estimate (required).
   */
  FeeEstimateQuery& setTransaction(const WrappedTransaction& transaction);

  /**
   * Get the current transaction. Returns nullptr if no transaction has been set.
   */
  [[nodiscard]] const WrappedTransaction* getTransaction() const;

  /**
   * Set the high-volume throttle utilization in basis points (0–10000, where 10000 = 100%). A value of 0
   * (the default) indicates no high-volume pricing simulation.
   *
   * @throws std::invalid_argument If throttle exceeds 10000.
   */
  FeeEstimateQuery& setHighVolumeThrottle(uint16_t throttle);

  /**
   * Get the current high-volume throttle utilization in basis points.
   */
  [[nodiscard]] uint16_t getHighVolumeThrottle() const;

  /**
   * Set the maximum number of retry attempts. Not part of the HIP-1261 design but kept for testability and
   * configurability of transient mirror node failures.
   */
  FeeEstimateQuery& setMaxAttempts(uint64_t maxAttempts);

  /**
   * Get the maximum number of retry attempts.
   */
  [[nodiscard]] uint64_t getMaxAttempts() const;

  /**
   * Build the mirror node REST URL that this query would target on the given client. Exposed for unit tests
   * that verify URL formation.
   */
  [[nodiscard]] std::string buildMirrorNodeUrl(const Client& client) const;

  /**
   * Decide whether the configured retry policy should retry on the given HTTP status / timeout signal.
   * Exposed for unit tests. Per HIP-1261: retry on 500, 503, or timeout; do not retry on 400 (or any 4xx).
   */
  [[nodiscard]] static bool shouldRetry(int statusCode, bool isTimeout);

  /**
   * Aggregate per-chunk fee estimates into a single response per the HIP-1261 chunk-transaction rule.
   * Exposed for unit tests; called internally for FileAppendTransaction and TopicMessageSubmitTransaction
   * when more than one chunk is required.
   */
  [[nodiscard]] static FeeEstimateResponse aggregateChunkResponses(const std::vector<FeeEstimateResponse>& chunks);

private:
  /**
   * POST the serialized transaction to the mirror node and parse the response. Implements the retry policy.
   */
  [[nodiscard]] FeeEstimateResponse callGetFeeEstimate(const Client& client, const proto::Transaction& protoTx);

  /**
   * Estimate fees for a single (non-chunked) transaction.
   */
  [[nodiscard]] FeeEstimateResponse estimateSingleTransaction(const Client& client);

  /**
   * Estimate fees for each chunk of a chunked transaction and aggregate the results.
   */
  [[nodiscard]] FeeEstimateResponse estimateChunkedTransaction(const Client& client,
                                                               std::vector<proto::Transaction> chunkProtos);

  FeeEstimateMode mMode = FeeEstimateMode::INTRINSIC;

  std::optional<WrappedTransaction> mTransaction;

  uint16_t mHighVolumeThrottle = 0;

  uint64_t mAttempt = 0;

  uint64_t mMaxAttempts = DEFAULT_MAX_ATTEMPTS;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_ESTIMATE_QUERY_H_
