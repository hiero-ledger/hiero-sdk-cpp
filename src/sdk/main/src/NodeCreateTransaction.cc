// SPDX-License-Identifier: Apache-2.0
#include "NodeCreateTransaction.h"
#include "TransactionId.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <grpcpp/client_context.h>
#include <node_create.pb.h>
#include <stdexcept>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
NodeCreateTransaction::NodeCreateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<NodeCreateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
NodeCreateTransaction::NodeCreateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<NodeCreateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
NodeCreateTransaction& NodeCreateTransaction::setAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAccountId = accountId;
  return *this;
}

//-----
NodeCreateTransaction& NodeCreateTransaction::setDescription(std::string_view description)
{
  requireNotFrozen();
  mDescription = description.data();
  return *this;
}

//-----
NodeCreateTransaction& NodeCreateTransaction::setGossipEndpoints(const std::vector<Endpoint>& endpoints)
{
  requireNotFrozen();
  mGossipEndpoints = endpoints;
  return *this;
}

//-----
NodeCreateTransaction& NodeCreateTransaction::setServiceEndpoints(const std::vector<Endpoint>& endpoints)
{
  requireNotFrozen();
  mServiceEndpoints = endpoints;
  return *this;
}

//-----
NodeCreateTransaction& NodeCreateTransaction::setGossipCaCertificate(const std::vector<std::byte>& certificate)
{
  requireNotFrozen();
  mGossipCaCertificate = certificate;
  return *this;
}

//-----
NodeCreateTransaction& NodeCreateTransaction::setGrpcCertificateHash(const std::vector<std::byte>& hash)
{
  requireNotFrozen();
  mGrpcCertificateHash = hash;
  return *this;
}

//-----
NodeCreateTransaction& NodeCreateTransaction::setAdminKey(const std::shared_ptr<Key>& key)
{
  requireNotFrozen();
  mAdminKey = key;
  return *this;
}

//-----
grpc::Status NodeCreateTransaction::submitRequest(const proto::Transaction& request,
                                                  const std::shared_ptr<internal::Node>& node,
                                                  const std::chrono::system_clock::time_point& deadline,
                                                  proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kNodeCreate, request, deadline, response);
}

//-----
void NodeCreateTransaction::validateChecksums(const Client& client) const
{
  mAccountId.validateChecksum(client);
}

//-----
void NodeCreateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_nodecreate(build());
}

//-----
void NodeCreateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_nodecreate())
  {
    throw std::invalid_argument("Transaction body doesn't contain NodeCreate data");
  }

  const aproto::NodeCreateTransactionBody& body = transactionBody.nodecreate();

  mAccountId = AccountId::fromProtobuf(body.account_id());

  mDescription = body.description();

  for (int i = 0; i < body.gossip_endpoint_size(); i++)
  {
    mGossipEndpoints.push_back(Endpoint::fromProtobuf(body.gossip_endpoint(i)));
  }

  for (int i = 0; i < body.service_endpoint_size(); i++)
  {
    mServiceEndpoints.push_back(Endpoint::fromProtobuf(body.service_endpoint(i)));
  }

  mGossipCaCertificate = internal::Utilities::stringToByteVector(body.gossip_ca_certificate());

  mGrpcCertificateHash = internal::Utilities::stringToByteVector(body.grpc_certificate_hash());

  if (body.has_admin_key())
  {
    mAdminKey = Key::fromProtobuf(body.admin_key());
  }
}

//-----
aproto::NodeCreateTransactionBody* NodeCreateTransaction::build() const
{
  auto body = std::make_unique<aproto::NodeCreateTransactionBody>();

  body->set_allocated_account_id(mAccountId.toProtobuf().release());

  if (mDescription.has_value())
  {
    body->set_description(mDescription.value());
  }

  for (const Endpoint& e : mGossipEndpoints)
  {
    body->mutable_gossip_endpoint()->AddAllocated(e.toProtobuf().release());
  }

  for (const Endpoint& e : mServiceEndpoints)
  {
    body->mutable_service_endpoint()->AddAllocated(e.toProtobuf().release());
  }

  body->set_gossip_ca_certificate(internal::Utilities::byteVectorToString(mGossipCaCertificate));

  if (mGrpcCertificateHash.has_value())
  {
    body->set_grpc_certificate_hash(internal::Utilities::byteVectorToString(mGrpcCertificateHash.value()));
  }

  if (mAdminKey != nullptr)
  {
    body->set_allocated_admin_key(mAdminKey->toProtobufKey().release());
  }

  return body.release();
}

} // namespace Hiero
