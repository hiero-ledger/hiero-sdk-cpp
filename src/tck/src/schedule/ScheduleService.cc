// SPDX-License-Identifier: Apache-2.0
#include "schedule/ScheduleService.h"

#include "account/params/CreateAccountParams.h"
#include "account/params/DeleteAccountParams.h"
#include "account/params/UpdateAccountParams.h"
#include "account/params/TransferCryptoParams.h"
#include "common/transfer/TransferParams.h"
#include "key/KeyService.h"
#include "schedule/params/CreateScheduleParams.h"
#include "sdk/SdkClient.h"
#include "token/params/CreateTokenParams.h"
#include "token/params/DeleteTokenParams.h"
#include "token/params/UpdateTokenParams.h"
#include "topic/params/CreateTopicParams.h"
#include "topic/params/DeleteTopicParams.h"
#include "topic/params/TopicMessageSubmitParams.h"

#include <AccountCreateTransaction.h>
#include <AccountDeleteTransaction.h>
#include <AccountUpdateTransaction.h>
#include <HbarUnit.h>
#include <ScheduleCreateTransaction.h>
#include <TokenCreateTransaction.h>
#include <TokenDeleteTransaction.h>
#include <TokenUpdateTransaction.h>
#include <TopicCreateTransaction.h>
#include <TopicDeleteTransaction.h>
#include <TopicMessageSubmitTransaction.h>
#include <TransactionReceipt.h>
#include <TransactionResponse.h>
#include <TransferTransaction.h>
#include <WrappedTransaction.h>
#include <impl/EntityIdHelper.h>

#include <chrono>
#include <nlohmann/json.hpp>
#include <string>

namespace Hiero::TCK::ScheduleService
{
namespace
{
/**
 * Translate a transferCrypto TCK JSON-RPC params into a WrappedTransaction.
 *
 * @param params The JSON object containing the params for the transferCrypto transaction.
 * @return The translated WrappedTransaction.
 */
WrappedTransaction translateTransferCrypto(const nlohmann::json& params)
{
  const auto transferParams = params.get<AccountService::TransferCryptoParams>();
  TransferTransaction transaction;

  if (transferParams.mTransfers.has_value())
  {
    for (const TransferParams& txParams : transferParams.mTransfers.value())
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
      else
      {
        const AccountId senderAccountId = AccountId::fromString(txParams.mNft->mSenderAccountId);
        const AccountId receiverAccountId = AccountId::fromString(txParams.mNft->mReceiverAccountId);
        const NftId nftId = NftId(TokenId::fromString(txParams.mNft->mTokenId),
                                  internal::EntityIdHelper::getNum(txParams.mNft->mSerialNumber));

        if (approved)
        {
          transaction.addApprovedNftTransfer(nftId, senderAccountId, receiverAccountId);
        }
        else
        {
          transaction.addNftTransfer(nftId, senderAccountId, receiverAccountId);
        }
      }
    }
  }

  return WrappedTransaction(transaction);
}

/**
 * Translate a createAccount TCK JSON-RPC params into a WrappedTransaction.
 *
 * @param params The JSON object containing the params for the createAccount transaction.
 * @return The translated WrappedTransaction.
 */
WrappedTransaction translateCreateAccount(const nlohmann::json& params)
{
  const auto createAccountParams = params.get<AccountService::CreateAccountParams>();
  AccountCreateTransaction transaction;

  if (createAccountParams.mKey.has_value())
  {
    transaction.setKeyWithoutAlias(KeyService::getHieroKey(createAccountParams.mKey.value()));
  }

  if (createAccountParams.mInitialBalance.has_value())
  {
    transaction.setInitialBalance(
      Hbar(internal::EntityIdHelper::getNum<int64_t>(createAccountParams.mInitialBalance.value()), HbarUnit::TINYBAR()));
  }

  if (createAccountParams.mReceiverSignatureRequired.has_value())
  {
    transaction.setReceiverSignatureRequired(createAccountParams.mReceiverSignatureRequired.value());
  }

  if (createAccountParams.mAutoRenewPeriod.has_value())
  {
    transaction.setAutoRenewPeriod(
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(createAccountParams.mAutoRenewPeriod.value())));
  }

