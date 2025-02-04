// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_EXECUTABLE_H_
#define HIERO_SDK_CPP_EXECUTABLE_H_

#include "AccountId.h"
#include "Defaults.h"
#include "Logger.h"

#include <chrono>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <vector>

namespace Hiero
{
namespace internal
{
class Node;
}
class Client;
class TransactionId;
enum class Status;
}

namespace grpc
{
class Status;
}

namespace Hiero
{
/**
 * Base class for queries and transactions that contains the behavior for executing and submitting requests.
 *
 * @tparam SdkRequestType    The SDK request type.
 * @tparam ProtoRequestType  The protobuf request message type.
 * @tparam ProtoResponseType The protobuf response message type.
 * @tparam SdkResponseType   The SDK response type.
 */
template<typename SdkRequestType, typename ProtoRequestType, typename ProtoResponseType, typename SdkResponseType>
class Executable
{
public:
  /**
   * Submit this Executable to a Hiero network.
   *
   * @param client The Client to use to submit this Executable.
   * @return The SdkResponseType object sent from the Hiero network that contains the result of the request.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  virtual SdkResponseType execute(const Client& client);

  /**
   * Submit this Executable to a Hiero network with a specific timeout.
   *
   * @param client  The Client to use to submit this Executable.
   * @param timeout The desired timeout for the execution of this Executable.
   * @return The SdkResponseType object sent from the Hiero network that contains the result of the request.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  virtual SdkResponseType execute(const Client& client, const std::chrono::system_clock::duration& timeout);

  /**
   * Submit this Executable to a Hiero network asynchronously.
   *
   * @param client The Client to use to submit this Executable.
   * @return The future SdkResponseType object sent from the Hiero network that contains the result of the request.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<SdkResponseType> executeAsync(const Client& client);

  /**
   * Submit this Executable to a Hiero network asynchronously with a specific timeout.
   *
   * @param client  The Client to use to submit this Executable.
   * @param timeout The desired timeout for the execution of this Executable.
   * @return The future SdkResponseType object sent from the Hiero network that contains the result of the request.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  virtual std::future<SdkResponseType> executeAsync(const Client& client,
                                                    const std::chrono::system_clock::duration& timeout);

  /**
   * Submit this Executable to a Hiero network asynchronously and consume the response and/or exception with a
   * callback.
   *
   * @param client   The Client to use to submit this Executable.
   * @param callback The callback that should consume the response/exception.
   */
  void executeAsync(const Client& client,
                    const std::function<void(const SdkResponseType&, const std::exception&)>& callback);

  /**
   * Submit this Executable to a Hiero network asynchronously with a specific timeout and consume the response and/or
   * exception with a callback.
   *
   * @param client   The Client to use to submit this Executable.
   * @param timeout  The desired timeout for the execution of this Executable.
   * @param callback The callback that should consume the response/exception.
   */
  void executeAsync(const Client& client,
                    const std::chrono::system_clock::duration& timeout,
                    const std::function<void(const SdkResponseType&, const std::exception&)>& callback);

  /**
   * Submit this Executable to a Hiero network asynchronously and consume the response and/or exception with separate
   * callbacks.
   *
   * @param client            The Client to use to submit this Executable.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   */
  void executeAsync(const Client& client,
                    const std::function<void(const SdkResponseType&)>& responseCallback,
                    const std::function<void(const std::exception&)>& exceptionCallback);

  /**
   * Submit this Executable to a Hiero network asynchronously with a specific timeout and consume the response and/or
   * exception with separate callbacks.
   *
   * @param client            The Client to use to submit this Executable.
   * @param timeout           The desired timeout for the execution of this Executable.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   */
  void executeAsync(const Client& client,
                    const std::chrono::system_clock::duration& timeout,
                    const std::function<void(const SdkResponseType&)>& responseCallback,
                    const std::function<void(const std::exception&)>& exceptionCallback);

  /**
   * Set the desired account IDs of nodes to which this request will be submitted.
   *
   * @param nodeAccountIds The desired list of account IDs of nodes to submit this request.
   * @return A reference to this Executable derived class with the newly-set node account IDs.
   */
  virtual SdkRequestType& setNodeAccountIds(std::vector<AccountId> nodeAccountIds);

  /**
   * Set the Logger to be used by this Executable.
   *
   * @param logger The Logger to be used by this Executable.
   * @return A reference to this Executable derived class with the newly-set logger.
   */
  SdkRequestType& setLogger(const Logger& logger);

