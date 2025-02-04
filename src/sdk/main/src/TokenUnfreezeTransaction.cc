// SPDX-License-Identifier: Apache-2.0
#include "TokenUnfreezeTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_unfreeze_account.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenUnfreezeTransaction::TokenUnfreezeTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenUnfreezeTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenUnfreezeTransaction::TokenUnfreezeTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenUnfreezeTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenUnfreezeTransaction& TokenUnfreezeTransaction::setAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAccountId = accountId;
  return *this;
}

//-----
TokenUnfreezeTransaction& TokenUnfreezeTransaction::setTokenId(const TokenId& tokenId)
{
  requireNotFrozen();
  mTokenId = tokenId;
  return *this;
}

//-----
grpc::Status TokenUnfreezeTransaction::submitRequest(const proto::Transaction& request,
                                                     const std::shared_ptr<internal::Node>& node,
                                                     const std::chrono::system_clock::time_point& deadline,
                                                     proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenUnfreeze, request, deadline, response);
}

//-----
void TokenUnfreezeTransaction::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);
  mAccountId.validateChecksum(client);
}

//-----
void TokenUnfreezeTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokenunfreeze(build());
}

//-----
void TokenUnfreezeTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokenunfreeze())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenUnfreeze data");
  }

  const proto::TokenUnfreezeAccountTransactionBody& body = transactionBody.tokenunfreeze();

  if (body.has_account())
  {
    mAccountId = AccountId::fromProtobuf(body.account());
  }

  if (body.has_token())
  {
    mTokenId = TokenId::fromProtobuf(body.token());
  }
}

//-----
proto::TokenUnfreezeAccountTransactionBody* TokenUnfreezeTransaction::build() const
{
  auto body = std::make_unique<proto::TokenUnfreezeAccountTransactionBody>();

  if (!(mAccountId == AccountId()))
  {
    body->set_allocated_account(mAccountId.toProtobuf().release());
  }

  if (!(mTokenId == TokenId()))
  {
    body->set_allocated_token(mTokenId.toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero