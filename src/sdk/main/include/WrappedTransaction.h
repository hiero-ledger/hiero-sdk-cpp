// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_UNKNOWN_TRANSACTION_H_
#define HIERO_SDK_CPP_UNKNOWN_TRANSACTION_H_

#include "AccountAllowanceApproveTransaction.h"
#include "AccountAllowanceDeleteTransaction.h"
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "AccountUpdateTransaction.h"
#include "ContractCreateTransaction.h"
#include "ContractDeleteTransaction.h"
#include "ContractExecuteTransaction.h"
#include "ContractUpdateTransaction.h"
#include "EthereumTransaction.h"
#include "FileAppendTransaction.h"
#include "FileCreateTransaction.h"
#include "FileDeleteTransaction.h"
#include "FileUpdateTransaction.h"
#include "FreezeTransaction.h"
#include "NodeCreateTransaction.h"
#include "NodeDeleteTransaction.h"
#include "NodeUpdateTransaction.h"
#include "PrngTransaction.h"
#include "ScheduleCreateTransaction.h"
#include "ScheduleDeleteTransaction.h"
#include "ScheduleSignTransaction.h"
#include "SystemDeleteTransaction.h"
#include "SystemUndeleteTransaction.h"
#include "TokenAirdropTransaction.h"
#include "TokenAssociateTransaction.h"
#include "TokenBurnTransaction.h"
#include "TokenCancelAirdropTransaction.h"
#include "TokenClaimAirdropTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenDeleteTransaction.h"
#include "TokenDissociateTransaction.h"
#include "TokenFeeScheduleUpdateTransaction.h"
#include "TokenFreezeTransaction.h"
#include "TokenGrantKycTransaction.h"
#include "TokenMintTransaction.h"
#include "TokenPauseTransaction.h"
#include "TokenRejectTransaction.h"
#include "TokenRevokeKycTransaction.h"
#include "TokenUnfreezeTransaction.h"
#include "TokenUnpauseTransaction.h"
#include "TokenUpdateNftsTransaction.h"
#include "TokenUpdateTransaction.h"
#include "TokenWipeTransaction.h"
#include "TopicCreateTransaction.h"
#include "TopicDeleteTransaction.h"
#include "TopicMessageSubmitTransaction.h"
#include "TopicUpdateTransaction.h"
#include "TransactionType.h"
#include "TransferTransaction.h"

#include <variant>

namespace proto
{
class SchedulableTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Helper class to hold a Transaction variant.
 */
class WrappedTransaction
{
public:
  /**
   * Helper alias to use to consolidate space and provide clarity.
   */
  using AnyPossibleTransaction = std::variant<AccountAllowanceApproveTransaction,
                                              AccountAllowanceDeleteTransaction,
                                              AccountCreateTransaction,
                                              AccountDeleteTransaction,
                                              AccountUpdateTransaction,
                                              ContractCreateTransaction,
                                              ContractDeleteTransaction,
                                              ContractExecuteTransaction,
                                              ContractUpdateTransaction,
                                              EthereumTransaction,
                                              FileAppendTransaction,
                                              FileCreateTransaction,
                                              FileDeleteTransaction,
                                              FileUpdateTransaction,
                                              FreezeTransaction,
                                              NodeCreateTransaction,
                                              NodeDeleteTransaction,
                                              NodeUpdateTransaction,
                                              PrngTransaction,
                                              ScheduleCreateTransaction,
                                              ScheduleDeleteTransaction,
                                              ScheduleSignTransaction,
                                              SystemDeleteTransaction,
                                              SystemUndeleteTransaction,
                                              TokenAirdropTransaction,
                                              TokenAssociateTransaction,
                                              TokenBurnTransaction,
                                              TokenCancelAirdropTransaction,
                                              TokenClaimAirdropTransaction,
                                              TokenCreateTransaction,
                                              TokenDeleteTransaction,
                                              TokenDissociateTransaction,
                                              TokenFeeScheduleUpdateTransaction,
                                              TokenFreezeTransaction,
                                              TokenGrantKycTransaction,
                                              TokenMintTransaction,
                                              TokenPauseTransaction,
                                              TokenRejectTransaction,
                                              TokenRevokeKycTransaction,
                                              TokenUnfreezeTransaction,
                                              TokenUnpauseTransaction,
                                              TokenUpdateNftsTransaction,
                                              TokenUpdateTransaction,
                                              TokenWipeTransaction,
                                              TopicCreateTransaction,
                                              TopicDeleteTransaction,
                                              TopicMessageSubmitTransaction,
                                              TopicUpdateTransaction,
                                              TransferTransaction>;

  WrappedTransaction() = default;

  /**
   * Construct with a transaction.
   *
   * @param transaction The transaction to wrap.
   */
  explicit WrappedTransaction(AnyPossibleTransaction transaction);

  /**
   * Construct a WrappedTransaction object from a TransactionBody protobuf object.
   *
   * @param proto The TransactionBody protobuf object from which to construct a WrappedTransaction object.
   * @return The constructed WrappedTransaction object.
   * @throws std::invalid_argument If the input TransactionBody doesn't contain a valid Transaction.
   */
  [[nodiscard]] static WrappedTransaction fromProtobuf(const proto::TransactionBody& proto);

  /**
   * Construct a WrappedTransaction object from a SchedulableTransactionBody protobuf object.
   *
   * @param proto The SchedulableTransactionBody protobuf object from which to construct a WrappedTransaction object.
   * @return The constructed WrappedTransaction object.
   * @throws std::invalid_argument If the input SchedulableTransactionBody doesn't contain a Transaction.
   */
  [[nodiscard]] static WrappedTransaction fromProtobuf(const proto::SchedulableTransactionBody& proto);

  /**
   * Construct a TransactionBody protobuf object from this WrappedTransaction object.
   *
   * @return A pointer to the created TransactionBody protobuf object.
   * @throws UninitializedException If no Transaction is contained within this WrappedTransaction.
   */
  [[nodiscard]] std::unique_ptr<proto::TransactionBody> toProtobuf() const;

  /**
   * Construct a SchedulableTransactionBody protobuf object from this WrappedTransaction object.
   *
   * @return A pointer to the created SchedulableTransactionBody protobuf object.
   * @throws UninitializedException If no Transaction is contained within this WrappedTransaction.
   */
  [[nodiscard]] std::unique_ptr<proto::SchedulableTransactionBody> toSchedulableProtobuf() const;

  /**
   * Set the transaction to wrap.
   *
   * @param transaction The transaction to wrap.
   * @return A reference to this WrappedTransaction with the newly-set transaction.
   */
  WrappedTransaction& setTransaction(const AnyPossibleTransaction& transaction);

  /**
   * Get the type of wrapped transaction.
   *
   * @return The type of wrapped transaction.
   */
  [[nodiscard]] inline TransactionType getTransactionType() const
  {
    return static_cast<TransactionType>(mTransaction.index());
  }

  /**
   * Get a pointer to the wrapped transaction.
   *
   * @return A pointer to the wrapped transaction. Returns nullptr if the TransactionType is unknown or uninitialized.
   */
  // clang-format off
  template<typename T> inline T* getTransaction() { return std::get_if<T>(&mTransaction); }
  template<typename T> inline const T* getTransaction() const { return std::get_if<T>(&mTransaction); }
  // clang-format on

private:
  /**
   * The actual wrapped transaction.
   */
  AnyPossibleTransaction mTransaction;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_UNKNOWN_TRANSACTION_H_
