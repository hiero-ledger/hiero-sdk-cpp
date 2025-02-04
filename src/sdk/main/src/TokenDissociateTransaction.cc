// SPDX-License-Identifier: Apache-2.0
#include "TokenDissociateTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_dissociate.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenDissociateTransaction::TokenDissociateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenDissociateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenDissociateTransaction::TokenDissociateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenDissociateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenDissociateTransaction& TokenDissociateTransaction::setAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAccountId = accountId;
  return *this;
}

//-----
TokenDissociateTransaction& TokenDissociateTransaction::setTokenIds(const std::vector<TokenId>& tokenIds)
{
  requireNotFrozen();
  mTokenIds = tokenIds;
  return *this;
}

//-----
grpc::Status TokenDissociateTransaction::submitRequest(const proto::Transaction& request,
                                                       const std::shared_ptr<internal::Node>& node,
                                                       const std::chrono::system_clock::time_point& deadline,
                                                       proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenDissociate, request, deadline, response);
}

//-----
void TokenDissociateTransaction::validateChecksums(const Client& client) const
{
  if (mAccountId.has_value())
  {
    mAccountId->validateChecksum(client);
  }

  std::for_each(
    mTokenIds.cbegin(), mTokenIds.cend(), [&client](const TokenId& tokenId) { tokenId.validateChecksum(client); });
}

//-----
void TokenDissociateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokendissociate(build());
}

//-----
void TokenDissociateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokendissociate())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenDissociate data");
  }

  const proto::TokenDissociateTransactionBody& body = transactionBody.tokendissociate();

  if (body.has_account())
  {
    mAccountId = AccountId::fromProtobuf(body.account());
  }

  for (int i = 0; i < body.tokens_size(); ++i)
  {
    mTokenIds.push_back(TokenId::fromProtobuf(body.tokens(i)));
  }
}

//-----
proto::TokenDissociateTransactionBody* TokenDissociateTransaction::build() const
{
  auto body = std::make_unique<proto::TokenDissociateTransactionBody>();

  if (mAccountId.has_value())
  {
    body->set_allocated_account(mAccountId->toProtobuf().release());
  }

  for (const auto& token : mTokenIds)
  {
    *body->add_tokens() = *token.toProtobuf();
  }

  return body.release();
}

} // namespace Hiero
