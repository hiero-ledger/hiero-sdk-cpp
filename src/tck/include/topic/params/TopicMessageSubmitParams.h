// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_TOPIC_MESSAGE_SUBMIT_PARAMS_H_
#define HIERO_TCK_CPP_TOPIC_MESSAGE_SUBMIT_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "nlohmann/json.hpp"

#include "json/JsonUtils.h"
#include <optional>
#include <string>

namespace Hiero::TCK::TopicService
{
/**
 * Struct to hold the arguments for a `submitTopicMessage` JSON-RPC method call
 */
struct TopicMessageSubmitParams
{
  /**
   * The ID of the topic to submit the message to.
   */
  std::optional<std::string> mTopicId;

  /**
   * The message content to submit. UTF-8 encoding. Will be automatically chunked if the message exceeds the chunk size.
   */
  std::optional<std::string> mMessage;

  /**
   * The maximum number of chunks the message can be split into. Default: 20. Used when message size exceeds chunk size.
   */
  std::optional<int> mMaxChunks;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::TopicService

namespace nlohmann
{
/**
 * Json serializer template specialisation required to convert TopicMessageSubmitParams argument properly
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::TopicService::TopicMessageSubmitParams>
{
  /**
   * Convert a Json Object to a TopicMessageSubmitParams
   *
   * @param jsonFrom The JSON object with which to fill the TopicMessageSubmitParams
   * @param params the TopicMessageSubmitParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::TopicService::TopicMessageSubmitParams& params)
  {
    params.mTopicId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "topicId");

    params.mMessage = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "message");

    params.mMaxChunks = Hiero::TCK::getOptionalJsonParameter<int>(jsonFrom, "maxChunks");

    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_TOPIC_MESSAGE_SUBMIT_PARAMS_H_
