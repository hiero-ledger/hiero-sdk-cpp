// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FEE_ESTIMATE_QUERY_H_
#define HIERO_SDK_CPP_FEE_ESTIMATE_QUERY_H_

#include "Defaults.h"
#include "FeeEstimateMode.h"
#include "FeeEstimateResponse.h"
#include "WrappedTransaction.h"

#include <cstdint>
#include <memory>
#include <string>

namespace proto
{
class Transaction;
}

namespace Hiero
{
class Client;

/**
 * FeeEstimateQuery allows users to query expected transaction fees without submitting transactions to the network.
 */
class FeeEstimateQuery
{
public:
  /**
   * Default constructor.
   */
  FeeEstimateQuery() = default;

  /**
   * Execute the fee estimation query with the provided client.
   *
   * @param client The Client to use for the query.
   * @return The FeeEstimateResponse containing the fee estimates.
   * @throws std::invalid_argument If client is nullptr or no transaction is set.
   * @throws IllegalStateException If the mirror node is not set or an error occurs during the query.
   */
  [[nodiscard]] FeeEstimateResponse execute(const Client& client);

  /**
   * Set the estimation mode (optional, defaults to STATE).
   *
   * @param mode The FeeEstimateMode to use.
   * @return A reference to this FeeEstimateQuery.
   */
  FeeEstimateQuery& setMode(FeeEstimateMode mode);

  /**
   * Get the current estimation mode.
   *
   * @return The current FeeEstimateMode.
   */
  [[nodiscard]] FeeEstimateMode getMode() const;

  /**
   * Set the transaction to estimate (required).
   *
   * @param transaction The WrappedTransaction to estimate.
   * @return A reference to this FeeEstimateQuery.
   */
  FeeEstimateQuery& setTransaction(const WrappedTransaction& transaction);

  /**
   * Get the current transaction.
   *
   * @return The current WrappedTransaction.
   */
  [[nodiscard]] const WrappedTransaction& getTransaction() const;

  /**
   * Set the maximum number of retry attempts.
   *
   * @param maxAttempts The maximum number of retry attempts.
   * @return A reference to this FeeEstimateQuery.
   */
  FeeEstimateQuery& setMaxAttempts(uint64_t maxAttempts);

  /**
   * Get the maximum number of retry attempts.
   *
   * @return The maximum number of retry attempts.
   */
  [[nodiscard]] uint64_t getMaxAttempts() const;

private:
  /**
   * Call the fee estimate REST API endpoint.
   *
   * @param client The Client to use.
   * @param protoTx The protobuf Transaction object.
   * @return The FeeEstimateResponse.
   */
  [[nodiscard]] FeeEstimateResponse callGetFeeEstimate(const Client& client, const proto::Transaction& protoTx);

  /**
   * Estimate fees for a single transaction.
   *
   * @param client The Client to use.
   * @return The FeeEstimateResponse.
   */
  [[nodiscard]] FeeEstimateResponse estimateSingleTransaction(const Client& client);

  /**
   * Execute chunked transaction fee estimation (for FileAppend or TopicMessageSubmit).
   *
   * @param client The Client to use.
   * @return The aggregated FeeEstimateResponse.
   */
  [[nodiscard]] FeeEstimateResponse executeChunkedTransaction(const Client& client);

  /**
   * Determine if an error should trigger a retry.
   *
   * @param statusCode The HTTP status code.
   * @return true if should retry, false otherwise.
   */
  [[nodiscard]] bool shouldRetry(int statusCode) const;

  /**
   * Build the mirror node REST API URL.
   *
   * @param client The Client to use.
   * @return The URL string.
   */
  [[nodiscard]] std::string buildMirrorNodeUrl(const Client& client) const;

  /**
   * The estimation mode (defaults to STATE).
   */
  FeeEstimateMode mMode = FeeEstimateMode::STATE;

  /**
   * The transaction to estimate.
   */
  WrappedTransaction mTransaction;

  /**
   * The current retry attempt.
   */
  uint64_t mAttempt = 0;

  /**
   * The maximum number of retry attempts.
   */
  uint64_t mMaxAttempts = DEFAULT_MAX_ATTEMPTS;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_ESTIMATE_QUERY_H_
