// SPDX-License-Identifier: Apache-2.0
#include "FileInfoQuery.h"
#include "FileInfo.h"
#include "impl/Node.h"

#include <file_get_info.pb.h>
#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>

namespace Hiero
{
//-----
FileInfoQuery& FileInfoQuery::setFileId(const FileId& fileId)
{
  mFileId = fileId;
  return *this;
}

//-----
FileInfo FileInfoQuery::mapResponse(const proto::Response& response) const
{
  return FileInfo::fromProtobuf(response.filegetinfo().fileinfo());
}

//-----
grpc::Status FileInfoQuery::submitRequest(const proto::Query& request,
                                          const std::shared_ptr<internal::Node>& node,
                                          const std::chrono::system_clock::time_point& deadline,
                                          proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kFileGetInfo, request, deadline, response);
}

//-----
void FileInfoQuery::validateChecksums(const Client& client) const
{
  mFileId.validateChecksum(client);
}

//-----
proto::Query FileInfoQuery::buildRequest(proto::QueryHeader* header) const
{
  auto fileGetInfoQuery = std::make_unique<proto::FileGetInfoQuery>();
  fileGetInfoQuery->set_allocated_header(header);
  fileGetInfoQuery->set_allocated_fileid(mFileId.toProtobuf().release());

  proto::Query query;
  query.set_allocated_filegetinfo(fileGetInfoQuery.release());
  return query;
}

//-----
proto::ResponseHeader FileInfoQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.filegetinfo().header());
  return response.filegetinfo().header();
}

} // namespace Hiero
