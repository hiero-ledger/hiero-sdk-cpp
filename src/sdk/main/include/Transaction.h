// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TRANSACTION_H_
#define HIERO_SDK_CPP_TRANSACTION_H_

#include "AccountId.h"
#include "Executable.h"
#include "Hbar.h"
#include "TransactionId.h"

#include <chrono>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace Hiero
{
class PrivateKey;
class TransactionResponse;
class ScheduleCreateTransaction;
class WrappedTransaction;
}

namespace proto
{
class SchedulableTransactionBody;
class SignedTransaction;
class Transaction;
class TransactionBody;
class TransactionResponse;
}

namespace Hiero
{
/**
 * Base class for all transactions that can be submitted to a Hiero network.
 *
 * @tparam SdkRequestType The SDK request type.
 */
template<typename SdkRequestType>
class Transaction
  : public Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>
{
public:
  /**
   * Construct a Transaction derived class from a byte array. The bytes can be a protobuf encoded TransactionList,
   * Transaction, or TransactionBody. Since C++ return types must be known at compile time and the type of Transaction
   * to create may not be known at compile time, a WrappedTransaction is used to encompass all possible Transactions in
   * a std::variant. Usage of this return type would look like the following:
   *
   * std::vector<std::byte> bytes = {...};
   *                                                                    The Transaction type here doesn't matter and is
   *                                           vvvvvvvvvvvvvvvvvvvvvvvv an unfortunate, ugly byproduct of this approach.
   * const WrappedTransaction tx = Transaction<AccountCreateTransaction>::fromBytes(bytes);
   *
   * switch (tx.getTransactionType())
   * {
   *    case TransactionType::ACCOUNT_CREATE_TRANSACTION:
   *    {
   *        AccountCreateTransaction* accountCreateTransaction = tx.getTransaction<AccountCreateTransaction>();
   *        ** do stuff with tx here **
   *        break;
   *    }
   *    case TransactionType::TRANSFER_TRANSACTION:
   *    {
   *        TransferTransaction* transferTransaction = tx.getTransaction<TransferTransaction>();
   *        ** do stuff with tx here **
   *        break;
   *    }
   *    ...
   * }
   *
   * @param bytes The bytes from which to construct a Transaction.
   * @return A WrappedTransaction which contains the deserialized Transaction.
   * @throws std::invalid_argument If unable to construct a Transaction from the input bytes.
   */
  [[nodiscard]] static WrappedTransaction fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a representative byte array from this Transaction object.
   *
   * @return The byte array representing this Transaction object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Sign this Transaction with the given PrivateKey. Signing a Transaction with a key that has already been used to
   * sign will be ignored.
   *
   * @param key The PrivateKey with which to sign this Transaction.
   * @return A reference to this derived Transaction object with the signature.
   * @throws IllegalStateException If this Transaction object is not frozen.
   * @throws std::invalid_argument If the signing key is not set.
   */
  SdkRequestType& sign(const std::shared_ptr<PrivateKey>& key);

  /**
   * Sign this Transaction with a given PublicKey and a signing callback. Signing a Transaction with a key that has
   * already been used to sign will be ignored.
   *
   * @param key    The PublicKey with which to sign this Transaction.
   * @param signer The callback function to use to sign this Transaction.
   * @return A reference to this derived Transaction object with the signature.
   * @throws IllegalStateException If this Transaction object is not frozen.
   */
  SdkRequestType& signWith(const std::shared_ptr<PublicKey>& key,
                           const std::function<std::vector<std::byte>(const std::vector<std::byte>&)>& signer);

  /**
   * Sign this Transaction with a configured Client. This will freeze this Transaction if it is not already frozen.
   *
   * @param client The Client with which to sign this Transaction.
   * @return A reference to this derived Transaction object with the client signature.
   * @throws UninitializedException If the input Client operator has not been initialized.
   */
  SdkRequestType& signWithOperator(const Client& client);

  /**
   * This method is used to mark a transaction as part of a batch transaction or make it so-called inner
   * transaction. The Transaction will be frozen and signed by the operator of the client.
   *
   * @param client The Client with which to sign this Transaction.
   * @param batchKey The batch key
   * @return A reference to this derived Transaction object.
   */
  SdkRequestType& batchify(const Client& client, const std::shared_ptr<Key>& batchKey);

  /**
   * Set the key that will sign the batch of which this Transaction is a part of.
   *
   * @param batchKey The batch key
   * @return A reference to this derived Transaction object.
   */
  SdkRequestType& setBatchKey(const std::shared_ptr<Key>& batchKey);

  /**
   * Get the key that will sign the batch of which this Transaction is a part of.
   *
   * @return The batch key of the Transaction.
   */
  [[nodiscard]] inline const std::shared_ptr<Key>& getBatchKey() { return mImpl->mBatchKey; }

  /**
   * Add a signature to this Transaction.
   *
   * @param publicKey The associated PublicKey of the PrivateKey that generated the signature.
   * @param signature The signature to add.
   * @return A reference to this derived Transaction object with the newly-added signature.
   * @throws IllegalStateException If there is not exactly one node account ID set or if this Transaction is not frozen.
   */
  virtual SdkRequestType& addSignature(const std::shared_ptr<PublicKey>& publicKey,
                                       const std::vector<std::byte>& signature);

  /**
   * Get the signatures of each potential Transaction protobuf object this Transaction may send.
   *
   * @return The map of node account IDs to their PublicKeys and signatures.
   */
  [[nodiscard]] virtual std::map<AccountId, std::map<std::shared_ptr<PublicKey>, std::vector<std::byte>>>
  getSignatures() const;

  /**
   * Freeze this Transaction.
   *
   * @return A reference to this derived Transaction object, now frozen.
   * @throws IllegalStateException If this Transaction's TransactionId and node account IDs have not been manually set.
   */
  SdkRequestType& freeze();

  /**
   * Freeze this transaction with a Client. The Client's operator will be used to generate a transaction ID, and the
   * Client's network will be used to generate a list of node account IDs.
   *
   * @param client A pointer to the Client with which to freeze this Transaction.
   * @return A reference to this derived Transaction object, now frozen.
   * @throws UninitializedException If the input Client operator has not been initialized.
   */
  SdkRequestType& freezeWith(const Client* client);

  /**
   * Put this Transaction in a ScheduleCreateTransaction.
   *
   * @return This Transaction put in a ScheduleCreateTransaction.
   * @throws IllegalStateException If this Transaction has node account IDs already set.
   */
  [[nodiscard]] ScheduleCreateTransaction schedule() const;

  /**
   * Get the SHA384 hash of this Transaction.
   *
   * @return The SHA384 hash of this Transaction.
   * @throws IllegalStateException If this Transaction is not frozen.
   */
  [[nodiscard]] virtual std::vector<std::byte> getTransactionHash() const;

  /**
   * Get the SHA384 hash of each potential Transaction protobuf object this Transaction may send.
   *
   * @return The map of node account IDs to the SHA384 hash of their Transaction.
   * @throws IllegalStateException If this Transaction is not frozen.
   */
  [[nodiscard]] virtual std::map<AccountId, std::vector<std::byte>> getTransactionHashPerNode() const;

  /**
   * Require that this Transaction has exactly one node account ID set.
   *
   * @throws IllegalStateException If there is not exactly one node account ID set.
   */
  void requireOneNodeAccountId() const;

  /**
   * Set the ID for this Transaction.
   *
   * @param id The desired transaction ID for this Transaction.
   * @return A reference to this derived Transaction object with the newly-set transaction ID.
   * @throws IllegalStateException If this Transaction is frozen.
   */
  SdkRequestType& setTransactionId(const TransactionId& id);

  /**
   * Derived from Executable. Set the desired account IDs of nodes to which this Transaction will be submitted. This is
   * not different from Executable::setNodeAccountIds() other than it checks to make sure the Transaction isn't frozen
   * and throws if it is.
   *
   * @param nodeAccountIds The desired list of account IDs of nodes to submit this Transaction.
   * @return A reference to this Executable derived class with the newly-set node account IDs.
   * @throws IllegalStateException If this Transaction is frozen.
   */
  SdkRequestType& setNodeAccountIds(std::vector<AccountId> nodeAccountIds) override;

  /**
   * Set the maximum transaction fee willing to be paid to execute this Transaction.
   *
   * @param fee The desired maximum transaction fee willing to be paid to execute this Transaction.
   * @return A reference to this derived Transaction object with the newly-set maximum transaction fee.
   * @throws IllegalStateException If this Transaction is frozen.
   */
  SdkRequestType& setMaxTransactionFee(const Hbar& fee);

  /**
   * Set the length of time that this Transaction will remain valid.
   *
   * @param duration The desired length of time to keep this Transaction valid.
   * @return A reference to this derived Transaction object with the newly-set valid duration.
   * @throws IllegalStateException If this Transaction is frozen.
   */
  SdkRequestType& setValidTransactionDuration(const std::chrono::system_clock::duration& duration);

  /**
   * Set the memo for this Transaction.
   *
   * @param memo The desired memo for this Transaction.
   * @return A reference to this derived Transaction object with the newly-set memo.
   * @throws IllegalStateException If this Transaction is frozen.
   */
  SdkRequestType& setTransactionMemo(const std::string& memo);

  /**
   * Set the transaction ID regeneration policy for this Transaction.
   *
   * @param regenerate \c TRUE if it is desired for this Transaction to regenerate a transaction ID upon receiving a
   *                   TRANSACTION_EXPIRED response from the network after submission, otherwise \c FALSE.
   * @return A reference to this derived Transaction object with the newly-set transaction ID regeneration policy.
   * @throws IllegalStateException If this Transaction is frozen.
   */
  SdkRequestType& setRegenerateTransactionIdPolicy(bool regenerate);

  /**
   * Get the ID of this Transaction.
   *
   * @return The ID of this Transaction.
   * @throws IllegalStateException If no TransactionId has been generated or set yet.
   */
  [[nodiscard]] TransactionId getTransactionId() const;

  /**
   * Get the maximum transaction fee willing to be paid to execute this Transaction. If not set manually, this
   * Transaction will use the max transaction fee set in the Client used to submit this Transaction upon execution.
   * If a Client max transaction fee has not been set, then the default max transaction fee will be used.
   *
   * @return The maximum transaction fee willing to be paid.
   */
  [[nodiscard]] Hbar getMaxTransactionFee() const;

  /**
   * Get the default maximum transaction fee. This can change between Transactions depending on their cost.
   *
   * @return The default maximum transaction fee.
   */
  [[nodiscard]] Hbar getDefaultMaxTransactionFee() const;

  /**
   * Get the desired length of time for this Transaction to remain valid upon submission. If not set manually,
   * DEFAULT_TRANSACTION_VALID_DURATION will be used.
   *
   * @return The length of time this Transaction will remain valid.
   */
  [[nodiscard]] std::chrono::system_clock::duration getValidTransactionDuration() const;

  /**
   * Get the memo for this Transaction.
   *
   * @return The memo for this Transaction.
   */
  [[nodiscard]] std::string getTransactionMemo() const;

  /**
   * Get the desired transaction ID regeneration policy of this Transaction.
   *
   * @return \c TRUE if this Transaction should regenerate its transaction ID upon receipt of a TRANSACTION_EXPIRED
   *         response from the network, \c FALSE if this Transaction shouldn't regenerate its transaction ID, and
   *         uninitialized if this Transaction will follow the default behavior.
   */
  [[nodiscard]] std::optional<bool> getRegenerateTransactionIdPolicy() const;

  /**
   * Get the size of the Transaction protobuf object for this Transaction.
   *
   * @return The byte size of the Transaction protobuf object for this Transaction.
   */
  [[nodiscard]] size_t getTransactionSize() const;

  /**
   * Get the size of the TransactionBody protobuf object for this Transaction.
   *
   * @return The byte size of the TransactionBody protobuf object for this Transaction.
   */
  [[nodiscard]] size_t getTransactionBodySize() const;

protected:
  /**
   * Dummy transaction and account IDs used to assist in deserializing incomplete Transactions.
   */
  static inline const AccountId DUMMY_ACCOUNT_ID = AccountId(0ULL, 0ULL, 0ULL);
  static inline const TransactionId DUMMY_TRANSACTION_ID =
    TransactionId::withValidStart(DUMMY_ACCOUNT_ID, std::chrono::system_clock::time_point());

  Transaction();
  ~Transaction();
  Transaction(const Transaction&);
  Transaction& operator=(const Transaction&);
  Transaction(Transaction&&) noexcept;
  Transaction& operator=(Transaction&&) noexcept;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param txBody The TransactionBody protobuf object from which to construct.
   */
  explicit Transaction(const proto::TransactionBody& txBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   * Assuming the input map isn't empty, this Transaction will set its values based on the values in the first
   * TransactionId -> [AccountId, proto::Transaction] mapping and ignore the rest.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit Transaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Derived from Executable. Construct a Transaction protobuf object from this Transaction, based on the node account
   * ID at the given index.
   *
   * @param index The index of the node account ID that's associated with the Node being used to execute this
   *              Transaction.
   * @return A Transaction protobuf object filled with this Transaction's data, based on the node account ID at the
   *         given index.
   */
  [[nodiscard]] proto::Transaction makeRequest(unsigned int index) const override;

  /**
   * Build all Transaction protobuf objects for this Transaction, each going to a different previously-selected node.
   */
  void buildAllTransactions() const;

  /**
   * Update mSourceTransactionBody. This will update all fields of mSourceTransactionBody except the transaction ID and
   * the node account ID.
   *
   * @param client A pointer to the Client that will sign and submit this Transaction.
   */
  void updateSourceTransactionBody(const Client* client) const;

  /**
   * Regenerate the SignedTransaction protobuf objects for this Transaction.
   *
   * @param client A pointer to the Client to use to regenerate the SignedTransaction protobuf objects.
   */
  virtual void regenerateSignedTransactions(const Client* client) const;

  /**
   * Add a Transaction or SignedTransaction protobuf object to this Transaction's Transaction or SignedTransaction
   * protobuf object list, respectively. If a Transaction protobuf object is being added, it will also parse a
   * SignedTransaction protobuf object from the Transaction protobuf object and add that SignedTransaction protobuf
   * object to this Transaction's SignedTransaction protobuf object list.
   *
   * @param transaction The Transaction or SignedTransaction protobuf object to add to this Transaction.
   */
  void addTransaction(const proto::Transaction& transaction) const;
  void addTransaction(const proto::SignedTransaction& transaction) const;

  /**
   * Add a SignedTransaction protobuf object created from the input TransactionBody protobuf object for each node
   * account ID of this Transaction. It is expected that every field in the input TransactionBody protobuf object is
   * valid except the node account ID field, which will be filled by the function.
   *
   * @param transaction The TransactionBody protobuf object from which to construct the SignedTransaction protobuf
   *                    objects.
   */
  void addSignedTransactionForEachNode(proto::TransactionBody transactionBody) const;

  /**
   * Clear the SignedTransaction and Transaction protobuf objects held by this Transaction.
   */
  virtual void clearTransactions() const;

  /**
   * Check and make sure this Transaction isn't frozen.
   *
   * @throws IllegalStateException If this Transaction is frozen.
   */
  void requireNotFrozen() const;

  /**
   * Is this Transaction frozen?
   *
   * @return \c TRUE if this Transaction is frozen, otherwise \c FALSE.
   */
  [[nodiscard]] bool isFrozen() const;

  /**
   * Set the default maximum transaction fee for this Transaction.
   *
   * @param fee The default maximum transaction fee for this Transaction.
   */
  void setDefaultMaxTransactionFee(const Hbar& fee);

  /**
   * Get the signatures of the Transaction protobuf object at the specified offset.
   *
   * @param offset The offset at which to grab the signatures.
   * @return The map of node account IDs to their PublicKeys and signatures for the specified offset.
   */
  [[nodiscard]] std::map<AccountId, std::map<std::shared_ptr<PublicKey>, std::vector<std::byte>>> getSignaturesInternal(
    size_t offset = 0ULL) const;

  /**
   * Get the Transaction protobuf object located at the given index in the Transaction protobuf object list.
   *
   * @param index The index at which to get the Transaction protobuf object.
   * @return The Transaction protobuf object located at the given index.
   */
  [[nodiscard]] proto::Transaction getTransactionProtobufObject(unsigned int index) const;

  /**
   * Get the source TransactionBody protobuf object from which this Transaction constructed itself.
   *
   * @return The source TransactionBody protobuf object from which this Transaction constructed itself.
   */
  [[nodiscard]] proto::TransactionBody getSourceTransactionBody() const;

  /**
   * Get the ID of this Transaction.
   *
   * @return The ID of this Transaction.
   */
  [[nodiscard]] virtual TransactionId getCurrentTransactionId() const;

private:
  friend class PrivateKey;

  /**
   * Build and add the derived Transaction's protobuf representation to the Transaction protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  virtual void addToBody(proto::TransactionBody& body) const = 0;

  /**
   * Verify that all the checksums in this Transaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This Transaction's checksums are not valid.
   */
  virtual void validateChecksums(const Client& client) const = 0;

  /**
   * Derived from Executable. Construct a TransactionResponse object from a TransactionResponse protobuf object.
   *
   * @param response The TransactionResponse protobuf object from which to construct a TransactionResponse object.
   * @return A TransactionResponse object filled with the TransactionResponse protobuf object's data.
   */
  [[nodiscard]] TransactionResponse mapResponse(const proto::TransactionResponse& response) const override;

  /**
   * Derived from Executable. Get the status response code for a submitted Transaction from a TransactionResponse
   * protobuf object.
   *
   * @param response The TransactionResponse protobuf object from which to grab the Transaction status response code.
   * @return The Transaction status response code of the input TransactionResponse protobuf object.
   */
  [[nodiscard]] Status mapResponseStatus(const proto::TransactionResponse& response) const override;

  /**
   * Derived from Executable. Determine the ExecutionStatus of this Transaction after being submitted.
   *
   * @param status   The response status from the network.
   * @param client   The Client that submitted this Transaction.
   * @param response The TransactionResponse protobuf object received from the network in response to submitting this
   *                 Transaction.
   * @return The status of the submitted Transaction.
   */
  [[nodiscard]]
  typename Executable<SdkRequestType, proto::Transaction, proto::TransactionResponse, TransactionResponse>::
    ExecutionStatus
    determineStatus(Status status,
                    const Client& client,
                    [[maybe_unused]] const proto::TransactionResponse& response) override;

  /**
   * Derived from Executable. Perform any needed actions for this Transaction when it is being submitted.
   *
   * @param client The Client being used to submit this Transaction.
   * @throws UninitializedException If the client doesn't have an AccountId from which to generate a TransactionId.
   */
  void onExecute(const Client& client) override;

  /**
   * Derived from Executable. Get the ID of this Transaction.
   *
   * @return The ID of this Transaction.
   */
  [[nodiscard]] std::optional<TransactionId> getTransactionIdInternal() const override;

  /**
   * Build a Transaction protobuf object from the SignedTransaction protobuf object at the specified index.
   *
   * @param index The index in the Transaction's SignedTransaction list from which the Transaction protobuf object
   *              should be built.
   */
  void buildTransaction(unsigned int index) const;

  /**
   * Determine if a PublicKey has already signed this Transaction.
   *
   * @param publicKey The PublicKey that could have already signed this Transaction.
   * @return \c TRUE if the input PublicKey has already signed this Transaction, otherwise \c FALSE.
   */
  [[nodiscard]] bool keyAlreadySigned(const std::shared_ptr<PublicKey>& publicKey) const;

  /**
   * Add a PublicKey and a signer function to this Transaction, and optionally the PrivateKey from which the signer
   * function was generated.
   *
   * @param publicKey  The PublicKey to add.
   * @param signer     The signer function to add.
   * @param privateKey The PrivateKey to add.
   * @return A reference to this derived Transaction object with the newly-set "signature(s)".
   */
  SdkRequestType& signInternal(const std::shared_ptr<PublicKey>& publicKey,
                               const std::function<std::vector<std::byte>(const std::vector<std::byte>&)>& signer,
                               const std::shared_ptr<PrivateKey>& privateKey = nullptr);

  /**
   * Implementation object used to hide implementation details and internal headers.
   */
  struct TransactionImpl;
  std::unique_ptr<TransactionImpl> mImpl;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TRANSACTION_H_