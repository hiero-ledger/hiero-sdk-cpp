// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_SCHEDULE_INFO_QUERY_H_
#define HIERO_SDK_CPP_SCHEDULE_INFO_QUERY_H_

#include "Query.h"
#include "ScheduleId.h"

namespace Hiero
{
class ScheduleInfo;
class TransactionRecord;
}

namespace Hiero
{
/**
 * A query that returns information about the current state of a schedule transaction on a Hiero network.
 */
class ScheduleInfoQuery : public Query<ScheduleInfoQuery, ScheduleInfo>
{
public:
  /**
   * Set the ID of the schedule of which to request the info.
   *
   * @param schedule The ID of the schedule of which to request the info.
   * @return A reference to this ScheduleInfoQuery object with the newly-set schedule ID.
   */
  ScheduleInfoQuery& setScheduleId(const ScheduleId& scheduleId);

  /**
   * Get the ID of the schedule of which this query is currently configured to get the info.
   *
   * @return The ID of the schedule for which this query is meant.
   */
  [[nodiscard]] inline ScheduleId getScheduleId() const { return mScheduleId; }

private:
  /**
   * Derived from Executable. Construct a ScheduleInfo object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct a ScheduleInfo object.
   * @return A ScheduleInfo object filled with the Response protobuf object's data
   */
  [[nodiscard]] ScheduleInfo mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this ScheduleInfoQuery's data to a Node.
   *
   * @param request  The Query protobuf object to submit.
   * @param node     The Node to which to submit the request.
   * @param deadline The deadline for submitting the request.
   * @param response Pointer to the ProtoResponseType object that gRPC should populate with the response information
   *                 from the gRPC server.
   * @return The gRPC status of the submission.
   */
  [[nodiscard]] grpc::Status submitRequest(const proto::Query& request,
                                           const std::shared_ptr<internal::Node>& node,
                                           const std::chrono::system_clock::time_point& deadline,
                                           proto::Response* response) const override;

  /**
   * Derived from Query. Verify that all the checksums in this ScheduleInfoQuery are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This ScheduleInfoQuery's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Query. Build a Query protobuf object with this ScheduleInfoQuery's data, with the input QueryHeader
   * protobuf object.
   *
   * @param header A pointer to the QueryHeader protobuf object to add to the Query protobuf object.
   * @return The constructed Query protobuf object.
   */
  [[nodiscard]] proto::Query buildRequest(proto::QueryHeader* header) const override;

  /**
   * Derived from Query. Get the ResponseHeader protobuf object from the input Response protobuf object.
   *
   * @param response The Response protobuf object from which to get the ResponseHeader protobuf object.
   * @return The ResponseHeader protobuf object of the input Response protobuf object for this derived Query.
   */
  [[nodiscard]] proto::ResponseHeader mapResponseHeader(const proto::Response& response) const override;

  /**
   * The ID of the schedule of which this query should get the info.
   */
  ScheduleId mScheduleId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_SCHEDULE_INFO_QUERY_H_
