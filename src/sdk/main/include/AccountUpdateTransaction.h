// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ACCOUNT_UPDATE_TRANSACTION_H_
#define HIERO_SDK_CPP_ACCOUNT_UPDATE_TRANSACTION_H_

#include "AccountId.h"
#include "PublicKey.h"
#include "Transaction.h"

#include <chrono>
#include <memory>
#include <string>

namespace proto
{
class CryptoUpdateTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Change properties for the given account. Any null field is ignored (left unchanged). This transaction must be signed
 * by the existing key for this account. If the transaction is changing the key field, then the transaction must be
 * signed by both the old key (from before the change) and the new key. The old key must sign for security. The new key
 * must sign as a safeguard to avoid accidentally changing to an invalid key, and then having no way to recover. If the
 * update transaction sets the <tt>auto_renew_account</tt> field to anything other than the sentinel <tt>0.0.0</tt>, the
 * key of the referenced account must sign.
 */
class AccountUpdateTransaction : public Transaction<AccountUpdateTransaction>
{
public:
  AccountUpdateTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a CryptoUpdate transaction.
   */
  explicit AccountUpdateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit AccountUpdateTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the account to update.
   *
   * @param accountId The ID of the account this transaction should update.
   * @return A reference to this AccountUpdateTransaction object with the newly-set account ID.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& setAccountId(const AccountId& accountId);

  /**
   * Set a new key for the account. The key must sign each transfer out of the account. If mReceiverSignatureRequired is
   * true, then it must also sign any transfer into the account.
   *
   * @param publicKey The desired new key for the account.
   * @return A reference to this AccountUpdateTransaction object with the newly-set key.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& setKey(const std::shared_ptr<Key>& key);

  /**
   * Set a new transfer receiver signature policy for the account.
   *
   * @param receiveSignatureRequired \c TRUE to require the account to now sign any Hbar transfer transactions that
   *                                 involve transferring Hbars into itself, otherwise \c FALSE.
   * @return A reference to this AccountUpdateTransaction object with the newly-set receiver signature policy.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& setReceiverSignatureRequired(bool receiveSignatureRequired);

  /**
   * Set a new auto renew period for the account. A Hiero account is charged to extend its expiration date every
   * renew period. If it doesn't have enough balance, it extends as long as possible. If the balance is zero when it
   * expires, then the account is deleted.
   *
   * @param autoRenewPeriod The desired new auto renew period for the account.
   * @return A reference to this AccountUpdateTransaction object with the newly-set auto renew period.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& setAutoRenewPeriod(const std::chrono::system_clock::duration& autoRenewPeriod);

  /**
   * Set a new expiration time for the account.
   *
   * @param expiration The desired new expiration time for the account.
   * @return A reference to this AccountUpdateTransaction object with the newly-set expiration time.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& setExpirationTime(const std::chrono::system_clock::time_point& expiration);

  /**
   * Set a new memo for the account.
   *
   * @param memo The desired new memo for the account.
   * @return A reference to this AccountUpdateTransaction object with the newly-set memo.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& setAccountMemo(std::string_view memo);

  /**
   * Clear the memo for the account.
   *
   * @return A reference to this AccountUpdateTransaction object with the newly-cleared memo.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& clearAccountMemo();

  /**
   * Set the new maximum automatic token associations the account can have.
   *
   * @param associations The desired new maximum amount of token associations for the account.
   * @return A reference to this AccountUpdateTransaction object with the newly-set maximum automatic token
   *         associations.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& setMaxAutomaticTokenAssociations(int32_t associations);

  /**
   * Set the new account to which the account should stake. This is mutually exclusive with mStakedNodeId, and will
   * reset the value of the mStakedNodeId if it is set.
   *
   * @param stakedAccountId The ID of the desired new account to which the account will stake.
   * @return A reference to this AccountUpdateTransaction object with the newly-set staked account ID.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& setStakedAccountId(const AccountId& stakedAccountId);

  /**
   * Clear the account to which this account should stake.
   *
   * @return A reference to this AccountUpdateTransaction object with the newly-cleared staked account ID.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& clearStakedAccountId();

  /**
   * Set the new node to which the account should stake. This is mutually exclusive with mStakedAccountId, and will
   * reset the value of the mStakedAccountId if it is set.
   *
   * @param stakedNodeId The ID of the desired new node to which the account will stake.
   * @return A reference to this AccountUpdateTransaction object with the newly-set staked node ID.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& setStakedNodeId(const uint64_t& stakedNodeId);

  /**
   * Clear the node to which this account should stake.
   *
   * @return A reference to this AccountUpdateTransaction object with the newly-cleared staked node ID.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& clearStakedNodeId();

  /**
   * Set a new staking reward reception policy for the account.
   *
   * @param declineReward \c TRUE if the account should decline receiving staking rewards, otherwise \c FALSE.
   * @return A reference to this AccountUpdateTransaction object with the newly-set staking rewards reception policy.
   * @throws IllegalStateException If this AccountUpdateTransaction is frozen.
   */
  AccountUpdateTransaction& setDeclineStakingReward(bool declineReward);

  /**
   * Get the ID of the account to update.
   *
   * @return The ID of the account to update.
   */
  [[nodiscard]] inline AccountId getAccountId() const { return mAccountId; }

