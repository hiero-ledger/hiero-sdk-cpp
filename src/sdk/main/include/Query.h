// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_QUERY_H_
#define HIERO_SDK_CPP_QUERY_H_

#include "Executable.h"

#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <optional>
#include <stdexcept>

namespace proto
{
class Query;
class QueryHeader;
class Response;
class ResponseHeader;
class Transaction;
}

namespace Hiero
{
class Client;
class Hbar;
class TransactionId;
}

namespace Hiero
{
/**
 * Base class for all queries that can be submitted to Hiero.
 *
 * @tparam SdkRequestType  The SDK request type.
 * @tparam SdkResponseType The SDK response type.
 */
template<typename SdkRequestType, typename SdkResponseType>
class Query : public Executable<SdkRequestType, proto::Query, proto::Response, SdkResponseType>
{
public:
  /**
   * Get the expected cost of this Query.
   *
   * @param client The Client to use to fetch the cost.
   * @return The expected cost of this Query.
   */
  [[nodiscard]] Hbar getCost(const Client& client);

  /**
   * Get the expected cost of this Query with a specific timeout.
   *
   * @param client  The Client to use to fetch the cost.
   * @param timeout The timeout to use to get the cost.
   * @return The expected cost of this Query.
   */
  [[nodiscard]] Hbar getCost(const Client& client, const std::chrono::system_clock::duration& timeout);

  /**
   * Get the expected cost of this Query asynchronously.
   *
   * @param client The Client to use to fetch the cost.
   * @return The expected cost of this Query.
   */
  [[nodiscard]] std::future<Hbar> getCostAsync(const Client& client);

  /**
   * Get the expected cost of this Query asynchronously with a specific timeout.
   *
   * @param client  The Client to use to fetch the cost.
   * @param timeout The timeout to use to get the cost.
   * @return The expected cost of this Query.
   */
  [[nodiscard]] std::future<Hbar> getCostAsync(const Client& client,
                                               const std::chrono::system_clock::duration& timeout);

  /**
   * Get the expected cost of this Query asynchronously and consume the response and/or exception with a callback.
   *
   * @param client   The Client to use to fetch the cost.
   * @param callback The callback that should consume the response/exception.
   */
  void getCostAsync(const Client& client, const std::function<void(const Hbar&, const std::exception&)>& callback);

  /**
   * Get the expected cost of this Query asynchronously with a specific timeout and consume the response and/or
   * exception with a callback.
   *
   * @param client   The Client to use to fetch the cost.
   * @param timeout  The timeout to use to get the cost.
   * @param callback The callback that should consume the response/exception.
   */
  void getCostAsync(const Client& client,
                    const std::chrono::system_clock::duration& timeout,
                    const std::function<void(const Hbar&, const std::exception&)>& callback);

  /**
   * Get the expected cost of this Query asynchronously and consume the response and/or exception with separate
   * callbacks.
   *
   * @param client            The Client to use to fetch the cost.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   */
  void getCostAsync(const Client& client,
                    const std::function<void(const Hbar&)>& responseCallback,
                    const std::function<void(const std::exception&)>& exceptionCallback);

  /**
   * Get the expected cost of this Query asynchronously with a specific timeout and consume the response and/or
   * exception with separate callbacks.
   *
   * @param client            The Client to use to fetch the cost.
   * @param timeout           The timeout to use to get the cost.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   */
  void getCostAsync(const Client& client,
                    const std::chrono::system_clock::duration& timeout,
                    const std::function<void(const Hbar&)>& responseCallback,
                    const std::function<void(const std::exception&)>& exceptionCallback);

  /**
   * Set an amount to pay for this Query. The Client will submit exactly this amount and no remainder will be returned.
   *
   * @param amount The amount to pay for this Query.
   * @return A reference to this derived Query with the newly-set payment.
   */
  SdkRequestType& setQueryPayment(const Hbar& amount);

