// SPDX-License-Identifier: Apache-2.0
#include "NodeDeleteTransaction.h"
#include "TransactionId.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <node_delete.pb.h>
#include <stdexcept>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
NodeDeleteTransaction::NodeDeleteTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<NodeDeleteTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
NodeDeleteTransaction::NodeDeleteTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<NodeDeleteTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
NodeDeleteTransaction& NodeDeleteTransaction::setNodeId(uint64_t nodeId)
{
  requireNotFrozen();
  mNodeId = nodeId;
  return *this;
}

//-----
grpc::Status NodeDeleteTransaction::submitRequest(const proto::Transaction& request,
                                                  const std::shared_ptr<internal::Node>& node,
                                                  const std::chrono::system_clock::time_point& deadline,
                                                  proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kNodeDelete, request, deadline, response);
}

//-----
void NodeDeleteTransaction::validateChecksums(const Client& client) const
{
  // No checksum to validate.
}

//-----
void NodeDeleteTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_nodedelete(build());
}

//-----
void NodeDeleteTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_nodedelete())
  {
    throw std::invalid_argument("Transaction body doesn't contain NodeDelete data");
  }

  const aproto::NodeDeleteTransactionBody& body = transactionBody.nodedelete();

  mNodeId = body.node_id();
}

//-----
aproto::NodeDeleteTransactionBody* NodeDeleteTransaction::build() const
{
  auto body = std::make_unique<aproto::NodeDeleteTransactionBody>();
  body->set_node_id(mNodeId);

  return body.release();
}

} // namespace Hiero
