// SPDX-License-Identifier: Apache-2.0
#include "TokenAssociateTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_associate.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenAssociateTransaction::TokenAssociateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenAssociateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenAssociateTransaction::TokenAssociateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenAssociateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenAssociateTransaction& TokenAssociateTransaction::setAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAccountId = accountId;
  return *this;
}

//-----
TokenAssociateTransaction& TokenAssociateTransaction::setTokenIds(const std::vector<TokenId>& tokenIds)
{
  requireNotFrozen();
  mTokenIds = tokenIds;
  return *this;
}

//-----
grpc::Status TokenAssociateTransaction::submitRequest(const proto::Transaction& request,
                                                      const std::shared_ptr<internal::Node>& node,
                                                      const std::chrono::system_clock::time_point& deadline,
                                                      proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenAssociate, request, deadline, response);
}

//-----
void TokenAssociateTransaction::validateChecksums(const Client& client) const
{
  if (mAccountId.has_value())
  {
    mAccountId->validateChecksum(client);
  }

  std::for_each(
    mTokenIds.cbegin(), mTokenIds.cend(), [&client](const TokenId& tokenId) { tokenId.validateChecksum(client); });
}

//-----
void TokenAssociateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokenassociate(build());
}

//-----
void TokenAssociateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokenassociate())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenAssociate data");
  }

  const proto::TokenAssociateTransactionBody& body = transactionBody.tokenassociate();

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
proto::TokenAssociateTransactionBody* TokenAssociateTransaction::build() const
{
  auto body = std::make_unique<proto::TokenAssociateTransactionBody>();

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
