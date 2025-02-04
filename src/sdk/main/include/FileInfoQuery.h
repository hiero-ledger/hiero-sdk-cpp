// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FILE_INFO_QUERY_H_
#define HIERO_SDK_CPP_FILE_INFO_QUERY_H_

#include "FileId.h"
#include "Query.h"

namespace Hiero
{
class FileInfo;
class TransactionRecord;
}

namespace Hiero
{
/**
 * A query that returns the current state of a file. Queries do not change the state of the file or require network
 * consensus. The information is returned from a single node processing the query.
 */
class FileInfoQuery : public Query<FileInfoQuery, FileInfo>
{
public:
  /**
   * Set the ID of the file of which to request the info.
   *
   * @param fileId The ID of the desired file of which to request the info.
   * @return A reference to this FileInfoQuery object with the newly-set file ID.
   */
  FileInfoQuery& setFileId(const FileId& fileId);

  /**
   * Get the ID of the file of which this query is currently configured to get the info.
   *
   * @return The ID of the file for which this query is meant.
   */
  [[nodiscard]] inline FileId getfileId() const { return mFileId; }

private:
  /**
   * Derived from Executable. Construct a FileInfo object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct a FileInfo object.
   * @return A FileInfo object filled with the Response protobuf object's data
   */
  [[nodiscard]] FileInfo mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this FileInfoQuery's data to a Node.
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
   * Derived from Query. Verify that all the checksums in this FileInfoQuery are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This FileInfoQuery's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Query. Build a Query protobuf object with this FileInfoQuery's data, with the input QueryHeader
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
   * The ID of the file of which this query should get the info.
   */
  FileId mFileId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FILE_INFO_QUERY_H_
