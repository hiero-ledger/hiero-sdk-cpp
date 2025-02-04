// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_REJECT_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_REJECT_TRANSACTION_H_

#include "AccountId.h"
#include "NftId.h"
#include "TokenId.h"
#include "Transaction.h"

namespace proto
{
class TokenRejectTransactionBody;
class TransactionBody;
class TokenReference;
}

namespace Hiero
{
/**
 * Reject undesired token(s).Transfer one or more token balances held by the requesting account to the treasury for each
 * token type. Each transfer SHALL be one of the following:
 * - A single non-fungible/unique token.
 * - The full balance held for a fungible/common token type.
 *
 * A single tokenReject transaction SHALL support a maximum of 10 transfers.
 *
 * Transaction Record Effects:
 * - Each successful transfer from `payer` to `treasury` SHALL be recorded in `token_transfer_list` for the transaction
 * record.
 */
class TokenRejectTransaction : public Transaction<TokenRejectTransaction>
{
public:
  TokenRejectTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenReject transaction.
   */
  explicit TokenRejectTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenRejectTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Get the account holding tokens to be rejected.
   *
   * @return Optional containing the account Id of the owner.
   */
  [[nodiscard]] std::optional<AccountId> getOwner() const { return mOwner; };

  /**
   * Get the list of fungible tokens to be rejected.
   *
   * @return A vector of TokenId objects.
   */
  [[nodiscard]] const std::vector<TokenId>& getFts() const { return mFts; };

  /**
   * Get the list of non-fungible tokens to be rejected.
   *
   * @return A vector of NftId objects.
   */
  [[nodiscard]] const std::vector<NftId>& getNfts() const { return mNfts; };

  /**
   * Set a new account holding tokens to be rejected.
   *
   * @param owner Account Id of the account.
   * @return A reference to this TokenRejectTransaction with the newly-set owner.
   */
  TokenRejectTransaction& setOwner(const AccountId& owner);

  /**
   * Set a new fungible tokens list of tokens to be rejected.
   *
   * @param fts List of token ids.
   * @return A reference to this TokenRejectTransaction with the newly-set tokens list.
   */
  TokenRejectTransaction& setFts(const std::vector<TokenId>& fts);

  /**
   * Set a new non-fungible tokens list of tokens to be rejected.
   *
   * @param nfts List of nft ids.
   * @return A reference to this TokenRejectTransaction with the newly-set nfts list.
   */
  TokenRejectTransaction& setNfts(const std::vector<NftId>& nfts);

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenRejectTransaction's data to
   * a Node.
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
   * Derived from Transaction. Verify that all the checksums in this TokenRejectTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenRejectTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenRejectTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenRejectTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenRejectTransactionBody protobuf object from this TokenRejectTransaction object.
   *
   * @return A pointer to a TokenRejectTransactionBody protobuf object filled with this TokenRejectTransaction object's
   *         data.
   */
  [[nodiscard]] proto::TokenRejectTransactionBody* build() const;

  /**
   * An account holding the tokens to be rejected.
   * If set, this account MUST sign this transaction.
   * If not set, the payer for this transaction SHALL be the account rejecting tokens.
   */
  std::optional<AccountId> mOwner;

  /**
   * On success each rejected token serial number or balance SHALL be transferred from
   * the requesting account to the treasury account for that token type.
   * After rejection the requesting account SHALL continue to be associated with the token.
   * if dissociation is desired then a separate TokenDissociate transaction MUST be submitted to remove the association.
   *
   * A list of one or more fungible token rejections.
   */
  std::vector<TokenId> mFts;

  /**
   * A list of one or more non-fungible token rejections.
   */
  std::vector<NftId> mNfts;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_REJECT_TRANSACTION_H_
