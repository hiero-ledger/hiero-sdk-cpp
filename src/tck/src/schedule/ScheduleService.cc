// SPDX-License-Identifier: Apache-2.0
#include "schedule/ScheduleService.h"
#include "account/params/ApproveAllowanceParams.h"
#include "account/params/CreateAccountParams.h"
#include "account/params/DeleteAccountParams.h"
#include "account/params/TransferCryptoParams.h"
#include "account/params/UpdateAccountParams.h"
#include "common/CommonTransactionParams.h"
#include "common/transfer/TransferParams.h"
#include "key/KeyService.h"
#include "schedule/params/CreateScheduleParams.h"
#include "schedule/params/DeleteScheduleParams.h"
#include "sdk/SdkClient.h"
#include "token/params/BurnTokenParams.h"
#include "token/params/CreateTokenParams.h"
#include "token/params/DeleteTokenParams.h"
#include "token/params/MintTokenParams.h"
#include "token/params/UpdateTokenParams.h"
#include "topic/params/CreateTopicParams.h"
#include "topic/params/DeleteTopicParams.h"
#include "topic/params/TopicMessageSubmitParams.h"

#include <AccountAllowanceApproveTransaction.h>
#include <AccountCreateTransaction.h>
#include <AccountDeleteTransaction.h>
#include <AccountUpdateTransaction.h>
#include <HbarUnit.h>
#include <ScheduleCreateTransaction.h>
#include <ScheduleDeleteTransaction.h>
#include <ScheduleId.h>
#include <TokenBurnTransaction.h>
#include <TokenCreateTransaction.h>
#include <TokenDeleteTransaction.h>
#include <TokenMintTransaction.h>
#include <TokenUpdateTransaction.h>
#include <TopicCreateTransaction.h>
#include <TopicDeleteTransaction.h>
#include <TopicMessageSubmitTransaction.h>
#include <TransactionReceipt.h>
#include <TransactionResponse.h>
#include <TransferTransaction.h>
#include <WrappedTransaction.h>
#include <impl/EntityIdHelper.h>
#include <impl/HexConverter.h>

#include <chrono>
#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>

