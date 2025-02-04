// SPDX-License-Identifier: Apache-2.0
#include "TopicMessageSubmitTransaction.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <consensus_submit_message.pb.h>
#include <grpcpp/client_context.h>
#include <stdexcept>
#include <string>
#include <transaction.pb.h>
#include <transaction_body.pb.h>
#include <transaction_contents.pb.h>

namespace Hiero
{
//-----
TopicMessageSubmitTransaction::TopicMessageSubmitTransaction(const proto::TransactionBody& transactionBody)
  : ChunkedTransaction<TopicMessageSubmitTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TopicMessageSubmitTransaction::TopicMessageSubmitTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : ChunkedTransaction<TopicMessageSubmitTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TopicMessageSubmitTransaction& TopicMessageSubmitTransaction::setTopicId(const TopicId& topicId)
{
  requireNotFrozen();
  mTopicId = topicId;
  return *this;
}

//-----
TopicMessageSubmitTransaction& TopicMessageSubmitTransaction::setMessage(const std::vector<std::byte>& message)
{
  requireNotFrozen();
  setData(message);
  return *this;
}

//-----
TopicMessageSubmitTransaction& TopicMessageSubmitTransaction::setMessage(std::string_view message)
{
  requireNotFrozen();
  setData(message);
  return *this;
}

//-----
grpc::Status TopicMessageSubmitTransaction::submitRequest(const proto::Transaction& request,
                                                          const std::shared_ptr<internal::Node>& node,
                                                          const std::chrono::system_clock::time_point& deadline,
                                                          proto::TransactionResponse* response) const
{
  return node->submitTransaction(
    proto::TransactionBody::DataCase::kConsensusSubmitMessage, request, deadline, response);
}

//-----
void TopicMessageSubmitTransaction::validateChecksums(const Client& client) const
{
  mTopicId.validateChecksum(client);
}

//-----
void TopicMessageSubmitTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_consensussubmitmessage(build());
}

//-----
void TopicMessageSubmitTransaction::addToChunk(uint32_t chunk, uint32_t total, proto::TransactionBody& body) const
{
  body.set_allocated_consensussubmitmessage(build(static_cast<int>(chunk)));
  body.mutable_consensussubmitmessage()->mutable_chunkinfo()->set_allocated_initialtransactionid(
    getTransactionId().toProtobuf().release());
  body.mutable_consensussubmitmessage()->mutable_chunkinfo()->set_number(static_cast<int32_t>(chunk + 1));
  body.mutable_consensussubmitmessage()->mutable_chunkinfo()->set_total(static_cast<int32_t>(total));
}

//-----
void TopicMessageSubmitTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_consensussubmitmessage())
  {
    throw std::invalid_argument("Transaction body doesn't contain ConsensusSubmitMessage data");
  }

  if (const proto::ConsensusSubmitMessageTransactionBody& body = transactionBody.consensussubmitmessage();
      body.has_topicid())
  {
    mTopicId = TopicId::fromProtobuf(body.topicid());
  }

  // Construct the data from the various Transaction protobuf objects.
  std::string data;
  bool dataStillExists = true;
  for (unsigned int i = 0; dataStillExists; ++i)
  {
    proto::Transaction tx;

    try
    {
      tx = getTransactionProtobufObject(i * static_cast<unsigned int>(getNodeAccountIds().size()));
    }
    catch (const std::out_of_range&)
    {
      dataStillExists = false;
      break;
    }

    proto::SignedTransaction signedTx;
    signedTx.ParseFromArray(tx.signedtransactionbytes().data(), static_cast<int>(tx.signedtransactionbytes().size()));

    proto::TransactionBody txBody;
    txBody.ParseFromArray(signedTx.bodybytes().data(), static_cast<int>(signedTx.bodybytes().size()));

    data += txBody.consensussubmitmessage().message();
  }

  setData(data);
}

//-----
proto::ConsensusSubmitMessageTransactionBody* TopicMessageSubmitTransaction::build(int chunk) const
{
  auto body = std::make_unique<proto::ConsensusSubmitMessageTransactionBody>();

  if (!(mTopicId == TopicId()))
  {
    body->set_allocated_topicid(mTopicId.toProtobuf().release());
  }

  body->set_message(internal::Utilities::byteVectorToString(
    (chunk >= 0) ? getDataForChunk(static_cast<unsigned int>(chunk)) : getData()));
  return body.release();
}

} // namespace Hiero
