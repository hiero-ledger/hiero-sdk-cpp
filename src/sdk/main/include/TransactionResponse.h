// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TRANSACTION_RESPONSE_H_
#define HIERO_SDK_CPP_TRANSACTION_RESPONSE_H_

#include "AccountId.h"
#include "TransactionId.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <string>
#include <vector>

namespace Hiero
{
class Client;
class TransactionReceipt;
class TransactionReceiptQuery;
class TransactionRecord;
class TransactionRecordQuery;
}

namespace Hiero
{
/**
 * When the client sends the node a transaction of any kind, the node replies with this, which simply says that the
 * transaction passed the pre-check (so the node will submit it to the network) or it failed (so it won't). To learn the
 * consensus result, the client should later obtain a receipt (free), or can buy a more detailed record (not free).
 */
class TransactionResponse
{
public:
  TransactionResponse() = default;

  /**
   * Construct with a node account ID, transaction ID, and transaction hash.
   *
   * @param nodeId The ID of the node account to which this TransactionResponse's corresponding Transaction was sent.
   * @param transactionId The ID of this TransactionResponse's corresponding Transaction.
   * @param hash The hash of this TransactionResponse's corresponding Transaction.
   */
  TransactionResponse(AccountId nodeId, TransactionId transactionId, std::vector<std::byte> hash);

  /**
   * Get a TransactionReceipt for this TransactionResponse's corresponding Transaction.
   *
   * @param client The Client to use to submit a TransactionReceiptQuery.
   * @return A TransactionReceipt object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionReceiptQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionReceipt is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionReceipt getReceipt(const Client& client) const;

  /**
   * Get a TransactionReceipt for this TransactionResponse's corresponding Transaction with a specified timeout.
   *
   * @param client  The Client to use to submit a TransactionReceiptQuery.
   * @param timeout The desired timeout for the execution of the TransactionReceiptQuery.
   * @return A TransactionReceipt object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionReceiptQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionReceipt is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionReceipt getReceipt(const Client& client, const std::chrono::system_clock::duration& timeout) const;

  /**
   * Construct a TransactionReceiptQuery for this TransactionResponse's corresponding Transaction.
   *
   * @return The constructed TransactionReceiptQuery.
   */
  [[nodiscard]] TransactionReceiptQuery getReceiptQuery() const;

  /**
   * Get a TransactionReceipt for this TransactionResponse's corresponding Transaction asynchronously.
   *
   * @param client The Client to use to submit a TransactionReceiptQuery.
   * @return A TransactionReceipt object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionReceiptQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionReceipt is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<TransactionReceipt> getReceiptAsync(const Client& client) const;

  /**
   * Get a TransactionReceipt for this TransactionResponse's corresponding Transaction asynchronously with a specified
   * timeout.
   *
   * @param client  The Client to use to submit a TransactionReceiptQuery.
   * @param timeout The desired timeout for the execution of the TransactionReceiptQuery.
   * @return A TransactionReceipt object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionReceiptQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionReceipt is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<TransactionReceipt> getReceiptAsync(const Client& client,
                                                  const std::chrono::system_clock::duration& timeout) const;

  /**
   * Get a TransactionReceipt for this TransactionResponse's corresponding Transaction asynchronously and consume the
   * response and/or exception with a callback.
   *
   * @param client   The Client to use to submit a TransactionReceiptQuery.
   * @param callback The callback that should consume the response/exception.
   * @return A TransactionReceipt object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionReceiptQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionReceipt is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  void getReceiptAsync(const Client& client,
                       const std::function<void(const TransactionReceipt&, const std::exception&)>& callback) const;

  /**
   * Get a TransactionReceipt for this TransactionResponse's corresponding Transaction asynchronously with a specified
   * timeout and consume the response and/or exception with a callback.
   *
   * @param client   The Client to use to submit a TransactionReceiptQuery.
   * @param timeout  The desired timeout for the execution of the TransactionReceiptQuery.
   * @param callback The callback that should consume the response/exception.
   * @return A TransactionReceipt object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionReceiptQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionReceipt is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  void getReceiptAsync(const Client& client,
                       const std::chrono::system_clock::duration& timeout,
                       const std::function<void(const TransactionReceipt&, const std::exception&)>& callback) const;

  /**
   * Get a TransactionReceipt for this TransactionResponse's corresponding Transaction asynchronously and consume the
   * response and/or exception with a callback.
   *
   * @param client            The Client to use to submit a TransactionReceiptQuery.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   * @return A TransactionReceipt object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionReceiptQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionReceipt is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  void getReceiptAsync(const Client& client,
                       const std::function<void(const TransactionReceipt&)>& responseCallback,
                       const std::function<void(const std::exception&)>& exceptionCallback) const;

  /**
   * Get a TransactionReceipt for this TransactionResponse's corresponding Transaction asynchronously with a specified
   * timeout and consume the response and/or exception with a callback.
   *
   * @param client            The Client to use to submit a TransactionReceiptQuery.
   * @param timeout           The desired timeout for the execution of the TransactionReceiptQuery.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   * @return A TransactionReceipt object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionReceiptQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionReceipt is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  void getReceiptAsync(const Client& client,
                       const std::chrono::system_clock::duration& timeout,
                       const std::function<void(const TransactionReceipt&)>& responseCallback,
                       const std::function<void(const std::exception&)>& exceptionCallback) const;

  /**
   * Get a TransactionRecord for this TransactionResponse's corresponding Transaction.
   *
   * @param client The Client to use to submit a TransactionRecordQuery.
   * @return A TransactionRecord object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionRecordQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  [[nodiscard]] TransactionRecord getRecord(const Client& client) const;

  /**
   * Get a TransactionRecord for this TransactionResponse's corresponding Transaction with a specified timeout.
   *
   * @param client  The Client to use to submit a TransactionRecordQuery.
   * @param timeout The desired timeout for the execution of the TransactionRecordQuery.
   * @return A TransactionRecord object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionRecordQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  [[nodiscard]] TransactionRecord getRecord(const Client& client,
                                            const std::chrono::system_clock::duration& timeout) const;

  /**
   * Construct a TransactionRecordQuery for this TransactionResponse's corresponding Transaction.
   *
   * @return The constructed TransactionRecordQuery.
   */
  [[nodiscard]] TransactionRecordQuery getRecordQuery() const;

