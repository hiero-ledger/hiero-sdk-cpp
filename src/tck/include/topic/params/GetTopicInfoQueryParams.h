// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_GET_TOPIC_INFO_QUERY_PARAMS_H_
#define HIERO_TCK_CPP_GET_TOPIC_INFO_QUERY_PARAMS_H_

#include "nlohmann/json.hpp"

#include "json/JsonUtils.h"
#include <optional>
#include <string>

namespace Hiero::TCK::TopicService
{
/**
 * Struct to hold the arguments for a `getTopicInfo` Json-RPC method call
 */
struct GetTopicInfoQueryParams
{
  /**
   * The ID of the topic to query.
   */
  std::optional<std::string> mTopicId;

  /**
   * Explicit payment amount for the query in tinybars.
   */
  std::optional<std::string> mQueryPayment;

  /**
   * Maximum payment amount for the query in tinybars.
   */
  std::optional<std::string> mMaxQueryPayment;
};

} // namespace Hiero::TCK::TopicService

namespace nlohmann
{
/**
 * JSON serializer template specialisation required to convert GetTopicInfoQueryParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TopicService::GetTopicInfoQueryParams>
{
  /**
   * Convert a Json Object to a GetTopicInfoQueryParams
   *
   * @param jsonFrom The json object which will fill the GetTopicInfoQueryParams.
   * @param params The GetTopicInfoQueryParams to fill the json object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TopicService::GetTopicInfoQueryParams& params)
  {
    params.mTopicId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "topicId");

    params.mQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "queryPayment");

    params.mMaxQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "maxQueryPayment");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_GET_TOPIC_INFO_QUERY_PARAMS_H_
