// SPDX-License-Identifier: Apache-2.0
#include "AccountAllowanceDeleteTransaction.h"
#include "NftId.h"
#include "impl/Node.h"

#include <crypto_delete_allowance.pb.h>
#include <grpcpp/client_context.h>
#include <stdexcept>
#include <transaction.pb.h>
#include <transaction_response.pb.h>

namespace Hiero
{
//-----
AccountAllowanceDeleteTransaction::AccountAllowanceDeleteTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<AccountAllowanceDeleteTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
AccountAllowanceDeleteTransaction::AccountAllowanceDeleteTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<AccountAllowanceDeleteTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
AccountAllowanceDeleteTransaction& AccountAllowanceDeleteTransaction::deleteAllTokenNftAllowances(
  const NftId& nftId,
  const AccountId& owner)
{
  requireNotFrozen();

  // Add the serial number to the token allowance if there's already an allowance for this token ID, owner, and spender.
  for (TokenNftAllowance& allowance : mNftAllowanceDeletions)
  {
    if (allowance.mTokenId == nftId.mTokenId && allowance.mOwnerAccountId == owner)
    {
      allowance.mSerialNumbers.push_back(nftId.mSerialNum);
      return *this;
    }
  }

  mNftAllowanceDeletions.emplace_back(
    nftId.mTokenId, owner, std::optional<AccountId>(), std::vector<uint64_t>{ nftId.mSerialNum });
  return *this;
}

//-----
grpc::Status AccountAllowanceDeleteTransaction::submitRequest(const proto::Transaction& request,
                                                              const std::shared_ptr<internal::Node>& node,
                                                              const std::chrono::system_clock::time_point& deadline,
                                                              proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kCryptoDeleteAllowance, request, deadline, response);
}

//-----
void AccountAllowanceDeleteTransaction::validateChecksums(const Client& client) const
{
  std::for_each(mNftAllowanceDeletions.cbegin(),
                mNftAllowanceDeletions.cend(),
                [&client](const TokenNftAllowance& allowance) { allowance.validateChecksums(client); });
}

//-----
void AccountAllowanceDeleteTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_cryptodeleteallowance(build());
}

//-----
void AccountAllowanceDeleteTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_cryptodeleteallowance())
  {
    throw std::invalid_argument("Transaction body doesn't contain CryptoDeleteAllowance data");
  }

  const proto::CryptoDeleteAllowanceTransactionBody& body = transactionBody.cryptodeleteallowance();

  for (int i = 0; i < body.nftallowances_size(); ++i)
  {
    std::vector<uint64_t> serialNumbers;
    serialNumbers.reserve(body.nftallowances(i).serial_numbers_size());

    for (int j = 0; j < body.nftallowances(i).serial_numbers_size(); ++j)
    {
      serialNumbers.push_back(static_cast<uint64_t>(body.nftallowances(i).serial_numbers(j)));
    }

    mNftAllowanceDeletions.emplace_back(TokenId::fromProtobuf(body.nftallowances(i).token_id()),
                                        AccountId::fromProtobuf(body.nftallowances(i).owner()),
                                        std::optional<AccountId>(),
                                        serialNumbers);
  }
}

//-----
proto::CryptoDeleteAllowanceTransactionBody* AccountAllowanceDeleteTransaction::build() const
{
  auto body = std::make_unique<proto::CryptoDeleteAllowanceTransactionBody>();

  for (const TokenNftAllowance& allowance : mNftAllowanceDeletions)
  {
    proto::NftRemoveAllowance* nftRemoveAllowance = body->add_nftallowances();

    if (allowance.mTokenId.has_value())
    {
      nftRemoveAllowance->set_allocated_token_id(allowance.mTokenId->toProtobuf().release());
    }

    if (allowance.mOwnerAccountId.has_value())
    {
      nftRemoveAllowance->set_allocated_owner(allowance.mOwnerAccountId->toProtobuf().release());
    }

    for (const uint64_t& num : allowance.mSerialNumbers)
    {
      nftRemoveAllowance->add_serial_numbers(static_cast<int64_t>(num));
    }
  }

  return body.release();
}

} // namespace Hiero
