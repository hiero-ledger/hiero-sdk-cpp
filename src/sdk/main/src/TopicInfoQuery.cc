// SPDX-License-Identifier: Apache-2.0
#include "TopicInfoQuery.h"
#include "Client.h"
#include "TopicInfo.h"
#include "TransactionRecord.h"
#include "TransferTransaction.h"
#include "impl/Node.h"

#include <consensus_get_topic_info.pb.h>
#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>

namespace Hiero
{
//-----
TopicInfoQuery& TopicInfoQuery::setTopicId(const TopicId& topicId)
{
  mTopicId = topicId;
  return *this;
}

//-----
TopicInfo TopicInfoQuery::mapResponse(const proto::Response& response) const
{
  return TopicInfo::fromProtobuf(response.consensusgettopicinfo());
}

//-----
grpc::Status TopicInfoQuery::submitRequest(const proto::Query& request,
                                           const std::shared_ptr<internal::Node>& node,
                                           const std::chrono::system_clock::time_point& deadline,
                                           proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kConsensusGetTopicInfo, request, deadline, response);
}

//-----
void TopicInfoQuery::validateChecksums(const Client& client) const
{
  mTopicId.validateChecksum(client);
}

//-----
proto::Query TopicInfoQuery::buildRequest(proto::QueryHeader* header) const
{
  auto consensusGetTopicInfoQuery = std::make_unique<proto::ConsensusGetTopicInfoQuery>();
  consensusGetTopicInfoQuery->set_allocated_header(header);
  consensusGetTopicInfoQuery->set_allocated_topicid(mTopicId.toProtobuf().release());

  proto::Query query;
  query.set_allocated_consensusgettopicinfo(consensusGetTopicInfoQuery.release());
  return query;
}

//-----
proto::ResponseHeader TopicInfoQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.consensusgettopicinfo().header());
  return response.consensusgettopicinfo().header();
}

} // namespace Hiero
