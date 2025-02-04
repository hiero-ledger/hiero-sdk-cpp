// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TRANSACTION_ID_H_
#define HIERO_SDK_CPP_TRANSACTION_ID_H_

#include "AccountId.h"

#include <chrono>
#include <cstddef>
#include <functional>
#include <future>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class TransactionID;
}

namespace Hiero
{
class Client;
class TransactionReceipt;
class TransactionRecord;
}

namespace Hiero
{
/**
 * The ID for a transaction. This is used for retrieving receipts and records for a transaction, for appending to a file
 * right after creating it, for instantiating a smart contract with bytecode in a file just created, and internally by
 * the network for detecting when duplicate transactions are submitted. A user might get a transaction processed faster
 * by submitting it to N nodes, each with a different node account, but all with the same TransactionID. Then, the
 * transaction will take effect when the first of all those nodes submits the transaction and it reaches consensus. The
 * other transactions will not take effect. So this could make the transaction take effect faster, if any given node
 * might be slow. However, the full transaction fee is charged for each transaction, so the total fee is N times as much
 * if the transaction is sent to N nodes.
 *
 * Applicable to Scheduled Transactions:
 *  - The ID of a Scheduled Transaction has transactionValidStart and accountIDs inherited from the
 *    ScheduleCreate transaction that created it. That is to say that they are equal
 *  - The scheduled property is true for Scheduled Transactions
 *  - transactionValidStart, accountID and scheduled properties should be omitted
 */
class TransactionId
{
public:
  TransactionId() = default;

  /**
   * Generate a new TransactionId.
   *
   * @param accountId The ID of the account to be charged for the execution of the transaction with which this ID will
   *                  be associated.
   * @param start     The valid start time for the transaction.
   * @return A generated TransactionId to be used for any transaction submitted by the account with the input ID.
   */
  [[nodiscard]] static TransactionId withValidStart(const AccountId& accountId,
                                                    const std::chrono::system_clock::time_point& start);

  /**
   * Generate a new TransactionId.
   *
   * @param accountId The ID of the account to be charged for the execution of the transaction with which this ID will
   *                  be associated.
   * @return A generated TransactionId to be used for any transaction submitted by the account with the input ID.
   */
  [[nodiscard]] static TransactionId generate(const AccountId& accountId);

  /**
   * Construct a TransactionId object from a TransactionID protobuf object.
   *
   * @param proto The TransactionID protobuf object from which to construct a TransactionId object.
   * @return The constructed TransactionId object.
   */
  [[nodiscard]] static TransactionId fromProtobuf(const proto::TransactionID& proto);

  /**
   * Construct a TransactionId object from a string of the form "<account>@<seconds>.<nanos>[?scheduled][/nonce]".
   *
   * @param id The transaction ID string from which to construct.
   * @return The constructed TransactionId object.
   * @throws std::invalid_argument If the input string is malformed.
   */
  [[nodiscard]] static TransactionId fromString(std::string_view id);

  /**
   * Construct a TransactionId object from a byte array.
   *
   * @param bytes The byte array from which to construct a TransactionId object.
   * @return The constructed TransactionId object.
   */
  [[nodiscard]] static TransactionId fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Fetch the TransactionReceipt of the transaction this ID represents.
   *
   * @param client The Client to use to fetch the TransactionReceipt.
   * @return The fetched TransactionReceipt.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If the TransactionReceiptQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionReceipt getReceipt(const Client& client) const;

  /**
   * Fetch the TransactionReceipt of the transaction this ID represents with a specified timeout.
   *
   * @param client  The Client to use to fetch the TransactionReceipt.
   * @param timeout The desired timeout for the execution of the TransactionReceiptQuery.
   * @return The fetched TransactionReceipt.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If the TransactionReceiptQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionReceipt getReceipt(const Client& client, const std::chrono::system_clock::duration& timeout) const;

  /**
   * Fetch the TransactionReceipt of the transaction this ID represents asynchronously.
   *
   * @param client The Client to use to fetch the TransactionReceipt.
   * @return The future of the fetched TransactionReceipt.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If the TransactionReceiptQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<TransactionReceipt> getReceiptAsync(const Client& client) const;

  /**
   * Fetch the TransactionReceipt of the transaction this ID represents asynchronously with a specified timeout.
   *
   * @param client  The Client to use to fetch the TransactionReceipt.
   * @param timeout The desired timeout for the execution of the TransactionReceiptQuery.
   * @return The future of the fetched TransactionReceipt.
   * @throws MaxAttemptsExceededException If the TransactionReceiptQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If the TransactionReceiptQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<TransactionReceipt> getReceiptAsync(const Client& client,
                                                  const std::chrono::system_clock::duration& timeout) const;

  /**
   * Fetch the TransactionReceipt of the transaction this ID represents asynchronously and consume the response and/or
   * exception with a callback.
   *
   * @param client   The Client to use to fetch the TransactionReceipt.
   * @param callback The callback that should consume the response/exception.
   */
  void getReceiptAsync(const Client& client,
                       const std::function<void(const TransactionReceipt&, const std::exception&)>& callback) const;

