// SPDX-License-Identifier: Apache-2.0
#include "TokenGrantKycTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_grant_kyc.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenGrantKycTransaction::TokenGrantKycTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenGrantKycTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenGrantKycTransaction::TokenGrantKycTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenGrantKycTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenGrantKycTransaction& TokenGrantKycTransaction::setAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAccountId = accountId;
  return *this;
}

//-----
TokenGrantKycTransaction& TokenGrantKycTransaction::setTokenId(const TokenId& tokenId)
{
  requireNotFrozen();
  mTokenId = tokenId;
  return *this;
}

//-----
grpc::Status TokenGrantKycTransaction::submitRequest(const proto::Transaction& request,
                                                     const std::shared_ptr<internal::Node>& node,
                                                     const std::chrono::system_clock::time_point& deadline,
                                                     proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenGrantKyc, request, deadline, response);
}

//-----
void TokenGrantKycTransaction::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);
  mAccountId.validateChecksum(client);
}

//-----
void TokenGrantKycTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokengrantkyc(build());
}

//-----
void TokenGrantKycTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokengrantkyc())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenGrantKyc data");
  }

  const proto::TokenGrantKycTransactionBody& body = transactionBody.tokengrantkyc();

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
proto::TokenGrantKycTransactionBody* TokenGrantKycTransaction::build() const
{
  auto body = std::make_unique<proto::TokenGrantKycTransactionBody>();

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