  if (createAccountParams.mMemo.has_value())
  {
    transaction.setAccountMemo(createAccountParams.mMemo.value());
  }

  if (createAccountParams.mMaxAutoTokenAssociations.has_value())
  {
    transaction.setMaxAutomaticTokenAssociations(createAccountParams.mMaxAutoTokenAssociations.value());
  }

  if (createAccountParams.mStakedAccountId.has_value())
  {
    transaction.setStakedAccountId(AccountId::fromString(createAccountParams.mStakedAccountId.value()));
  }

  if (createAccountParams.mStakedNodeId.has_value())
  {
    transaction.setStakedNodeId(internal::EntityIdHelper::getNum<int64_t>(createAccountParams.mStakedNodeId.value()));
  }

  if (createAccountParams.mDeclineStakingReward.has_value())
  {
    transaction.setDeclineStakingReward(createAccountParams.mDeclineStakingReward.value());
  }

  if (createAccountParams.mAlias.has_value())
  {
    transaction.setAlias(EvmAddress::fromString(createAccountParams.mAlias.value()));
  }

  return WrappedTransaction(transaction);
}

/**
 * Translate a deleteAccount TCK JSON-RPC params into a WrappedTransaction.
 *
 * @param params The JSON object containing the params for the deleteAccount transaction.
 * @return The translated WrappedTransaction.
 */
WrappedTransaction translateDeleteAccount(const nlohmann::json& params)
{
  const auto deleteAccountParams = params.get<AccountService::DeleteAccountParams>();
  AccountDeleteTransaction transaction;

  if (deleteAccountParams.mDeleteAccountId.has_value())
  {
    transaction.setDeleteAccountId(AccountId::fromString(deleteAccountParams.mDeleteAccountId.value()));
  }

  if (deleteAccountParams.mTransferAccountId.has_value())
  {
    transaction.setTransferAccountId(AccountId::fromString(deleteAccountParams.mTransferAccountId.value()));
  }

  return WrappedTransaction(transaction);
}

/**
 * Translate an updateAccount TCK JSON-RPC params into a WrappedTransaction.
 *
 * @param params The JSON object containing the params for the updateAccount transaction.
 * @return The translated WrappedTransaction.
 */
WrappedTransaction translateUpdateAccount(const nlohmann::json& params)
{
  const auto updateAccountParams = params.get<AccountService::UpdateAccountParams>();
  AccountUpdateTransaction transaction;

  if (updateAccountParams.mAccountId.has_value())
  {
    transaction.setAccountId(AccountId::fromString(updateAccountParams.mAccountId.value()));
  }

  if (updateAccountParams.mKey.has_value())
  {
    transaction.setKey(KeyService::getHieroKey(updateAccountParams.mKey.value()));
  }

  if (updateAccountParams.mAutoRenewPeriod.has_value())
  {
    transaction.setAutoRenewPeriod(
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(updateAccountParams.mAutoRenewPeriod.value())));
  }

  if (updateAccountParams.mExpirationTime.has_value())
  {
    transaction.setExpirationTime(
      std::chrono::system_clock::from_time_t(0) +
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(updateAccountParams.mExpirationTime.value())));
  }

  if (updateAccountParams.mReceiverSignatureRequired.has_value())
  {
    transaction.setReceiverSignatureRequired(updateAccountParams.mReceiverSignatureRequired.value());
  }

  if (updateAccountParams.mMemo.has_value())
  {
    transaction.setAccountMemo(updateAccountParams.mMemo.value());
  }

  if (updateAccountParams.mMaxAutoTokenAssociations.has_value())
  {
    transaction.setMaxAutomaticTokenAssociations(updateAccountParams.mMaxAutoTokenAssociations.value());
  }

  if (updateAccountParams.mStakedAccountId.has_value())
  {
    transaction.setStakedAccountId(AccountId::fromString(updateAccountParams.mStakedAccountId.value()));
  }

  if (updateAccountParams.mStakedNodeId.has_value())
  {
    transaction.setStakedNodeId(internal::EntityIdHelper::getNum<int64_t>(updateAccountParams.mStakedNodeId.value()));
  }

  if (updateAccountParams.mDeclineStakingReward.has_value())
  {
    transaction.setDeclineStakingReward(updateAccountParams.mDeclineStakingReward.value());
  }

  return WrappedTransaction(transaction);
}

