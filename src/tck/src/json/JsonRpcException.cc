// SPDX-License-Identifier: Apache-2.0
#include "json/JsonRpcException.h"
#include "json/JsonErrorType.h"

#include <nlohmann/json.hpp>
#include <string>
#include <utility>

namespace Hiero::TCK
{
//-----
JsonRpcException::JsonRpcException(JsonErrorType code, std::string_view message) noexcept
  : mCode(code)
  , mMessage(message)
  , mData(nullptr)
  , mError(std::to_string(static_cast<int>(code)) + ": " + mMessage)
{
}

//-----
JsonRpcException::JsonRpcException(JsonErrorType code, std::string_view message, nlohmann::json data) noexcept
  : mCode(code)
  , mMessage(message)
  , mData(std::move(data))
  , mError(std::to_string(static_cast<int>(code)) + ": " + mMessage + ", data: " + mData.dump())
{
}

} // namespace Hiero::TCK
