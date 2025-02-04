// SPDX-License-Identifier: Apache-2.0
#include "TokenUnpauseTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_unpause.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenUnpauseTransaction::TokenUnpauseTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenUnpauseTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenUnpauseTransaction::TokenUnpauseTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenUnpauseTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenUnpauseTransaction& TokenUnpauseTransaction::setTokenId(const TokenId& tokenId)
{
  requireNotFrozen();
  mTokenId = tokenId;
  return *this;
}

//-----
grpc::Status TokenUnpauseTransaction::submitRequest(const proto::Transaction& request,
                                                    const std::shared_ptr<internal::Node>& node,
                                                    const std::chrono::system_clock::time_point& deadline,
                                                    proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenUnpause, request, deadline, response);
}

//-----
void TokenUnpauseTransaction::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);
}

//-----
void TokenUnpauseTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_token_unpause(build());
}

//-----
void TokenUnpauseTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_token_unpause())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenUnpause data");
  }

  const proto::TokenUnpauseTransactionBody& body = transactionBody.token_unpause();

  if (body.has_token())
  {
    mTokenId = TokenId::fromProtobuf(body.token());
  }
}

//-----
proto::TokenUnpauseTransactionBody* TokenUnpauseTransaction::build() const
{
  auto body = std::make_unique<proto::TokenUnpauseTransactionBody>();

  if (!(mTokenId == TokenId()))
  {
    body->set_allocated_token(mTokenId.toProtobuf().release());
  }

  return body.release();
}

} // namespace Hiero