/**
 * Translate a createToken TCK JSON-RPC params into a WrappedTransaction.
 *
 * @param params The JSON object containing the params for the createToken transaction.
 * @return The translated WrappedTransaction.
 */
WrappedTransaction translateCreateToken(const nlohmann::json& params)
{
  const auto createTokenParams = params.get<TokenService::CreateTokenParams>();
  TokenCreateTransaction transaction;

  if (createTokenParams.mName.has_value())
  {
    transaction.setTokenName(createTokenParams.mName.value());
  }

  if (createTokenParams.mSymbol.has_value())
  {
    transaction.setTokenSymbol(createTokenParams.mSymbol.value());
  }

  if (createTokenParams.mDecimals.has_value())
  {
    transaction.setDecimals(createTokenParams.mDecimals.value());
  }

  if (createTokenParams.mInitialSupply.has_value())
  {
    transaction.setInitialSupply(internal::EntityIdHelper::getNum<int64_t>(createTokenParams.mInitialSupply.value()));
  }

  if (createTokenParams.mTreasuryAccountId.has_value())
  {
    transaction.setTreasuryAccountId(AccountId::fromString(createTokenParams.mTreasuryAccountId.value()));
  }

  if (createTokenParams.mAdminKey.has_value())
  {
    transaction.setAdminKey(KeyService::getHieroKey(createTokenParams.mAdminKey.value()));
  }

  if (createTokenParams.mKycKey.has_value())
  {
    transaction.setKycKey(KeyService::getHieroKey(createTokenParams.mKycKey.value()));
  }

  if (createTokenParams.mFreezeKey.has_value())
  {
    transaction.setFreezeKey(KeyService::getHieroKey(createTokenParams.mFreezeKey.value()));
  }

  if (createTokenParams.mWipeKey.has_value())
  {
    transaction.setWipeKey(KeyService::getHieroKey(createTokenParams.mWipeKey.value()));
  }

  if (createTokenParams.mSupplyKey.has_value())
  {
    transaction.setSupplyKey(KeyService::getHieroKey(createTokenParams.mSupplyKey.value()));
  }

  if (createTokenParams.mFreezeDefault.has_value())
  {
    transaction.setFreezeDefault(createTokenParams.mFreezeDefault.value());
  }

  if (createTokenParams.mExpirationTime.has_value())
  {
    transaction.setExpirationTime(
      std::chrono::system_clock::from_time_t(0) +
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(createTokenParams.mExpirationTime.value())));
  }

  if (createTokenParams.mAutoRenewAccountId.has_value())
  {
    transaction.setAutoRenewAccountId(AccountId::fromString(createTokenParams.mAutoRenewAccountId.value()));
  }

  if (createTokenParams.mAutoRenewPeriod.has_value())
  {
    transaction.setAutoRenewPeriod(
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(createTokenParams.mAutoRenewPeriod.value())));
  }

  if (createTokenParams.mMemo.has_value())
  {
    transaction.setTokenMemo(createTokenParams.mMemo.value());
  }

  return WrappedTransaction(transaction);
}

/**
 * Translate a deleteToken TCK JSON-RPC params into a WrappedTransaction.
 *
 * @param params The JSON object containing the params for the deleteToken transaction.
 * @return The translated WrappedTransaction.
 */