  /**
   * Set a callback to be called right before a request is sent. The callback will receive the request protobuf and the
   * callback should return the same request protobuf. The callback has an opportunity to read, copy, or modify the
   * request that will be sent.
   *
   * @param listener The request listener.
   * @return A reference to this Executable derived class with the newly-set request listener.
   */
  SdkRequestType& setRequestListener(const std::function<ProtoRequestType(ProtoRequestType&)>& listener);

  /**
   * Set a callback to be called right before a response is returned. The callback will receive the response protobuf
   * and the callback should return the same response protobuf. The callback has an opportunity to read, copy, or modify
   * the response that will be used.
   *
   * @param listener The response listener.
   * @return A reference to this Executable derived class with the newly-set response listener.
   */
  SdkRequestType& setResponseListener(const std::function<ProtoResponseType(ProtoResponseType&)>& listener);

  /**
   * Set the maximum number of times this Executable should try to resubmit itself after a failed attempt before it
   * considers itself a failure. This will override the maximum number of attempts of the Client used to submit this
   * Executable.
   *
   * @param attempts The desired maximum number of execution attempts.
   * @return A reference to this Executable derived class with the newly-set maximum number of execution attempts.
   */
  SdkRequestType& setMaxAttempts(uint32_t attempts);

  /**
   * Set the minimum amount of time a Node should wait after this Executable failed to execute before being willing to
   * submit this Executable again. This will override the minimum backoff time of the Client used to submit this
   * Executable.
   *
   * @param backoff The desired minimum amount of time this Executable should wait between retries.
   * @return A reference to this Executable derived class with the newly-set minimum backoff time.
   * @throws std::invalid_argument If the desired minimum backoff duration is longer than the set maximum backoff time
   *                               (DEFAULT_MAX_BACKOFF if the maximum backoff time has not been set).
   */
  SdkRequestType& setMinBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Set the maximum amount of time a Node should wait after this Executable failed to execute before being willing to
   * submit this Executable again. This will override the maximum backoff time of the Client used to submit this
   * Executable.
   *
   * @param backoff The desired maximum amount of time this Executable should wait between retries.
   * @return A reference to this Executable derived class with the newly-set maximum backoff time.
   * @throws std::invalid_argument If the desired maximum backoff duration is shorter than the set minimum backoff time
   *                               (DEFAULT_MIN_BACKOFF if the minimum backoff time has not been set).
   */
  SdkRequestType& setMaxBackoff(const std::chrono::system_clock::duration& backoff);

  /**
   * Set the maximum amount of time this Executable should spend trying to execute a request before giving up on that
   * request attempt. This will override the gRPC deadline of the Client used to submit this Executable.
   *
   * @param deadline The desired maximum amount of time this Executable should spend trying to execute one request.
   * @return A reference to this Executable derived class with the newly-set gRPC deadline time.
   */
  SdkRequestType& setGrpcDeadline(const std::chrono::system_clock::duration& deadline);

  /**
   * Get the list of account IDs for nodes with which execution will be attempted.
   *
   * @return The list of account IDs of nodes this Executable would attempt request submission.
   */
  [[nodiscard]] inline std::vector<AccountId> getNodeAccountIds() const { return mNodeAccountIds; }

  /**
   * Get the maximum number of times this Executable should try to resubmit itself after a failed attempt before it
   * considers itself a failure.
   *
   * @return The maximum number of execution attempts. Uninitialized value if not previously set.
   */
  [[nodiscard]] inline std::optional<uint32_t> getMaxAttempts() const { return mMaxAttempts; }

  /**
   * Get the minimum amount of time a Node should wait after this Executable failed to execute before being willing to
   * submit this Executable again.
   *
   * @return The minimum backoff time for Nodes for execution attempts of this Executable. Uninitialized value if not
   *         previously set.
   */
  [[nodiscard]] inline std::optional<std::chrono::system_clock::duration> getMinBackoff() const { return mMinBackoff; }

  /**
   * Get the maximum amount of time a Node should wait after this Executable failed to execute before being willing to
   * submit this Executable again.
   *
   * @return The maximum backoff time for Nodes for execution attempts of this Executable. Uninitialized value if not
   *         previously set.
   */
  [[nodiscard]] inline std::optional<std::chrono::system_clock::duration> getMaxBackoff() const { return mMaxBackoff; }

