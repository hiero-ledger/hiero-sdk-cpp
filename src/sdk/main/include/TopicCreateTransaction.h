// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOPIC_CREATE_TRANSACTION_H_
#define HIERO_SDK_CPP_TOPIC_CREATE_TRANSACTION_H_

#include "AccountId.h"
#include "CustomFixedFee.h"
#include "Defaults.h"
#include "Key.h"
#include "Transaction.h"

#include <chrono>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

namespace proto
{
class ConsensusCreateTopicTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that creates a new topic recognized by the Hiero network. The newly generated topic can be referenced
 * by its topic ID. The topic ID is used to identify a unique topic to which to submit messages. You can obtain the new
 * topic ID by requesting the receipt of the transaction. All messages within a topic are sequenced with respect to one
 * another and are provided a unique sequence number.
 *
 * You can also create a private topic where only authorized parties can submit messages to that topic. To create a
 * private topic you would need to set the mSubmitKey of the transaction. The mSubmitKey value is then shared with the
 * authorized parties and is required to successfully submit messages to the private topic.
 *
 * Transaction Signing Requirements:
 *  - If an admin key is specified, the admin key must sign the transaction.
 *  - If an admin key is not specified, the topic is immutable.
 *  - If an auto-renew account is specified, that account must also sign this transaction.
 */
class TopicCreateTransaction : public Transaction<TopicCreateTransaction>
{
public:
  TopicCreateTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a ConsensusCreateTopic transaction.
   */
  explicit TopicCreateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TopicCreateTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the desired memo for the new topic.
   *
   * @param memo The desired memo for the new topic.
   * @return A reference to this TopicCreateTransaction object with the newly-set memo.
   * @throws IllegalStateException If this TopicCreateTransaction is frozen.
   */
  TopicCreateTransaction& setMemo(std::string_view memo);

  /**
   * Set the desired admin key for the new topic.
   *
   * @param key The desired admin key for the new topic.
   * @return A reference to this TopicCreateTransaction object with the newly-set admin key.
   * @throws IllegalStateException If this TopicCreateTransaction is frozen.
   */
  TopicCreateTransaction& setAdminKey(const std::shared_ptr<Key>& key);

  /**
   * Set the desired submit key for the new topic.
   *
   * @param key The desired submit key for the new topic.
   * @return A reference to this TopicCreateTransaction object with the newly-set submit key.
   * @throws IllegalStateException If this TopicCreateTransaction is frozen.
   */
  TopicCreateTransaction& setSubmitKey(const std::shared_ptr<Key>& key);

  /**
   * Set the desired auto-renew period for the new topic.
   *
   * @param autoRenew The desired auto-renew period for the new topic.
   * @return A reference to this TopicCreateTransaction object with the newly-set auto-renew period.
   * @throws IllegalStateException If this TopicCreateTransaction is frozen.
   */
  TopicCreateTransaction& setAutoRenewPeriod(const std::chrono::system_clock::duration& autoRenew);

  /**
   * Set the ID of the desired auto-renew account for the new topic.
   *
   * @param accountId The ID of the desired auto-renew account for the new topic.
   * @return A reference to this TopicCreateTransaction object with the newly-set auto-renew account ID.
   * @throws IllegalStateException If this TopicCreateTransaction is frozen.
   */
  TopicCreateTransaction& setAutoRenewAccountId(const AccountId& accountId);

  /**
   * Assignment logic of the autoRenewAccount for a transaction.
   *
   * @param account The auto renew account to be assigned.
   */
  void assignAutoRenewAccount(proto::TransactionBody& body, const AccountId& accountId) const override;

  /**
   * Set the fee schedule key for the new topic.
   *
   * @param key The key that controls fee schedule updates.
   * @return A reference to this TopicCreateTransaction object with the newly-set fee schedule key.
   * @throws IllegalStateException If this TopicCreateTransaction is frozen.
   */
  TopicCreateTransaction& setFeeScheduleKey(const std::shared_ptr<Key>& key);

