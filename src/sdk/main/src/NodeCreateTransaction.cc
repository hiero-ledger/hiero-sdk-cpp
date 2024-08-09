/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2024 Hedera Hashgraph, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include "NodeCreateTransaction.h"
#include "TransactionId.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <grpcpp/client_context.h>
#include <proto/node_create.pb.h>
#include <proto/transaction.pb.h>
#include <proto/transaction_body.pb.h>
#include <stdexcept>

namespace Hedera
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
NodeCreateTransaction& NodeCreateTransaction::setDescription(const std::optional<std::string>& description)
{
  requireNotFrozen();
  mDescription = description;
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

  if (!transactionBody.has_nodeupdate())
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
    auto se = std::make_unique<proto::ServiceEndpoint>();
    se->set_domain_name(e.getAddress().toString());
    se->set_port(e.getPort());
    body->mutable_gossip_endpoint()->AddAllocated(se.release());
  }

  for (const Endpoint& e : mServiceEndpoints)
  {
    auto se = std::make_unique<proto::ServiceEndpoint>();
    se->set_domain_name(e.getAddress().toString());
    se->set_port(e.getPort());
    body->mutable_service_endpoint()->AddAllocated(se.release());
  }

  body->set_gossip_ca_certificate(internal::Utilities::byteVectorToString(mGossipCaCertificate));

  if (mGrpcCertificateHash.has_value())
  {
    body->set_grpc_certificate_hash(internal::Utilities::byteVectorToString(mGrpcCertificateHash.value()));
  }

  body->set_allocated_admin_key(mAdminKey->toProtobufKey().release());

  return body.release();
}

} // namespace Hedera