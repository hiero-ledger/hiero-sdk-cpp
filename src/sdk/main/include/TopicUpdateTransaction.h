// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOPIC_UPDATE_TRANSACTION_H_
#define HIERO_SDK_CPP_TOPIC_UPDATE_TRANSACTION_H_

#include "AccountId.h"
#include "CustomFixedFee.h"
#include "Key.h"
#include "TopicId.h"
#include "Transaction.h"

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace proto
{
class ConsensusUpdateTopicTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that updates the properties of an existing topic. This includes the topic memo, admin key, submit key,
 * auto-renew account, and auto-renew period.
 *
 * Transaction Signing Requirements:
 *  - If an admin key is updated, the transaction must be signed by the pre-update admin key and post-update admin key.
 *  - If an admin key was set during the creation of the topic, the admin key must sign the transaction to update any
 *    of the topic's properties.
 *  - If no adminKey was defined during the creation of the topic, you can only extend the expirationTime.
 */
class TopicUpdateTransaction : public Transaction<TopicUpdateTransaction>
{
public:
  TopicUpdateTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a ConsensusUpdateTopic transaction.
   */
  explicit TopicUpdateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TopicUpdateTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the topic to update.
   *
   * @param topicId The ID of the topic to update.
   * @return A reference to this TopicUpdateTransaction object with the newly-set topic ID.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& setTopicId(const TopicId& topicId);

  /**
   * Set a new memo for the topic.
   *
   * @param memo The new memo for the topic.
   * @return A reference to this TopicUpdateTransaction object with the newly-set memo.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& setMemo(std::string_view memo);

  /**
   * Set a new expiration time for the topic.
   *
   * @param expiry The new expiration time for the topic.
   * @return A reference to this TopicUpdateTransaction object with the newly-set expiration time.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& setExpirationTime(const std::chrono::system_clock::time_point& expiry);

  /**
   * Set a new admin key for the topic.
   *
   * @param key The new admin key for the topic.
   * @return A reference to this TopicUpdateTransaction object with the newly-set admin key.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& setAdminKey(const std::shared_ptr<Key>& key);

  /**
   * Set a new submit key for the topic.
   *
   * @param key The new submit key for the topic.
   * @return A reference to this TopicUpdateTransaction object with the newly-set submit key.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& setSubmitKey(const std::shared_ptr<Key>& key);

  /**
   * Set a new auto-renew period for the topic.
   *
   * @param autoRenew The new auto-renew period for the topic.
   * @return A reference to this TopicUpdateTransaction object with the newly-set auto-renew period.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& setAutoRenewPeriod(const std::chrono::system_clock::duration& autoRenew);

  /**
   * Set the ID of a new auto-renew account for the topic.
   *
   * @param accountId The ID of the new auto-renew account for the topic.
   * @return A reference to this TopicUpdateTransaction object with the newly-set auto-renew account ID.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& setAutoRenewAccountId(const AccountId& accountId);

  /**
   * Set the new fee schedule key for the topic.
   *
   * @param key The new fee schedule key.
   * @return A reference to this TopicUpdateTransaction object with the newly-set fee schedule key.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& setFeeScheduleKey(const std::shared_ptr<Key>& key);

  /**
   * Set the list of fee-exempt keys for the topic.
   *
   * @param keys The list of fee-exempt keys.
   * @return A reference to this TopicUpdateTransaction object with the newly-set fee-exempt keys.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& setFeeExemptKeys(const std::vector<std::shared_ptr<Key>>& keys);

  /**
   * Add a key to the list of fee-exempt keys for the topic.
   *
   * @param key The key to add to the exempt list.
   * @return A reference to this TopicUpdateTransaction object with the newly-added exempt key.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& addFeeExemptKey(const std::shared_ptr<Key>& key);

  /**
   * Set the custom fixed fees for the topic.
   *
   * @param fees The new custom fixed fees.
   * @return A reference to this TopicUpdateTransaction object with the newly-set custom fixed fees.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& setCustomFixedFees(const std::vector<CustomFixedFee>& fees);

  /**
   * Add a custom fixed fee to the topic.
   *
   * @param fee The custom fixed fee to add.
   * @return A reference to this TopicUpdateTransaction object with the newly-added custom fixed fee.
   * @throws IllegalStateException If this TopicUpdateTransaction is frozen.
   */
  TopicUpdateTransaction& addCustomFixedFee(const CustomFixedFee& fee);

  /**
   * Clear the memo for the topic.
   *
   * @return A reference to this TokenUpdateTransaction with the newly-cleared memo.
   */
  TopicUpdateTransaction& clearTopicMemo();

  /**
   * Clear the admin key for the topic.
   *
   * @return A reference to this TokenUpdateTransaction with the newly-cleared admin key.
   */
  TopicUpdateTransaction& clearAdminKey();

  /**
   * Clear the submit key for the topic.
   *
   * @return A reference to this TokenUpdateTransaction with the newly-cleared submit key.
   */
  TopicUpdateTransaction& clearSubmitKey();

  /**
   * Clear the auto-renew account for the topic.
   *
   * @return A reference to this TokenUpdateTransaction with the newly-cleared auto-renew account ID.
   */
  TopicUpdateTransaction& clearAutoRenewAccountId();

  /**
   * Get the ID of the topic to update.
   *
   * @return The ID of the topic to update.
   */
  [[nodiscard]] inline TopicId getTopicId() const { return mTopicId; }

  /**
   * Get the new memo for the topic.
   *
   * @return The new memo for the topic. Returns uninitialized if the memo has not been set.
   */
  [[nodiscard]] inline std::optional<std::string> getMemo() const { return mMemo; }

  /**
   * Get the new expiration time for the topic.
   *
   * @return The new expiration time for the topic. Returns uninitialized if the expiration time has not been set.
   */
  [[nodiscard]] inline std::optional<std::chrono::system_clock::time_point> getExpirationTime() const
  {
    return mExpirationTime;
  }

  /**
   * Get the new admin key for the topic.
   *
   * @return The new admin key for the topic. Returns nullptr if the submit key has not been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getAdminKey() const { return mAdminKey; }

  /**
   * Get the new submit key for the topic.
   *
   * @return The new submit key for the topic. Returns nullptr if the submit key has not been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getSubmitKey() const { return mSubmitKey; }

  /**
   * Get the desired fee schedule key for the new topic.
   *
   * @return The fee schedule key for the new topic.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getFeeScheduleKey() const { return mFeeScheduleKey; }

  /**
   * Get the list of fee-exempt keys for the new topic.
   *
   * @return The list of fee-exempt keys.
   */
  [[nodiscard]] inline std::vector<std::shared_ptr<Key>> getFeeExemptKeys() const { return mFeeExemptKeys; }

  /**
   * Get the list of custom fixed fees for the new topic.
   *
   * @return The list of custom fixed fees.
   */
  [[nodiscard]] inline std::vector<CustomFixedFee> getCustomFixedFees() const { return mCustomFixedFees; }

  /**
   * Get the new auto-renew period for the topic.
   *
   * @return The new auto-renew period for the topic. Returns uninitialized if the auto-renew period has not been set.
   */
  [[nodiscard]] inline std::optional<std::chrono::system_clock::duration> getAutoRenewPeriod() const
  {
    return mAutoRenewPeriod;
  }

  /**
   * Get the ID of the new auto-renew account for the topic.
   *
   * @return The ID of the new auto-renew account for the topic. Returns uninitialized if the auto-renew account has not
   *         been set.
   */
  [[nodiscard]] inline std::optional<AccountId> getAutoRenewAccountId() const { return mAutoRenewAccountId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TopicUpdateTransaction's data to
   * a Node.
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
   * Derived from Transaction. Verify that all the checksums in this TopicUpdateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TopicUpdateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TopicUpdateTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TopicUpdateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a ConsensusUpdateTopicTransactionBody protobuf object from this TopicUpdateTransaction object.
   *
   * @return A pointer to a ConsensusUpdateTopicTransactionBody protobuf object filled with this TopicUpdateTransaction
   *         object's data.
   */
  [[nodiscard]] proto::ConsensusUpdateTopicTransactionBody* build() const;

  /**
   * The ID of the topic to update.
   */
  TopicId mTopicId;

  /**
   * The new publicly visible memo for the topic.
   */
  std::optional<std::string> mMemo;

  /**
   * The new expiration time for the topic.
   */
  std::optional<std::chrono::system_clock::time_point> mExpirationTime;

  /**
   * The new key to be used for access control to update or delete the topic.
   */
  std::shared_ptr<Key> mAdminKey = nullptr;

  /**
   * The new key that must sign to submit a message to the topic (via a TopicMessageSubmitTransaction).
   */
  std::shared_ptr<Key> mSubmitKey = nullptr;

  /**
   * The new amount of time by which to attempt to extend the topic's lifetime automatically at its expiration time.
   */
  std::optional<std::chrono::system_clock::duration> mAutoRenewPeriod;

  /**
   * The ID of the new account that should be charged to extend the lifetime of the topic at its expiration time.
   */
  std::optional<AccountId> mAutoRenewAccountId;

  /**
   * Access control for update or delete of custom fees.
   * If set, subsequent `consensus_update_topic` transactions signed with this
   * key MAY update or delete the custom fees for this topic.
   * If not set, the custom fees for this topic SHALL BE immutable.
   * If not set when the topic is created, this field CANNOT be set via update.
   * If set when the topic is created, this field MAY be changed via update.
   */
  std::shared_ptr<Key> mFeeScheduleKey = nullptr;

  /**
   * A set of keys.
   * Keys in this list are permitted to submit messages to this topic without
   * paying custom fees associated with this topic.
   * If a submit transaction is signed by _any_ key included in this set,
   * custom fees SHALL NOT be charged for that transaction.
   * This field MUST NOT contain more than 10 keys.
   * fee_exempt_key_list SHALL NOT contain any duplicate keys.
   * fee_exempt_key_list MAY contain keys for accounts that are inactive,
   * deleted, or non-existent.
   * If fee_exempt_key_list is unset in this transaction, there SHALL NOT be
   * any fee-exempt keys.  In particular, the following keys SHALL NOT be
   * implicitly or automatically added to this list:
   * `adminKey`, `submitKey`, `fee_schedule_key`.
   */
  std::vector<std::shared_ptr<Key>> mFeeExemptKeys;

  /**
   * A set of custom fee definitions.
   * These are fees to be assessed for each submit to this topic.
   * Each fee defined in this set SHALL be evaluated for
   * each message submitted to this topic, and the resultant
   * total assessed fees SHALL be charged.
   * Custom fees defined here SHALL be assessed in addition to the base
   * network and node fees.
   * custom_fees list SHALL NOT contain more than
   * `MAX_CUSTOM_FEE_ENTRIES_FOR_TOPICS` entries.
   */
  std::vector<CustomFixedFee> mCustomFixedFees;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOPIC_UPDATE_TRANSACTION_H_
