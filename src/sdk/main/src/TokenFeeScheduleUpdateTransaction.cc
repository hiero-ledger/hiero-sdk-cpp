// SPDX-License-Identifier: Apache-2.0
#include "TokenFeeScheduleUpdateTransaction.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_fee_schedule_update.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TokenFeeScheduleUpdateTransaction::TokenFeeScheduleUpdateTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenFeeScheduleUpdateTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenFeeScheduleUpdateTransaction::TokenFeeScheduleUpdateTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenFeeScheduleUpdateTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenFeeScheduleUpdateTransaction& TokenFeeScheduleUpdateTransaction::setTokenId(const TokenId& tokenId)
{
  requireNotFrozen();
  mTokenId = tokenId;
  return *this;
}

//-----
TokenFeeScheduleUpdateTransaction& TokenFeeScheduleUpdateTransaction::setCustomFees(
  const std::vector<std::shared_ptr<CustomFee>>& fees)
{
  requireNotFrozen();
  mCustomFees = fees;
  return *this;
}

//-----
grpc::Status TokenFeeScheduleUpdateTransaction::submitRequest(const proto::Transaction& request,
                                                              const std::shared_ptr<internal::Node>& node,
                                                              const std::chrono::system_clock::time_point& deadline,
                                                              proto::TransactionResponse* response) const
{
  return node->submitTransaction(
    proto::TransactionBody::DataCase::kTokenFeeScheduleUpdate, request, deadline, response);
}

//-----
void TokenFeeScheduleUpdateTransaction::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);

  std::for_each(mCustomFees.cbegin(),
                mCustomFees.cend(),
                [&client](const std::shared_ptr<CustomFee>& fee) { fee->validateChecksums(client); });
}

//-----
void TokenFeeScheduleUpdateTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_token_fee_schedule_update(build());
}

//-----
void TokenFeeScheduleUpdateTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_token_fee_schedule_update())
  {
    throw std::invalid_argument("Transaction body doesn't contain TokenFeeScheduleUpdate data");
  }

  const proto::TokenFeeScheduleUpdateTransactionBody& body = transactionBody.token_fee_schedule_update();

  if (body.has_token_id())
  {
    mTokenId = TokenId::fromProtobuf(body.token_id());
  }

  for (int i = 0; i < body.custom_fees_size(); ++i)
  {
    mCustomFees.push_back(CustomFee::fromProtobuf(body.custom_fees(i)));
  }
}

//-----
proto::TokenFeeScheduleUpdateTransactionBody* TokenFeeScheduleUpdateTransaction::build() const
{
  auto body = std::make_unique<proto::TokenFeeScheduleUpdateTransactionBody>();

  if (!(mTokenId == TokenId()))
  {
    body->set_allocated_token_id(mTokenId.toProtobuf().release());
  }

  for (const auto& fee : mCustomFees)
  {
    *body->add_custom_fees() = *fee->toProtobuf();
  }

  return body.release();
}

} // namespace Hiero