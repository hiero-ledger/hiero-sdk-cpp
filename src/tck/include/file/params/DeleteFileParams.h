// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_DELETE_FILE_PARAMS_H_
#define HIERO_TCK_CPP_DELETE_FILE_PARAMS_H_

#include "common/CommonTransactionParams.h"
#include "json/JsonUtils.h"

#include <nlohmann/json.hpp>
#include <optional>
#include <string>

namespace Hiero::TCK::FileService
{
/**
 * Struct to hold the arguments for a `deleteFile` JSON-RPC method call.
 */
struct DeleteFileParams
{
  /**
   * The ID of the file to delete.
   */
  std::optional<std::string> mFileId;

  /**
   * Any parameters common to all transaction types.
   */
  std::optional<CommonTransactionParams> mCommonTxParams;
};

} // namespace Hiero::TCK::FileService

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert DeleteFileParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::FileService::DeleteFileParams>
{
  /**
   * Convert a JSON object to a DeleteFileParams.
   *
   * @param jsonFrom The JSON object with which to fill the DeleteFileParams.
   * @param params   The DeleteFileParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::FileService::DeleteFileParams& params)
  {
    params.mFileId = Hiero::TCK::getOptionalJsonParameter<std::string>(jsonFrom, "fileId");
    params.mCommonTxParams =
      Hiero::TCK::getOptionalJsonParameter<Hiero::TCK::CommonTransactionParams>(jsonFrom, "commonTransactionParams");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_DELETE_FILE_PARAMS_H_
