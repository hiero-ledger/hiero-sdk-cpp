// SPDX-License-Identifier: Apache-2.0
#include "TransferTransaction.h"
#include "TransactionResponse.h"
#include "impl/Node.h"

#include <crypto_transfer.pb.h>
#include <response.pb.h>
#include <transaction.pb.h>

namespace Hiero
{
//-----
TransferTransaction::TransferTransaction(const proto::TransactionBody& transactionBody)
  : Transaction<TransferTransaction>(transactionBody)
{
  initFromSourceTransactionBody();
}

//-----
TransferTransaction::TransferTransaction(
  const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions)
  : Transaction<TransferTransaction>(transactions)
{
  initFromSourceTransactionBody();
}

//-----
TransferTransaction& TransferTransaction::addHbarTransfer(const AccountId& accountId, const Hbar& amount)
{
  requireNotFrozen();

  doHbarTransfer(accountId, amount, false);
  return *this;
}

//-----
TransferTransaction& TransferTransaction::addHbarTransfer(const EvmAddress& evmAddress, const Hbar& amount)
{
  requireNotFrozen();
  return addHbarTransfer(AccountId::fromEvmAddress(evmAddress), amount);
}

//-----
TransferTransaction& TransferTransaction::addTokenTransfer(const TokenId& tokenId,
                                                           const AccountId& accountId,
                                                           const int64_t& amount)
{
  requireNotFrozen();

  doTokenTransfer(TokenTransfer(tokenId, accountId, amount, false));
  return *this;
}

//-----
TransferTransaction& TransferTransaction::addNftTransfer(const NftId& nftId,
                                                         const AccountId& senderAccountId,
                                                         const AccountId& receiverAccountId)
{
  requireNotFrozen();

  doNftTransfer(nftId, senderAccountId, receiverAccountId, false);
  return *this;
}

//-----
TransferTransaction& TransferTransaction::addTokenTransferWithDecimals(const TokenId& tokenId,
                                                                       const AccountId& accountId,
                                                                       const int64_t& amount,
                                                                       uint32_t decimals)
{
  requireNotFrozen();

  doTokenTransfer(TokenTransfer(tokenId, accountId, amount, decimals, false));
  return *this;
}

//-----
TransferTransaction& TransferTransaction::addApprovedHbarTransfer(const AccountId& accountId, const Hbar& amount)
{
  requireNotFrozen();

  doHbarTransfer(accountId, amount, true);
  return *this;
}

//-----
TransferTransaction& TransferTransaction::addApprovedTokenTransfer(const TokenId& tokenId,
                                                                   const AccountId& accountId,
                                                                   const int64_t& amount)
{
  requireNotFrozen();

  doTokenTransfer(TokenTransfer(tokenId, accountId, amount, true));
  return *this;
}

//-----
TransferTransaction& TransferTransaction::addApprovedNftTransfer(const NftId& nftId,
                                                                 const AccountId& senderAccountId,
                                                                 const AccountId& receiverAccountId)
{
  requireNotFrozen();

  doNftTransfer(nftId, senderAccountId, receiverAccountId, true);
  return *this;
}

//-----
TransferTransaction& TransferTransaction::addApprovedTokenTransferWithDecimals(const TokenId& tokenId,
                                                                               const AccountId& accountId,
                                                                               const int64_t& amount,
                                                                               uint32_t decimals)
{
  requireNotFrozen();

  doTokenTransfer(TokenTransfer(tokenId, accountId, amount, decimals, true));
  return *this;
}

//-----
std::unordered_map<AccountId, Hbar> TransferTransaction::getHbarTransfers() const
{
  std::unordered_map<AccountId, Hbar> hbarTransfers;

  for (const HbarTransfer& transfer : mHbarTransfers)
  {
    hbarTransfers[transfer.mAccountId] += transfer.mAmount;
  }

  return hbarTransfers;
}

//-----
std::unordered_map<TokenId, std::unordered_map<AccountId, int64_t>> TransferTransaction::getTokenTransfers() const
{
  std::unordered_map<TokenId, std::unordered_map<AccountId, int64_t>> tokenTransfers;

  for (const TokenTransfer& transfer : mTokenTransfers)
  {
    tokenTransfers[transfer.mTokenId][transfer.mAccountId] += transfer.mAmount;
  }

  return tokenTransfers;
}

//-----
std::unordered_map<TokenId, std::vector<TokenNftTransfer>> TransferTransaction::getNftTransfers() const
{
  std::unordered_map<TokenId, std::vector<TokenNftTransfer>> nftTransfers;

  for (const TokenNftTransfer& transfer : mNftTransfers)
  {
    nftTransfers[transfer.mNftId.mTokenId].push_back(transfer);
  }

  return nftTransfers;
}

//-----
std::unordered_map<TokenId, uint32_t> TransferTransaction::getTokenIdDecimals() const
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
grpc::Status TransferTransaction::submitRequest(const proto::Transaction& request,
                                                const std::shared_ptr<internal::Node>& node,
                                                const std::chrono::system_clock::time_point& deadline,
                                                proto::TransactionResponse* response) const
{
  return node->submitTransaction(proto::TransactionBody::DataCase::kCryptoTransfer, request, deadline, response);
}

//-----
void TransferTransaction::validateChecksums(const Client& client) const
{
  std::for_each(mHbarTransfers.cbegin(),
                mHbarTransfers.cend(),
                [&client](const HbarTransfer& transfer) { transfer.mAccountId.validateChecksum(client); });
  std::for_each(mTokenTransfers.cbegin(),
                mTokenTransfers.cend(),
                [&client](const TokenTransfer& transfer) { transfer.validateChecksums(client); });
  std::for_each(mNftTransfers.cbegin(),
                mNftTransfers.cend(),
                [&client](const TokenNftTransfer& transfer) { transfer.validateChecksums(client); });
}

//-----
void TransferTransaction::addToBody(proto::TransactionBody& body) const
{
  body.set_allocated_cryptotransfer(build());
}

//-----
void TransferTransaction::initFromSourceTransactionBody()
{
  const proto::TransactionBody transactionBody = getSourceTransactionBody();

  if (!transactionBody.has_cryptotransfer())
  {
    throw std::invalid_argument("Transaction body doesn't contain CryptoTransfer data");
  }

  const proto::CryptoTransferTransactionBody& body = transactionBody.cryptotransfer();

  for (int i = 0; i < body.transfers().accountamounts_size(); ++i)
  {
    mHbarTransfers.push_back(HbarTransfer::fromProtobuf(body.transfers().accountamounts(i)));
  }

  for (int i = 0; i < body.tokentransfers_size(); ++i)
  {
    const proto::TokenTransferList& transfer = body.tokentransfers(i);
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
proto::CryptoTransferTransactionBody* TransferTransaction::build() const
{
  auto body = std::make_unique<proto::CryptoTransferTransactionBody>();

  for (const HbarTransfer& transfer : mHbarTransfers)
  {
    *body->mutable_transfers()->add_accountamounts() = *transfer.toProtobuf();
  }

  for (const TokenTransfer& transfer : mTokenTransfers)
  {
    // If this token already has a transfer, grab that token's list. Otherwise, add this token
    proto::TokenTransferList* list = nullptr;
    for (int i = 0; i < body->mutable_tokentransfers()->size(); ++i)
    {
      if (TokenId::fromProtobuf(body->mutable_tokentransfers(i)->token()) == transfer.mTokenId)
      {
        list = body->mutable_tokentransfers(i);
      }
    }

    if (!list)
    {
      list = body->add_tokentransfers();
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
    for (int i = 0; i < body->mutable_tokentransfers()->size(); ++i)
    {
      if (TokenId::fromProtobuf(body->mutable_tokentransfers(i)->token()) == transfer.mNftId.mTokenId)
      {
        list = body->mutable_tokentransfers(i);
      }
    }

    if (!list)
    {
      list = body->add_tokentransfers();
      list->set_allocated_token(transfer.mNftId.mTokenId.toProtobuf().release());
    }

    *list->add_nfttransfers() = *transfer.toProtobuf();
  }

  return body.release();
}

//----
void TransferTransaction::doHbarTransfer(const AccountId& accountId, const Hbar& amount, bool approved)
{
  // If a transfer has already been added for an account, just update the amount if the approval status is the same
  for (auto transferIter = mHbarTransfers.begin(); transferIter != mHbarTransfers.end(); ++transferIter)
  {
    if (transferIter->mAccountId == accountId && transferIter->mIsApproved == approved)
    {
      if (const auto newValue = Hbar(transferIter->mAmount.toTinybars() + amount.toTinybars(), HbarUnit::TINYBAR());
          newValue.toTinybars() == 0LL)
      {
        mHbarTransfers.erase(transferIter);
      }
      else
      {
        transferIter->mAmount = newValue;
      }

      return;
    }
  }

  mHbarTransfers.emplace_back(accountId, amount, approved);
}

//-----
void TransferTransaction::doTokenTransfer(const TokenTransfer& transfer)
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
void TransferTransaction::doNftTransfer(const NftId& nftId,
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
