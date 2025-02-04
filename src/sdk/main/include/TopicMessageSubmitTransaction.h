// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOPIC_MESSAGE_SUBMIT_TRANSACTION_H_
#define HIERO_SDK_CPP_TOPIC_MESSAGE_SUBMIT_TRANSACTION_H_

#include "ChunkedTransaction.h"
#include "TopicId.h"
#include "TransactionId.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace proto
{
class ConsensusSubmitMessageTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that submits a topic message to the Hiero network. To access the messages submitted to a topic ID,
 * subscribe to the topic via a mirror node. The mirror node will publish the ordered messages to subscribers. Once the
 * transaction is successfully executed, the receipt of the transaction will include the topic's updated sequence number
 * and topic running hash.
 *
 * Transaction Signing Requirements:
 *  - Anyone can submit a message to a public topic.
 *  - The submit key is required to sign the transaction for a private topic.
 */
class TopicMessageSubmitTransaction : public ChunkedTransaction<TopicMessageSubmitTransaction>
{
public:
  TopicMessageSubmitTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a ConsensusSubmitMessage transaction.
   */
  explicit TopicMessageSubmitTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TopicMessageSubmitTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the topic to which to submit a message.
   *
   * @param topicId The ID of the topic to which to submit a message.
   * @return A reference to this TopicMessageSubmitTransaction object with the newly-set topic ID.
   * @throws IllegalStateException If this TopicMessageSubmitTransaction is frozen.
   */
  TopicMessageSubmitTransaction& setTopicId(const TopicId& topicId);

  /**
   * Set the message to submit.
   *
   * @param message The message to submit.
   * @return A reference to this TopicMessageSubmitTransaction object with the newly-set message.
   * @throws IllegalStateException If this TopicMessageSubmitTransaction is frozen.
   */
  TopicMessageSubmitTransaction& setMessage(const std::vector<std::byte>& message);
  TopicMessageSubmitTransaction& setMessage(std::string_view message);

  /**
   * Get the ID of the topic to which to submit a message.
   *
   * @return The ID of the topic to which to submit a message.
   */
  [[nodiscard]] inline TopicId getTopicId() const { return mTopicId; }

  /**
   * Get the message to submit.
   *
   * @return The message to submit.
   */
  [[nodiscard]] inline std::vector<std::byte> getMessage() const { return getData(); }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TopicMessageSubmitTransaction's
   * data to a Node.
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
   * Derived from Transaction. Verify that all the checksums in this TopicMessageSubmitTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TopicMessageSubmitTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TopicMessageSubmitTransaction protobuf representation to the
   * Transaction protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Derived from ChunkedTransaction. Build and add this TopicMessageSubmitTransaction's chunked protobuf representation
   * to the TransactionBody protobuf object.
   *
   * @param chunk The chunk number.
   * @param total The total number of chunks being created.
   * @param body  The TransactionBody protobuf object to which to add the chunked data.
   */
  void addToChunk(uint32_t chunk, uint32_t total, proto::TransactionBody& body) const override;

  /**
   * Initialize this TopicMessageSubmitTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a ConsensusSubmitMessageTransactionBody protobuf object from this TopicMessageSubmitTransaction object.
   * Optionally, build this TopicMessageSubmitTransaction for a specific chunk.
   *
   * @param chunk The chunk number for which to build this TopicMessageSubmitTransaction. The default value (-1)
   *              indicates to build for all chunks (i.e. build with all data).
   * @return A pointer to a ConsensusSubmitMessageTransactionBody protobuf object filled with this
   *         TopicMessageSubmitTransaction object's data.
   */
  [[nodiscard]] proto::ConsensusSubmitMessageTransactionBody* build(int chunk = -1) const;

  /**
   * The ID of the topic to which to send a message.
   */
  TopicId mTopicId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOPIC_MESSAGE_SUBMIT_TRANSACTION_H_
