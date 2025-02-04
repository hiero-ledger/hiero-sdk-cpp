// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_RECEIPT_STATUS_EXCEPTION_H_
#define HIERO_SDK_CPP_RECEIPT_STATUS_EXCEPTION_H_

#include "Status.h"
#include "TransactionId.h"

#include <exception>
#include <string>
#include <string_view>

namespace Hiero
{
/**
 * Exception that is thrown when a TransactionReceipt contains a non-successful status (i.e. not Status::SUCCESS) and it
 * is configured to do so.
 */
class ReceiptStatusException : public std::exception
{
public:
  /**
   * Construct with the ID of the transaction that failed and its status.
   *
   * @param transactionId The ID of the transaction that failed.
   * @param status        The status of the transaction.
   */
  explicit ReceiptStatusException(const TransactionId& transactionId, Status status)
    : mTransactionId(transactionId)
    , mStatus(status)
    , mError("receipt for transaction " + mTransactionId.toString() + " raised status " + gStatusToString.at(mStatus))
  {
  }

  /**
   * Get the descriptor message for this error.
   *
   * @return The descriptor message for this error.
   */
  [[nodiscard]] const char* what() const noexcept override { return mError.c_str(); };

  /**
   * The ID of the transaction that failed.
   */
  TransactionId mTransactionId;

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

#endif // HIERO_SDK_CPP_RECEIPT_STATUS_EXCEPTION_H_
