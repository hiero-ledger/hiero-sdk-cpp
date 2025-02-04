// SPDX-License-Identifier: Apache-2.0
#include "TokenBurnTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_burn.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenBurnTransaction::TokenBurnTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenBurnTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenBurnTransaction::TokenBurnTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenBurnTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenBurnTransaction& TokenBurnTransaction::setTokenId(const TokenId& tokenId)
{
  requireNotFrozen();
  mTokenId = tokenId;
  return *this;
}

//-----
TokenBurnTransaction& TokenBurnTransaction::setAmount(uint64_t amount)
{
  requireNotFrozen();
  mAmount = amount;
  return *this;
}

//-----
TokenBurnTransaction& TokenBurnTransaction::setSerialNumbers(const std::vector<uint64_t>& serialNumbers)
{
  requireNotFrozen();
  mSerialNumbers = serialNumbers;
  return *this;
}

//-----
grpc::Status TokenBurnTransaction::submitRequest(const proto::Transaction& request,
                                                 const std::shared_ptr<internal::Node>& node,
                                                 const std::chrono::system_clock::time_point& deadline,
                                                 proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenBurn, request, deadline, response);
}

//-----
void TokenBurnTransaction::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);
}

//-----
void TokenBurnTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokenburn(build());
}

//-----
void TokenBurnTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokenburn())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenBurn data");
  }

  const proto::TokenBurnTransactionBody& body = transactionBody.tokenburn();

  if (body.has_token())
  {
    mTokenId = TokenId::fromProtobuf(body.token());
  }

  mAmount = body.amount();

  for (int i = 0; i < body.serialnumbers_size(); ++i)
  {
    mSerialNumbers.push_back(static_cast<uint64_t>(body.serialnumbers(i)));
  }
}

//-----
proto::TokenBurnTransactionBody* TokenBurnTransaction::build() const
{
  auto body = std::make_unique<proto::TokenBurnTransactionBody>();

  if (!(mTokenId == TokenId()))
  {
    body->set_allocated_token(mTokenId.toProtobuf().release());
  }

  body->set_amount(mAmount);

  for (const auto& num : mSerialNumbers)
  {
    body->add_serialnumbers(static_cast<int64_t>(num));
  }

  return body.release();
}

} // namespace Hiero