WrappedTransaction translateDeleteToken(const nlohmann::json& params)
{
  const auto deleteTokenParams = params.get<TokenService::DeleteTokenParams>();
  TokenDeleteTransaction transaction;

  if (deleteTokenParams.mTokenId.has_value())
  {
    transaction.setTokenId(TokenId::fromString(deleteTokenParams.mTokenId.value()));
  }

  return WrappedTransaction(transaction);
}

/**
 * Translate an updateToken TCK JSON-RPC params into a WrappedTransaction.
 *
 * @param params The JSON object containing the params for the updateToken transaction.
 * @return The translated WrappedTransaction.
 */
WrappedTransaction translateUpdateToken(const nlohmann::json& params)
{
  const auto updateTokenParams = params.get<TokenService::UpdateTokenParams>();
  TokenUpdateTransaction transaction;

  if (updateTokenParams.mTokenId.has_value())
  {
    transaction.setTokenId(TokenId::fromString(updateTokenParams.mTokenId.value()));
  }

  if (updateTokenParams.mSymbol.has_value())
  {
    transaction.setTokenSymbol(updateTokenParams.mSymbol.value());
  }

  if (updateTokenParams.mName.has_value())
  {
    transaction.setTokenName(updateTokenParams.mName.value());
  }

  if (updateTokenParams.mTreasuryAccountId.has_value())
  {
    transaction.setTreasuryAccountId(AccountId::fromString(updateTokenParams.mTreasuryAccountId.value()));
  }

  if (updateTokenParams.mAdminKey.has_value())
  {
    transaction.setAdminKey(KeyService::getHieroKey(updateTokenParams.mAdminKey.value()));
  }

  if (updateTokenParams.mKycKey.has_value())
  {
    transaction.setKycKey(KeyService::getHieroKey(updateTokenParams.mKycKey.value()));
  }

  if (updateTokenParams.mFreezeKey.has_value())
  {
    transaction.setFreezeKey(KeyService::getHieroKey(updateTokenParams.mFreezeKey.value()));
  }

  if (updateTokenParams.mWipeKey.has_value())
  {
    transaction.setWipeKey(KeyService::getHieroKey(updateTokenParams.mWipeKey.value()));
  }

  if (updateTokenParams.mSupplyKey.has_value())
  {
    transaction.setSupplyKey(KeyService::getHieroKey(updateTokenParams.mSupplyKey.value()));
  }

  if (updateTokenParams.mAutoRenewAccountId.has_value())
  {
    transaction.setAutoRenewAccountId(AccountId::fromString(updateTokenParams.mAutoRenewAccountId.value()));
  }

  if (updateTokenParams.mAutoRenewPeriod.has_value())
  {
    transaction.setAutoRenewPeriod(
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(updateTokenParams.mAutoRenewPeriod.value())));
  }

  if (updateTokenParams.mExpirationTime.has_value())
  {
    transaction.setExpirationTime(
      std::chrono::system_clock::from_time_t(0) +
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(updateTokenParams.mExpirationTime.value())));
  }

  if (updateTokenParams.mMemo.has_value())
  {
    transaction.setTokenMemo(updateTokenParams.mMemo.value());
  }

  return WrappedTransaction(transaction);
}

/**
 * Translate a createTopic TCK JSON-RPC params into a WrappedTransaction.
 *
 * @param params The JSON object containing the params for the createTopic transaction.
 * @return The translated WrappedTransaction.
 */
