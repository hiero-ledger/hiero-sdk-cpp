// SPDX-License-Identifier: Apache-2.0
#include "account/AccountService.h"
#include "account/params/ApproveAllowanceParams.h"
#include "account/params/CreateAccountParams.h"
#include "account/params/DeleteAccountParams.h"
#include "account/params/DeleteAllowanceParams.h"
#include "account/params/TransferCryptoParams.h"
#include "account/params/UpdateAccountParams.h"
#include "account/params/allowance/AllowanceParams.h"
#include "account/params/allowance/RemoveAllowanceParams.h"
#include "account/params/transfer/TransferParams.h"
#include "common/CommonTransactionParams.h"
#include "key/KeyService.h"
#include "sdk/SdkClient.h"

#include <AccountAllowanceApproveTransaction.h>
#include <AccountAllowanceDeleteTransaction.h>
#include <AccountCreateTransaction.h>
#include <AccountDeleteTransaction.h>
#include <AccountId.h>
#include <AccountUpdateTransaction.h>
#include <EvmAddress.h>
#include <HbarUnit.h>
#include <NftId.h>
#include <Status.h>
#include <TokenId.h>
#include <TransactionReceipt.h>
#include <TransactionResponse.h>
#include <TransferTransaction.h>
#include <impl/EntityIdHelper.h>

#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>

namespace Hiero::TCK::AccountService
{
//-----
nlohmann::json approveAllowance(const ApproveAllowanceParams& params)
{
  AccountAllowanceApproveTransaction accountAllowanceApproveTransaction;
  accountAllowanceApproveTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  for (const AllowanceParams& allowance : params.mAllowances)
  {
    const AccountId owner = AccountId::fromString(allowance.mOwnerAccountId);
    const AccountId spender = AccountId::fromString(allowance.mSpenderAccountId);

    if (allowance.mHbar.has_value())
    {
      accountAllowanceApproveTransaction.approveHbarAllowance(
        owner, spender, Hbar::fromTinybars(internal::EntityIdHelper::getNum<int64_t>(allowance.mHbar->mAmount)));
    }
    else if (allowance.mToken.has_value())
    {
      accountAllowanceApproveTransaction.approveTokenAllowance(
        TokenId::fromString(allowance.mToken->mTokenId),
        owner,
        spender,
        internal::EntityIdHelper::getNum<int64_t>(allowance.mToken->mAmount));
    }
    else
    {
      if (allowance.mNft->mSerialNumbers.has_value())
      {
        for (const std::string& serialNumber : allowance.mNft->mSerialNumbers.value())
        {
          accountAllowanceApproveTransaction.approveTokenNftAllowance(
            NftId(TokenId::fromString(allowance.mNft->mTokenId), internal::EntityIdHelper::getNum(serialNumber)),
            owner,
            spender,
            (allowance.mNft->mDelegateSpenderAccountId.has_value())
              ? AccountId::fromString(allowance.mNft->mDelegateSpenderAccountId.value())
              : AccountId());
        }
      }
      else
      {
        if (allowance.mNft->mApprovedForAll.value())
        {
          accountAllowanceApproveTransaction.approveNftAllowanceAllSerials(
            TokenId::fromString(allowance.mNft->mTokenId), owner, spender);
        }
        else
        {
          accountAllowanceApproveTransaction.deleteNftAllowanceAllSerials(
            TokenId::fromString(allowance.mNft->mTokenId), owner, spender);
        }
      }
    }
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(accountAllowanceApproveTransaction, SdkClient::getClient());
  }

  return {
    {"status",
     gStatusToString.at(
        accountAllowanceApproveTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus)}
  };
}

//-----
nlohmann::json deleteAllowance(const DeleteAllowanceParams& params)
{
  AccountAllowanceDeleteTransaction accountAllowanceDeleteTransaction;
  accountAllowanceDeleteTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  for (const RemoveAllowanceParams& allowance : params.mAllowances)
  {
    const AccountId owner = AccountId::fromString(allowance.mOwnerAccountId);
    const TokenId tokenId = TokenId::fromString(allowance.mTokenId);

    for (const std::string& serialNumber : allowance.mSerialNumbers)
    {
      accountAllowanceDeleteTransaction.deleteAllTokenNftAllowances(
        NftId(tokenId, internal::EntityIdHelper::getNum(serialNumber)), owner);
    }
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(accountAllowanceDeleteTransaction, SdkClient::getClient());
  }

  return {
    {"status",
     gStatusToString.at(
        accountAllowanceDeleteTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus)}
  };
}

//-----
nlohmann::json createAccount(const CreateAccountParams& params)
{
  AccountCreateTransaction accountCreateTransaction;
  accountCreateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mKey.has_value())
  {
    accountCreateTransaction.setKeyWithoutAlias(KeyService::getHieroKey(params.mKey.value()));
  }

