// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_FEE_SCHEDULE_UPDATE_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_FEE_SCHEDULE_UPDATE_TRANSACTION_H_

#include "CustomFee.h"
#include "TokenId.h"
#include "Transaction.h"

#include <memory>
#include <vector>

namespace proto
{
class TokenFeeScheduleUpdateTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Update the custom fees for a given token. If the token does not have a fee schedule, the network response returned
 * will be CUSTOM_SCHEDULE_ALREADY_HAS_NO_FEES. You will need to sign the transaction with the fee schedule key to
 * update the fee schedule for the token. If you do not have a fee schedule key set for the token, you will not be able
 * to update the fee schedule.
 *
 * Transaction Signing Requirements:
 *  - Fee schedule key.
 *  - Transaction fee payer account key.
 */
class TokenFeeScheduleUpdateTransaction : public Transaction<TokenFeeScheduleUpdateTransaction>
{
public:
  TokenFeeScheduleUpdateTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenFeeScheduleUpdate transaction.
   */
  explicit TokenFeeScheduleUpdateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenFeeScheduleUpdateTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the token whose fee schedule is to be updated.
   *
   * @param tokenId The ID of the token whose fee schedule is to be updated.
   * @return A reference to this TokenFeeScheduleUpdateTransaction object with the newly-set token ID.
   * @throws IllegalStateException If this TokenFeeScheduleUpdateTransaction is frozen.
   */
  TokenFeeScheduleUpdateTransaction& setTokenId(const TokenId& tokenId);

  /**
   * Set the new custom fees to be assessed during transfers of the token.
   *
   * @param fees The new custom fees to be assessed during transfers of the token.
   * @return A reference to this TokenFeeScheduleUpdateTransaction object with the newly-set fees.
   * @throws IllegalStateException If this TokenFeeScheduleUpdateTransaction is frozen.
   */
  TokenFeeScheduleUpdateTransaction& setCustomFees(const std::vector<std::shared_ptr<CustomFee>>& fees);

  /**
   * Get the ID of the token whose fee schedule is to be updated.
   *
   * @return The ID of the token whose fee schedule is to be updated.
   */
  [[nodiscard]] inline TokenId getTokenId() const { return mTokenId; }

  /**
   * Get the new custom fees to be assessed during transfers of the token.
   *
   * @return The new custom fees to be assessed during transfers of the token.
   */
  [[nodiscard]] inline std::vector<std::shared_ptr<CustomFee>> getCustomFees() const { return mCustomFees; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this
   * TokenFeeScheduleUpdateTransaction's data to a Node.
   *
   * @param request  The Transaction protobuf object to submit.
   * @param node     The Node to which to submit the request.
   * @param deadline The deadline for submitting the request.
   * @param response Pointer to the ProtoResponseType object that gRPC should populate with the response information
   *                 from the gRPC server.
   * @return The gRPC status of the submission.
   */
  [[nodiscard]] grpc::Status submitRequest(const proto::Transaction& request,
                                           const std::shared_ptr<internal::Node>& node,
                                           const std::chrono::system_clock::time_point& deadline,
                                           proto::TransactionResponse* response) const override;

  /**
   * Derived from Transaction. Verify that all the checksums in this TokenFeeScheduleUpdateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenFeeScheduleUpdateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenFeeScheduleUpdateTransaction protobuf representation to the
   * Transaction protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenFeeScheduleUpdateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenFeeScheduleUpdateTransactionBody protobuf object from this TokenFeeScheduleUpdateTransaction object.
   *
   * @return A pointer to a TokenFeeScheduleUpdateTransactionBody protobuf object filled with this
   * TokenFeeScheduleUpdateTransaction object's data.
   */
  [[nodiscard]] proto::TokenFeeScheduleUpdateTransactionBody* build() const;

  /**
   * The ID of the token whose fee schedule is to be updated.
   */
  TokenId mTokenId;

  /**
   * The new custom fees to be assessed during transfers of the token.
   */
  std::vector<std::shared_ptr<CustomFee>> mCustomFees;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_FEE_SCHEDULE_UPDATE_TRANSACTION_H_