WrappedTransaction translateCreateTopic(const nlohmann::json& params)
{
  const auto createTopicParams = params.get<TopicService::CreateTopicParams>();
  TopicCreateTransaction transaction;

  if (createTopicParams.mMemo.has_value())
  {
    transaction.setMemo(createTopicParams.mMemo.value());
  }

  if (createTopicParams.mAdminKey.has_value())
  {
    transaction.setAdminKey(KeyService::getHieroKey(createTopicParams.mAdminKey.value()));
  }

  if (createTopicParams.mSubmitKey.has_value())
  {
    transaction.setSubmitKey(KeyService::getHieroKey(createTopicParams.mSubmitKey.value()));
  }

  if (createTopicParams.mAutoRenewPeriod.has_value())
  {
    transaction.setAutoRenewPeriod(
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(createTopicParams.mAutoRenewPeriod.value())));
  }

  if (createTopicParams.mAutoRenewAccount.has_value())
  {
    transaction.setAutoRenewAccountId(AccountId::fromString(createTopicParams.mAutoRenewAccount.value()));
  }

  return WrappedTransaction(transaction);
}

/**
 * Translate a deleteTopic TCK JSON-RPC params into a WrappedTransaction.
 *
 * @param params The JSON object containing the params for the deleteTopic transaction.
 * @return The translated WrappedTransaction.
 */
WrappedTransaction translateDeleteTopic(const nlohmann::json& params)
{
  const auto deleteTopicParams = params.get<TopicService::DeleteTopicParams>();
  TopicDeleteTransaction transaction;

  if (deleteTopicParams.mTopicId.has_value())
  {
    transaction.setTopicId(TopicId::fromString(deleteTopicParams.mTopicId.value()));
  }

  return WrappedTransaction(transaction);
}

/**
 * Translate a submitTopicMessage TCK JSON-RPC params into a WrappedTransaction.
 *
 * @param params The JSON object containing the params for the submitTopicMessage transaction.
 * @return The translated WrappedTransaction.
 */
WrappedTransaction translateSubmitTopicMessage(const nlohmann::json& params)
{
  const auto submitParams = params.get<TopicService::TopicMessageSubmitParams>();
  TopicMessageSubmitTransaction transaction;

  if (submitParams.mTopicId.has_value())
  {
    transaction.setTopicId(TopicId::fromString(submitParams.mTopicId.value()));
  }

  if (submitParams.mMessage.has_value())
  {
    transaction.setMessage(submitParams.mMessage.value());
  }

  if (submitParams.mMaxChunks.has_value())
  {
    transaction.setMaxChunks(static_cast<int>(submitParams.mMaxChunks.value()));
  }

  return WrappedTransaction(transaction);
}

/**
 * Translate a TCK JSON-RPC scheduled transaction into a WrappedTransaction.
 *
 * @param json The JSON object containing the method and params for the scheduled transaction.
 * @return The translated WrappedTransaction.
 */
WrappedTransaction translateScheduledTransaction(const nlohmann::json& json)
{
  const std::string method = json.at("method").get<std::string>();
  const nlohmann::json params = json.at("params");

  if (method == "transferCrypto")
  {
    return translateTransferCrypto(params);
  }

  if (method == "createAccount")
  {
    return translateCreateAccount(params);
  }

  if (method == "deleteAccount")
  {
    return translateDeleteAccount(params);
  }

  if (method == "updateAccount")
  {
    return translateUpdateAccount(params);
  }

  if (method == "createToken")
  {
    return translateCreateToken(params);
  }

  if (method == "deleteToken")
  {
    return translateDeleteToken(params);
  }

  if (method == "updateToken")
  {
    return translateUpdateToken(params);
  }

  if (method == "createTopic")
  {
    return translateCreateTopic(params);
  }

  if (method == "deleteTopic")
  {
    return translateDeleteTopic(params);
  }

  if (method == "submitTopicMessage")
  {
    return translateSubmitTopicMessage(params);
  }

  throw std::invalid_argument("Unsupported scheduled transaction method: " + method);
}
} // namespace

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
    scheduleCreateTransaction.setExpirationTime(
      std::chrono::system_clock::from_time_t(0) +
      std::chrono::seconds(internal::EntityIdHelper::getNum<int64_t>(params.mExpirationTime.value())));
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
    {"status",     gStatusToString.at(txReceipt.mStatus)   },
    {"scheduleId", txReceipt.mScheduleId.value().toString()}
  };
}

} // namespace Hiero::TCK::ScheduleService
