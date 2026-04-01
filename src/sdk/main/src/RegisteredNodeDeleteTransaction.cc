// SPDX-License-Identifier: Apache-2.0
#include "RegisteredNodeDeleteTransaction.h"
#include "TransactionId.h"
#include "impl/Node.h"

#include <services/registered_node_delete.pb.h>
#include <services/transaction.pb.h>

#include <stdexcept>

namespace Hiero
{

//-----
RegisteredNodeDeleteTransaction::RegisteredNodeDeleteTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<RegisteredNodeDeleteTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
RegisteredNodeDeleteTransaction::RegisteredNodeDeleteTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<RegisteredNodeDeleteTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
RegisteredNodeDeleteTransaction& RegisteredNodeDeleteTransaction::setRegisteredNodeId(uint64_t registeredNodeId)
{
  requireNotFrozen();
  mRegisteredNodeId = registeredNodeId;
  return *this;
}

//-----
grpc::Status RegisteredNodeDeleteTransaction::submitRequest(const proto::Transaction& request,
                                                            const std::shared_ptr<internal::Node>& node,
                                                            const std::chrono::system_clock::time_point& deadline,
                                                            proto::TransactionResponse* response) const
{
  return node->submitTransaction(
    proto::TransactionBody::DataCase::kRegisteredNodeDelete, request, deadline, response);
}

//-----
void RegisteredNodeDeleteTransaction::validateChecksums(const Client& /*client*/) const
{
  // No entity IDs to validate.
}

//-----
void RegisteredNodeDeleteTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_registerednodedelete(build());
}

//-----
void RegisteredNodeDeleteTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_registerednodedelete())
  {
    throw std::invalid_argument("Transaction body doesn't contain RegisteredNodeDelete data");
  }

  mRegisteredNodeId = transactionBody.registerednodedelete().registered_node_id();
}

//-----
aproto::RegisteredNodeDeleteTransactionBody* RegisteredNodeDeleteTransaction::build() const
{
  auto body = std::make_unique<aproto::RegisteredNodeDeleteTransactionBody>();
  body->set_registered_node_id(mRegisteredNodeId);
  return body.release();
}

} // namespace Hiero
