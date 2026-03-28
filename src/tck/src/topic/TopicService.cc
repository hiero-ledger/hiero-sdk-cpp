// SPDX-License-Identifier: Apache-2.0
#include "topic/TopicService.h"
#include "common/CommonTransactionParams.h"
#include "key/KeyService.h"
#include "sdk/SdkClient.h"
#include "topic/params/CreateTopicParams.h"
#include "topic/params/DeleteTopicParams.h"
#include "topic/params/GetTopicInfoQueryParams.h"
#include "topic/params/TopicMessageSubmitParams.h"
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"

#include <AccountId.h>
#include <CustomFixedFee.h>
#include <Key.h>
#include <TopicCreateTransaction.h>
#include <TopicDeleteTransaction.h>
#include <TopicId.h>
#include <TopicInfo.h>
#include <TopicInfoQuery.h>
#include <TopicMessageSubmitTransaction.h>
#include <TransactionReceipt.h>
#include <TransactionResponse.h>
#include <impl/EntityIdHelper.h>
#include <impl/HexConverter.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Hiero::TCK::TopicService
{
//-----
nlohmann::json createTopic(const CreateTopicParams& params)
{
  TopicCreateTransaction topicCreateTransaction;
  topicCreateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mMemo.has_value())
  {
    topicCreateTransaction.setMemo(params.mMemo.value());
  }

  if (params.mAdminKey.has_value())
  {
    topicCreateTransaction.setAdminKey(KeyService::getHieroKey(params.mAdminKey.value()));
  }

  if (params.mSubmitKey.has_value())
  {
    topicCreateTransaction.setSubmitKey(KeyService::getHieroKey(params.mSubmitKey.value()));
  }

  topicCreateTransaction.setAutoRenewPeriod(
    std::chrono::seconds(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mAutoRenewPeriod)));

  if (params.mAutoRenewAccount.has_value())
  {
    topicCreateTransaction.setAutoRenewAccountId(AccountId::fromString(params.mAutoRenewAccount.value()));
  }

  if (params.mFeeScheduleKey.has_value())
  {
    topicCreateTransaction.setFeeScheduleKey(KeyService::getHieroKey(params.mFeeScheduleKey.value()));
  }

  if (params.mFeeExemptKeys.has_value())
  {
    std::vector<std::shared_ptr<Key>> feeExemptKeys;
    for (const auto& keyStr : params.mFeeExemptKeys.value())
    {
      feeExemptKeys.push_back(KeyService::getHieroKey(keyStr));
    }
    topicCreateTransaction.setFeeExemptKeys(feeExemptKeys);
  }

  if (params.mCustomFees.has_value())
  {
    std::vector<CustomFixedFee> fixedFees;
    for (const auto& fee : params.mCustomFees.value())
    {
      if (CustomFixedFee* feePtr = dynamic_cast<CustomFixedFee*>(fee.get()))
      {
        fixedFees.push_back(*feePtr);
      }
    }
    topicCreateTransaction.setCustomFixedFees(fixedFees);
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(topicCreateTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    topicCreateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());

  return {
    {"topicId", txReceipt.mTopicId.value().toString()},
    { "status", gStatusToString.at(txReceipt.mStatus)}
  };
}

//-----
nlohmann::json deleteTopic(const DeleteTopicParams& params)
{
  TopicDeleteTransaction topicDeleteTransaction;
  topicDeleteTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTopicId.has_value())
  {
    topicDeleteTransaction.setTopicId(TopicId::fromString(params.mTopicId.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(topicDeleteTransaction, SdkClient::getClient());
  }

  return {
    {"status",
     gStatusToString.at(
        topicDeleteTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus)},
  };
}

//-----
nlohmann::json getTopicInfo(const GetTopicInfoQueryParams& params)
{
  TopicInfoQuery getTopicInfoQuery;
  getTopicInfoQuery.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTopicId.has_value())
  {
    getTopicInfoQuery.setTopicId(TopicId::fromString(params.mTopicId.value()));
  }

  if (params.mQueryPayment.has_value())
  {
    getTopicInfoQuery.setQueryPayment(Hbar::fromTinybars(std::stoll(params.mQueryPayment.value())));
  }

  if (params.mMaxQueryPayment.has_value())
  {
    getTopicInfoQuery.setMaxQueryPayment(Hbar::fromTinybars(std::stoll(params.mMaxQueryPayment.value())));
  }

  const TopicInfo info = getTopicInfoQuery.execute(SdkClient::getClient());

  nlohmann::json response;

  response["topicId"] = info.mTopicId.toString();
  response["topicMemo"] = info.mMemo;
  response["sequenceNumber"] = std::to_string(info.mSequenceNumber);

  auto expirySeconds =
    std::chrono::duration_cast<std::chrono::seconds>(info.mExpirationTime.time_since_epoch()).count();
  response["expirationTime"] = std::to_string(expirySeconds);

  if (info.mAutoRenewPeriod.has_value())
  {
    response["autoRenewPeriod"] =
      std::to_string(std::chrono::duration_cast<std::chrono::seconds>(info.mAutoRenewPeriod.value()).count());
  }

  if (info.mAutoRenewAccountId.has_value())
  {
    response["autoRenewAccountId"] = info.mAutoRenewAccountId.value().toString();
  }

  if (info.mAdminKey)
  {
    response["adminKey"] = internal::HexConverter::bytesToHex(info.mAdminKey->toBytes());
  }

  if (info.mSubmitKey)
  {
    response["submitKey"] = internal::HexConverter::bytesToHex(info.mSubmitKey->toBytes());
  }

  if (info.mFeeScheduleKey)
  {
    response["feeScheduleKey"] = internal::HexConverter::bytesToHex(info.mFeeScheduleKey->toBytes());
  }

  response["feeExemptKeys"] = nlohmann::json::array();
  for (const auto& key : info.mFeeExemptKeys)
  {
    response["feeExemptKeys"].push_back(internal::HexConverter::bytesToHex(key->toBytes()));
  }

  response["customFees"] = nlohmann::json::array();
  for (const auto& fee : info.mCustomFixedFees)
  {
    nlohmann::json customFeeItem;
    customFeeItem["amount"] = std::to_string(fee.getAmount());
    if (fee.getDenominatingTokenId().has_value())
    {
      customFeeItem["denominatingTokenId"] = fee.getDenominatingTokenId().value().toString();
    }
    if (!(fee.getFeeCollectorAccountId() == AccountId()))
    {
      customFeeItem["feeCollectorAccountId"] = fee.getFeeCollectorAccountId().toString();
    }
    response["customFees"].push_back(customFeeItem);
  }

  response["ledgerId"] = info.mLedgerId.toString();

  return response;
}

//-----
nlohmann::json submitTopicMessage(const TopicMessageSubmitParams& params)
{
  TopicMessageSubmitTransaction topicMessageSubmitTransaction;

  topicMessageSubmitTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mTopicId.has_value())
  {
    topicMessageSubmitTransaction.setTopicId(TopicId::fromString(params.mTopicId.value()));
  }

  if (params.mMessage.has_value())
  {
    topicMessageSubmitTransaction.setMessage(params.mMessage.value());
  }

  if (params.mMaxChunks.has_value())
  {
    topicMessageSubmitTransaction.setMaxChunks(static_cast<int>(params.mMaxChunks.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(topicMessageSubmitTransaction, SdkClient::getClient());
  }

  return {
    {"status",
     gStatusToString.at(
        topicMessageSubmitTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus)}
  };
}

} // namespace Hiero::TCK::TopicService
