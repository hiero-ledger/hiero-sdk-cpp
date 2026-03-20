// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_GET_FILE_CONTENTS_PARAMS_H_
#define HIERO_TCK_CPP_GET_FILE_CONTENTS_PARAMS_H_

#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::FileService
{
/**
 * Struct to hold the arguments for a `getFileContents` JSON-RPC method call.
 */
struct GetFileContentsParams
{
  /**
   * The ID of the file to query.
   */
  std::string mFileId;

  /**
   * Explicit payment amount for the query in tinybars.
   */
  std::optional<std::string> mQueryPayment;

  /**
   * Maximum payment amount for the query in tinybars.
   */
  std::optional<std::string> mMaxQueryPayment;
};

} //  namespace Hiero::TCK::FileService

namespace nlohmann
{
/**
 * JSON serializer template specialisation required to convert GetFileContentsParams arguments properly
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::FileService::GetFileContentsParams>
{
  /**
   * Convert a Json Object to a GetFileContentsParams
   *
   * @param jsonFrom The JSON object with which to fill the GetFileContentsParams
   * @param params The GetFileContentsParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::FileService::GetFileContentsParams& params)
  {
    params.mFileId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "fileId");

    params.mQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "queryPayment");

    params.mMaxQueryPayment = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "maxQueryPayment");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_GET_FILE_CONTENTS_PARAMS_H_
