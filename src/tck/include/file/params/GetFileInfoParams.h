// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_GET_FILE_INFO_PARAMS_H_
#define HIERO_TCK_CPP_GET_FILE_INFO_PARAMS_H_

#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::FileService
{
/**
 * Struct to hold the arguments for a `getFileInfo` JSON-RPC method call.
 */
struct GetFileInfoParams
{
  /**
   * The ID of the file to query.
   */
  std::optional<std::string> mFileId;
};

} // namespace Hiero::TCK::FileService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert GetFileInfoParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::FileService::GetFileInfoParams>
{
  static void from_json(const json& jsonFrom, Hiero::TCK::FileService::GetFileInfoParams& params)
  {
    params.mFileId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "fileId");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_GET_FILE_INFO_PARAMS_H_
