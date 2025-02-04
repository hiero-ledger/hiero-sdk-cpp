// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_AIRDROP_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_AIRDROP_TRANSACTION_H_

#include "TokenNftTransfer.h"
#include "TokenTransfer.h"
#include "Transaction.h"

namespace proto
{
class TokenAirdropTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Airdrop one or more tokens to one or more accounts.
 *
 * ### Effects
 * This distributes tokens from the balance of one or more sending account(s) to the balance
 * of one or more recipient accounts. Accounts MAY receive the tokens in one of four ways.
 *
 *  - An account already associated to the token to be distributed SHALL receive the
 *    airdropped tokens immediately to the recipient account balance.
 *    The fee for this transfer SHALL include the transfer, the airdrop fee, and any custom fees.
 *  - An account with available automatic association slots SHALL be automatically
 *    associated to the token, and SHALL immediately receive the airdropped tokens to the
 *    recipient account balance.
 *    The fee for this transfer SHALL include the transfer, the association, the cost to renew
 *    that association once, the airdrop fee, and any custom fees.
 *  - An account with "receiver signature required" set SHALL have a "Pending Airdrop" created
 *    and must claim that airdrop with a `claimAirdrop` transaction.
 *    The fee for this transfer SHALL include the transfer, the association, the cost to renew
 *    that association once, the airdrop fee, and any custom fees. If the pending airdrop is not
 *    claimed immediately, the `sender` SHALL pay the cost to renew the token association, and
 *    the cost to maintain the pending airdrop, until the pending airdrop is claimed or cancelled.
 *  - An account with no available automatic association slots SHALL have a "Pending Airdrop"
 *    created and must claim that airdrop with a `claimAirdrop` transaction.
 *    The fee for this transfer SHALL include the transfer, the association, the cost to renew
 *    that association once, the airdrop fee, and any custom fees. If the pending airdrop is not
 *    claimed immediately, the `sender` SHALL pay the cost to renew the token association, and
 *    the cost to maintain the pending airdrop, until the pending airdrop is claimed or cancelled.
 *
 * If an airdrop would create a pending airdrop for a fungible/common token, and a pending airdrop
 * for the same sender, receiver, and token already exists, the existing pending airdrop
 * SHALL be updated to add the new amount to the existing airdrop, rather than creating a new
 * pending airdrop.
 *
 * Any airdrop that completes immediately SHALL be irreversible. Any airdrop that results in a
 * "Pending Airdrop" MAY be canceled via a `cancelAirdrop` transaction.
 *
 * All transfer fees (including custom fees and royalties), as well as the rent cost for the
 * first auto-renewal period for any automatic-association slot occupied by the airdropped
 * tokens, SHALL be charged to the account paying for this transaction.
 *
 * ### Record Stream Effects
 * - Each successful transfer SHALL be recorded in `token_transfer_list` for the transaction record.
 * - Each successful transfer that consumes an automatic association slot SHALL populate the
 *   `automatic_association` field for the record.
 * - Each pending transfer _created_ SHALL be added to the `pending_airdrops` field for the record.
 * - Each pending transfer _updated_ SHALL be added to the `pending_airdrops` field for the record.
 */
class TokenAirdropTransaction : public Transaction<TokenAirdropTransaction>
{
public:
  TokenAirdropTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenAirdrop transaction.
   */
  explicit TokenAirdropTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenAirdropTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Add a token transfer to be submitted as part of this TokenAirdropTransaction.
   *
   * @param tokenId   The ID of the token associated with this transfer.
   * @param accountId The ID of the account associated with this transfer.
   * @param amount    The number of tokens to transfer.
   * @return A reference to this TokenAirdropTransaction object with the newly-added token transfer.
   * @throws IllegalStateException If this TokenAirdropTransaction is frozen.
   */
  TokenAirdropTransaction& addTokenTransfer(const TokenId& tokenId, const AccountId& accountId, const int64_t& amount);

  /**
   * Add an NFT transfer to be submitted as part of this TokenAirdropTransaction.
   *
   * @param nftId             The ID of the NFT associated with this transfer.
   * @param senderAccountId   The ID of the account sending the NFT.
   * @param receiverAccountId The ID of the receiving sending the NFT.
   * @return A reference to this TokenAirdropTransaction object with the newly-added NFT transfer.
   * @throws IllegalStateException If this TokenAirdropTransaction is frozen.
   */
  TokenAirdropTransaction& addNftTransfer(const NftId& nftId,
                                          const AccountId& senderAccountId,
                                          const AccountId& receiverAccountId);

