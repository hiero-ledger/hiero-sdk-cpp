// SPDX-License-Identifier: Apache-2.0
#include "file/FileService.h"
#include "file/params/CreateFileParams.h"
#include "file/params/UpdateFileParams.h"
#include "key/KeyService.h"
#include "sdk/SdkClient.h"
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"

#include <FileCreateTransaction.h>
#include <FileId.h>
#include <FileUpdateTransaction.h>
#include <Key.h>
#include <KeyList.h>
#include <PrivateKey.h>
#include <Status.h>
#include <TransactionReceipt.h>
#include <TransactionResponse.h>
#include <impl/EntityIdHelper.h>
#include <impl/Utilities.h>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace Hiero::TCK::FileService
{
//-----
nlohmann::json createFile(const CreateFileParams& params)
{
  FileCreateTransaction fileCreateTransaction;
  fileCreateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mKeys.has_value())
  {
    std::vector<std::shared_ptr<Key>> keys;
    for (const std::string& keyString : params.mKeys.value())
    {
      keys.push_back(KeyService::getHieroKey(keyString));
    }
    fileCreateTransaction.setKeys(keys);
  }

  if (params.mContents.has_value())
  {
    fileCreateTransaction.setContents(params.mContents.value());
  }

  if (params.mFileMemo.has_value())
  {
    fileCreateTransaction.setFileMemo(params.mFileMemo.value());
  }

  if (params.mExpirationTime.has_value())
  {
    fileCreateTransaction.setExpirationTime(
      std::chrono::system_clock::from_time_t(0) +
      std::chrono::seconds(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mExpirationTime.value())));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(fileCreateTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    fileCreateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());

  return {
    {"fileId",  txReceipt.mFileId.value().toString() },
    { "status", gStatusToString.at(txReceipt.mStatus)}
  };
}

//-----
nlohmann::json updateFile(const UpdateFileParams& params)
{
  FileUpdateTransaction fileUpdateTransaction;
  fileUpdateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  // Set the file ID (required)
  fileUpdateTransaction.setFileId(FileId::fromString(params.mFileId));

  if (params.mKeys.has_value())
  {
    std::vector<std::shared_ptr<Key>> keys;
    for (const std::string& keyString : params.mKeys.value())
    {
      keys.push_back(KeyService::getHieroKey(keyString));
    }
    fileUpdateTransaction.setKeys(keys);
  }

  if (params.mContents.has_value())
  {
    fileUpdateTransaction.setContents(params.mContents.value());
  }

  if (params.mFileMemo.has_value())
  {
    fileUpdateTransaction.setFileMemo(params.mFileMemo.value());
  }

  if (params.mExpirationTime.has_value())
  {
    fileUpdateTransaction.setExpirationTime(
      std::chrono::system_clock::from_time_t(0) +
      std::chrono::seconds(Hiero::internal::EntityIdHelper::getNum<int64_t>(params.mExpirationTime.value())));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(fileUpdateTransaction, SdkClient::getClient());
  }

  const TransactionReceipt txReceipt =
    fileUpdateTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient());

  return {
    { "status", gStatusToString.at(txReceipt.mStatus) }
  };
}

} // namespace Hiero::TCK::FileService