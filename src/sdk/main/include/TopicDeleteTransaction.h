// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOPIC_DELETE_TRANSACTION_H_
#define HIERO_SDK_CPP_TOPIC_DELETE_TRANSACTION_H_

#include "TopicId.h"
#include "Transaction.h"

#include <optional>

namespace proto
{
class ConsensusDeleteTopicTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that deletes a topic from the Hiero network. Once a topic is deleted, the topic cannot be recovered to
 * receive messages and all submitMessage calls will fail. Older messages can still be accessed, even after the topic is
 * deleted, via the mirror node.
 *
 * Transaction Signing Requirements:
 *  - If the adminKey was set upon the creation of the topic, the adminKey is required to sign to successfully delete
 *    the topic.
 *  - If no adminKey was set upon the creating of the topic, you cannot delete the topic and will receive an
 *    UNAUTHORIZED error.
 */
class TopicDeleteTransaction : public Transaction<TopicDeleteTransaction>
{
public:
  TopicDeleteTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a ConsensusDeleteTopic transaction.
   */
  explicit TopicDeleteTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TopicDeleteTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the topic to delete.
   *
   * @param topicId The ID of the topic to delete.
   * @return A reference to this TopicDeleteTransaction object with the newly-set topic ID.
   * @throws IllegalStateException If this TopicDeleteTransaction is frozen.
   */
  TopicDeleteTransaction& setTopicId(const TopicId& topicId);

  /**
   * Get the ID of the topic this TopicDeleteTransaction is currently configured to delete.
   *
   * @return The ID of the topic this TopicDeleteTransaction is currently configured to delete. Uninitialized if no
   *         topic ID has been set.
   */
  [[nodiscard]] inline std::optional<TopicId> getTopicId() const { return mTopicId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TopicDeleteTransaction's data to
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
   * Derived from Transaction. Verify that all the checksums in this TopicDeleteTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TopicDeleteTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TopicDeleteTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TopicDeleteTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a ConsensusDeleteTopicTransactionBody protobuf object from this TopicDeleteTransaction object.
   *
   * @return A pointer to a ConsensusDeleteTopicTransactionBody protobuf object filled with this TopicDeleteTransaction
   *         object's data.
   */
  [[nodiscard]] proto::ConsensusDeleteTopicTransactionBody* build() const;

  /**
   * The ID of the topic to delete.
   */
  std::optional<TopicId> mTopicId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOPIC_DELETE_TRANSACTION_H_
