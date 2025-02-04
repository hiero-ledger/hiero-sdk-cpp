// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ACCOUNT_ALLOWANCE_DELETE_TRANSACTION_H_
#define HIERO_SDK_CPP_ACCOUNT_ALLOWANCE_DELETE_TRANSACTION_H_

#include "TokenNftAllowance.h"
#include "Transaction.h"

#include <vector>

namespace proto
{
class CryptoDeleteAllowanceTransactionBody;
class TransactionBody;
}

namespace Hiero
{
class AccountId;
class NftId;
}

namespace Hiero
{
/**
 * A transaction called by the token owner to delete allowances for NFTs only. In order to delete an existing Hbar or
 * fungible token allowance the AccountAllowanceApproveTransaction API should be used with an amount of 0.
 *
 * The total number of NFT serial number deletions contained within the transaction body cannot exceed 20.
 */
class AccountAllowanceDeleteTransaction : public Transaction<AccountAllowanceDeleteTransaction>
{
public:
  AccountAllowanceDeleteTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a CryptoDeleteAllowance transaction.
   */
  explicit AccountAllowanceDeleteTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit AccountAllowanceDeleteTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Remove all NFT allowances from an account.
   *
   * @param nftId The ID of the NFT to remove as an allowance.
   * @param owner The ID of the account from which to remove the allowances.
   * @return A reference to this AccountAllowanceDeleteTransaction object with the added allowance removal.
   * @throws IllegalStateException If this AccountAllowanceDeleteTransaction is frozen.
   */
  AccountAllowanceDeleteTransaction& deleteAllTokenNftAllowances(const NftId& nftId, const AccountId& owner);

  /**
   * Get the list of a NFT allowance removals added to this AccountAllowanceDeleteTransaction.
   *
   * @return The list of a NFT allowance removals added to this AccountAllowanceDeleteTransaction.
   */
  [[nodiscard]] inline std::vector<TokenNftAllowance> getTokenNftAllowanceDeletions() const
  {
    return mNftAllowanceDeletions;
  }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this
   * AccountAllowanceDeleteTransaction's data to a Node.
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
   * Derived from Transaction. Verify that all the checksums in this AccountAllowanceDeleteTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This AccountAllowanceDeleteTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the AccountAllowanceDeleteTransaction protobuf representation to the
   * Transaction protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this AccountAllowanceDeleteTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a CryptoDeleteAllowanceTransactionBody protobuf object from this AccountAllowanceDeleteTransaction object.
   *
   * @return A pointer to a CryptoDeleteAllowanceTransactionBody protobuf object filled with this
   *         AccountAllowanceDeleteTransaction object's data.
   */
  [[nodiscard]] proto::CryptoDeleteAllowanceTransactionBody* build() const;

  /**
   * The list of NFT allowances to be deleted.
   */
  std::vector<TokenNftAllowance> mNftAllowanceDeletions;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ACCOUNT_ALLOWANCE_DELETE_TRANSACTION_H_
