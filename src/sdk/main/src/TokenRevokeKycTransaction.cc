// SPDX-License-Identifier: Apache-2.0
#include "TokenRevokeKycTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_revoke_kyc.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenRevokeKycTransaction::TokenRevokeKycTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenRevokeKycTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenRevokeKycTransaction::TokenRevokeKycTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenRevokeKycTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenRevokeKycTransaction& TokenRevokeKycTransaction::setAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAccountId = accountId;
  return *this;
}

//-----
TokenRevokeKycTransaction& TokenRevokeKycTransaction::setTokenId(const TokenId& tokenId)
{
  requireNotFrozen();
  mTokenId = tokenId;
  return *this;
}

//-----
grpc::Status TokenRevokeKycTransaction::submitRequest(const proto::Transaction& request,
                                                      const std::shared_ptr<internal::Node>& node,
                                                      const std::chrono::system_clock::time_point& deadline,
                                                      proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenRevokeKyc, request, deadline, response);
}

//-----
void TokenRevokeKycTransaction::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);
  mAccountId.validateChecksum(client);
}

//-----
void TokenRevokeKycTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokenrevokekyc(build());
}

//-----
void TokenRevokeKycTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokenrevokekyc())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenRevokeKyc data");
  }

  const proto::TokenRevokeKycTransactionBody& body = transactionBody.tokenrevokekyc();

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
proto::TokenRevokeKycTransactionBody* TokenRevokeKycTransaction::build() const
{
  auto body = std::make_unique<proto::TokenRevokeKycTransactionBody>();

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
