// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_CANCEL_AIRDROP_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_CANCEL_AIRDROP_TRANSACTION_H_

#include "PendingAirdropId.h"
#include "Transaction.h"

namespace proto
{
class TokenCancelAirdropTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Token cancel airdrop
 * Remove one or more pending airdrops from state on behalf of the sender(s)
 * for each airdrop.
 *
 * Each pending airdrop canceled SHALL be removed from state and SHALL NOT be available to claim.
 * Each cancellation SHALL be represented in the transaction body and SHALL NOT be restated
 * in the record file.
 * All cancellations MUST succeed for this transaction to succeed.
 */
class TokenCancelAirdropTransaction : public Transaction<TokenCancelAirdropTransaction>
{
public:
  TokenCancelAirdropTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenCancelAirdrop transaction.
   */
  explicit TokenCancelAirdropTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenCancelAirdropTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Get the list of pending airdrops to be claimed.
   *
   * @return A vector of Pending Airdrop Id objects.
   */
  [[nodiscard]] std::vector<PendingAirdropId> getPendingAirdrops() const { return mPendingAirdrops; };

  /**
   * Set the Pending Airdrops to be claimed
   *
   * @param pendingAirdrops The list of Pending Airdrop Id objects
   * @return A reference to this TokenClaimAirdropTransaction with the newly-set pending airdrops.
   */
  TokenCancelAirdropTransaction& setPendingAirdrops(const std::vector<PendingAirdropId>& pendingAirdrops);

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenCancelAirdropTransaction's
   * data to a Node.
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
   * Derived from Transaction. Verify that all the checksums in this TokenCancelAirdropTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenCancelAirdropTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenCancelAirdropTransaction protobuf representation to the
   * Transaction protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenCancelAirdropTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenCancelAirdropTransactionBody protobuf object from this TokenCancelAirdropTransaction object.
   *
   * @return A pointer to a TokenCancelAirdropTransactionBody protobuf object filled with this
   * TokenCancelAirdropTransaction object's data.
   */
  [[nodiscard]] proto::TokenCancelAirdropTransactionBody* build() const;

  /**
   * A list of one or more pending airdrop identifiers.
   * This list declares the set of pending airdrop entries that the client
   * wishes to cancel; on success all listed pending airdrop entries
   * will be removed.
   * This transaction MUST be signed by the account referenced by a `sender_id` for
   * each entry in this list.
   * This list MUST NOT have any duplicate entries.
   * This list MUST contain between 1 and 10 entries, inclusive.
   */
  std::vector<PendingAirdropId> mPendingAirdrops;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_CANCEL_AIRDROP_TRANSACTION_H_