  /**
   * Add a token transfer with decimals to be submitted as part of this TokenAirdropTransaction.
   *
   * @param tokenId   The ID of the token associated with this transfer.
   * @param accountId The ID of the account associated with this transfer.
   * @param amount    The number of tokens to transfer.
   * @param decimals  The number of decimals in the transfer amount.
   * @return A reference to this TokenAirdropTransaction object with the newly-added token transfer.
   * @throws IllegalStateException If this TokenAirdropTransaction is frozen.
   * @throws std::invalid_argument If decimals does not match previously set decimals for this token.
   */
  TokenAirdropTransaction& addTokenTransferWithDecimals(const TokenId& tokenId,
                                                        const AccountId& accountId,
                                                        const int64_t& amount,
                                                        uint32_t decimals);

  /**
   * Add an approved token transfer to be submitted as part of this TokenAirdropTransaction.
   *
   * @param tokenId   The ID of the token associated with this transfer.
   * @param accountId The ID of the account associated with this transfer.
   * @param amount    The number of tokens to transfer.
   * @return A reference to this TokenAirdropTransaction object with the newly-added approved token transfer.
   * @throws IllegalStateException If this TokenAirdropTransaction is frozen.
   */
  TokenAirdropTransaction& addApprovedTokenTransfer(const TokenId& tokenId,
                                                    const AccountId& accountId,
                                                    const int64_t& amount);

  /**
   * Add an approved NFT transfer to be submitted as part of this TokenAirdropTransaction.
   *
   * @param nftId             The ID of the NFT associated with this transfer.
   * @param senderAccountId   The ID of the account sending the NFT.
   * @param receiverAccountId The ID of the receiving sending the NFT.
   * @return A reference to this TokenAirdropTransaction object with the newly-added approved NFT transfer.
   * @throws IllegalStateException If this TokenAirdropTransaction is frozen.
   */
  TokenAirdropTransaction& addApprovedNftTransfer(const NftId& nftId,
                                                  const AccountId& senderAccountId,
                                                  const AccountId& receiverAccountId);

  /**
   * Add an approved token transfer with decimals to be submitted as part of this TokenAirdropTransaction.
   *
   * @param tokenId   The ID of the token associated with this transfer.
   * @param accountId The ID of the account associated with this transfer.
   * @param amount    The number of tokens to transfer.
   * @param decimals  The number of decimals in the transfer amount.
   * @return A reference to this TokenAirdropTransaction object with the newly-added approved token transfer.
   * @throws IllegalStateException If this TokenAirdropTransaction is frozen.
   * @throws std::invalid_argument If decimals does not match previously set decimals for this token.
   */
  TokenAirdropTransaction& addApprovedTokenTransferWithDecimals(const TokenId& tokenId,
                                                                const AccountId& accountId,
                                                                const int64_t& amount,
                                                                uint32_t decimals);

  /**
   * Get all token transfers that have been added to this TokenAirdropTransaction.
   *
   * @return The map of token transfers.
   */
  [[nodiscard]] std::unordered_map<TokenId, std::unordered_map<AccountId, int64_t>> getTokenTransfers() const;

  /**
   * Get all NFT transfers that have been added to this TokenAirdropTransaction.
   *
   * @return The map of NFT transfers.
   */
  [[nodiscard]] std::unordered_map<TokenId, std::vector<TokenNftTransfer>> getNftTransfers() const;

  /**
   * Get the expected decimals for token transfers that have been added to this TokenAirdropTransaction.
   *
   * @return The map of expected decimals.
   */
  [[nodiscard]] std::unordered_map<TokenId, uint32_t> getTokenIdDecimals() const;

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenAirdropTransaction's data to
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
   * Derived from Transaction. Verify that all the checksums in this TokenAirdropTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenAirdropTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenAirdropTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenAirdropTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenAirdropTransactionBody protobuf object from this TokenAirdropTransaction object.
   *
   * @return A pointer to a TokenAirdropTransactionBody protobuf object filled with this TokenAirdropTransaction
   * object's data.
   */
  [[nodiscard]] proto::TokenAirdropTransactionBody* build() const;

  /**
   * Add a token transfer to the token transfers list.
   *
   * @param transfer The token transfer to add.
   */
  void doTokenTransfer(const TokenTransfer& transfer);

  /**
   * Add an NFT transfer to the NFT transfers list.
   *
   * @param nftId The ID of the NFT.
   * @param sender The ID of the account sending the NFT.
   * @param receiver The ID of the account receiving the NFT.
   * @param approved \c TRUE if this is an approved allowance NFT transfer, otherwise \c FALSE.
   */
  void doNftTransfer(const NftId& nftId, const AccountId& sender, const AccountId& receiver, bool approved);

  /**
   * The desired token adjustments.
   */
  std::vector<TokenTransfer> mTokenTransfers;

  /**
   * The desired NFT adjustments.
   */
  std::vector<TokenNftTransfer> mNftTransfers;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_AIRDROP_TRANSACTION_H_