  if (params.mInitialBalance.has_value())
  {
    accountCreateTransaction.setInitialBalance(
      Hbar(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mInitialBalance.value()), HbarUnit::TINYBAR()));
  }

  if (params.mReceiverSignatureRequired.has_value())
  {
    accountCreateTransaction.setReceiverSignatureRequired(params.mReceiverSignatureRequired.value());
  }

  if (params.mAutoRenewPeriod.has_value())
  {
    accountCreateTransaction.setAutoRenewPeriod(
      std::chrono::seconds(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mAutoRenewPeriod.value())));
  }

  if (params.mMemo.has_value())
  {
    accountCreateTransaction.setAccountMemo(params.mMemo.value());
  }

  if (params.mMaxAutoTokenAssociations.has_value())
  {
    accountCreateTransaction.setMaxAutomaticTokenAssociations(params.mMaxAutoTokenAssociations.value());
  }

  if (params.mStakedAccountId.has_value())
  {
    accountCreateTransaction.setStakedAccountId(AccountId::fromString(params.mStakedAccountId.value()));
  }

  if (params.mStakedNodeId.has_value())
  {
    accountCreateTransaction.setStakedNodeId(
      Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mStakedNodeId.value()));
  }

  if (params.mDeclineStakingReward.has_value())
  {
    accountCreateTransaction.setDeclineStakingReward(params.mDeclineStakingReward.value());
  }

  if (params.mAlias.has_value())
  {
    accountCreateTransaction.setAlias(EvmAddress::fromString(params.mAlias.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(accountCreateTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    accountCreateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());
  return {
    {"accountId", txReceipt.mAccountId->toString()     },
    { "status",   gStatusToString.at(txReceipt.mStatus)}
  };
}

//-----
nlohmann::json deleteAccount(const DeleteAccountParams& params)
{
  AccountDeleteTransaction accountDeleteTransaction;
  accountDeleteTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mDeleteAccountId.has_value())
  {
    accountDeleteTransaction.setDeleteAccountId(AccountId::fromString(params.mDeleteAccountId.value()));
  }

  if (params.mTransferAccountId.has_value())
  {
    accountDeleteTransaction.setTransferAccountId(AccountId::fromString(params.mTransferAccountId.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(accountDeleteTransaction, SdkClient::getClient());
  }

  return {
    {"status",
     gStatusToString.at(
        accountDeleteTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus)}
  };
}

//-----
nlohmann::json transferCrypto(const TransferCryptoParams& params)
{
  TransferTransaction transferTransaction;
  transferTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTransfers.has_value())
  {
    for (const TransferParams& txParams : params.mTransfers.value())
    {
      const bool approved = txParams.mApproved.has_value() && txParams.mApproved.value();

      if (txParams.mHbar.has_value())
      {
        const Hbar amount = Hbar::fromTinybars(internal::EntityIdHelper::getNum<int64_t>(txParams.mHbar->mAmount));

        if (txParams.mHbar->mAccountId.has_value())
        {
          const AccountId accountId = AccountId::fromString(txParams.mHbar->mAccountId.value());

          if (approved)
          {
            transferTransaction.addApprovedHbarTransfer(accountId, amount);
          }
          else
          {
            transferTransaction.addHbarTransfer(accountId, amount);
          }
        }
        else
        {
          const EvmAddress evmAddress = EvmAddress::fromString(txParams.mHbar->mEvmAddress.value());

          if (approved)
          {
            transferTransaction.addApprovedHbarTransfer(AccountId::fromEvmAddress(evmAddress), amount);
          }
          else
          {
            transferTransaction.addHbarTransfer(evmAddress, amount);
          }
        }
      }
      else if (txParams.mToken.has_value())
      {
        const AccountId accountId = AccountId::fromString(txParams.mToken->mAccountId);
        const TokenId tokenId = TokenId::fromString(txParams.mToken->mTokenId);
        const auto amount = internal::EntityIdHelper::getNum<int64_t>(txParams.mToken->mAmount);

        if (txParams.mToken->mDecimals.has_value())
        {
          const uint32_t decimals = txParams.mToken->mDecimals.value();
          if (approved)
          {
            transferTransaction.addApprovedTokenTransferWithDecimals(tokenId, accountId, amount, decimals);
          }
          else
          {
            transferTransaction.addTokenTransferWithDecimals(tokenId, accountId, amount, decimals);
          }
        }
        else
        {
          if (approved)
          {
            transferTransaction.addApprovedTokenTransfer(tokenId, accountId, amount);
          }
          else
          {
            transferTransaction.addTokenTransfer(tokenId, accountId, amount);
          }
        }
      }
      else
      {
        const AccountId senderAccountId = AccountId::fromString(txParams.mNft->mSenderAccountId);
        const AccountId receiverAccountId = AccountId::fromString(txParams.mNft->mReceiverAccountId);
        const NftId nftId = NftId(TokenId::fromString(txParams.mNft->mTokenId),
                                  internal::EntityIdHelper::getNum(txParams.mNft->mSerialNumber));

        if (approved)
        {
          transferTransaction.addApprovedNftTransfer(nftId, senderAccountId, receiverAccountId);
        }
        else
        {
          transferTransaction.addNftTransfer(nftId, senderAccountId, receiverAccountId);
        }
      }
    }
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(transferTransaction, SdkClient::getClient());
  }

  return {
    {"status",
     gStatusToString.at(
        transferTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus)}
  };
}

//-----
nlohmann::json updateAccount(const UpdateAccountParams& params)
{
  AccountUpdateTransaction accountUpdateTransaction;
  accountUpdateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mAccountId.has_value())
  {
    accountUpdateTransaction.setAccountId(AccountId::fromString(params.mAccountId.value()));
  }

  if (params.mKey.has_value())
  {
    accountUpdateTransaction.setKey(KeyService::getHieroKey(params.mKey.value()));
  }

  if (params.mAutoRenewPeriod.has_value())
  {
    accountUpdateTransaction.setAutoRenewPeriod(
      std::chrono::seconds(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mAutoRenewPeriod.value())));
  }

  if (params.mExpirationTime.has_value())
  {
    accountUpdateTransaction.setExpirationTime(
      std::chrono::system_clock::from_time_t(0) +
      std::chrono::seconds(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mExpirationTime.value())));
  }

  if (params.mReceiverSignatureRequired.has_value())
  {
    accountUpdateTransaction.setReceiverSignatureRequired(params.mReceiverSignatureRequired.value());
  }

  if (params.mMemo.has_value())
  {
    accountUpdateTransaction.setAccountMemo(params.mMemo.value());
  }

  if (params.mMaxAutoTokenAssociations.has_value())
  {
    accountUpdateTransaction.setMaxAutomaticTokenAssociations(params.mMaxAutoTokenAssociations.value());
  }

  if (params.mStakedAccountId.has_value())
  {
    accountUpdateTransaction.setStakedAccountId(AccountId::fromString(params.mStakedAccountId.value()));
  }

  if (params.mStakedNodeId.has_value())
  {
    accountUpdateTransaction.setStakedNodeId(
      Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mStakedNodeId.value()));
  }

  if (params.mDeclineStakingReward.has_value())
  {
    accountUpdateTransaction.setDeclineStakingReward(params.mDeclineStakingReward.value());
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(accountUpdateTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    accountUpdateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());
  return {
    {"status", gStatusToString.at(txReceipt.mStatus)}
  };
}

} // namespace Hiero::TCK::AccountService
