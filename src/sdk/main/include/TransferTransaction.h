// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TRANSFER_TRANSACTION_H_
#define HIERO_SDK_CPP_TRANSFER_TRANSACTION_H_

#include "HbarTransfer.h"
#include "TokenNftTransfer.h"
#include "TokenTransfer.h"
#include "Transaction.h"

#include <unordered_map>
#include <vector>

namespace proto
{
class CryptoTransferTransactionBody;
}

namespace Hiero
{
class AccountId;
class EvmAddress;
class NftId;
class TokenId;
}

namespace Hiero
{
/**
 * Transfers cryptocurrency among two or more accounts by making the desired adjustments to their balances. Each
 * transfer list can specify up to 10 adjustments. Each negative amount is withdrawn from the corresponding account (a
 * sender), and each positive one is added to the corresponding account (a receiver). The amounts list must sum to zero.
 * Each amount is a number of tinybars (there are 100,000,000 tinybars in one hbar). If any sender account fails to
 * have sufficient hbars, then the entire transaction fails, and none of those transfers occur, though the transaction
 * fee is still charged. This transaction must be signed by the keys for all the sending accounts, and for any receiving
 * accounts that have mReceiverSigRequired == \c TRUE. The signatures are in the same order as the accounts, skipping
 * those accounts that don't need a signature.
 */
class TransferTransaction : public Transaction<TransferTransaction>
{
public:
  TransferTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a CryptoTransfer transaction.
   */
  explicit TransferTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TransferTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Add an Hbar transfer to be submitted as part of this TransferTransaction.
   *
   * @param accountId The ID of the account associated with this transfer.
   * @param amount    The amount of Hbar to transfer.
   * @return A reference to this TransferTransaction object with the newly-added Hbar transfer.
   * @throws IllegalStateException If this TransferTransaction is frozen.
   */
  TransferTransaction& addHbarTransfer(const AccountId& accountId, const Hbar& amount);

  /**
   * Add an Hbar transfer to be submitted as part of this TransferTransaction.
   *
   * @param evmAddress The EVM address associated with this transfer.
   * @param amount     The amount of Hbar to transfer.
   * @return A reference to this TransferTransaction object with the newly-added Hbar transfer.
   * @throws IllegalStateException If this TransferTransaction is frozen.
   */
  TransferTransaction& addHbarTransfer(const EvmAddress& evmAddress, const Hbar& amount);

  /**
   * Add a token transfer to be submitted as part of this TransferTransaction.
   *
   * @param tokenId   The ID of the token associated with this transfer.
   * @param accountId The ID of the account associated with this transfer.
   * @param amount    The number of tokens to transfer.
   * @return A reference to this TransferTransaction object with the newly-added token transfer.
   * @throws IllegalStateException If this TransferTransaction is frozen.
   */
  TransferTransaction& addTokenTransfer(const TokenId& tokenId, const AccountId& accountId, const int64_t& amount);

  /**
   * Add an NFT transfer to be submitted as part of this TransferTransaction.
   *
   * @param nftId             The ID of the NFT associated with this transfer.
   * @param senderAccountId   The ID of the account sending the NFT.
   * @param receiverAccountId The ID of the receiving sending the NFT.
   * @return A reference to this TransferTransaction object with the newly-added NFT transfer.
   * @throws IllegalStateException If this TransferTransaction is frozen.
   */
  TransferTransaction& addNftTransfer(const NftId& nftId,
                                      const AccountId& senderAccountId,
                                      const AccountId& receiverAccountId);

  /**
   * Add a token transfer with decimals to be submitted as part of this TransferTransaction.
   *
   * @param tokenId   The ID of the token associated with this transfer.
   * @param accountId The ID of the account associated with this transfer.
   * @param amount    The number of tokens to transfer.
   * @param decimals  The number of decimals in the transfer amount.
   * @return A reference to this TransferTransaction object with the newly-added token transfer.
   * @throws IllegalStateException If this TransferTransaction is frozen.
   * @throws std::invalid_argument If decimals does not match previously set decimals for this token.
   */
  TransferTransaction& addTokenTransferWithDecimals(const TokenId& tokenId,
                                                    const AccountId& accountId,
                                                    const int64_t& amount,
                                                    uint32_t decimals);

