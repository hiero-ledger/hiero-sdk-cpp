// SPDX-License-Identifier: Apache-2.0
#include "NodeUpdateTransaction.h"
#include "TransactionId.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <grpcpp/client_context.h>
#include <node_update.pb.h>
#include <stdexcept>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
NodeUpdateTransaction::NodeUpdateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<NodeUpdateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
NodeUpdateTransaction::NodeUpdateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<NodeUpdateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
NodeUpdateTransaction& NodeUpdateTransaction::setNodeId(uint64_t nodeId)
{
  requireNotFrozen();
  mNodeId = nodeId;
  return *this;
}

//-----
NodeUpdateTransaction& NodeUpdateTransaction::setAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAccountId = accountId;
  return *this;
}

//-----
NodeUpdateTransaction& NodeUpdateTransaction::setDescription(std::string_view description)
{
  requireNotFrozen();
  mDescription = description.data();
  return *this;
}

//-----
NodeUpdateTransaction& NodeUpdateTransaction::setGossipEndpoints(const std::vector<Endpoint>& endpoints)
{
  requireNotFrozen();
  mGossipEndpoints = endpoints;
  return *this;
}

//-----
NodeUpdateTransaction& NodeUpdateTransaction::setServiceEndpoints(const std::vector<Endpoint>& endpoints)
{
  requireNotFrozen();
  mServiceEndpoints = endpoints;
  return *this;
}

//-----
NodeUpdateTransaction& NodeUpdateTransaction::setGossipCaCertificate(const std::vector<std::byte>& certificate)
{
  requireNotFrozen();
  mGossipCaCertificate = certificate;
  return *this;
}

//-----
NodeUpdateTransaction& NodeUpdateTransaction::setGrpcCertificateHash(const std::vector<std::byte>& hash)
{
  requireNotFrozen();
  mGrpcCertificateHash = hash;
  return *this;
}

//-----
NodeUpdateTransaction& NodeUpdateTransaction::setAdminKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mAdminKey = key;
  return *this;
}

//-----
grpc::Status NodeUpdateTransaction::submitRequest(const proto::Transaction& request,
                                                  const std::shared_ptr<internal::Node>& node,
                                                  const std::chrono::system_clock::time_point& deadline,
                                                  proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kNodeUpdate, request, deadline, response);
}

//-----
void NodeUpdateTransaction::validateChecksums(const Client& client) const
{
  mAccountId.validateChecksum(client);
}

//-----
void NodeUpdateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_nodeupdate(build());
}

//-----
void NodeUpdateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_nodeupdate())
  {
    throw std::invalid_argument("Transaction body doesn't contain NodeUpdate data");
  }

  const aproto::NodeUpdateTransactionBody& body = transactionBody.nodeupdate();

  mNodeId = body.node_id();
  mAccountId = AccountId::fromProtobuf(body.account_id());

  if (body.has_description())
  {
    mDescription = body.description().value();
  }

  for (int i = 0; i < body.gossip_endpoint_size(); i++)
  {
    mGossipEndpoints.push_back(Endpoint::fromProtobuf(body.gossip_endpoint(i)));
  }

  for (int i = 0; i < body.service_endpoint_size(); i++)
  {
    mServiceEndpoints.push_back(Endpoint::fromProtobuf(body.service_endpoint(i)));
  }

  mGossipCaCertificate = internal::Utilities::stringToByteVector(body.gossip_ca_certificate().value());

  if (body.has_grpc_certificate_hash())
  {
    mGrpcCertificateHash = internal::Utilities::stringToByteVector(body.grpc_certificate_hash().value());
  }

  if (body.has_admin_key())
  {
    mAdminKey = Key::fromProtobuf(body.admin_key());
  }
}

//-----
aproto::NodeUpdateTransactionBody* NodeUpdateTransaction::build() const
{
  auto body = std::make_unique<aproto::NodeUpdateTransactionBody>();

  body->set_node_id(mNodeId);
  body->set_allocated_account_id(mAccountId.toProtobuf().release());

  if (mDescription.has_value())
  {
    auto value = std::make_unique<google::protobuf::StringValue>();
    value->set_value(mDescription.value());
    body->set_allocated_description(value.release());
  }

  for (const Endpoint& e : mGossipEndpoints)
  {
    body->mutable_gossip_endpoint()->AddAllocated(e.toProtobuf().release());
  }

  for (const Endpoint& e : mServiceEndpoints)
  {
    body->mutable_service_endpoint()->AddAllocated(e.toProtobuf().release());
  }

  body->mutable_gossip_ca_certificate()->set_value(internal::Utilities::byteVectorToString(mGossipCaCertificate));

  if (mGrpcCertificateHash.has_value())
  {
    body->mutable_grpc_certificate_hash()->set_value(
      internal::Utilities::byteVectorToString(mGrpcCertificateHash.value()));
  }

  if (mAdminKey != nullptr)
  {
    body->set_allocated_admin_key(mAdminKey->toProtobufKey().release());
  }

  return body.release();
}

} // namespace Hiero
