// SPDX-License-Identifier: Apache-2.0
#include "TokenAirdropTransaction.h"
#include "AccountId.h"
#include "NftId.h"
#include "TokenId.h"
#include "impl/Node.h"

#include <grpcpp/client_context.h>
#include <stdexcept>
#include <token_airdrop.pb.h>
#include <transaction.pb.h>
#include <transaction_body.pb.h>

namespace Hiero
{
//-----
TokenAirdropTransaction::TokenAirdropTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TokenAirdropTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TokenAirdropTransaction::TokenAirdropTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TokenAirdropTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TokenAirdropTransaction& TokenAirdropTransaction::addTokenTransfer(const TokenId& tokenId,
                                                                   const AccountId& accountId,
                                                                   const int64_t& amount)
{
  requireNotFrozen();

  doTokenTransfer(TokenTransfer(tokenId, accountId, amount, false));
  return *this;
}

//-----
TokenAirdropTransaction& TokenAirdropTransaction::addNftTransfer(const NftId& nftId,
                                                                 const AccountId& senderAccountId,
                                                                 const AccountId& receiverAccountId)
{
  requireNotFrozen();

  doNftTransfer(nftId, senderAccountId, receiverAccountId, false);
  return *this;
}

//-----
TokenAirdropTransaction& TokenAirdropTransaction::addTokenTransferWithDecimals(const TokenId& tokenId,
                                                                               const AccountId& accountId,
                                                                               const int64_t& amount,
                                                                               uint32_t decimals)
{
  requireNotFrozen();

  doTokenTransfer(TokenTransfer(tokenId, accountId, amount, decimals, false));
  return *this;
}

//-----
TokenAirdropTransaction& TokenAirdropTransaction::addApprovedTokenTransfer(const TokenId& tokenId,
                                                                           const AccountId& accountId,
                                                                           const int64_t& amount)
{
  requireNotFrozen();

  doTokenTransfer(TokenTransfer(tokenId, accountId, amount, true));
  return *this;
}

//-----
TokenAirdropTransaction& TokenAirdropTransaction::addApprovedNftTransfer(const NftId& nftId,
                                                                         const AccountId& senderAccountId,
                                                                         const AccountId& receiverAccountId)
{
  requireNotFrozen();

  doNftTransfer(nftId, senderAccountId, receiverAccountId, true);
  return *this;
}

//-----
TokenAirdropTransaction& TokenAirdropTransaction::addApprovedTokenTransferWithDecimals(const TokenId& tokenId,
                                                                                       const AccountId& accountId,
                                                                                       const int64_t& amount,
                                                                                       uint32_t decimals)
{
  requireNotFrozen();

  doTokenTransfer(TokenTransfer(tokenId, accountId, amount, decimals, true));
  return *this;
}

//-----
std::unordered_map<TokenId, std::unordered_map<AccountId, int64_t>> TokenAirdropTransaction::getTokenTransfers() const
{
  std::unordered_map<TokenId, std::unordered_map<AccountId, int64_t>> tokenTransfers;

  for (const TokenTransfer& transfer : mTokenTransfers)
  {
    tokenTransfers[transfer.mTokenId][transfer.mAccountId] += transfer.mAmount;
  }

  return tokenTransfers;
}

//-----
std::unordered_map<TokenId, std::vector<TokenNftTransfer>> TokenAirdropTransaction::getNftTransfers() const
{
  std::unordered_map<TokenId, std::vector<TokenNftTransfer>> nftTransfers;

  for (const TokenNftTransfer& transfer : mNftTransfers)
  {
    nftTransfers[transfer.mNftId.mTokenId].push_back(transfer);
  }

  return nftTransfers;
}

//-----
std::unordered_map<TokenId, uint32_t> TokenAirdropTransaction::getTokenIdDecimals() const
{
  std::unordered_map<TokenId, uint32_t> decimals;

  for (const TokenTransfer& transfer : mTokenTransfers)
  {
    if (transfer.mExpectedDecimals != 0U)
    {
      decimals[transfer.mTokenId] = transfer.mExpectedDecimals;
    }
  }

  return decimals;
}

//-----
grpc::Status TokenAirdropTransaction::submitRequest(const proto::Transaction& request,
                                                    const std::shared_ptr<internal::Node>& node,
                                                    const std::chrono::system_clock::time_point& deadline,
                                                    proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kTokenAirdrop, request, deadline, response);
}

//-----
void TokenAirdropTransaction::validateChecksums(const Client& client) const {}

//-----
void TokenAirdropTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_tokenairdrop(build());
}