  /**
   * Set the maximum amount of time this Executable should spend trying to execute a request before giving up on that
   * request attempt.
   *
   * @return The maximum backoff time this Executable should spend on one submission attempt. Uninitialized value if not
   *         previously set.
   */
  [[nodiscard]] inline std::optional<std::chrono::system_clock::duration> getGrpcDeadline() const
  {
    return mGrpcDeadline;
  }

protected:
  Executable() = default;
  ~Executable() = default;
  Executable(const Executable&) = default;
  Executable& operator=(const Executable&) = default;
  Executable(Executable&&) noexcept = default;
  Executable& operator=(Executable&&) noexcept = default;

  /**
   * Enumeration describing the status of a submitted Executable.
   */
  enum class ExecutionStatus
  {
    /**
     * The request was successfully executed.
     */
    SUCCESS,
    /**
     * There was an error on the server side that shouldn't be retried.
     */
    SERVER_ERROR,
    /**
     * There was a user error with the request.
     */
    REQUEST_ERROR,
    /**
     * The call was successful but an operation did not complete.
     */
    RETRY
  };

  /**
   * Determine the ExecutionStatus of this Executable after being submitted.
   *
   * @param status   The response status from the network.
   * @param client   The Client that submitted this Executable.
   * @param response The ProtoResponseType received from the network in response to submitting this Executable.
   * @return The status of the submitted Executable.
   */
  [[nodiscard]] virtual ExecutionStatus determineStatus(Status status,
                                                        [[maybe_unused]] const Client& client,
                                                        [[maybe_unused]] const ProtoResponseType& response);

  /**
   * Gets the mirror node resolution for the query.
   *
   * This function returns the mirror node resolution by accessing the first element
   * of the vector mMirrorNodeIds.
   *
   * @return The mirror node resolution as a std::string.
   */
  [[nodiscard]] std::string getMirrorNodeResolution() const { return mMirrorNodeIds[0]; }

private:
  /**
   * Construct a ProtoRequestType object from this Executable, based on the node account ID at the given index.
   *
   * @param index The index of the node account ID that's associated with the Node being used to execute this
   *              Executable.
   * @return A ProtoRequestType object filled with this Executable's data, based on the node account ID at the given
   *         index.
   */
  [[nodiscard]] virtual ProtoRequestType makeRequest(unsigned int index) const = 0;

  /**
   * Construct an SdkResponseType from a ProtoResponseType object.
   *
   * @param response The ProtoResponseType object from which to construct an SdkResponseType object.
   * @return An SdkResponseType object filled with the ProtoResponseType object's data.
   */
  [[nodiscard]] virtual SdkResponseType mapResponse(const ProtoResponseType& response) const = 0;

  /**
   * Get the status response code from a ProtoResponseType object.
   *
   * @param response The ProtoResponseType object from which to grab the status response code.
   * @return The status response code of the input ProtoResponseType object.
   */
  [[nodiscard]] virtual Status mapResponseStatus(const ProtoResponseType& response) const = 0;

  /**
   * Submit a ProtoRequestType object which contains this Executable's data to a Node.
   *
   * @param request  The ProtoRequestType object to submit.
   * @param node     The Node to which to submit the request.
   * @param deadline The deadline for submitting the request.
   * @param response Pointer to the ProtoResponseType object that gRPC should populate with the response information
   *                 from the gRPC server.
   * @return The gRPC status of the submission.
   */
  [[nodiscard]] virtual grpc::Status submitRequest(const ProtoRequestType& request,
                                                   const std::shared_ptr<internal::Node>& node,
                                                   const std::chrono::system_clock::time_point& deadline,
                                                   ProtoResponseType* response) const = 0;

  /**
   * Perform any needed actions for this Executable when it is being submitted.
   *
   * @param client The Client being used to submit this Executable.
   */
  virtual void onExecute([[maybe_unused]] const Client& client) = 0;

  /**
   * Get the ID of the transaction for this Executable. For Queries, this is the ID of the payment transaction. For
   * Transactions, this is simply its own transaction ID.
   *
   * @return The ID of the transaction of this Executable.
   */
  [[nodiscard]] virtual std::optional<TransactionId> getTransactionIdInternal() const = 0;

  /**
   * Set the execution parameters to be used to submit this Executable. If any of mMaxAttempts, mMinBackoff, or
   * mMaxBackoff have been set with setMaxAttempts(), setMinBackoff(), or setMaxBackoff() respectively, these values
   * will be placed into mCurrentMaxAttempts, mCurrentMinBackoff, and mCurrentMaxBackoff respectively. If these values
   * have not been set for this Executable, the respective values for the Client will be grabbed and set instead. If
   * these values have not been set in the Client, the defaults are set.
   *
   * @param client The Client submitting this Executable.
   */
  void setExecutionParameters(const Client& client);

