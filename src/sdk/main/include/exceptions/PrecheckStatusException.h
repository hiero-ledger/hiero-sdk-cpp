// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_PRECHECK_STATUS_EXCEPTION_H_
#define HIERO_SDK_CPP_PRECHECK_STATUS_EXCEPTION_H_

#include "Status.h"
#include "TransactionId.h"

#include <exception>
#include <optional>
#include <string_view>

namespace Hiero
{
/**
 * Exception that is thrown when a request has failed its pre-check.
 */
class PrecheckStatusException : public std::exception
{
public:
  /**
   * Construct with the ID of the transaction that failed and its status.
   *
   * @param transactionId The ID of the transaction that failed.
   * @param status        The status of the transaction.
   */
  explicit PrecheckStatusException(Status status,
                                   const std::optional<TransactionId>& transactionId = std::optional<TransactionId>())
    : mTransactionId(transactionId)
    , mStatus(status)
    , mError(((transactionId.has_value()) ? "Hiero transaction " + transactionId->toString() + ' ' : "") +
             "failed precheck with status " + gStatusToString.at(mStatus))
  {
  }

  /**
   * Get the descriptor message for this error.
   *
   * @return The descriptor message for this error.
   */
  [[nodiscard]] const char* what() const noexcept override { return mError.data(); };

  /**
   * The ID of the transaction that failed. This can be uninitialized if a query fails pre-check without an associated
   * payment transaction.
   */
  std::optional<TransactionId> mTransactionId;

  /**
   * The status of the failed transaction.
   */
  Status mStatus = Status::OK;

  /**
   * Descriptive error message.
   */
  std::string mError;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_PRECHECK_STATUS_EXCEPTION_H_
