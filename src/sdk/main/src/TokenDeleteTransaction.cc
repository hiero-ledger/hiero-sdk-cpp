// SPDX-License-Identifier: Apache-2.0
#include "TokenDeleteTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_delete.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenDeleteTransaction::TokenDeleteTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenDeleteTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenDeleteTransaction::TokenDeleteTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenDeleteTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenDeleteTransaction& TokenDeleteTransaction::setTokenId(const TokenId& tokenId)
{
  requireNotFrozen();
  mTokenId = tokenId;
  return *this;
}

//-----
grpc::Status TokenDeleteTransaction::submitRequest(const proto::Transaction& request,
                                                   const std::shared_ptr<internal::Node>& node,
                                                   const std::chrono::system_clock::time_point& deadline,
                                                   proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenDeletion, request, deadline, response);
}

//-----
void TokenDeleteTransaction::validateChecksums(const Client& client) const
{
  if (mTokenId.has_value())
  {
    mTokenId->validateChecksum(client);
  }
}

//-----
void TokenDeleteTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokendeletion(build());
}

//-----
void TokenDeleteTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokendeletion())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenDelete data");
  }

  const proto::TokenDeleteTransactionBody& body = transactionBody.tokendeletion();

  if (body.has_token())
  {
    mTokenId = TokenId::fromProtobuf(body.token());
  }
}

//-----
proto::TokenDeleteTransactionBody* TokenDeleteTransaction::build() const
{
  auto body = std::make_unique<proto::TokenDeleteTransactionBody>();

  if (mTokenId.has_value())
  {
    body->set_allocated_token(mTokenId->toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero