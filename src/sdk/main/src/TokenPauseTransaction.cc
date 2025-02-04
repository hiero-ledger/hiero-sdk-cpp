// SPDX-License-Identifier: Apache-2.0
#include "TokenPauseTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_pause.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenPauseTransaction::TokenPauseTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenPauseTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenPauseTransaction::TokenPauseTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenPauseTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenPauseTransaction& TokenPauseTransaction::setTokenId(const TokenId& tokenId)
{
  requireNotFrozen();
  mTokenId = tokenId;
  return *this;
}

//-----
grpc::Status TokenPauseTransaction::submitRequest(const proto::Transaction& request,
                                                  const std::shared_ptr<internal::Node>& node,
                                                  const std::chrono::system_clock::time_point& deadline,
                                                  proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenPause, request, deadline, response);
}

//-----
void TokenPauseTransaction::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);
}

//-----
void TokenPauseTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_token_pause(build());
}

//-----
void TokenPauseTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_token_pause())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenPause data");
  }

  const proto::TokenPauseTransactionBody& body = transactionBody.token_pause();

  if (body.has_token())
  {
    mTokenId = TokenId::fromProtobuf(body.token());
  }
}

//-----
proto::TokenPauseTransactionBody* TokenPauseTransaction::build() const
{
  auto body = std::make_unique<proto::TokenPauseTransactionBody>();

  if (!(mTokenId == TokenId()))
  {
    body->set_allocated_token(mTokenId.toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero
