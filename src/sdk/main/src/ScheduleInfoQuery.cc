// SPDX-License-Identifier: Apache-2.0
#include "ScheduleInfoQuery.h"
#include "ScheduleInfo.h"
#include "impl/Node.h"

#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>
#include <schedule_get_info.pb.h>

namespace Hiero
{
//-----
ScheduleInfoQuery& ScheduleInfoQuery::setScheduleId(const ScheduleId& scheduleId)
{
  mScheduleId = scheduleId;
  return *this;
}

//-----
ScheduleInfo ScheduleInfoQuery::mapResponse(const proto::Response& response) const
{
  return ScheduleInfo::fromProtobuf(response.schedulegetinfo().scheduleinfo());
}

//-----
grpc::Status ScheduleInfoQuery::submitRequest(const proto::Query& request,
                                              const std::shared_ptr<internal::Node>& node,
                                              const std::chrono::system_clock::time_point& deadline,
                                              proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kScheduleGetInfo, request, deadline, response);
}

//-----
void ScheduleInfoQuery::validateChecksums(const Client& client) const
{
  mScheduleId.validateChecksum(client);
}

//-----
proto::Query ScheduleInfoQuery::buildRequest(proto::QueryHeader* header) const
{
  auto scheduleGetInfoQuery = std::make_unique<proto::ScheduleGetInfoQuery>();
  scheduleGetInfoQuery->set_allocated_header(header);
  scheduleGetInfoQuery->set_allocated_scheduleid(mScheduleId.toProtobuf().release());

  proto::Query query;
  query.set_allocated_schedulegetinfo(scheduleGetInfoQuery.release());
  return query;
}

//-----
proto::ResponseHeader ScheduleInfoQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.schedulegetinfo().header());
  return response.schedulegetinfo().header();
}

} // namespace Hiero
