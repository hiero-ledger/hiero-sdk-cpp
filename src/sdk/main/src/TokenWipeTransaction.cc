// SPDX-License-Identifier: Apache-2.0
#include "TokenWipeTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_wipe_account.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenWipeTransaction::TokenWipeTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenWipeTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenWipeTransaction::TokenWipeTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenWipeTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenWipeTransaction& TokenWipeTransaction::setTokenId(const TokenId& tokenId)
{
  requireNotFrozen();
  mTokenId = tokenId;
  return *this;
}

//-----
TokenWipeTransaction& TokenWipeTransaction::setAccountId(const AccountId& accountId)
{
  requireNotFrozen();
  mAccountId = accountId;
  return *this;
}

//-----
TokenWipeTransaction& TokenWipeTransaction::setAmount(uint64_t amount)
{
  requireNotFrozen();
  mAmount = amount;
  return *this;
}

//-----
TokenWipeTransaction& TokenWipeTransaction::setSerialNumbers(const std::vector<uint64_t>& serialNumbers)
{
  requireNotFrozen();
  mSerialNumbers = serialNumbers;
  return *this;
}

//-----
grpc::Status TokenWipeTransaction::submitRequest(const proto::Transaction& request,
                                                 const std::shared_ptr<internal::Node>& node,
                                                 const std::chrono::system_clock::time_point& deadline,
                                                 proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenWipe, request, deadline, response);
}

//-----
void TokenWipeTransaction::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);
  mAccountId.validateChecksum(client);
}

//-----
void TokenWipeTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokenwipe(build());
}

//-----
void TokenWipeTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokenwipe())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenWipe data");
  }

  const proto::TokenWipeAccountTransactionBody& body = transactionBody.tokenwipe();

  if (body.has_token())
  {
    mTokenId = TokenId::fromProtobuf(body.token());
  }

  if (body.has_account())
  {
    mAccountId = AccountId::fromProtobuf(body.account());
  }

  mAmount = body.amount();

  for (int i = 0; i < body.serialnumbers_size(); ++i)
  {
    mSerialNumbers.push_back(static_cast<uint64_t>(body.serialnumbers(i)));
  }
}

//-----
proto::TokenWipeAccountTransactionBody* TokenWipeTransaction::build() const
{
  auto body = std::make_unique<proto::TokenWipeAccountTransactionBody>();

  if (!(mTokenId == TokenId()))
  {
    body->set_allocated_token(mTokenId.toProtobuf().release());
  }

  if (!(mAccountId == AccountId()))
  {
    body->set_allocated_account(mAccountId.toProtobuf().release());
  }

  body->set_amount(mAmount);

  for (const auto& num : mSerialNumbers)
  {
    body->add_serialnumbers(static_cast<int64_t>(num));
  }

  return body.release();
}

} // namespace Hiero