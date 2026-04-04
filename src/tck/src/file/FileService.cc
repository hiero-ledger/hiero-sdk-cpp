// SPDX-License-Identifier: Apache-2.0
#include "file/FileService.h"
#include "file/params/AppendFileParams.h"
#include "file/params/CreateFileParams.h"
#include "file/params/DeleteFileParams.h"
#include "file/params/GetFileContentsParams.h"
#include "file/params/GetFileInfoParams.h"
#include "file/params/UpdateFileParams.h"
#include "key/KeyService.h"
#include "sdk/SdkClient.h"
#include "json/JsonErrorType.h"
#include "json/JsonRpcException.h"

#include <FileAppendTransaction.h>
#include <FileContentsQuery.h>
#include <FileCreateTransaction.h>
#include <FileDeleteTransaction.h>
#include <FileId.h>
#include <FileInfo.h>
#include <FileInfoQuery.h>
#include <FileUpdateTransaction.h>
#include <Hbar.h>
#include <Key.h>
#include <KeyList.h>
#include <PrivateKey.h>
#include <Status.h>
#include <TransactionReceipt.h>
#include <TransactionResponse.h>
#include <impl/EntityIdHelper.h>
#include <impl/HexConverter.h>
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
nlohmann::json appendFile(const AppendFileParams& params)
{
  FileAppendTransaction fileAppendTransaction;
  fileAppendTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mFileId.has_value())
  {
    fileAppendTransaction.setFileId(FileId::fromString(params.mFileId.value()));
  }

  fileAppendTransaction.setContents(params.mContents);

  if (params.mMaxChunks.has_value())
  {
    fileAppendTransaction.setMaxChunks(static_cast<unsigned int>(params.mMaxChunks.value()));
  }

  if (params.mChunkSize.has_value())
  {
    fileAppendTransaction.setChunkSize(static_cast<unsigned int>(params.mChunkSize.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(fileAppendTransaction, SdkClient::getClient());
  }

  return {
    {"status",
     gStatusToString.at(
        fileAppendTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus)}
  };
}

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
    {"status", gStatusToString.at(txReceipt.mStatus)}
  };
}

//-----
nlohmann::json deleteFile(const DeleteFileParams& params)
{
  FileDeleteTransaction fileDeleteTransaction;
  fileDeleteTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mFileId.has_value())
  {
    fileDeleteTransaction.setFileId(FileId::fromString(params.mFileId.value()));
  }

  if (params.mCommonTxParams.has_value())
  {
    params.mCommonTxParams->fillOutTransaction(fileDeleteTransaction, SdkClient::getClient());
  }

  return {
    {"status",
     gStatusToString.at(
        fileDeleteTransaction.execute(SdkClient::getClient()).getReceipt(SdkClient::getClient()).mStatus)}
  };
}

//-----
nlohmann::json getFileContents(const GetFileContentsParams& params)
{
  FileContentsQuery query;
  query.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  query.setFileId(FileId::fromString(params.mFileId));

  // Parsing the tinybar string to int64_t and set the query payment
  if (params.mQueryPayment.has_value())
  {
    query.setQueryPayment(Hbar::fromTinybars(std::stoll(params.mQueryPayment.value())));
  }

  // Parsing the tinybar string to int64_t and set the max query payment
  if (params.mMaxQueryPayment.has_value())
  {
    query.setMaxQueryPayment(Hbar::fromTinybars(std::stoll(params.mMaxQueryPayment.value())));
  }

  // Execute
  const std::vector<std::byte> contents = query.execute(SdkClient::getClient());

  return {
    {"contents", internal::Utilities::byteVectorToString(contents)}
  };
}

//-----
nlohmann::json getFileInfo(const GetFileInfoParams& params)
{
  FileInfoQuery query;
  query.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  if (params.mFileId.has_value())
  {
    query.setFileId(FileId::fromString(params.mFileId.value()));
  }

  const FileInfo info = query.execute(SdkClient::getClient());

  nlohmann::json response;

  response["fileId"] = info.mFileId.toString();
  response["size"] = std::to_string(info.mSize);
  response["isDeleted"] = info.mIsDeleted;

  auto expirySeconds =
    std::chrono::duration_cast<std::chrono::seconds>(info.mExpirationTime.time_since_epoch()).count();
  response["expirationTime"] = std::to_string(expirySeconds);

  response["memo"] = info.mMemo;
  response["ledgerId"] = info.mLedgerId.toString();

  // Handle the keys
  response["keys"] = nlohmann::json::array();
  if (!info.mAdminKeys.empty())
  {
    response["keys"].push_back(internal::HexConverter::bytesToHex(info.mAdminKeys.toBytes()));
  }
  return response;
}

//-----
nlohmann::json updateFile(const UpdateFileParams& params)
{
  FileUpdateTransaction fileUpdateTransaction;
  fileUpdateTransaction.setGrpcDeadline(SdkClient::DEFAULT_TCK_REQUEST_TIMEOUT);

  // Set the file ID (optional)
  if (params.mFileId.has_value())
  {
    fileUpdateTransaction.setFileId(FileId::fromString(params.mFileId.value()));
  }

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
    {"status", gStatusToString.at(txReceipt.mStatus)}
  };
}

} // namespace Hiero::TCK::FileService
