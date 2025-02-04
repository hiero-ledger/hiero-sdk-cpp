// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_NETWORK_VERSION_INFO_QUERY_H_
#define HIERO_SDK_CPP_NETWORK_VERSION_INFO_QUERY_H_

#include "Query.h"

namespace Hiero
{
class NetworkVersionInfo;
}

namespace Hiero
{
/**
 * Get the deployed versions of Hiero Services and the HAPI proto in semantic version format.
 */
class NetworkVersionInfoQuery : public Query<NetworkVersionInfoQuery, NetworkVersionInfo>
{
private:
  /**
   * Derived from Executable. Construct a NetworkVersionInfo object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct a NetworkVersionInfo object.
   * @return A NetworkVersionInfo object filled with the Response protobuf object's data
   */
  [[nodiscard]] NetworkVersionInfo mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this NetworkVersionInfoQuery's data to a
   * Node.
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
   * Derived from Query. Verify that all the checksums in this NetworkVersionInfoQuery are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This NetworkVersionInfoQuery's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Query. Build a Query protobuf object with this NetworkVersionInfoQuery's data, with the input
   * QueryHeader protobuf object.
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
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_NETWORK_VERSION_INFO_QUERY_H_