namespace Hiero::TCK::ScheduleService
{
namespace
{
// Helper to parse seconds from string
std::chrono::seconds parseSeconds(const std::optional<std::string>& secondsStr)
{
  return std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(secondsStr.value()));
}

// Helper to parse time from seconds string
std::chrono::system_clock::time_point parseTime(const std::optional<std::string>& secondsStr)
{
  return std::chrono::system_clock::from_time_t(0) + parseSeconds(secondsStr);
}

/**
 * Helper to add an Hbar transfer to a transaction.
 */
void addHbarTransfer(TransferTransaction& transaction, const TransferParams& txParams)
{
  const Hbar amount = Hbar::fromTinybars(internal::EntityIdHelper::getNum<int64_t>(txParams.mHbar->mAmount));
  const bool approved = txParams.mApproved.has_value() && txParams.mApproved.value();

  if (txParams.mHbar->mAccountId.has_value())
  {
    const AccountId accountId = AccountId::fromString(txParams.mHbar->mAccountId.value());
    if (approved)
    {
      transaction.addApprovedHbarTransfer(accountId, amount);
    }
    else
    {
      transaction.addHbarTransfer(accountId, amount);
    }
  }
  else
  {
    const EvmAddress evmAddress = EvmAddress::fromString(txParams.mHbar->mEvmAddress.value());
    if (approved)
    {
      transaction.addApprovedHbarTransfer(AccountId::fromEvmAddress(evmAddress), amount);
    }
    else
    {
      transaction.addHbarTransfer(evmAddress, amount);
    }
  }
}

/**
 * Helper to add a Token transfer to a transaction.
 */
void addTokenTransfer(TransferTransaction& transaction, const TransferParams& txParams)
{
  const AccountId accountId = AccountId::fromString(txParams.mToken->mAccountId);
  const TokenId tokenId = TokenId::fromString(txParams.mToken->mTokenId);
  const auto amount = internal::EntityIdHelper::getNum<int64_t>(txParams.mToken->mAmount);
  const bool approved = txParams.mApproved.has_value() && txParams.mApproved.value();

  if (txParams.mToken->mDecimals.has_value())
  {
    const uint32_t decimals = txParams.mToken->mDecimals.value();
    if (approved)
    {
      transaction.addApprovedTokenTransferWithDecimals(tokenId, accountId, amount, decimals);
    }
    else
    {
      transaction.addTokenTransferWithDecimals(tokenId, accountId, amount, decimals);
    }
  }
  else
  {
    if (approved)
    {
      transaction.addApprovedTokenTransfer(tokenId, accountId, amount);
    }
    else
    {
      transaction.addTokenTransfer(tokenId, accountId, amount);
    }
  }
}

/**
 * Helper to add an NFT transfer to a transaction.
 */
void addNftTransfer(TransferTransaction& transaction, const TransferParams& txParams)
{
  const AccountId senderAccountId = AccountId::fromString(txParams.mNft->mSenderAccountId);
  const AccountId receiverAccountId = AccountId::fromString(txParams.mNft->mReceiverAccountId);
  const NftId nftId =
    NftId(TokenId::fromString(txParams.mNft->mTokenId), internal::EntityIdHelper::getNum(txParams.mNft->mSerialNumber));
  const bool approved = txParams.mApproved.has_value() && txParams.mApproved.value();

  if (approved)
  {
    transaction.addApprovedNftTransfer(nftId, senderAccountId, receiverAccountId);
  }
  else
  {
    transaction.addNftTransfer(nftId, senderAccountId, receiverAccountId);
  }
}

/**
 * Translate a transferCrypto TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateTransferCrypto(const nlohmann::json& params)
{
  const auto transferParams = params.get<AccountService::TransferCryptoParams>();
  TransferTransaction transaction;

  if (transferParams.mTransfers.has_value())
  {
    for (const TransferParams& txParams : transferParams.mTransfers.value())
    {
      if (txParams.mHbar.has_value())
      {
        addHbarTransfer(transaction, txParams);
      }
      else if (txParams.mToken.has_value())
      {
        addTokenTransfer(transaction, txParams);
      }
      else
      {
        addNftTransfer(transaction, txParams);
      }
    }
  }

  return WrappedTransaction(transaction);
}

/**
 * Set account staking parameters.
 */
template<typename T, typename P>
void setAccountStaking(T& tx, const P& p)
{
  if (p.mStakedAccountId.has_value())
  {
    tx.setStakedAccountId(AccountId::fromString(p.mStakedAccountId.value()));
  }
  if (p.mStakedNodeId.has_value())
  {
    tx.setStakedNodeId(internal::EntityIdHelper::getNum<int64_t>(p.mStakedNodeId.value()));
  }
  if (p.mDeclineStakingReward.has_value())
  {
    tx.setDeclineStakingReward(p.mDeclineStakingReward.value());
  }
}

/**
 * Translate a createAccount TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateCreateAccount(const nlohmann::json& params)
{
  const auto p = params.get<AccountService::CreateAccountParams>();
  AccountCreateTransaction tx;

  if (p.mKey.has_value())
  {
    tx.setKeyWithoutAlias(KeyService::getHieroKey(p.mKey.value()));
  }
  if (p.mInitialBalance.has_value())
  {
    tx.setInitialBalance(
      Hbar(internal::EntityIdHelper::getNum<int64_t>(p.mInitialBalance.value()), HbarUnit::TINYBAR()));
  }
  if (p.mReceiverSignatureRequired.has_value())
  {
    tx.setReceiverSignatureRequired(p.mReceiverSignatureRequired.value());
  }
  if (p.mAutoRenewPeriod.has_value())
  {
    tx.setAutoRenewPeriod(parseSeconds(p.mAutoRenewPeriod));
  }
  if (p.mMemo.has_value())
  {
    tx.setAccountMemo(p.mMemo.value());
  }
  if (p.mMaxAutoTokenAssociations.has_value())
  {
    tx.setMaxAutomaticTokenAssociations(p.mMaxAutoTokenAssociations.value());
  }
  if (p.mAlias.has_value())
  {
    tx.setAlias(EvmAddress::fromString(p.mAlias.value()));
  }

  setAccountStaking(tx, p);
  return WrappedTransaction(tx);
}

/**
 * Translate a deleteAccount TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateDeleteAccount(const nlohmann::json& params)
{
  const auto p = params.get<AccountService::DeleteAccountParams>();
  AccountDeleteTransaction tx;

  if (p.mDeleteAccountId.has_value())
  {
    tx.setDeleteAccountId(AccountId::fromString(p.mDeleteAccountId.value()));
  }
  if (p.mTransferAccountId.has_value())
  {
    tx.setTransferAccountId(AccountId::fromString(p.mTransferAccountId.value()));
  }

  return WrappedTransaction(tx);
}

/**
 * Translate an updateAccount TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateUpdateAccount(const nlohmann::json& params)
{
  const auto p = params.get<AccountService::UpdateAccountParams>();
  AccountUpdateTransaction tx;

  if (p.mAccountId.has_value())
  {
    tx.setAccountId(AccountId::fromString(p.mAccountId.value()));
  }
  if (p.mKey.has_value())
  {
    tx.setKey(KeyService::getHieroKey(p.mKey.value()));
  }
  if (p.mAutoRenewPeriod.has_value())
  {
    tx.setAutoRenewPeriod(parseSeconds(p.mAutoRenewPeriod));
  }
  if (p.mExpirationTime.has_value())
  {
    tx.setExpirationTime(parseTime(p.mExpirationTime));
  }
  if (p.mReceiverSignatureRequired.has_value())
  {
    tx.setReceiverSignatureRequired(p.mReceiverSignatureRequired.value());
  }
  if (p.mMemo.has_value())
  {
    tx.setAccountMemo(p.mMemo.value());
  }
  if (p.mMaxAutoTokenAssociations.has_value())
  {
    tx.setMaxAutomaticTokenAssociations(p.mMaxAutoTokenAssociations.value());
  }

  setAccountStaking(tx, p);
  return WrappedTransaction(tx);
}

/**
 * Set common token parameters.
 */
template<typename T, typename P>
void setTokenKeys(T& tx, const P& p)
{
  if (p.mAdminKey.has_value())
  {
    tx.setAdminKey(KeyService::getHieroKey(p.mAdminKey.value()));
  }
  if (p.mKycKey.has_value())
  {
    tx.setKycKey(KeyService::getHieroKey(p.mKycKey.value()));
  }
  if (p.mFreezeKey.has_value())
  {
    tx.setFreezeKey(KeyService::getHieroKey(p.mFreezeKey.value()));
  }
  if (p.mWipeKey.has_value())
  {
    tx.setWipeKey(KeyService::getHieroKey(p.mWipeKey.value()));
  }
  if (p.mSupplyKey.has_value())
  {
    tx.setSupplyKey(KeyService::getHieroKey(p.mSupplyKey.value()));
  }
}

/**
 * Set common token renewal and memo parameters.
 */
template<typename T, typename P>
void setTokenRenewAndMemo(T& tx, const P& p)
{
  if (p.mAutoRenewAccountId.has_value())
  {
    tx.setAutoRenewAccountId(AccountId::fromString(p.mAutoRenewAccountId.value()));
  }
  if (p.mAutoRenewPeriod.has_value())
  {
    tx.setAutoRenewPeriod(parseSeconds(p.mAutoRenewPeriod));
  }
  if (p.mMemo.has_value())
  {
    tx.setTokenMemo(p.mMemo.value());
  }
}

/**
 * Translate a createToken TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateCreateToken(const nlohmann::json& params)
{
  const auto p = params.get<TokenService::CreateTokenParams>();
  TokenCreateTransaction tx;

  if (p.mName.has_value())
  {
    tx.setTokenName(p.mName.value());
  }
  if (p.mSymbol.has_value())
  {
    tx.setTokenSymbol(p.mSymbol.value());
  }
  if (p.mDecimals.has_value())
  {
    tx.setDecimals(p.mDecimals.value());
  }
  if (p.mInitialSupply.has_value())
  {
    tx.setInitialSupply(internal::EntityIdHelper::getNum<int64_t>(p.mInitialSupply.value()));
  }
  if (p.mTreasuryAccountId.has_value())
  {
    tx.setTreasuryAccountId(AccountId::fromString(p.mTreasuryAccountId.value()));
  }
  if (p.mFreezeDefault.has_value())
  {
    tx.setFreezeDefault(p.mFreezeDefault.value());
  }
  if (p.mExpirationTime.has_value())
  {
    tx.setExpirationTime(parseTime(p.mExpirationTime));
  }

  setTokenKeys(tx, p);
  setTokenRenewAndMemo(tx, p);
  return WrappedTransaction(tx);
}

/**
 * Translate a deleteToken TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateDeleteToken(const nlohmann::json& params)
{
  const auto p = params.get<TokenService::DeleteTokenParams>();
  TokenDeleteTransaction tx;

  if (p.mTokenId.has_value())
  {
    tx.setTokenId(TokenId::fromString(p.mTokenId.value()));
  }

  return WrappedTransaction(tx);
}

/**
 * Translate an updateToken TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateUpdateToken(const nlohmann::json& params)
{
  const auto p = params.get<TokenService::UpdateTokenParams>();
  TokenUpdateTransaction tx;

  if (p.mTokenId.has_value())
  {
    tx.setTokenId(TokenId::fromString(p.mTokenId.value()));
  }
  if (p.mSymbol.has_value())
  {
    tx.setTokenSymbol(p.mSymbol.value());
  }
  if (p.mName.has_value())
  {
    tx.setTokenName(p.mName.value());
  }
  if (p.mTreasuryAccountId.has_value())
  {
    tx.setTreasuryAccountId(AccountId::fromString(p.mTreasuryAccountId.value()));
  }
  if (p.mExpirationTime.has_value())
  {
    tx.setExpirationTime(parseTime(p.mExpirationTime));
  }

  setTokenKeys(tx, p);
  setTokenRenewAndMemo(tx, p);
  return WrappedTransaction(tx);
}

/**
 * Translate a burnToken TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateBurnToken(const nlohmann::json& params)
{
  const auto p = params.get<TokenService::BurnTokenParams>();
  TokenBurnTransaction tx;

  if (p.mTokenId.has_value())
  {
    tx.setTokenId(TokenId::fromString(p.mTokenId.value()));
  }

  if (p.mAmount.has_value())
  {
    tx.setAmount(internal::EntityIdHelper::getNum(p.mAmount.value()));
  }

  if (p.mSerialNumbers.has_value())
  {
    std::vector<uint64_t> serialNumbers;
    for (const std::string& serialNumber : p.mSerialNumbers.value())
    {
      serialNumbers.push_back(internal::EntityIdHelper::getNum(serialNumber));
    }

    tx.setSerialNumbers(serialNumbers);
  }

  return WrappedTransaction(tx);
}

/**
 * Translate a mintToken TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateMintToken(const nlohmann::json& params)
{
  const auto p = params.get<TokenService::MintTokenParams>();
  TokenMintTransaction tx;

  if (p.mTokenId.has_value())
  {
    tx.setTokenId(TokenId::fromString(p.mTokenId.value()));
  }

  if (p.mAmount.has_value())
  {
    tx.setAmount(internal::EntityIdHelper::getNum(p.mAmount.value()));
  }

  if (p.mMetadata.has_value())
  {
    std::vector<std::vector<std::byte>> allMetadata;
    for (const std::string& metadata : p.mMetadata.value())
    {
      allMetadata.push_back(internal::HexConverter::hexToBytes(metadata));
    }

    tx.setMetadata(allMetadata);
  }

  return WrappedTransaction(tx);
}

/**
 * Translate an approveAllowance TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateApproveAllowance(const nlohmann::json& params)
{
  const auto p = params.get<AccountService::ApproveAllowanceParams>();
  AccountAllowanceApproveTransaction tx;

  for (const AccountService::AllowanceParams& allowance : p.mAllowances)
  {
    const AccountId owner = AccountId::fromString(allowance.mOwnerAccountId);
    const AccountId spender = AccountId::fromString(allowance.mSpenderAccountId);

    if (allowance.mHbar.has_value())
    {
      tx.approveHbarAllowance(
        owner, spender, Hbar::fromTinybars(internal::EntityIdHelper::getNum<int64_t>(allowance.mHbar->mAmount)));
    }
    else if (allowance.mToken.has_value())
    {
      tx.approveTokenAllowance(TokenId::fromString(allowance.mToken->mTokenId),
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
          tx.approveTokenNftAllowance(
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
          tx.approveNftAllowanceAllSerials(TokenId::fromString(allowance.mNft->mTokenId), owner, spender);
        }
        else
        {
          tx.deleteNftAllowanceAllSerials(TokenId::fromString(allowance.mNft->mTokenId), owner, spender);
        }
      }
    }
  }

  return WrappedTransaction(tx);
}

/**
 * Translate a createTopic TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateCreateTopic(const nlohmann::json& params)
{
  const auto p = params.get<TopicService::CreateTopicParams>();
  TopicCreateTransaction tx;

  if (p.mMemo.has_value())
  {
    tx.setMemo(p.mMemo.value());
  }
  if (p.mAdminKey.has_value())
  {
    tx.setAdminKey(KeyService::getHieroKey(p.mAdminKey.value()));
  }
  if (p.mSubmitKey.has_value())
  {
    tx.setSubmitKey(KeyService::getHieroKey(p.mSubmitKey.value()));
  }
  if (p.mAutoRenewPeriod.has_value())
  {
    tx.setAutoRenewPeriod(parseSeconds(p.mAutoRenewPeriod));
  }
  if (p.mAutoRenewAccount.has_value())
  {
    tx.setAutoRenewAccountId(AccountId::fromString(p.mAutoRenewAccount.value()));
  }

  return WrappedTransaction(tx);
}

/**
 * Translate a deleteTopic TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateDeleteTopic(const nlohmann::json& params)
{
  const auto p = params.get<TopicService::DeleteTopicParams>();
  TopicDeleteTransaction tx;

  if (p.mTopicId.has_value())
  {
    tx.setTopicId(TopicId::fromString(p.mTopicId.value()));
  }

  return WrappedTransaction(tx);
}

/**
 * Translate a submitTopicMessage TCK JSON-RPC params into a WrappedTransaction.
 */
WrappedTransaction translateSubmitTopicMessage(const nlohmann::json& params)
{
  const auto p = params.get<TopicService::TopicMessageSubmitParams>();
  TopicMessageSubmitTransaction tx;

  if (p.mTopicId.has_value())
  {
    tx.setTopicId(TopicId::fromString(p.mTopicId.value()));
  }
  if (p.mMessage.has_value())
  {
    tx.setMessage(p.mMessage.value());
  }
  if (p.mMaxChunks.has_value())
  {
    tx.setMaxChunks(static_cast<int>(p.mMaxChunks.value()));
  }

  return WrappedTransaction(tx);
}

/**
 * Translate a TCK JSON-RPC scheduled transaction into a WrappedTransaction.
 */
WrappedTransaction translateScheduledTransaction(const nlohmann::json& json)
{
  static const std::unordered_map<std::string, std::function<WrappedTransaction(const nlohmann::json&)>> dispatcher = {
    {"transferCrypto",    translateTransferCrypto    },
    { "createAccount",    translateCreateAccount     },
    { "deleteAccount",    translateDeleteAccount     },
    { "updateAccount",    translateUpdateAccount     },
    { "createToken",      translateCreateToken       },
    { "deleteToken",      translateDeleteToken       },
    { "updateToken",      translateUpdateToken       },
    { "burnToken",        translateBurnToken         },
    { "mintToken",        translateMintToken         },
    { "approveAllowance", translateApproveAllowance  },
    { "createTopic",      translateCreateTopic       },
    { "deleteTopic",      translateDeleteTopic       },
    { "submitMessage",    translateSubmitTopicMessage}
  };

  const std::string method = json.at("method").get<std::string>();
  const auto it = dispatcher.find(method);

  if (it != dispatcher.end())
  {
    return it->second(json.at("params"));
  }

  throw std::invalid_argument("Unsupported scheduled transaction method: " + method);
}
} // namespace

