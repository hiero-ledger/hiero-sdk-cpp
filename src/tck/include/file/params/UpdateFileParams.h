// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_UPDATE_FILE_PARAMS_H_
#define HIERO_TCK_CPP_UPDATE_FILE_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace Hiero::TCK::FileService
{
/**
 * Struct to hold the arguments for an `updateFile` JSON-RPC method call.
 */
struct UpdateFileParams
{
  /**
   * The ID of the file to update.
   */
  std::string mFileId;

  /**
   * The keys that must sign when mutating the file.
   */
  std::optional<std::vector<std::string>> mKeys;

  /**
   * The new contents of the file.
   */
  std::optional<std::string> mContents;

  /**
   * The new memo for the file.
   */
  std::optional<std::string> mFileMemo;

  /**
   * The new time at which the file will expire.
   */
  std::optional<std::string> mExpirationTime;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::FileService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert UpdateFileParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::FileService::UpdateFileParams>
{
  static void from_json(const json& jsonFrom, Hiero::TCK::FileService::UpdateFileParams& params)
  {
    params.mFileId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "fileId");
    params.mKeys = Hiero::TCK::getOptionalJsonParameter<std::vector<std::string>>(jsonFrom, "keys");
    params.mContents = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "contents");
    params.mFileMemo = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "fileMemo");
    params.mExpirationTime = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "expirationTime");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_UPDATE_FILE_PARAMS_H_