  /**
   * Set the list of keys exempt from paying custom fees.
   *
   * @param keys The list of keys that will not be charged custom fees when submitting messages to this topic.
   * @return A reference to this TopicCreateTransaction object with the newly-set exempt keys.
   * @throws IllegalStateException If this TopicCreateTransaction is frozen.
   */
  TopicCreateTransaction& setFeeExemptKeys(const std::vector<std::shared_ptr<Key>>& keys);

  /**
   * Add a key to the list of keys exempt from paying custom fees.
   *
   * @param key The key to be added to the exempt list.
   * @return A reference to this TopicCreateTransaction object with the newly-added exempt key.
   * @throws IllegalStateException If this TopicCreateTransaction is frozen.
   */
  TopicCreateTransaction& addFeeExemptKey(const std::shared_ptr<Key>& key);

  /**
   * Set the list of custom fixed fees for this topic.
   *
   * @param fees The list of custom fixed fees.
   * @return A reference to this TopicCreateTransaction object with the newly-set custom fees.
   * @throws IllegalStateException If this TopicCreateTransaction is frozen.
   */
  TopicCreateTransaction& setCustomFixedFees(const std::vector<CustomFixedFee>& fees);

  /**
   * Add a custom fixed fee to this topic.
   *
   * @param fee The custom fixed fee to be added.
   * @return A reference to this TopicCreateTransaction object with the newly-added fee.
   * @throws IllegalStateException If this TopicCreateTransaction is frozen.
   */
  TopicCreateTransaction& addCustomFixedFee(const CustomFixedFee& fee);

  /**
   * Get the desired memo of the new topic.
   *
   * @return The desired memo of the new topic.
   */
  [[nodiscard]] inline std::string getMemo() const { return mMemo; }

  /**
   * Get the desired admin key for the new topic.
   *
   * @return The desired admin key for the new topic.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getAdminKey() const { return mAdminKey; }

  /**
   * Get the desired submit key for the new topic.
   *
   * @return The desired submit key for the new topic.
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
   * Get the desired auto-renew period for the new topic.
   *
   * @return The desired auto-renew period for the new topic.
   */
  [[nodiscard]] inline std::chrono::system_clock::duration getAutoRenewPeriod() const { return mAutoRenewPeriod; }

  /**
   * Get the ID of the desired auto-renew account for the new topic.
   *
   * @return The ID of the desired auto-renew account for the new topic. Returns uninitialized if the auto-renew account
   *         has not been set.
   */
  [[nodiscard]] inline std::optional<AccountId> getAutoRenewAccountId() const { return mAutoRenewAccountId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TopicCreateTransaction's data to
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
   * Derived from Transaction. Verify that all the checksums in this TopicCreateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TopicCreateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TopicCreateTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TopicCreateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a ConsensusCreateTopicTransactionBody protobuf object from this TopicCreateTransaction object.
   *
   * @return A pointer to a ConsensusCreateTopicTransactionBody protobuf object filled with this TopicCreateTransaction
   *         object's data.
   */
  [[nodiscard]] proto::ConsensusCreateTopicTransactionBody* build() const;

  /**
   * The publicly visible memo for the new topic.
   */
  std::string mMemo;

  /**
   * The key used for access control to update or delete the new topic. Anyone can increase the topic's mExpirationTime
   * via a TopicUpdateTransaction regardless of the mAdminKey. If no mAdminKey is specified, TopicUpdateTransaction may
   * only be used to extend the topic's mExpirationTime, and TopicDeleteTransaction is disallowed.
   */
  std::shared_ptr<Key> mAdminKey = nullptr;

  /**
   * The key that must sign to submit a message to the new topic (via a TopicMessageSubmitTransaction). If nullptr, all
   * message submissions are allowed on this topic.
   */
  std::shared_ptr<Key> mSubmitKey = nullptr;

  /**
   * The amount of time by which to attempt to extend the new topic's lifetime automatically at its expiration time.
   */
  std::chrono::system_clock::duration mAutoRenewPeriod = DEFAULT_AUTO_RENEW_PERIOD;

  /**
   * The account that should be charged to extend the lifetime of the new topic at its expiration time.
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

#endif // HIERO_SDK_CPP_TOPIC_CREATE_TRANSACTION_H_