  /**
   * Set a maximum amount to pay for this Query. A Query without an explicit payment amount set will first query for the
   * cost of the Query and attach a payment for that amount from the operator account on the client to the node account
   * ID. Set to 0 to disable automatic implicit payments.
   *
   * @param maxAmount The maximum amount to pay for this Query.
   * @return A reference to this derived Query with the newly-set payment.
   */
  SdkRequestType& setMaxQueryPayment(const Hbar& maxAmount);

  /**
   * Set the TransactionId of the payment Transaction for this Query.
   *
   * @param transactionId The TransactionId to set.
   * @return A reference to this derived Query with the newly-set payment transaction ID.
   */
  SdkRequestType& setPaymentTransactionId(const TransactionId& transactionId);

  /**
   * Get the TransactionId of the payment Transaction for this Query.
   *
   * @return The TransactionId of the payment Transaction for this Query. Uninitialized if no payment transaction ID has
   *         been set.
   */
  [[nodiscard]] std::optional<TransactionId> getPaymentTransactionId() const;

protected:
  /**
   * Prevent public copying and moving to prevent slicing. Use the 'clone()' virtual method instead.
   */
  Query();
  ~Query();
  Query(const Query&);
  Query& operator=(const Query&);
  Query(Query&&) noexcept;
  Query& operator=(Query&&) noexcept;

  /**
   * Get the cost of the Query from the ResponseHeader protobuf object and set it in this Query, if this Query was
   * configured to get the cost.
   *
   * @param header The ResponseHeader protobuf object from which to get the cost.
   */
  void saveCostFromHeader(const proto::ResponseHeader& header) const;

  /**
   * Is this Query meant a cost query?
   *
   * @return \c TRUE if this Query is meant to get the cost, otherwise \c FALSE.
   */
  [[nodiscard]] bool isCostQuery() const;

private:
  /**
   * Build a Query protobuf object with this Query's data, with the input QueryHeader protobuf object.
   *
   * @param header A pointer to the QueryHeader protobuf object to add to the Query protobuf object.
   * @return The constructed Query protobuf object.
   */
  [[nodiscard]] virtual proto::Query buildRequest(proto::QueryHeader* header) const = 0;

  /**
   * Get the ResponseHeader protobuf object from the input Response protobuf object.
   *
   * @param response The Response protobuf object from which to get the ResponseHeader protobuf object.
   * @return The ResponseHeader protobuf object of the input Response protobuf object for this derived Query.
   */
  [[nodiscard]] virtual proto::ResponseHeader mapResponseHeader(const proto::Response& response) const = 0;

  /**
   * Verify that all the checksums in this Transaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This Transaction's checksums are not valid.
   */
  virtual void validateChecksums(const Client& client) const = 0;

  /**
   * Derived from Executable. Construct a Query protobuf object from this Query, based on the node account ID at the
   * given index.
   *
   * @param index The index of the node account ID that's associated with the Node being used to execute this Query.
   * @return A Query protobuf object filled with this Query's data, based on the node account ID at the given index.
   */
  [[nodiscard]] proto::Query makeRequest(unsigned int index) const override;

  /**
   * Derived from Executable. Get the status response code from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to grab the status response code.
   * @return The status response code of the input Response protobuf object.
   */
  [[nodiscard]] Status mapResponseStatus(const proto::Response& response) const override;

  /**
   * Derived from Executable. Perform any needed actions for this Query when it is being submitted.
   *
   * @param client The Client being used to submit this Query.
   */
  void onExecute(const Client& client) override;

  /**
   * Derived from Executable. Get the ID of the payment transaction for this Query.
   *
   * @return The ID of the payment transaction of this Query.
   */
  [[nodiscard]] std::optional<TransactionId> getTransactionIdInternal() const override;

  /**
   * Does this Query require payment? Default to \c TRUE, as most Queries do.
   *
   * @return \c TRUE if this Query requires payment, otherwise \c FALSE.
   */
  [[nodiscard]] inline virtual bool isPaymentRequired() const { return true; }

  /**
   * Implementation object used to hide implementation details and internal headers.
   */
  struct QueryImpl;
  std::unique_ptr<QueryImpl> mImpl;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_QUERY_H_