  /**
   * Get a TransactionRecord for this TransactionResponse's corresponding Transaction asynchronously.
   *
   * @param client The Client to use to submit a TransactionRecordQuery.
   * @return A TransactionRecord object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionRecordQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionRecord is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<TransactionRecord> getRecordAsync(const Client& client) const;

  /**
   * Get a TransactionRecord for this TransactionResponse's corresponding Transaction asynchronously with a specified
   * timeout.
   *
   * @param client  The Client to use to submit a TransactionRecordQuery.
   * @param timeout The desired timeout for the execution of the TransactionRecordQuery.
   * @return A TransactionRecord object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionRecordQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionRecord is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<TransactionRecord> getRecordAsync(const Client& client,
                                                const std::chrono::system_clock::duration& timeout) const;

  /**
   * Get a TransactionRecord for this TransactionResponse's corresponding Transaction asynchronously and consume the
   * response and/or exception with a callback.
   *
   * @param client   The Client to use to submit a TransactionRecordQuery.
   * @param callback The callback that should consume the response/exception.
   * @return A TransactionRecord object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionRecordQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionRecord is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  void getRecordAsync(const Client& client,
                      const std::function<void(const TransactionRecord&, const std::exception&)>& callback) const;

  /**
   * Get a TransactionRecord for this TransactionResponse's corresponding Transaction asynchronously with a specified
   * timeout and consume the response and/or exception with a callback.
   *
   * @param client   The Client to use to submit a TransactionRecordQuery.
   * @param timeout  The desired timeout for the execution of the TransactionRecordQuery.
   * @param callback The callback that should consume the response/exception.
   * @return A TransactionRecord object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionRecordQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionRecord is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  void getRecordAsync(const Client& client,
                      const std::chrono::system_clock::duration& timeout,
                      const std::function<void(const TransactionRecord&, const std::exception&)>& callback) const;

  /**
   * Get a TransactionRecord for this TransactionResponse's corresponding Transaction asynchronously and consume the
   * response and/or exception with a callback.
   *
   * @param client            The Client to use to submit a TransactionRecordQuery.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   * @return A TransactionRecord object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionRecordQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionRecord is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  void getRecordAsync(const Client& client,
                      const std::function<void(const TransactionRecord&)>& responseCallback,
                      const std::function<void(const std::exception&)>& exceptionCallback) const;

  /**
   * Get a TransactionRecord for this TransactionResponse's corresponding Transaction asynchronously with a specified
   * timeout and consume the response and/or exception with a callback.
   *
   * @param client            The Client to use to submit a TransactionRecordQuery.
   * @param timeout           The desired timeout for the execution of the TransactionRecordQuery.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   * @return A TransactionRecord object containing data about this TransactionResponse's corresponding Transaction.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If this TransactionRecordQuery fails its pre-check.
   * @throws ReceiptStatusException       If the retrieved TransactionRecord is not successful, and this
   *                                      TransactionResponse is configured to throw.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  void getRecordAsync(const Client& client,
                      const std::chrono::system_clock::duration& timeout,
                      const std::function<void(const TransactionRecord&)>& responseCallback,
                      const std::function<void(const std::exception&)>& exceptionCallback) const;

  /**
   * Construct a string representation of this TransactionResponse object.
   *
   * @return The string representation of this TransactionResponse object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this TransactionResponse to an output stream.
   *
   * @param os       The output stream.
   * @param response The TransactionResponse to print.
   * @return The output stream with this TransactionResponse written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const TransactionResponse& response);

  /**
   * Set this TransactionResponse's TransactionReceipt validation policy.
   *
   * @param validate \c TRUE if this TransactionResponse should throw if it retrieves a TransactionReceipt that does not
   *                 have a successful status (i.e. Status::SUCCESS), otherwise \c FALSE.
   */
  TransactionResponse& setValidateStatus(bool validate);

  /**
   * Get this TransactionResponse's TransactionReceipt validation policy.
   *
   * @return \c TRUE if this TransactionResponse should throw if it retrieves a TransactionReceipt that does not have a
   *         successful status (i.e. Status::SUCCESS), otherwise \c FALSE.
   */
  [[nodiscard]] inline bool getValidateStatus() const { return mValidateStatus; }

  /**
   * The ID of node to which this TransactionResponse's corresponding Transaction was sent.
   */
  AccountId mNodeId;

  /**
   * The hash of this TransactionResponse's corresponding Transaction.
   */
  std::vector<std::byte> mTransactionHash;

  /**
   * The ID of this TransactionResponse's corresponding Transaction.
   */
  TransactionId mTransactionId;

private:
  /**
   * Did this TransactionResponse's corresponding Transaction have a successful pre-check?
   */
  bool mValidateStatus = true;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TRANSACTION_RESPONSE_H_
