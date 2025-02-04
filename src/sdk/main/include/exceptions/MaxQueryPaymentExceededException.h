// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_MAX_QUERY_PAYMENT_EXCEEDED_EXCEPTION_H_
#define HIERO_SDK_CPP_MAX_QUERY_PAYMENT_EXCEEDED_EXCEPTION_H_

#include <exception>
#include <string_view>

namespace Hiero
{
/**
 * Exception that is thrown when the cost to execute a Query is larger than the maximum allowed amount.
 */
class MaxQueryPaymentExceededException : public std::exception
{
public:
  /**
   * Construct with a message.
   *
   * @param msg The error message to further describe this exception.
   */
  explicit MaxQueryPaymentExceededException(std::string_view msg)
    : mError(msg)
  {
  }

  /**
   * Get the descriptor message for this error.
   *
   * @return The descriptor message for this error.
   */
  [[nodiscard]] const char* what() const noexcept override { return mError.data(); };

private:
  /**
   * Descriptive error message.
   */
  std::string_view mError;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_MAX_QUERY_PAYMENT_EXCEEDED_EXCEPTION_H_
