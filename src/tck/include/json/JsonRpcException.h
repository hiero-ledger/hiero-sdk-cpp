// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_JSON_RPC_EXCEPTION_H_
#define HIERO_TCK_CPP_JSON_RPC_EXCEPTION_H_

#include "json/JsonErrorType.h"

#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <string_view>

namespace Hiero::TCK
{
/**
 * An exception thrown when there is any issue parsing/executing JSON RPC calls.
 */
class JsonRpcException : public std::exception
{
public:
  /**
   * Construct with an error code and an error message.
   *
   * @param code    The error code.
   * @param message The error message.
   */
  JsonRpcException(JsonErrorType code, std::string_view message) noexcept;

  /**
   * Construct with an error code, an error message, and additional JSON data.
   *
   * @param code    The error code.
   * @param message The error message.
   * @param data    The additional JSON data.
   */
  JsonRpcException(JsonErrorType code, std::string_view message, nlohmann::json data) noexcept;

  /**
   * Derived from std::exception. Get the full error message as a C-string.
   *
   * @return The full error message as a C-string.
   */
  [[nodiscard]] const char* what() const noexcept override { return mError.c_str(); }

  /**
   * Get the error code.
   *
   * @return The error code.
   */
  [[nodiscard]] inline JsonErrorType getCode() const noexcept { return mCode; }

  /**
   * Get the error message.
   *
   * @return The error message.
   */
  [[nodiscard]] inline std::string getMessage() const noexcept { return mMessage; }

  /**
   * Get the additional JSON data.
   *
   * @return The additional JSON data.
   */
  [[nodiscard]] inline nlohmann::json getData() const noexcept { return mData; }

private:
  /**
   * The error code.
   */
  JsonErrorType mCode;

  /**
   * The error message.
   */
  std::string mMessage;

  /**
   * The additional JSON data.
   */
  nlohmann::json mData;

  /**
   * The full error message.
   */
  std::string mError;
};

} // namespace Hiero::TCK

#endif // HIERO_TCK_CPP_JSON_RPC_EXCEPTION_H_
