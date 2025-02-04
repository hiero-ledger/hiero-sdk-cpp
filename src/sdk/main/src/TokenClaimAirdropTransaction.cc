// SPDX-License-Identifier: Apache-2.0
#include "TokenClaimAirdropTransaction.h"
#include "AccountId.h"
#include "NftId.h"
#include "TokenId.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_claim_airdrop.pb.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
TokenClaimAirdropTransaction::TokenClaimAirdropTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenClaimAirdropTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenClaimAirdropTransaction::TokenClaimAirdropTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenClaimAirdropTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenClaimAirdropTransaction& TokenClaimAirdropTransaction::setPendingAirdrops(
  const std::vector<PendingAirdropId>& pendingAirdrops)
{
  requireNotFrozen();
  mPendingAirdrops = pendingAirdrops;
  return *this;
}

//-----
grpc::Status TokenClaimAirdropTransaction::submitRequest(const proto::Transaction& request,
                                                         const std::shared_ptr<internal::Node>& node,
                                                         const std::chrono::system_clock::time_point& deadline,
                                                         proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenClaimAirdrop, request, deadline, response);
}

//-----
void TokenClaimAirdropTransaction::validateChecksums(const Client& client) const {}

//-----
void TokenClaimAirdropTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokenclaimairdrop(build());
}

//-----
void TokenClaimAirdropTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokenclaimairdrop())
  {
    throw std::invalid_argument("Transaction body doesn't contain Token Claim Airdrop data");
  }

  const proto::TokenClaimAirdropTransactionBody& body = transactionBody.tokenclaimairdrop();

  for (int i = 0; i < body.pending_airdrops_size(); i++)
  {
    mPendingAirdrops.push_back(PendingAirdropId::fromProtobuf(body.pending_airdrops(i)));
  }
}

//-----
proto::TokenClaimAirdropTransactionBody* TokenClaimAirdropTransaction::build() const
{
  auto body = std::make_unique<proto::TokenClaimAirdropTransactionBody>();

  for (const PendingAirdropId& pendingAirdrop : mPendingAirdrops)
  {
    body->mutable_pending_airdrops()->AddAllocated(pendingAirdrop.toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero