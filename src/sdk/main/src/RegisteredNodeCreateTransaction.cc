// SPDX-License-Identifier: Apache-2.0
#include "RegisteredNodeCreateTransaction.h"
#include "BlockNodeServiceEndpoint.h"
#include "MirrorNodeServiceEndpoint.h"
#include "RpcRelayServiceEndpoint.h"
#include "TransactionId.h"
#include "impl/Node.h"

#include <services/registered_node_create.pb.h>
#include <services/transaction.pb.h>

#include <stdexcept>

namespace Hiero
{

//-----
RegisteredNodeCreateTransaction::RegisteredNodeCreateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<RegisteredNodeCreateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
RegisteredNodeCreateTransaction::RegisteredNodeCreateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<RegisteredNodeCreateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
RegisteredNodeCreateTransaction& RegisteredNodeCreateTransaction::setAdminKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mAdminKey = key;
  return *this;
}

//-----
RegisteredNodeCreateTransaction& RegisteredNodeCreateTransaction::setDescription(std::string_view description)
{
  requireNotFrozen();
  mDescription = description;
  return *this;
}

//-----
RegisteredNodeCreateTransaction& RegisteredNodeCreateTransaction::addServiceEndpoint(
  std::shared_ptr<RegisteredServiceEndpoint> endpoint)
{
  requireNotFrozen();
  mServiceEndpoints.push_back(std::move(endpoint));
  return *this;
}

//-----
RegisteredNodeCreateTransaction& RegisteredNodeCreateTransaction::clearServiceEndpoints()
{
  requireNotFrozen();
  mServiceEndpoints.clear();
  return *this;
}

//-----
grpc::Status RegisteredNodeCreateTransaction::submitRequest(const proto::Transaction& request,
                                                            const std::shared_ptr<internal::Node>& node,
                                                            const std::chrono::system_clock::time_point& deadline,
                                                            proto::TransactionResponse* response) const
{
  return node->submitTransaction(
    proto::TransactionBody::DataCase::kRegisteredNodeCreate, request, deadline, response);
}

//-----
void RegisteredNodeCreateTransaction::validateChecksums(const Client& /*client*/) const
{
  // No entity IDs to validate.
}

//-----
void RegisteredNodeCreateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_registerednodecreate(build());
}

//-----
void RegisteredNodeCreateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_registerednodecreate())
  {
    throw std::invalid_argument("Transaction body doesn't contain RegisteredNodeCreate data");
  }

  const aproto::RegisteredNodeCreateTransactionBody& body = transactionBody.registerednodecreate();

  if (body.has_admin_key())
  {
    mAdminKey = Key::fromProtobuf(body.admin_key());
  }

  mDescription = body.description();

  for (int i = 0; i < body.service_endpoint_size(); ++i)
  {
    mServiceEndpoints.push_back(RegisteredServiceEndpoint::fromProtobuf(body.service_endpoint(i)));
  }
}

//-----
aproto::RegisteredNodeCreateTransactionBody* RegisteredNodeCreateTransaction::build() const
{
  auto body = std::make_unique<aproto::RegisteredNodeCreateTransactionBody>();

  if (mAdminKey != nullptr)
  {
    body->set_allocated_admin_key(mAdminKey->toProtobufKey().release());
  }

  body->set_description(mDescription);

  for (const auto& ep : mServiceEndpoints)
  {
    body->mutable_service_endpoint()->AddAllocated(ep->toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero
