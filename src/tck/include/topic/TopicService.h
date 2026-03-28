// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_TOPIC_SERVICE_H_
#define HIERO_TCK_CPP_TOPIC_SERVICE_H_

#include <nlohmann/json_fwd.hpp>

namespace Hiero::TCK::TopicService
{
/**
 * Forward declarations.
 */
struct CreateTopicParams;
struct DeleteTopicParams;
struct GetTopicInfoQueryParams;
struct TopicMessageSubmitParams;

/**
 * Create a topic.
 *
 * @param params The parameters to use to create a topic.
 * @return A JSON response containing the status of the topic creation and the new topic's ID.
 */
nlohmann::json createTopic(const CreateTopicParams& params);

/**
 * Delete a topic
 *
 * @param params The parameters to use to delete a topic.
 * @return A json response containing the status of the deletion of the topic.
 */
nlohmann::json deleteTopic(const DeleteTopicParams& params);

/**
 * Get topic info.
 *
 * @param params The parameters to use to get info of a topic.
 * @return A JSON response containing the status of the topic info.
 */
nlohmann::json getTopicInfo(const GetTopicInfoQueryParams& params);

/**
 * Submit a topic message.
 *
 * @param params The parameters to use to submit a topic message.
 * @return A JSON response containing the status of the submission of the topic message.
 */
nlohmann::json submitTopicMessage(const TopicMessageSubmitParams& params);

} // namespace Hiero::TCK::TopicService

#endif // HIERO_TCK_CPP_TOPIC_SERVICE_H_