//-----
nlohmann::json deleteSchedule(const DeleteScheduleParams& params)
{
  ScheduleDeleteTransaction scheduleDeleteTransaction;
  scheduleDeleteTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mScheduleId.has_value())
  {
    scheduleDeleteTransaction.setScheduleId(ScheduleId::fromString(params.mScheduleId.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(scheduleDeleteTransaction, SdkClient::getClient());
  }

  return {
    {"status",
     gStatusToString.at(
        scheduleDeleteTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus)},
  };
}

//-----
nlohmann::json createSchedule(const CreateScheduleParams& params)
{
  ScheduleCreateTransaction scheduleCreateTransaction;
  scheduleCreateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mMemo.has_value())
  {
    scheduleCreateTransaction.setScheduleMemo(params.mMemo.value());
  }
  if (params.mAdminKey.has_value())
  {
    scheduleCreateTransaction.setAdminKey(KeyService::getHieroKey(params.mAdminKey.value()));
  }
  if (params.mPayerAccountId.has_value())
  {
    scheduleCreateTransaction.setPayerAccountId(AccountId::fromString(params.mPayerAccountId.value()));
  }
  if (params.mExpirationTime.has_value())
  {
    scheduleCreateTransaction.setExpirationTime(parseTime(params.mExpirationTime));
  }
  if (params.mWaitForExpiry.has_value())
  {
    scheduleCreateTransaction.setWaitForExpiry(params.mWaitForExpiry.value());
  }

  scheduleCreateTransaction.setScheduledTransaction(translateScheduledTransaction(params.mScheduledTransaction));

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(scheduleCreateTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    scheduleCreateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());

  return {
    {"status",      gStatusToString.at(txReceipt.mStatus)   },
    { "scheduleId", txReceipt.mScheduleId.value().toString()}
  };
}

} // namespace Hiero::TCK::ScheduleService