//-----
void TokenAirdropTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_tokenairdrop())
  {
    throw std::invalid_argument("Transaction body doesn't contain Token Airdrop data");
  }

  const proto::TokenAirdropTransactionBody& body = transactionBody.tokenairdrop();

  for (int i = 0; i < body.token_transfers_size(); ++i)
  {
    const proto::TokenTransferList& transfer = body.token_transfers(i);
    const TokenId tokenId = TokenId::fromProtobuf(transfer.token());

    for (int j = 0; j < transfer.transfers_size(); ++j)
    {
      const proto::AccountAmount& accountAmount = transfer.transfers(j);
      mTokenTransfers.emplace_back(tokenId,
                                   AccountId::fromProtobuf(accountAmount.accountid()),
                                   accountAmount.amount(),
                                   transfer.has_expected_decimals() ? transfer.expected_decimals().value() : 0U,
                                   accountAmount.is_approval());
    }

    for (int j = 0; j < transfer.nfttransfers_size(); ++j)
    {
      mNftTransfers.emplace_back(TokenNftTransfer::fromProtobuf(transfer.nfttransfers(j), tokenId));
    }
  }
}

//-----
proto::TokenAirdropTransactionBody* TokenAirdropTransaction::build() const
{
  auto body = std::make_unique<proto::TokenAirdropTransactionBody>();

  for (const TokenTransfer& transfer : mTokenTransfers)
  {
    // If this token already has a transfer, grab that token's list. Otherwise, add this token
    proto::TokenTransferList* list = nullptr;
    for (int i = 0; i < body->mutable_token_transfers()->size(); ++i)
    {
      if (TokenId::fromProtobuf(body->mutable_token_transfers(i)->token()) == transfer.mTokenId)
      {
        list = body->mutable_token_transfers(i);
      }
    }

    if (!list)
    {
      list = body->add_token_transfers();
      list->set_allocated_token(transfer.mTokenId.toProtobuf().release());
    }

    proto::AccountAmount* amount = list->add_transfers();
    amount->set_allocated_accountid(transfer.mAccountId.toProtobuf().release());
    amount->set_amount(transfer.mAmount);
    amount->set_is_approval(transfer.mIsApproval);

    // Shouldn't ever overwrite a different value here because it is checked when the transfer is added to this
    // TransferTransaction.
    list->mutable_expected_decimals()->set_value(transfer.mExpectedDecimals);
  }

  for (const TokenNftTransfer& transfer : mNftTransfers)
  {
    // If this token already has a transfer, grab that token's list. Otherwise, add this token
    proto::TokenTransferList* list = nullptr;
    for (int i = 0; i < body->mutable_token_transfers()->size(); ++i)
    {
      if (TokenId::fromProtobuf(body->mutable_token_transfers(i)->token()) == transfer.mNftId.mTokenId)
      {
        list = body->mutable_token_transfers(i);
      }
    }

    if (!list)
    {
      list = body->add_token_transfers();
      list->set_allocated_token(transfer.mNftId.mTokenId.toProtobuf().release());
    }

    *list->add_nfttransfers() = *transfer.toProtobuf();
  }

  return body.release();
}

//-----
void TokenAirdropTransaction::doTokenTransfer(const TokenTransfer& transfer)
{
  for (auto transferIter = mTokenTransfers.begin(); transferIter != mTokenTransfers.end(); ++transferIter)
  {
    if (transferIter->mTokenId == transfer.mTokenId && transferIter->mAccountId == transfer.mAccountId &&
        transferIter->mIsApproval == transfer.mIsApproval)
    {
      if (transferIter->mExpectedDecimals != transfer.mExpectedDecimals)
      {
        throw std::invalid_argument("Expected decimals for token do not match previously set decimals");
      }
      else if (const int64_t newAmount = transferIter->mAmount + transfer.mAmount; newAmount == 0LL)
      {
        mTokenTransfers.erase(transferIter);
      }
      else
      {
        transferIter->mAmount = newAmount;
      }

      return;
    }
  }

  mTokenTransfers.push_back(transfer);
}

//-----
void TokenAirdropTransaction::doNftTransfer(const NftId& nftId,
                                            const AccountId& sender,
                                            const AccountId& receiver,
                                            bool approved)
{
  for (auto transferIter = mNftTransfers.begin(); transferIter != mNftTransfers.end(); ++transferIter)
  {
    if (transferIter->mNftId.mSerialNum == nftId.mSerialNum && transferIter->mSenderAccountId == receiver &&
        transferIter->mReceiverAccountId == sender && transferIter->mIsApproval == approved)
    {
      mNftTransfers.erase(transferIter);
      return;
    }
  }

  mNftTransfers.emplace_back(nftId, sender, receiver, approved);
}

} // namespace Hiero