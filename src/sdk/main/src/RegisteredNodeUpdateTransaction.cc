// SPDX-License-Identifier: Apache-2.0
#include "RegisteredNodeUpdateTransaction.h"
#include "TransactionId.h"
#include "impl/Node.h"

#include <google/protobuf/wrappers.pb.h>
#include <services/registered_node_update.pb.h>
#include <services/transaction.pb.h>

#include <stdexcept>

namespace Hiero
{

//-----
RegisteredNodeUpdateTransaction::RegisteredNodeUpdateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<RegisteredNodeUpdateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
RegisteredNodeUpdateTransaction::RegisteredNodeUpdateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<RegisteredNodeUpdateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
RegisteredNodeUpdateTransaction& RegisteredNodeUpdateTransaction::setRegisteredNodeId(uint64_t registeredNodeId)
{
  requireNotFrozen();
  mRegisteredNodeId = registeredNodeId;
  return *this;
}

//-----
RegisteredNodeUpdateTransaction& RegisteredNodeUpdateTransaction::setAdminKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mAdminKey = key;
  return *this;
}

//-----
RegisteredNodeUpdateTransaction& RegisteredNodeUpdateTransaction::setDescription(std::string_view description)
{
  requireNotFrozen();
  mDescription = description;
  return *this;
}

//-----
RegisteredNodeUpdateTransaction& RegisteredNodeUpdateTransaction::clearDescription()
{
  requireNotFrozen();
  mDescription.reset();
  return *this;
}

//-----
RegisteredNodeUpdateTransaction& RegisteredNodeUpdateTransaction::addServiceEndpoint(
  std::shared_ptr<RegisteredServiceEndpoint> endpoint)
{
  requireNotFrozen();
  mServiceEndpoints.push_back(std::move(endpoint));
  return *this;
}

//-----
RegisteredNodeUpdateTransaction& RegisteredNodeUpdateTransaction::clearServiceEndpoints()
{
  requireNotFrozen();
  mServiceEndpoints.clear();
  return *this;
}

//-----
grpc::Status RegisteredNodeUpdateTransaction::submitRequest(const proto::Transaction& request,
                                                            const std::shared_ptr<internal::Node>& node,
                                                            const std::chrono::system_clock::time_point& deadline,
                                                            proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kRegisteredNodeUpdate, request, deadline, response);
}

//-----
void RegisteredNodeUpdateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_registerednodeupdate(build());
}

//-----
void RegisteredNodeUpdateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_registerednodeupdate())
  {
    throw std::invalid_argument("Transaction body doesn't contain RegisteredNodeUpdate data");
  }

  const auto& body = transactionBody.registerednodeupdate();

  mRegisteredNodeId = body.registered_node_id();

  if (body.has_admin_key())
  {
    mAdminKey = Key::fromProtobuf(body.admin_key());
  }

  if (body.has_description())
  {
    mDescription = body.description().value();
  }

  for (int i = 0; i < body.service_endpoint_size(); ++i)
  {
    mServiceEndpoints.push_back(RegisteredServiceEndpoint::fromProtobuf(body.service_endpoint(i)));
  }
}

//-----
com::hedera::hapi::node::addressbook::RegisteredNodeUpdateTransactionBody* RegisteredNodeUpdateTransaction::build()
  const
{
  auto body = std::make_unique<com::hedera::hapi::node::addressbook::RegisteredNodeUpdateTransactionBody>();

  body->set_registered_node_id(mRegisteredNodeId);

  if (mAdminKey != nullptr)
  {
    body->set_allocated_admin_key(mAdminKey->toProtobufKey().release());
  }

  if (mDescription.has_value())
  {
    body->mutable_description()->set_value(mDescription.value());
  }

  for (const auto& ep : mServiceEndpoints)
  {
    body->mutable_service_endpoint()->AddAllocated(ep->toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero
