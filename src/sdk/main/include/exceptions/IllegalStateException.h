// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ILLEGAL_STATE_EXCEPTION_H_
#define HIERO_SDK_CPP_ILLEGAL_STATE_EXCEPTION_H_

#include <exception>
#include <string>
#include <string_view>

namespace Hiero
{
/**
 * Exception that is thrown when an action is attempted when an object is not in the correct state for said action.
 */
class IllegalStateException : public std::exception
{
public:
  /**
   * Construct with a message.
   *
   * @param msg The error message to further describe this exception.
   */
  explicit IllegalStateException(std::string_view msg)
    : mError(msg)
  {
  }

  /**
   * Get the descriptor message for this error.
   *
   * @return The descriptor message for this error.
   */
  [[nodiscard]] const char* what() const noexcept override { return mError.c_str(); };

private:
  /**
   * Descriptive error message. Stored by value so the exception remains valid after the constructing
   * expression's temporaries are destroyed.
   */
  std::string mError;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ILLEGAL_STATE_EXCEPTION_H_
