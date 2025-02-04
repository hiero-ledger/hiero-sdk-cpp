// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ACCOUNT_CREATE_TRANSACTION_H_
#define HIERO_SDK_CPP_ACCOUNT_CREATE_TRANSACTION_H_

#include "AccountId.h"
#include "Defaults.h"
#include "EvmAddress.h"
#include "Hbar.h"
#include "Key.h"
#include "Transaction.h"

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace proto
{
class CryptoCreateTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * The transaction used to create a new account. After the account is created, the AccountID for it is in the receipt.
 * It can also be retrieved with a GetByKey query. Threshold values can be defined, and records are generated and stored
 * for 25 hours for any transfer that exceeds the thresholds. This account is charged for each record generated, so the
 * thresholds are useful for limiting record generation to happen only for large transactions.
 *
 * The mPublicKey field is the key used to sign transactions for this account. If the account has
 * mReceiverSignatureRequired set to true, then all cryptocurrency transfers must be signed by this account's key, both
 * for transfers in and out. If it is false, then only transfers out have to be signed by it. When the account is
 * created, the payer account is charged enough Hbars so that the new account will not expire for the next
 * mAutoRenewPeriod seconds. When it reaches the expiration time, the new account will then be automatically charged to
 * renew for another mAutoRenewPeriod seconds. If it does not have enough Hbars to renew for that long, then the
 * remaining Hbars are used to extend its expiration as long as possible. If it is has a zero balance when it expires,
 * then it is deleted. This transaction must be signed by the payer account. If the mAutoRenewPeriod field is set, the
 * key of the referenced account must sign.
 *
 * An account must be created in a particular realm. If the realmID is left null, then a new realm will be created with
 * the given admin key. If a new realm has a null adminKey, then anyone can create/modify/delete entities in that realm.
 * But if an admin key is given, then any transaction to create/modify/delete an entity in that realm must be signed by
 * that key, though anyone can still call functions on smart contract instances that exist in that realm. A realm ceases
 * to exist when everything within it has expired and no longer exists.
 *
 * The current API ignores shardID, realmID, and newRealmAdminKey, and creates everything in shard 0 and realm 0, with a
 * null key. Future versions of the API will support multiple realms and multiple shards.
 */
class AccountCreateTransaction : public Transaction<AccountCreateTransaction>
{
public:
  /**
   * Default constructor. Sets the maximum transaction fee to 5 Hbars.
   */
  AccountCreateTransaction();

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a CryptoCreateAccount transaction.
   */
  explicit AccountCreateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit AccountCreateTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the key for the new account. The key that must sign each transfer out of the account. If
   * mReceiverSignatureRequired is true, then it must also sign any transfer into the account.
   *
   * @param key The desired key for the new account.
   * @return A reference to this AccountCreateTransaction object with the newly-set key.
   * @throws IllegalStateException If this AccountCreateTransaction is frozen.
   */
  AccountCreateTransaction& setKey(const std::shared_ptr<Key>& key);

  /**
   * Set the initial amount to transfer into the new account from the paying account.
   *
   * @param initialBalance The desired balance to transfer into the new account.
   * @return A reference to this AccountCreateTransaction object with the newly-set initial balance.
   * @throws IllegalStateException If this AccountCreateTransaction is frozen.
   */
  AccountCreateTransaction& setInitialBalance(const Hbar& initialBalance);

  /**
   * Set the new account's transfer receiver signature policy. This requires the signature of the new account's key.
   *
   * @param receiveSignatureRequired \c TRUE to require the new account to sign any Hbar transfer transactions that
   *                                 involve transferring Hbars into itself, otherwise \c FALSE.
   * @return A reference to this AccountCreateTransaction object with the newly-set receiver signature policy.
   * @throws IllegalStateException If this AccountCreateTransaction is frozen.
   */
  AccountCreateTransaction& setReceiverSignatureRequired(bool receiveSignatureRequired);

  /**
   * Set the auto renew period for the new account. A Hiero account is charged to extend its expiration date every
   * renew period. If it doesn't have enough balance, it extends as long as possible. If the balance is zero when it
   * expires, then the account is deleted.
   *
   * @param autoRenewPeriod The desired auto renew period for the new account.
   * @return A reference to this AccountCreateTransaction object with the newly-set auto renew period.
   * @throws IllegalStateException If this AccountCreateTransaction is frozen.
   */
  AccountCreateTransaction& setAutoRenewPeriod(const std::chrono::system_clock::duration& autoRenewPeriod);

  /**
   * Set a memo for the new account.
   *
   * @param memo The desired memo for the new account.
   * @return A reference to this AccountCreateTransaction object with the newly-set memo.
   * @throws IllegalStateException If this AccountCreateTransaction is frozen.
   */
  AccountCreateTransaction& setAccountMemo(std::string_view memo);

  /**
   * Set the maximum automatic token associations the new account can have.
   *
   * @param associations The desired maximum amount of token associations for the new account. A value of -1 means
   *                     the new account can have unlimited automatic token associations.
   * @return A reference to this AccountCreateTransaction object with the newly-set maximum automatic token
   *         associations.
   * @throws IllegalStateException If this AccountCreateTransaction is frozen.
   */
  AccountCreateTransaction& setMaxAutomaticTokenAssociations(int32_t associations);

  /**
   * Set the account to which the new account should stake. This is mutually exclusive with mStakedNodeId, and will
   * reset the value of the mStakedNodeId if it is set.
   *
   * @param stakedAccountId The ID of the desired account to which the new account will stake.
   * @return A reference to this AccountCreateTransaction object with the newly-set staked account ID.
   * @throws IllegalStateException If this AccountCreateTransaction is frozen.
   */
  AccountCreateTransaction& setStakedAccountId(const AccountId& stakedAccountId);

  /**
   * Set the node to which the new account should stake. This is mutually exclusive with mStakedAccountId, and will
   * reset the value of the mStakedAccountId if it is set.
   *
   * @param stakedNodeId The ID of the desired node to which the new account will stake.
   * @return A reference to this AccountCreateTransaction object with the newly-set staked node ID.
   * @throws IllegalStateException If this AccountCreateTransaction is frozen.
   */
  AccountCreateTransaction& setStakedNodeId(const uint64_t& stakedNodeId);

  /**
   * Set the staking reward reception policy for the new account.
   *
   * @param declineReward \c TRUE if the new account should decline receiving staking rewards, otherwise \c FALSE.
   * @return A reference to this AccountCreateTransaction object with the newly-set staking rewards reception policy.
   * @throws IllegalStateException If this AccountCreateTransaction is frozen.
   */
  AccountCreateTransaction& setDeclineStakingReward(bool declineReward);

  /**
   * Set the EOA 20-byte address to create for the new account. This EVM address may be either the encoded form of the
   * <shard>.<realm>.<num>, or the keccak-256 hash of a ECDSA_SECP256K1 primitive key.
   *
   * @param address The desired EVM address to be used as the account's alias.
   * @return A reference to this AccountCreateTransaction object with the newly-set EVM address account alias.
   * @throws IllegalStateException If this AccountCreateTransaction is frozen.
   */
  AccountCreateTransaction& setAlias(const EvmAddress& address);

  /**
   * Get the key to be used for the new account.
   *
   * @return A pointer to the key to be used for the new account. Nullptr if the key has not yet been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getKey() const { return mKey; }

  /**
   * Get the initial balance to be transferred into the new account upon creation (from the paying account).
   *
   * @return The initial balance to be transferred into the new account upon creation (from the paying account).
   */
  [[nodiscard]] inline Hbar getInitialBalance() const { return mInitialBalance; }

  /**
   * Get the Hbar transfer receiver signature policy to be used by the new account.
   *
   * @return \c TRUE if the new account should be required to sign all incoming Hbar transfers, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool getReceiverSignatureRequired() const { return mReceiverSignatureRequired; }

  /**
   * Get the auto renew period for the new account.
   *
   * @return The auto renew period for the new account.
   */
  [[nodiscard]] inline std::chrono::system_clock::duration getAutoRenewPeriod() const { return mAutoRenewPeriod; }

  /**
   * Get the desired memo for the new account.
   *
   * @return The desired memo for the new account.
   */
  [[nodiscard]] inline std::string getAccountMemo() const { return mAccountMemo; }

  /**
   * Get the maximum automatic token associations for the new account.
   *
   * @return The maximum automatic token associations for the new account.
   */
  [[nodiscard]] inline int32_t getMaxAutomaticTokenAssociations() const { return mMaxAutomaticTokenAssociations; }

  /**
   * Get the ID of the account to which the new account will stake.
   *
   * @return The ID of the desired account to which the new account will stake. Returns uninitialized if a value has not
   *         yet been set, or if a staked node ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<AccountId> getStakedAccountId() const { return mStakedAccountId; }

  /**
   * Get the ID of the desired node to which the new account will stake.
   *
   * @return The ID of the desired node to which the new account will stake. Returns uninitialized if a value has not
   *         yet been set, or if a staked account ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<uint64_t> getStakedNodeId() const { return mStakedNodeId; }

  /**
   * Get the staking rewards reception policy for the new account.
   *
   * @return \c TRUE if the new account should decline from receiving staking rewards, otherwise \c FALSE
   */
  [[nodiscard]] inline bool getDeclineStakingReward() const { return mDeclineStakingReward; }

  /**
   * Get the EVM address of the new account.
   *
   * @return The EVM address of the new account. Returns uninitialized if a value has not yet been set.
   */
  [[nodiscard]] inline std::optional<EvmAddress> getAlias() const { return mAlias; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this AccountCreateTransaction's data
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
   * Derived from Transaction. Verify that all the checksums in this AccountCreateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This AccountCreateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the AccountCreateTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this AccountCreateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a CryptoCreateTransactionBody protobuf object from this AccountCreateTransaction object.
   *
   * @return A pointer to a CryptoCreateTransactionBody protobuf object filled with this AccountCreateTransaction
   *         object's data.
   */
  [[nodiscard]] proto::CryptoCreateTransactionBody* build() const;

  /**
   * The key that must sign each transfer out of the account. If mReceiverSignatureRequired is \c TRUE, then it must
   * also sign any transfer into the account.
   */
  std::shared_ptr<Key> mKey = nullptr;

  /**
   * The initial amount to transfer into the new account.
   */
  Hbar mInitialBalance = Hbar(0LL);

  /**
   * If \c TRUE, the new account's key must sign any transaction being deposited into it (in addition to all
   * withdrawals).
   */
  bool mReceiverSignatureRequired = false;

  /**
   * A Hiero account is charged to extend its expiration date every renew period. If it doesn't have enough balance, it
   * extends as long as possible. If the balance is zero when it expires, then the account is deleted. Defaults to 90
   * days (2160 hours).
   */
  std::chrono::system_clock::duration mAutoRenewPeriod = DEFAULT_AUTO_RENEW_PERIOD;

  /**
   * The memo to be associated with the account (UTF-8 encoding max 100 bytes).
   */
  std::string mAccountMemo;

  /**
   * The maximum number of tokens with which the new account can be implicitly associated. Only allows values up to a
   * maximum value of 5000. A value of -1 means the new account can have unlimited automatic token associations.
   */
  int32_t mMaxAutomaticTokenAssociations = 0;

  /**
   * The ID of the account to which the new account will be staked. Mutually exclusive with mStakedNodeId.
   */
  std::optional<AccountId> mStakedAccountId;

  /**
   * The ID of the node to which the new account will be staked. Mutually exclusive with mStakedAccountId.
   */
  std::optional<uint64_t> mStakedNodeId;

  /**
   * If \c TRUE, the new account will decline receiving staking rewards.
   */
  bool mDeclineStakingReward = false;

  /**
   * The EOA 20-byte address to create that is derived from the keccak-256 hash of a ECDSA_SECP256K1 primitive key.
   */
  std::optional<EvmAddress> mAlias;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ACCOUNT_CREATE_TRANSACTION_H_