  /**
   * Fetch the TransactionReceipt of the transaction this ID represents asynchronously with a specific timeout and
   * consume the response and/or exception with a callback.
   *
   * @param client   The Client to use to fetch the TransactionReceipt.
   * @param timeout  The desired timeout for the execution of the TransactionReceiptQuery.
   * @param callback The callback that should consume the response/exception.
   */
  void getReceiptAsync(const Client& client,
                       const std::chrono::system_clock::duration& timeout,
                       const std::function<void(const TransactionReceipt&, const std::exception&)>& callback) const;

  /**
   * Fetch the TransactionReceipt of the transaction this ID represents asynchronously and consume the response and/or
   * exception with separate callbacks.
   *
   * @param client            The Client to use to fetch the TransactionReceipt.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   */
  void getReceiptAsync(const Client& client,
                       const std::function<void(const TransactionReceipt&)>& responseCallback,
                       const std::function<void(const std::exception&)>& exceptionCallback) const;

  /**
   * Fetch the TransactionReceipt of the transaction this ID represents asynchronously with a specific timeout and
   * consume the response and/or exception with separate callbacks.
   *
   * @param client            The Client to use to fetch the TransactionReceipt.
   * @param timeout           The desired timeout for the execution of the TransactionReceiptQuery.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   */
  void getReceiptAsync(const Client& client,
                       const std::chrono::system_clock::duration& timeout,
                       const std::function<void(const TransactionReceipt&)>& responseCallback,
                       const std::function<void(const std::exception&)>& exceptionCallback) const;

  /**
   * Fetch the TransactionRecord of the transaction this ID represents.
   *
   * @param client The Client to use to fetch the TransactionRecord.
   * @return The fetched TransactionRecord.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If the TransactionRecordQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionRecord getRecord(const Client& client) const;

  /**
   * Fetch the TransactionRecord of the transaction this ID represents with a specified timeout.
   *
   * @param client  The Client to use to fetch the TransactionRecord.
   * @param timeout The desired timeout for the execution of the TransactionRecordQuery.
   * @return The fetched TransactionRecord.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If the TransactionReceiptQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionRecord getRecord(const Client& client, const std::chrono::system_clock::duration& timeout) const;

  /**
   * Fetch the TransactionRecord of the transaction this ID represents asynchronously.
   *
   * @param client The Client to use to fetch the TransactionRecord.
   * @return The future of the fetched TransactionRecord.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If the TransactionRecordQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<TransactionRecord> getRecordAsync(const Client& client) const;

  /**
   * Fetch the TransactionRecord of the transaction this ID represents asynchronously with a specified timeout.
   *
   * @param client  The Client to use to fetch the TransactionRecord.
   * @param timeout The desired timeout for the execution of the TransactionRecordQuery.
   * @return The future of the fetched TransactionRecord.
   * @throws MaxAttemptsExceededException If the TransactionRecordQuery attempts to execute past the number of
   *                                      allowable attempts.
   * @throws PrecheckStatusException      If the TransactionRecordQuery fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<TransactionRecord> getRecordAsync(const Client& client,
                                                const std::chrono::system_clock::duration& timeout) const;

  /**
   * Fetch the TransactionRecord of the transaction this ID represents asynchronously and consume the response and/or
   * exception with a callback.
   *
   * @param client   The Client to use to fetch the TransactionRecord.
   * @param callback The callback that should consume the response/exception.
   */
  void getRecordAsync(const Client& client,
                      const std::function<void(const TransactionRecord&, const std::exception&)>& callback) const;

  /**
   * Fetch the TransactionRecord of the transaction this ID represents asynchronously with a specific timeout and
   * consume the response and/or exception with a callback.
   *
   * @param client   The Client to use to fetch the TransactionRecord.
   * @param timeout  The desired timeout for the execution of the TransactionRecordQuery.
   * @param callback The callback that should consume the response/exception.
   */
  void getRecordAsync(const Client& client,
                      const std::chrono::system_clock::duration& timeout,
                      const std::function<void(const TransactionRecord&, const std::exception&)>& callback) const;

  /**
   * Fetch the TransactionRecord of the transaction this ID represents asynchronously and consume the response and/or
   * exception with separate callbacks.
   *
   * @param client            The Client to use to fetch the TransactionRecord.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   */
  void getRecordAsync(const Client& client,
                      const std::function<void(const TransactionRecord&)>& responseCallback,
                      const std::function<void(const std::exception&)>& exceptionCallback) const;