  /**
   * Add an approved Hbar transfer to be submitted as part of this TransferTransaction.
   *
   * @param accountId The ID of the account associated with this transfer.
   * @param amount    The amount of Hbar to transfer.
   * @return A reference to this TransferTransaction object with the newly-added approved Hbar transfer.
   * @throws IllegalStateException If this TransferTransaction is frozen.
   */
  TransferTransaction& addApprovedHbarTransfer(const AccountId& accountId, const Hbar& amount);

  /**
   * Add an approved token transfer to be submitted as part of this TransferTransaction.
   *
   * @param tokenId   The ID of the token associated with this transfer.
   * @param accountId The ID of the account associated with this transfer.
   * @param amount    The number of tokens to transfer.
   * @return A reference to this TransferTransaction object with the newly-added approved token transfer.
   * @throws IllegalStateException If this TransferTransaction is frozen.
   */
  TransferTransaction& addApprovedTokenTransfer(const TokenId& tokenId,
                                                const AccountId& accountId,
                                                const int64_t& amount);

  /**
   * Add an approved NFT transfer to be submitted as part of this TransferTransaction.
   *
   * @param nftId             The ID of the NFT associated with this transfer.
   * @param senderAccountId   The ID of the account sending the NFT.
   * @param receiverAccountId The ID of the receiving sending the NFT.
   * @return A reference to this TransferTransaction object with the newly-added approved NFT transfer.
   * @throws IllegalStateException If this TransferTransaction is frozen.
   */
  TransferTransaction& addApprovedNftTransfer(const NftId& nftId,
                                              const AccountId& senderAccountId,
                                              const AccountId& receiverAccountId);

  /**
   * Add an approved token transfer with decimals to be submitted as part of this TransferTransaction.
   *
   * @param tokenId   The ID of the token associated with this transfer.
   * @param accountId The ID of the account associated with this transfer.
   * @param amount    The number of tokens to transfer.
   * @param decimals  The number of decimals in the transfer amount.
   * @return A reference to this TransferTransaction object with the newly-added approved token transfer.
   * @throws IllegalStateException If this TransferTransaction is frozen.
   * @throws std::invalid_argument If decimals does not match previously set decimals for this token.
   */
  TransferTransaction& addApprovedTokenTransferWithDecimals(const TokenId& tokenId,
                                                            const AccountId& accountId,
                                                            const int64_t& amount,
                                                            uint32_t decimals);

  /**
   * Get all Hbar transfers that have been added to this TransferTransaction.
   *
   * @return The map of Hbar transfers.
   */
  [[nodiscard]] std::unordered_map<AccountId, Hbar> getHbarTransfers() const;

  /**
   * Get all token transfers that have been added to this TransferTransaction.
   *
   * @return The map of token transfers.
   */
  [[nodiscard]] std::unordered_map<TokenId, std::unordered_map<AccountId, int64_t>> getTokenTransfers() const;

  /**
   * Get all NFT transfers that have been added to this TransferTransaction.
   *
   * @return The map of NFT transfers.
   */
  [[nodiscard]] std::unordered_map<TokenId, std::vector<TokenNftTransfer>> getNftTransfers() const;

  /**
   * Get the expected decimals for token transfers that have been added to this TransferTransaction.
   *
   * @return The map of expected decimals.
   */
  [[nodiscard]] std::unordered_map<TokenId, uint32_t> getTokenIdDecimals() const;

private:
  /**
   * Allow Queries to create Transaction protobuf objects from TransferTransactions (to use as payments).
   */
  template<typename SdkRequestType, typename SdkResponseType>
  friend class Query;
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TransferTransaction's data to a
   * Node.
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
   * Derived from Transaction. Verify that all the checksums in this TransferTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TransferTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TransferTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TransferTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a CryptoTransferTransactionBody protobuf object from this TransferTransaction object.
   *
   * @return A pointer to a CryptoTransferTransactionBody protobuf object filled with this TransferTransaction object's
   *         data.
   */
  [[nodiscard]] proto::CryptoTransferTransactionBody* build() const;

  /**
   * Add an Hbar transfer to the Hbar transfers list.
   *
   * @param accountId The ID of the account doing the transfer.
   * @param amount    The amount to transfer.
   * @param approved  \c TRUE if this is an approved allowance transfer, otherwise \c FALSE.
   */
  void doHbarTransfer(const AccountId& accountId, const Hbar& amount, bool approved);

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
   * The desired Hbar balance adjustments.
   */
  std::vector<HbarTransfer> mHbarTransfers;

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

#endif // HIERO_SDK_CPP_TRANSFER_TRANSACTION_H_
