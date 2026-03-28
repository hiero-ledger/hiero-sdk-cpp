// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_CREATE_TOPIC_PARAMS_H_
#define HIERO_TCK_CPP_CREATE_TOPIC_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "nlohmann/json.hpp"

#include "token/CustomFeeSerializer.h"

#include "json/JsonUtils.h"
#include <optional>
#include <string>
#include <vector>

namespace Hiero::TCK::TopicService
{
/**
 * Struct to hold the arguments for a `createTopic` JSON-RPC method call.
 */
struct CreateTopicParams
{
  /**
   * Short publicly visible memo about the topic. No guarantee of uniqueness. (UTF-8 encoding max 100 bytes)
   */
  std::optional<std::string> mMemo;

  /**
   * Access control for update/delete of the topic. DER-encoded hex string representation for private or public keys.
   * Keylists and threshold keys are the hex of the serialized protobuf bytes.
   */
  std::optional<std::string> mAdminKey;

  /**
   * Access control for submit message. DER-encoded hex string representation for private or public keys. Keylists and
   * threshold keys are the hex of the serialized protobuf bytes.
   */
  std::optional<std::string> mSubmitKey;

  /**
   * The amount of time to attempt to extend the topic's lifetime by automatically at the topic's expirationTime. Units
   * of seconds. Min: 6999999 (≈30 days), Max: 8000001 (≈92 days)
   */
  std::string mAutoRenewPeriod;

  /**
   * Optional account to be used at the topic's expirationTime to extend the life of the topic. Must sign transaction if
   * specified.
   */
  std::optional<std::string> mAutoRenewAccount;

  /**
   * A key that controls updates and deletions of topic fees. DER-encoded hex string representation for private or
   * public keys. Keylists and threshold keys are the hex of the serialized protobuf bytes.
   */
  std::optional<std::string> mFeeScheduleKey;

  /**
   * A list of keys that, if used to sign a message submission, allow the sender to bypass fees. DER-encoded hex string
   * representation for private or public keys.
   */
  std::optional<std::vector<std::string>> mFeeExemptKeys;

  /**
   * A fee structure applied to message submissions for revenue generation.
   */
  std::optional<std::vector<std::shared_ptr<CustomFee>>> mCustomFees;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::TopicService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert CreateTopicParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TopicService::CreateTopicParams>
{
  /**
   * Convert a Json Object to a CreateTopicParams
   *
   * @param jsonFrom The json object which will fill the CreateTopicParams.
   * @param params The CreateTopicParam to fill the json object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TopicService::CreateTopicParams& params)
  {
    params.mMemo = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "memo");

    params.mAdminKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "adminKey");

    params.mSubmitKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "submitKey");

    params.mAutoRenewPeriod = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "autoRenewPeriod");

    params.mAutoRenewAccount = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "autoRenewAccount");

    params.mFeeScheduleKey = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "feeScheduleKey");

    params.mFeeExemptKeys = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "feeExemptKeys");

    params.mCustomFees =
      Hiero::TCK::getOptionalJsonParameter<std::vector<std::shared_ptr<Hiero::CustomFee>>>(jsonFrom, "customFees");

    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_CREATE_TOPIC_PARAMS_H_