  /**
   * Fetch the TransactionRecord of the transaction this ID represents asynchronously with a specific timeout and
   * consume the response and/or exception with separate callbacks.
   *
   * @param client            The Client to use to fetch the TransactionRecord.
   * @param timeout           The desired timeout for the execution of the TransactionRecordQuery.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   */
  void getRecordAsync(const Client& client,
                      const std::chrono::system_clock::duration& timeout,
                      const std::function<void(const TransactionRecord&)>& responseCallback,
                      const std::function<void(const std::exception&)>& exceptionCallback) const;

  /**
   * Compare this TransactionId to another TransactionId and determine if they represent the same Transaction.
   *
   * @param other The other TransactionId with which to compare this TransactionId.
   * @return \c TRUE if this TransactionId is the same as the input TransactionId, otherwise \c FALSE.
   */
  bool operator==(const TransactionId& other) const;

  /**
   * Compare this TransactionId to another TransactionId and determine if they represent different Transactions.
   *
   * @param other The other TransactionId with which to compare this TransactionId.
   * @return \c TRUE if this TransactionId is different from the input TransactionId, otherwise \c FALSE.
   */
  bool operator!=(const TransactionId& other) const;

  /**
   * Construct a TransactionID protobuf object from this TransactionId object.
   *
   * @return A pointer to the created TransactionID protobuf object filled with this TransactionId object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::TransactionID> toProtobuf() const;

  /**
   * Get a string representation of this TransactionId object.
   *
   * @return A string representation of this TransactionId.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Get a string representation of this TransactionId object with the account ID checksum.
   *
   * @param client The Client with which to generate the checksum.
   * @return A string representation of this TransactionId with the account ID checksum.
   */
  [[nodiscard]] std::string toStringWithChecksum(const Client& client) const;

  /**
   * Get a byte array representation of this TransactionId object.
   *
   * @return A byte array representation of this TransactionId object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Set if this ID represents a scheduled transaction.
   *
   * @param scheduled \c TRUE if this ID represents a scheduled transaction, otherwise \c FALSE.
   * @return A reference to this TransactionId with the newly-set scheduled.
   */
  TransactionId& setScheduled(bool scheduled);

  /**
   * Set this TransactionId's nonce value.
   *
   * @param nonce The desired nonce value for this TransactionId.
   * @return A reference to this TransactionId with the newly-set nonce.
   */
  TransactionId& setNonce(int nonce);

  /**
   * Get if this ID represents a scheduled transaction.
   *
   * @return \c TRUE if this ID represents a scheduled transaction, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool getScheduled() const { return mScheduled; }

  /**
   * Get this TransactionId's nonce value.
   *
   * @return The nonce value for of TransactionId.
   */
  [[nodiscard]] inline int getNonce() const { return mNonce; }

  /**
   * The time at which the transaction associated with this TransactionId is considered "valid".
   *
   * When a transaction is submitted there is additionally a validDuration (defaults to 120s) and together they define a
   * time window in which the transaction may be processed.
   */
  std::chrono::system_clock::time_point mValidTransactionTime;

  /**
   * The ID of the account that is paying for this transaction associated with this TransactionId.
   */
  AccountId mAccountId;

private:
  /**
   * Construct with an AccountId and a valid transaction start time.
   *
   * @param accountId      The AccountId with which to construct the TransactionId.
   * @param validStartTime The valid start time with which to construct the Transactionid.
   */
  TransactionId(AccountId accountId, const std::chrono::system_clock::time_point& validStartTime);

  /**
   * Get the string with the timestamp, scheduled, and nonce.
   *
   * @return The string with this TransactionId's timestamp, scheduled, and nonce.
   */
  [[nodiscard]] std::string getTimestampScheduleNonceString() const;

  /**
   * Is this the ID of a scheduled transaction?
   */
  bool mScheduled = false;

  /**
   * The nonce value of this child transaction (0 if not a child transaction).
   */
  int mNonce = 0;
};

} // namespace Hiero

namespace std
{
template<>
struct hash<Hiero::TransactionId>
{
  /**
   * Operator override to enable use of TransactionId as map key.
   */
  size_t operator()(const Hiero::TransactionId& id) const { return hash<string>()(id.toString()); }
};

template<>
struct less<Hiero::TransactionId>
{
  /**
   * Operator override to enable use of TransactionId in a std::map, which requires fair ordering.
   */
  bool operator()(const Hiero::TransactionId& lhs, const Hiero::TransactionId& rhs) const
  {
    return lhs.toString() < rhs.toString();
  }
};

} // namespace std

#endif // HIERO_SDK_CPP_TRANSACTION_ID_H_