  /**
   * Get the new public key to be used for the account.
   *
   * @return A pointer to the new public key to be used for the account. Nullptr if the key has not yet been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getKey() const { return mKey; }

  /**
   * Get the new Hbar transfer receiver signature policy to be used by the account.
   *
   * @return \c TRUE if the new account should be required to sign all incoming Hbar transfers, otherwise \c FALSE.
   */
  [[nodiscard]] inline std::optional<bool> getReceiverSignatureRequired() const { return mReceiverSignatureRequired; }

  /**
   * Get the new auto renew period for the account.
   *
   * @return The new auto renew period for the new account.
   */
  [[nodiscard]] inline std::optional<std::chrono::system_clock::duration> getAutoRenewPeriod() const
  {
    return mAutoRenewPeriod;
  }

  /**
   * Get the new expiration time for the account.
   *
   * @return The new expiration time for the account.
   */
  [[nodiscard]] inline std::optional<std::chrono::system_clock::time_point> getExpirationTime() const
  {
    return mExpirationTime;
  }

  /**
   * Get the new memo for the account.
   *
   * @return The new memo for the account.
   */
  [[nodiscard]] inline std::optional<std::string> getAccountMemo() const { return mAccountMemo; }

  /**
   * Get the new maximum automatic token associations for the account.
   *
   * @return The new maximum automatic token associations for the account.
   */
  [[nodiscard]] inline std::optional<int32_t> getMaxAutomaticTokenAssociations() const
  {
    return mMaxAutomaticTokenAssociations;
  }

  /**
   * Get the ID of the new account to which the account will stake.
   *
   * @return The ID of the desired new account to which the account will stake. Returns uninitialized if a value has not
   *         yet been set, or if a staked node ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<AccountId> getStakedAccountId() const { return mStakedAccountId; }

  /**
   * Get the ID of the desired new node to which the account will stake.
   *
   * @return The ID of the desired new node to which the account will stake. Returns uninitialized if a value has not
   *         yet been set, or if a staked account ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<uint64_t> getStakedNodeId() const { return mStakedNodeId; }

  /**
   * Get the new staking rewards reception policy for the account.
   *
   * @return \c TRUE if the new account should decline from receiving staking rewards, otherwise \c FALSE
   */
  [[nodiscard]] inline std::optional<bool> getDeclineStakingReward() const { return mDeclineStakingReward; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this AccountUpdateTransaction's data
   * to a Node.
   *
   * @param request  The Transaction protobuf object to submit.
   * @param node     The Node to which to submit the request.
   * @param deadline The deadline for submitting the request.
   * @param response Pointer to the ProtoResponseType object that gRPC should populate with the response information
   *                 from the gRPC server.
   * @return The gRPC status of the submission.
   */
  [[nodiscard]] grpc::Status submitRequest(const proto::Transaction& request,
                                           const std::shared_ptr<internal::Node>& node,
                                           const std::chrono::system_clock::time_point& deadline,
                                           proto::TransactionResponse* response) const override;

  /**
   * Derived from Transaction. Verify that all the checksums in this AccountUpdateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This AccountUpdateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the AccountUpdateTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this AccountUpdateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a CryptoUpdateTransactionBody protobuf object from this AccountUpdateTransaction object.
   *
   * @return A pointer to a CryptoUpdateTransactionBody protobuf object filled with this AccountUpdateTransaction
   *         object's data.
   */
  [[nodiscard]] proto::CryptoUpdateTransactionBody* build() const;

  /**
   * The ID of the account to update.
   */
  AccountId mAccountId;

  /**
   * The new key to use to sign each transfer out of the account. If mReceiverSignatureRequired is \c TRUE, then it must
   * also sign any transfer into the account.
   */
  std::shared_ptr<Key> mKey = nullptr;

  /**
   * If \c TRUE, the account will have to sign any transaction being deposited into it (in addition to all withdrawals).
   */
  std::optional<bool> mReceiverSignatureRequired;

  /**
   * The new duration to use for the account to automatically extend its expiration period. It it doesn't have enough
   * balance, it extends as long as possible. If it is empty when it expires, then it is deleted.
   */
  std::optional<std::chrono::system_clock::duration> mAutoRenewPeriod;

  /**
   * The new expiration time to which to extend this account.
   */
  std::optional<std::chrono::system_clock::time_point> mExpirationTime;

  /**
   * The new memo to be associated with the account (UTF-8 encoding max 100 bytes).
   */
  std::optional<std::string> mAccountMemo;

  /**
   * The new maximum number of tokens with which the account can be implicitly associated. Only allows values up to a
   * maximum value of 5000. A value of -1 means the account can have unlimited token associations.
   */
  std::optional<int32_t> mMaxAutomaticTokenAssociations;

  /**
   * The ID of the new account to which this account will be staked. Mutually exclusive with mStakedNodeId.
   */
  std::optional<AccountId> mStakedAccountId;

  /**
   * The ID of the new node to which this account will be staked. Mutually exclusive with mStakedAccountId.
   */
  std::optional<uint64_t> mStakedNodeId;

  /**
   * If \c TRUE, the account will now decline receiving staking rewards.
   */
  std::optional<bool> mDeclineStakingReward;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ACCOUNT_UPDATE_TRANSACTION_H_