  /**
   * Get a list of Nodes that are on the input Client's Network that are being run by this Executable's node account
   * IDs.
   *
   * @param client The Client from which to get the list of Nodes.
   * @return A list of Nodes that are being run by this Executable's node account IDs.
   */
  [[nodiscard]] std::vector<std::shared_ptr<internal::Node>> getNodesFromNodeAccountIds(const Client& client) const;

  /**
   * Get the index of a Node from a list of Nodes to which to try and send this Executable. This will prioritize getting
   * "healthy" Nodes first in order to ensure as little wait time to submit as possible.
   *
   * @param nodes   The list of Nodes from which to select a Node.
   * @param attempt The attempt number of trying to submit this Executable.
   * @return A pointer to a Node to which to try and send this Executable.
   */
  [[nodiscard]] unsigned int getNodeIndexForExecute(const std::vector<std::shared_ptr<internal::Node>>& nodes,
                                                    unsigned int attempt) const;

  /**
   * The Logger to be used by this Executable.
   */
  Logger mLogger;

  /**
   * The list of account IDs of the nodes with which execution should be attempted.
   */
  std::vector<AccountId> mNodeAccountIds;

  /**
   * The list of node IDs of the mirror nodes with which query should be attempted.
   */
  std::vector<std::string> mMirrorNodeIds;

  /**
   * The callback to be called before a request is sent. The callback will receive the protobuf of the request and
   * return the protobuf of the request. This allows the callback to read, copy, and/or modify the request that will be
   * sent.
   */
  std::function<ProtoRequestType(ProtoRequestType&)> mRequestListener;

  /**
   * The callback to be called before a response is returned. The callback will receive the protobuf of the response and
   * return the protobuf of the response. This allows the callback to read, copy, and/or modify the response that will
   * be used.
   */
  std::function<ProtoResponseType(ProtoResponseType&)> mResponseListener;

  /**
   * The maximum number of attempts that will be made to submit this Executable. If not set, a submission will use the
   * Client's set maximum number of attempts. If that's not set, DEFAULT_MAX_ATTEMPTS will be used.
   */
  std::optional<uint32_t> mMaxAttempts;

  /**
   * The minimum amount of time to wait between submission attempts. If not set, a submission will use the Client's set
   * minimum backoff. If that's not set, DEFAULT_MIN_BACKOFF will be used.
   */
  std::optional<std::chrono::system_clock::duration> mMinBackoff;

  /**
   * The maximum amount of time to wait between submission attempts. If not set, a submission will use the Client's set
   * maximum backoff. If that's not set, DEFAULT_MAX_BACKOFF will be used.
   */
  std::optional<std::chrono::system_clock::duration> mMaxBackoff;

  /**
   * The timeout for one single execution attempt. If not set, a submission will use the Client's set gRPC deadline. If
   * that's not set, DEFAULT_GRPC_DEADLINE will be used.
   */
  std::optional<std::chrono::system_clock::duration> mGrpcDeadline;

  /**
   * The maximum number of attempts to be used for an execution. This may be this Executable's mMaxAttempts, the
   * Client's max attempts, or DEFAULT_MAX_ATTEMPTS.
   */
  uint32_t mCurrentMaxAttempts = 0;

  /**
   * The minimum backoff to be used for an execution. This may be this Executable's mMinBackoff, the Client's set
   * minimum backoff, or DEFAULT_MIN_BACKOFF.
   */
  std::chrono::system_clock::duration mCurrentMinBackoff = DEFAULT_MIN_BACKOFF;

  /**
   * The maximum backoff to be used for an execution. This may be this Executable's mMaxBackoff, the Client's set
   * maximum backoff, or DEFAULT_MAX_BACKOFF.
   */
  std::chrono::system_clock::duration mCurrentMaxBackoff = DEFAULT_MAX_BACKOFF;

  /**
   * The current backoff time being used during the current execution. Every failed submission attempt waits a certain
   * amount of time that is double the previous amount of time this Executable waited for its previous submission
   * attempt, up to the specified maximum backoff time, at which point the execution is considered a failure.
   */
  std::chrono::system_clock::duration mCurrentBackoff = DEFAULT_MIN_BACKOFF;

  /**
   * The current gRPC deadline being used for the current execution. This may be the Executable's mGrpcDeadline, the
   * Client's set gRPC deadline, or DEFAULT_GRPC_DEADLINE.
   */
  std::chrono::system_clock::duration mCurrentGrpcDeadline = DEFAULT_GRPC_DEADLINE;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_EXECUTABLE_H_
