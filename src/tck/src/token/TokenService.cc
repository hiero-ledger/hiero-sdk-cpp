// SPDX-License-Identifier: Apache-2.0
#include "token/TokenService.h"
#include "key/KeyService.h"
#include "sdk/SdkClient.h"
#include "token/params/AirdropTokenParams.h"
#include "token/params/AssociateTokenParams.h"
#include "token/params/BurnTokenParams.h"
#include "token/params/CancelAirdropParams.h"
#include "token/params/ClaimAirdropParams.h"
#include "token/params/CreateTokenParams.h"
#include "token/params/DeleteTokenParams.h"
#include "token/params/DissociateTokenParams.h"
#include "token/params/FreezeTokenParams.h"
#include "token/params/GrantTokenKycParams.h"
#include "token/params/MintTokenParams.h"
#include "token/params/PauseTokenParams.h"
#include "token/params/RevokeTokenKycParams.h"
#include "token/params/TokenRejectParams.h"
#include "token/params/UnfreezeTokenParams.h"
#include "token/params/UnpauseTokenParams.h"
#include "token/params/UpdateTokenFeeScheduleParams.h"
#include "token/params/UpdateTokenParams.h"
#include "token/params/WipeTokenParams.h"
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"

#include <AccountId.h>
#include <Status.h>
#include <TokenAirdropTransaction.h>
#include <TokenAssociateTransaction.h>
#include <TokenBurnTransaction.h>
#include <TokenCancelAirdropTransaction.h>
#include <TokenClaimAirdropTransaction.h>
#include <TokenCreateTransaction.h>
#include <TokenDeleteTransaction.h>
#include <TokenDissociateTransaction.h>
#include <TokenFeeScheduleUpdateTransaction.h>
#include <TokenFreezeTransaction.h>
#include <TokenGrantKycTransaction.h>
#include <TokenId.h>
#include <TokenMintTransaction.h>
#include <TokenPauseTransaction.h>
#include <TokenRejectTransaction.h>
#include <TokenRevokeKycTransaction.h>
#include <TokenSupplyType.h>
#include <TokenType.h>
#include <TokenUnfreezeTransaction.h>
#include <TokenUnpauseTransaction.h>
#include <TokenUpdateTransaction.h>
#include <TokenWipeTransaction.h>
#include <TransactionReceipt.h>
#include <TransactionResponse.h>
#include <impl/EntityIdHelper.h>
#include <impl/HexConverter.h>
#include <impl/Utilities.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Hiero::TCK::TokenService
{
//-----
nlohmann::json airdropToken(const AirdropTokenParams& params)
{
  TokenAirdropTransaction tokenAirdropTransaction;
  tokenAirdropTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTokenTransfers.has_value())
  {
    for (const TransferParams& txParams : params.mTokenTransfers.value())
    {
      const bool approved = txParams.mApproved.has_value() && txParams.mApproved.value();

      if (txParams.mToken.has_value())
      {
        const AccountId accountId = AccountId::fromString(txParams.mToken->mAccountId);
        const TokenId tokenId = TokenId::fromString(txParams.mToken->mTokenId);
        const auto amount = internal::EntityIdHelper::getNum<int64_t>(txParams.mToken->mAmount);

        if (txParams.mToken->mDecimals.has_value())
        {
          const uint32_t decimals = txParams.mToken->mDecimals.value();
          if (approved)
          {
            tokenAirdropTransaction.addApprovedTokenTransferWithDecimals(tokenId, accountId, amount, decimals);
          }
          else
          {
            tokenAirdropTransaction.addTokenTransferWithDecimals(tokenId, accountId, amount, decimals);
          }
        }
        else
        {
          if (approved)
          {
            tokenAirdropTransaction.addApprovedTokenTransfer(tokenId, accountId, amount);
          }
          else
          {
            tokenAirdropTransaction.addTokenTransfer(tokenId, accountId, amount);
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
          tokenAirdropTransaction.addApprovedNftTransfer(nftId, senderAccountId, receiverAccountId);
        }
        else
        {
          tokenAirdropTransaction.addNftTransfer(nftId, senderAccountId, receiverAccountId);
        }
      }
    }
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenAirdropTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenAirdropTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json associateToken(const AssociateTokenParams& params)
{
  TokenAssociateTransaction tokenAssociateTransaction;
  tokenAssociateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mAccountId.has_value())
  {
    tokenAssociateTransaction.setAccountId(AccountId::fromString(params.mAccountId.value()));
  }

  if (params.mTokenIds.has_value())
  {
    std::vector<TokenId> tokenIds;
    for (const std::string& tokenId : params.mTokenIds.value())
    {
      tokenIds.push_back(TokenId::fromString(tokenId));
    }

    tokenAssociateTransaction.setTokenIds(tokenIds);
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenAssociateTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenAssociateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json burnToken(const BurnTokenParams& params)
{
  TokenBurnTransaction tokenBurnTransaction;
  tokenBurnTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTokenId.has_value())
  {
    tokenBurnTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mAmount.has_value())
  {
    tokenBurnTransaction.setAmount(internal::EntityIdHelper::getNum(params.mAmount.value()));
  }

  if (params.mSerialNumbers.has_value())
  {
    std::vector<uint64_t> serialNumbers;
    for (const std::string& serialNumber : params.mSerialNumbers.value())
    {
      serialNumbers.push_back(internal::EntityIdHelper::getNum(serialNumber));
    }

    tokenBurnTransaction.setSerialNumbers(serialNumbers);
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenBurnTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    tokenBurnTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());
  return {
    { "status",         gStatusToString.at(txReceipt.mStatus)             },
    { "newTotalSupply", std::to_string(txReceipt.mNewTotalSupply.value()) }
  };
}

//-----
nlohmann::json cancelAirdrop(const CancelAirdropParams& params)
{
  TokenCancelAirdropTransaction tokenCancelAirdropTransaction;
  tokenCancelAirdropTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  PendingAirdropId pendingAirdropId;
  pendingAirdropId.mSender = AccountId::fromString(params.mSenderAccountId);
  pendingAirdropId.mReceiver = AccountId::fromString(params.mReceiverAccountId);

  const TokenId tokenId = TokenId::fromString(params.mTokenId);

  if (params.mSerialNumbers.has_value())
  {
    std::vector<PendingAirdropId> nftAirdrops;
    for (const auto& serialNumber : params.mSerialNumbers.value())
    {
      nftAirdrops.push_back(pendingAirdropId);
      nftAirdrops.back().mNft = NftId(tokenId, Hiero::internal::EntityIdHelper::getNum(serialNumber));
    }
  }
  else
  {
    pendingAirdropId.mFt = tokenId;
    tokenCancelAirdropTransaction.setPendingAirdrops({ pendingAirdropId });
  }

  return {
    { "status",
     gStatusToString.at(
        tokenCancelAirdropTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json claimAirdrop(const ClaimAirdropParams& params)
{
  TokenClaimAirdropTransaction tokenClaimAirdropTransaction;
  tokenClaimAirdropTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  PendingAirdropId pendingAirdropId;
  pendingAirdropId.mSender = AccountId::fromString(params.mSenderAccountId);
  pendingAirdropId.mReceiver = AccountId::fromString(params.mReceiverAccountId);

  const TokenId tokenId = TokenId::fromString(params.mTokenId);

  if (params.mSerialNumbers.has_value())
  {
    std::vector<PendingAirdropId> nftAirdrops;
    for (const auto& serialNumber : params.mSerialNumbers.value())
    {
      nftAirdrops.push_back(pendingAirdropId);
      nftAirdrops.back().mNft = NftId(tokenId, Hiero::internal::EntityIdHelper::getNum(serialNumber));
    }
  }
  else
  {
    pendingAirdropId.mFt = tokenId;
    tokenClaimAirdropTransaction.setPendingAirdrops({ pendingAirdropId });
  }

  return {
    { "status",
     gStatusToString.at(
        tokenClaimAirdropTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json createToken(const CreateTokenParams& params)
{
  TokenCreateTransaction tokenCreateTransaction;
  tokenCreateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mName.has_value())
  {
    tokenCreateTransaction.setTokenName(params.mName.value());
  }

  if (params.mSymbol.has_value())
  {
    tokenCreateTransaction.setTokenSymbol(params.mSymbol.value());
  }

  if (params.mDecimals.has_value())
  {
    tokenCreateTransaction.setDecimals(params.mDecimals.value());
  }

  if (params.mInitialSupply.has_value())
  {
    tokenCreateTransaction.setInitialSupply(
      Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mInitialSupply.value()));
  }

  if (params.mTreasuryAccountId.has_value())
  {
    tokenCreateTransaction.setTreasuryAccountId(AccountId::fromString(params.mTreasuryAccountId.value()));
  }

  if (params.mAdminKey.has_value())
  {
    tokenCreateTransaction.setAdminKey(KeyService::getHieroKey(params.mAdminKey.value()));
  }

  if (params.mKycKey.has_value())
  {
    tokenCreateTransaction.setKycKey(KeyService::getHieroKey(params.mKycKey.value()));
  }

  if (params.mFreezeKey.has_value())
  {
    tokenCreateTransaction.setFreezeKey(KeyService::getHieroKey(params.mFreezeKey.value()));
  }

  if (params.mWipeKey.has_value())
  {
    tokenCreateTransaction.setWipeKey(KeyService::getHieroKey(params.mWipeKey.value()));
  }

  if (params.mSupplyKey.has_value())
  {
    tokenCreateTransaction.setSupplyKey(KeyService::getHieroKey(params.mSupplyKey.value()));
  }

  if (params.mFreezeDefault.has_value())
  {
    tokenCreateTransaction.setFreezeDefault(params.mFreezeDefault.value());
  }

  if (params.mExpirationTime.has_value())
  {
    tokenCreateTransaction.setExpirationTime(
      std::chrono::system_clock::from_time_t(0) +
      std::chrono::seconds(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mExpirationTime.value())));
  }

  if (params.mAutoRenewAccountId.has_value())
  {
    tokenCreateTransaction.setAutoRenewAccountId(AccountId::fromString(params.mAutoRenewAccountId.value()));
  }

  if (params.mAutoRenewPeriod.has_value())
  {
    tokenCreateTransaction.setAutoRenewPeriod(
      std::chrono::seconds(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mAutoRenewPeriod.value())));
  }

  if (params.mMemo.has_value())
  {
    tokenCreateTransaction.setTokenMemo(params.mMemo.value());
  }

  if (params.mTokenType.has_value())
  {
    if (params.mTokenType.value() != "ft" && params.mTokenType.value() != "nft")
    {
      throw JsonRpcException(JsonErrorType::INVALID_PARAMS, "invalid params: tokenType MUST be one of ft or nft.");
    }

    tokenCreateTransaction.setTokenType(params.mTokenType.value() == "ft" ? TokenType::FUNGIBLE_COMMON
                                                                          : TokenType::NON_FUNGIBLE_UNIQUE);
  }

  if (params.mSupplyType.has_value())
  {
    if (params.mSupplyType.value() != "infinite" && params.mSupplyType.value() != "finite")
    {
      throw JsonRpcException(JsonErrorType::INVALID_PARAMS,
                             "invalid params: supplyType MUST be one of infinite or finite.");
    }

    tokenCreateTransaction.setSupplyType(params.mSupplyType.value() == "finite" ? TokenSupplyType::FINITE
                                                                                : TokenSupplyType::INFINITE);
  }

  if (params.mMaxSupply.has_value())
  {
    tokenCreateTransaction.setMaxSupply(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mMaxSupply.value()));
  }

  if (params.mFeeScheduleKey.has_value())
  {
    tokenCreateTransaction.setFeeScheduleKey(KeyService::getHieroKey(params.mFeeScheduleKey.value()));
  }

  if (params.mCustomFees.has_value())
  {
    tokenCreateTransaction.setCustomFees(params.mCustomFees.value());
  }

  if (params.mPauseKey.has_value())
  {
    tokenCreateTransaction.setPauseKey(KeyService::getHieroKey(params.mPauseKey.value()));
  }

  if (params.mMetadata.has_value())
  {
    tokenCreateTransaction.setMetadata(internal::Utilities::stringToByteVector(params.mMetadata.value()));
  }

  if (params.mMetadataKey.has_value())
  {
    tokenCreateTransaction.setMetadataKey(KeyService::getHieroKey(params.mMetadataKey.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenCreateTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    tokenCreateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());
  return {
    { "tokenId", txReceipt.mTokenId->toString()        },
    { "status",  gStatusToString.at(txReceipt.mStatus) }
  };
}

//-----
nlohmann::json deleteToken(const DeleteTokenParams& params)
{
  TokenDeleteTransaction tokenDeleteTransaction;
  tokenDeleteTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTokenId.has_value())
  {
    tokenDeleteTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenDeleteTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenDeleteTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json dissociateToken(const DissociateTokenParams& params)
{
  TokenDissociateTransaction tokenDissociateTransaction;
  tokenDissociateTransaction.setGrpcDeadline(std::chrono::seconds(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT));

  if (params.mAccountId.has_value())
  {
    tokenDissociateTransaction.setAccountId(AccountId::fromString(params.mAccountId.value()));
  }

  if (params.mTokenIds.has_value())
  {
    std::vector<TokenId> tokenIds;
    for (const std::string& tokenId : params.mTokenIds.value())
    {
      tokenIds.push_back(TokenId::fromString(tokenId));
    }

    tokenDissociateTransaction.setTokenIds(tokenIds);
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenDissociateTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenDissociateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json freezeToken(const FreezeTokenParams& params)
{
  TokenFreezeTransaction tokenFreezeTransaction;
  tokenFreezeTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTokenId.has_value())
  {
    tokenFreezeTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mAccountId.has_value())
  {
    tokenFreezeTransaction.setAccountId(AccountId::fromString(params.mAccountId.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenFreezeTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenFreezeTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json grantTokenKyc(const GrantTokenKycParams& params)
{
  TokenGrantKycTransaction tokenGrantKycTransaction;
  tokenGrantKycTransaction.setGrpcDeadline(std::chrono::seconds(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT));

  if (params.mTokenId.has_value())
  {
    tokenGrantKycTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mAccountId.has_value())
  {
    tokenGrantKycTransaction.setAccountId(AccountId::fromString(params.mAccountId.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenGrantKycTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenGrantKycTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json mintToken(const MintTokenParams& params)
{
  TokenMintTransaction tokenMintTransaction;
  tokenMintTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTokenId.has_value())
  {
    tokenMintTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mAmount.has_value())
  {
    tokenMintTransaction.setAmount(Hiero::internal::EntityIdHelper::getNum(params.mAmount.value()));
  }

  if (params.mMetadata.has_value())
  {
    std::vector<std::vector<std::byte>> allMetadata;
    for (const std::string& metadata : params.mMetadata.value())
    {
      allMetadata.push_back(internal::HexConverter::hexToBytes(metadata));
    }

    tokenMintTransaction.setMetadata(allMetadata);
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenMintTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    tokenMintTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());

  nlohmann::json response = {
    { "status",         gStatusToString.at(txReceipt.mStatus)             },
    { "newTotalSupply", std::to_string(txReceipt.mNewTotalSupply.value()) }
  };

  if (!txReceipt.mSerialNumbers.empty())
  {
    for (const uint64_t& serialNumber : txReceipt.mSerialNumbers)
    {
      response["serialNumbers"].push_back(std::to_string(serialNumber));
    }
  }

  return response;
}

//-----
nlohmann::json pauseToken(const PauseTokenParams& params)
{
  TokenPauseTransaction tokenPauseTransaction;
  tokenPauseTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTokenId.has_value())
  {
    tokenPauseTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenPauseTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenPauseTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json revokeTokenKyc(const RevokeTokenKycParams& params)
{
  TokenRevokeKycTransaction tokenRevokeKycTransaction;
  tokenRevokeKycTransaction.setGrpcDeadline(std::chrono::seconds(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT));

  if (params.mTokenId.has_value())
  {
    tokenRevokeKycTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mAccountId.has_value())
  {
    tokenRevokeKycTransaction.setAccountId(AccountId::fromString(params.mAccountId.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenRevokeKycTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenRevokeKycTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json unfreezeToken(const UnfreezeTokenParams& params)
{
  TokenUnfreezeTransaction tokenUnfreezeTransaction;
  tokenUnfreezeTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTokenId.has_value())
  {
    tokenUnfreezeTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mAccountId.has_value())
  {
    tokenUnfreezeTransaction.setAccountId(AccountId::fromString(params.mAccountId.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenUnfreezeTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenUnfreezeTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json unpauseToken(const UnpauseTokenParams& params)
{
  TokenUnpauseTransaction tokenUnpauseTransaction;
  tokenUnpauseTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTokenId.has_value())
  {
    tokenUnpauseTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenUnpauseTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenUnpauseTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json updateTokenFeeSchedule(const UpdateTokenFeeScheduleParams& params)
{
  TokenFeeScheduleUpdateTransaction tokenFeeScheduleUpdateTransaction;
  tokenFeeScheduleUpdateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTokenId.has_value())
  {
    tokenFeeScheduleUpdateTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mCustomFees.has_value())
  {
    tokenFeeScheduleUpdateTransaction.setCustomFees(params.mCustomFees.value());
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenFeeScheduleUpdateTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenFeeScheduleUpdateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json updateToken(const UpdateTokenParams& params)
{
  TokenUpdateTransaction tokenUpdateTransaction;
  tokenUpdateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTokenId.has_value())
  {
    tokenUpdateTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mSymbol.has_value())
  {
    tokenUpdateTransaction.setTokenSymbol(params.mSymbol.value());
  }

  if (params.mName.has_value())
  {
    tokenUpdateTransaction.setTokenName(params.mName.value());
  }

  if (params.mTreasuryAccountId.has_value())
  {
    tokenUpdateTransaction.setTreasuryAccountId(AccountId::fromString(params.mTreasuryAccountId.value()));
  }

  if (params.mAdminKey.has_value())
  {
    tokenUpdateTransaction.setAdminKey(KeyService::getHieroKey(params.mAdminKey.value()));
  }

  if (params.mKycKey.has_value())
  {
    tokenUpdateTransaction.setKycKey(KeyService::getHieroKey(params.mKycKey.value()));
  }

  if (params.mFreezeKey.has_value())
  {
    tokenUpdateTransaction.setFreezeKey(KeyService::getHieroKey(params.mFreezeKey.value()));
  }

  if (params.mWipeKey.has_value())
  {
    tokenUpdateTransaction.setWipeKey(KeyService::getHieroKey(params.mWipeKey.value()));
  }

  if (params.mSupplyKey.has_value())
  {
    tokenUpdateTransaction.setSupplyKey(KeyService::getHieroKey(params.mSupplyKey.value()));
  }

  if (params.mAutoRenewAccountId.has_value())
  {
    tokenUpdateTransaction.setAutoRenewAccountId(AccountId::fromString(params.mAutoRenewAccountId.value()));
  }

  if (params.mAutoRenewPeriod.has_value())
  {
    tokenUpdateTransaction.setAutoRenewPeriod(
      std::chrono::seconds(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mAutoRenewPeriod.value())));
  }

  if (params.mExpirationTime.has_value())
  {
    tokenUpdateTransaction.setExpirationTime(
      std::chrono::system_clock::from_time_t(0) +
      std::chrono::seconds(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mExpirationTime.value())));
  }

  if (params.mMemo.has_value())
  {
    tokenUpdateTransaction.setTokenMemo(params.mMemo.value());
  }

  if (params.mFeeScheduleKey.has_value())
  {
    tokenUpdateTransaction.setFeeScheduleKey(KeyService::getHieroKey(params.mFeeScheduleKey.value()));
  }

  if (params.mPauseKey.has_value())
  {
    tokenUpdateTransaction.setPauseKey(KeyService::getHieroKey(params.mPauseKey.value()));
  }

  if (params.mMetadata.has_value())
  {
    tokenUpdateTransaction.setMetadata(internal::Utilities::stringToByteVector(params.mMetadata.value()));
  }

  if (params.mMetadataKey.has_value())
  {
    tokenUpdateTransaction.setMetadataKey(KeyService::getHieroKey(params.mMetadataKey.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenUpdateTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenUpdateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json wipeToken(const WipeTokenParams& params)
{
  TokenWipeTransaction tokenWipeTransaction;
  tokenWipeTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTokenId.has_value())
  {
    tokenWipeTransaction.setTokenId(TokenId::fromString(params.mTokenId.value()));
  }

  if (params.mAccountId.has_value())
  {
    tokenWipeTransaction.setAccountId(AccountId::fromString(params.mAccountId.value()));
  }

  if (params.mAmount.has_value())
  {
    tokenWipeTransaction.setAmount(Hiero::internal::EntityIdHelper::getNum(params.mAmount.value()));
  }

  if (params.mSerialNumbers.has_value())
  {
    std::vector<uint64_t> serialNumbers;
    for (const std::string& serialNumber : params.mSerialNumbers.value())
    {
      serialNumbers.push_back(internal::EntityIdHelper::getNum(serialNumber));
    }
    tokenWipeTransaction.setSerialNumbers(serialNumbers);
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenWipeTransaction, SdkClient::getClient());
  }

  return {
    { "status",
     gStatusToString.at(
        tokenWipeTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus) }
  };
}

//-----
nlohmann::json rejectToken(const RejectTokenParams& params)
{
  TokenRejectTransaction tokenRejectTransaction;
  tokenRejectTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mOwnerAccountId.has_value())
  {
    tokenRejectTransaction.setOwner(AccountId::fromString(params.mOwnerAccountId.value()));
  }

  if (params.mFungibleTokenIds.has_value())
  {
    std::vector<TokenId> fts;
    for (const std::string& tokenId : params.mFungibleTokenIds.value())
    {
      fts.push_back(TokenId::fromString(tokenId));
    }
    tokenRejectTransaction.setFts(fts);
  }

  if (params.mNftIds.has_value())
  {
    std::vector<NftId> nfts;
    for (const std::string& nftId : params.mNftIds.value())
    {
      nfts.push_back(NftId::fromString(nftId));
    }
    tokenRejectTransaction.setNfts(nfts);
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(tokenRejectTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    tokenRejectTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());
  return {
    { "status", gStatusToString.at(txReceipt.mStatus) }
  };
}

} // namespace Hiero::TCK::TokenService
