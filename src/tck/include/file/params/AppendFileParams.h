// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_APPEND_FILE_PARAMS_H_
#define HIERO_TCK_CPP_APPEND_FILE_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::FileService
{
/**
 * Struct to hold the arguments for an `appendFile` JSON-RPC method call.
 */
struct AppendFileParams
{
  /**
   * The ID of the file to append to.
   */
  std::optional<std::string> mFileId;

  /**
   * The contents of the file.
   */
  std::string mContents;

  /**
   * Maximum number of chunks allowed for this transaction
   */
  std::optional<int> mMaxChunks;

  /**
   * Size of each chunk in bytes
   */
  std::optional<int> mChunkSize;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::FileService

namespace nlohmann
{
/**
 * JSON serializer template specialisation required to convert AppendFileParams argument Properly
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::FileService::AppendFileParams>
{
  /**
   * Convert a Json Object to a AppendFileParams
   *
   * @param jsonFrom The JSON object with which to fill the AppendFileParams
   * @param params The AppendFileParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::FileService::AppendFileParams& params)
  {
    params.mFileId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "fileId");

    params.mContents = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "contents");

    params.mMaxChunks = Hiero::TCK::getOptionalJsonParameter<int>(jsonFrom, "maxChunks");

    params.mChunkSize = Hiero::TCK::getOptionalJsonParameter<int>(jsonFrom, "chunkSize");

    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_APPEND_FILE_PARAMS_H_
