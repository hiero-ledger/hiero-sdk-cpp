// SPDX-License-Identifier: Apache-2.0
#include "TokenFreezeTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_freeze_account.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenFreezeTransaction::TokenFreezeTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenFreezeTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenFreezeTransaction::TokenFreezeTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenFreezeTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenFreezeTransaction& TokenFreezeTransaction::setAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAccountId = accountId;
  return *this;
}

//-----
TokenFreezeTransaction& TokenFreezeTransaction::setTokenId(const TokenId& tokenId)
{
  requireNotFrozen();
  mTokenId = tokenId;
  return *this;
}

//-----
grpc::Status TokenFreezeTransaction::submitRequest(const proto::Transaction& request,
                                                   const std::shared_ptr<internal::Node>& node,
                                                   const std::chrono::system_clock::time_point& deadline,
                                                   proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenFreeze, request, deadline, response);
}

//-----
void TokenFreezeTransaction::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);
  mAccountId.validateChecksum(client);
}

//-----
void TokenFreezeTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokenfreeze(build());
}

//-----
void TokenFreezeTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokenfreeze())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenFreeze data");
  }

  const proto::TokenFreezeAccountTransactionBody& body = transactionBody.tokenfreeze();

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
proto::TokenFreezeAccountTransactionBody* TokenFreezeTransaction::build() const
{
  auto body = std::make_unique<proto::TokenFreezeAccountTransactionBody>();

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
