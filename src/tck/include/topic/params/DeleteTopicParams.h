// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_DELETE_TOPIC_PARAMS_H_
#define HIERO_TCK_CPP_DELETE_TOPIC_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "nlohmann/json.hpp"

#include "json/JsonUtils.h"
#include <optional>
#include <string>

namespace Hiero::TCK::TopicService
{
/**
 * Struct to hold the arguments for a `deleteFile` Json-RPC method call.
 */
struct DeleteTopicParams
{
  /**
   * The ID of the topic to delete.
   */
  std::optional<std::string> mTopicId;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::TopicService

namespace nlohmann
{
/**
 * JSON serializer template specialisation required to convert DeleteTopicParams argument properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TopicService::DeleteTopicParams>
{
  /**
   * Convert a JSON object to a DeleteTopicParams.
   *
   * @param jsonFrom The JSON object with which to fill the DeleteTopicParams.
   * @param params   The DeleteTopicParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TopicService::DeleteTopicParams& params)
  {
    params.mTopicId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "topicId");

    params.mCommonTxParams = params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohamann

#endif // HIERO_TCK_CPP_DELETE_TOPIC_PARAMS_H_
