// SPDX-License-Identifier: Apache-2.0
#include "FileContentsQuery.h"
#include "impl/Node.h"
#include "impl/Utilities.h"

#include <file_get_contents.pb.h>
#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>

namespace Hiero
{
//-----
FileContentsQuery& FileContentsQuery::setFileId(const FileId& fileId)
{
  mFileId = fileId;
  return *this;
}

//-----
FileContents FileContentsQuery::mapResponse(const proto::Response& response) const
{
  return internal::Utilities::stringToByteVector(response.filegetcontents().filecontents().contents());
}

//-----
grpc::Status FileContentsQuery::submitRequest(const proto::Query& request,
                                              const std::shared_ptr<internal::Node>& node,
                                              const std::chrono::system_clock::time_point& deadline,
                                              proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kFileGetContents, request, deadline, response);
}

//-----
void FileContentsQuery::validateChecksums(const Client& client) const
{
  mFileId.validateChecksum(client);
}

//-----
proto::Query FileContentsQuery::buildRequest(proto::QueryHeader* header) const
{
  auto fileGetContentsQuery = std::make_unique<proto::FileGetContentsQuery>();
  fileGetContentsQuery->set_allocated_header(header);
  fileGetContentsQuery->set_allocated_fileid(mFileId.toProtobuf().release());

  proto::Query query;
  query.set_allocated_filegetcontents(fileGetContentsQuery.release());
  return query;
}

//-----
proto::ResponseHeader FileContentsQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.filegetcontents().header());
  return response.filegetcontents().header();
}

} // namespace Hiero
