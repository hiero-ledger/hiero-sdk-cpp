// SPDX-License-Identifier: Apache-2.0
#include "TokenCancelAirdropTransaction.h"
#include "AccountId.h"
#include "NftId.h"
#include "TokenId.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_cancel_airdrop.pb.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
TokenCancelAirdropTransaction::TokenCancelAirdropTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenCancelAirdropTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenCancelAirdropTransaction::TokenCancelAirdropTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenCancelAirdropTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenCancelAirdropTransaction& TokenCancelAirdropTransaction::setPendingAirdrops(
  const std::vector<PendingAirdropId>& pendingAirdrops)
{
  requireNotFrozen();
  mPendingAirdrops = pendingAirdrops;
  return *this;
}

//-----
grpc::Status TokenCancelAirdropTransaction::submitRequest(const proto::Transaction& request,
                                                          const std::shared_ptr<internal::Node>& node,
                                                          const std::chrono::system_clock::time_point& deadline,
                                                          proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenCancelAirdrop, request, deadline, response);
}

//-----
void TokenCancelAirdropTransaction::validateChecksums(const Client& client) const {}

//-----
void TokenCancelAirdropTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokencancelairdrop(build());
}

//-----
void TokenCancelAirdropTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokencancelairdrop())
  {
    throw std::invalid_argument("Transaction body doesn't contain Token Cancel Airdrop data");
  }

  const proto::TokenCancelAirdropTransactionBody& body = transactionBody.tokencancelairdrop();

  for (int i = 0; i < body.pending_airdrops_size(); i++)
  {
    mPendingAirdrops.push_back(PendingAirdropId::fromProtobuf(body.pending_airdrops(i)));
  }
}

//-----
proto::TokenCancelAirdropTransactionBody* TokenCancelAirdropTransaction::build() const
{
  auto body = std::make_unique<proto::TokenCancelAirdropTransactionBody>();

  for (const PendingAirdropId& pendingAirdrop : mPendingAirdrops)
  {
    body->mutable_pending_airdrops()->AddAllocated(pendingAirdrop.toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero