// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ACCOUNT_ALLOWANCE_APPROVE_TRANSACTION_H_
#define HIERO_SDK_CPP_ACCOUNT_ALLOWANCE_APPROVE_TRANSACTION_H_

#include "HbarAllowance.h"
#include "TokenAllowance.h"
#include "TokenNftAllowance.h"
#include "Transaction.h"

#include <vector>

namespace proto
{
class CryptoApproveAllowanceTransactionBody;
class TransactionBody;
}

namespace Hiero
{
class AccountId;
class Hbar;
class NftId;
class TokenId;
}

namespace Hiero
{
/**
 * A transaction that allows a token owner to delegate a token spender to spend the specified token amount on behalf of
 * the token owner. An owner can provide a token allowance for HBARs, non-fungible and fungible tokens. The owner is the
 * account that owns the tokens and grants the allowance to the spender. The spender is the account that spends tokens
 * authorized by the owner from the owners account. The spender pays for the transaction fees when transferring tokens
 * from the owners account to another recipient.
 *
 * The total number of approvals in this transaction cannot exceed 20. Note that each NFT serial number counts as a
 * single approval, hence a transaction granting 20 serial numbers to a spender will use all of the approvals permitted
 * for the transaction.
 *
 * A single NFT serial number can only be granted to one spender at a time. If an approval assigns a previously approved
 * NFT serial number to a new user, the old user will have their approval removed.
 *
 * Each account is limited to 100 allowances. This limit spans HBAR and fungible token allowances and non-fungible token
 * approved_for_all grants. There is no limit on the number of NFT serial number approvals an owner may grant.
 *
 * The number of allowances set on an account will increase the auto-renewal fee for the account. Conversely, removing
 * allowances will decrease the auto-renewal fee for the account.
 *
 * To decrease the allowance for a given spender, you will need to set the amount to the value you would like to
 * authorize the spender account for. If the spender account was authorized to spend 25 HBARs and the owner now wants to
 * modify their allowance to 5 HBARs, the owner would submit the AccountAllowanceApproveTransaction for 5 HBARs.
 *
 * Only when a spender is set on an explicit NFT ID of a token, do we return the spender ID in TokenNftInfoQuery for the
 * respective NFT. If approveTokenNftAllowanceAllSerials is used to approve all NFTs for a given token class and no NFT
 * ID is specified, we will not return a spender ID for all the serial numbers of that token.
 */
class AccountAllowanceApproveTransaction : public Transaction<AccountAllowanceApproveTransaction>
{
public:
  AccountAllowanceApproveTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a CryptoApproveAllowance transaction.
   */
  explicit AccountAllowanceApproveTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit AccountAllowanceApproveTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Add an Hbar allowance to this AccountAllowanceApproveTransaction.
   *
   * @param ownerAccountId   The ID of the account that is allowing the spending of its Hbar.
   * @param spenderAccountId The ID of the account that is being allowed to spend the owning account's Hbar.
   * @param amount           The amount of Hbar that is being approved to spend. This amount must be positive.
   * @return A reference to this AccountAllowanceApproveTransaction object with the added Hbar allowance.
   * @throws IllegalStateException If this AccountAllowanceApproveTransaction is frozen.
   */
  AccountAllowanceApproveTransaction& approveHbarAllowance(const AccountId& ownerAccountId,
                                                           const AccountId& spenderAccountId,
                                                           const Hbar& amount);

  /**
   * Add a token allowance to this AccountAllowanceApproveTransaction.
   *
   * @param tokenId          The ID of the token of which to approve the spending.
   * @param ownerAccountId   The ID of the account that is allowing the spending of its tokens.
   * @param spenderAccountId The ID of the account that is being allowed to spend the owning account's tokens.
   * @param amount           The amount of the token that is being approved to spend.
   * @return A reference to this AccountAllowanceApproveTransaction object with the added token allowance.
   * @throws IllegalStateException If this AccountAllowanceApproveTransaction is frozen.
   */
  AccountAllowanceApproveTransaction& approveTokenAllowance(const TokenId& tokenId,
                                                            const AccountId& ownerAccountId,
                                                            const AccountId& spenderAccountId,
                                                            const uint64_t& amount);

  /**
   * Add an NFT allowance to this AccountAllowanceApproveTransaction.
   *
   * @param nftId                      The ID of the NFT of which to approve the spending.
   * @param ownerAccountId             The ID of the account that is allowing the spending of the NFT.
   * @param spenderAccountId           The ID of the account that is being allowed to spend the owning account's NFT.
   * @param delegatingSpenderAccountId The ID of the account who has an 'approveForAll' allowance from the owner, and is
   *                                   granting approval to the spender to spend on their behalf.
   * @return A reference to this AccountAllowanceApproveTransaction object with the added NFT allowance.
   * @throws IllegalStateException If this AccountAllowanceApproveTransaction is frozen.
   */
  AccountAllowanceApproveTransaction& approveTokenNftAllowance(
    const NftId& nftId,
    const AccountId& ownerAccountId,
    const AccountId& spenderAccountId,
    const AccountId& delegatingSpenderAccountId = AccountId());

  /**
   * Add an allowance for all NFTs of a specific token ID to this AccountAllowanceApproveTransaction.
   *
   * @param tokenId          The ID of the NFT tokens of which to approve the spending.
   * @param ownerAccountId   The ID of the account that is allowing the spending of its NFTs.
   * @param spenderAccountId The ID of the account that is being allowed to spend the owning account's NFTs.
   * @return A reference to this AccountAllowanceApproveTransaction object with the added NFT allowances.
   * @throws IllegalStateException If this AccountAllowanceApproveTransaction is frozen.
   */
  AccountAllowanceApproveTransaction& approveNftAllowanceAllSerials(const TokenId& tokenId,
                                                                    const AccountId& ownerAccountId,
                                                                    const AccountId& spenderAccountId);

  /**
   * Remove all allowances for all NFTs of a specific token ID for a spender.
   *
   * @param tokenId          The ID of the NFT tokens of which to revoke the allowance.
   * @param ownerAccountId   The ID of the account that is revoking the spending of its NFTs.
   * @param spenderAccountId The ID of the account that is no longer allowed to spend the owning account's NFTs.
   * @return A reference to this AccountAllowanceApproveTransaction object with the deleted NFT allowances.
   * @throws IllegalStateException If this AccountAllowanceApproveTransaction is frozen.
   */
  AccountAllowanceApproveTransaction& deleteNftAllowanceAllSerials(const TokenId& tokenId,
                                                                   const AccountId& ownerAccountId,
                                                                   const AccountId& spenderAccountId);

  /**
   * Get the list of Hbar allowance approvals added to this AccountAllowanceApproveTransaction.
   *
   * @return The list of Hbar allowance approvals added to this AccountAllowanceApproveTransaction.
   */
  [[nodiscard]] inline std::vector<HbarAllowance> getHbarApprovals() const { return mHbarAllowances; }

  /**
   * Get the list of fungible token allowance approvals added to this AccountAllowanceApproveTransaction.
   *
   * @return The list of fungible token allowance approvals added to this AccountAllowanceApproveTransaction.
   */
  [[nodiscard]] inline std::vector<TokenAllowance> getTokenApprovals() const { return mTokenAllowances; }

  /**
   * Get the list of NFT allowance approvals added to this AccountAllowanceApproveTransaction.
   *
   * @return The list of NFT allowance approvals added to this AccountAllowanceApproveTransaction.
   */
  [[nodiscard]] inline std::vector<TokenNftAllowance> getNftApprovals() const { return mNftAllowances; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this
   * AccountAllowanceApproveTransaction's data to a Node.
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
   * Derived from Transaction. Verify that all the checksums in this AccountAllowanceApproveTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This AccountAllowanceApproveTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the AccountAllowanceApproveTransaction protobuf representation to the
   * Transaction protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this AccountAllowanceApproveTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a CryptoApproveAllowanceTransactionBody protobuf object from this AccountAllowanceApproveTransaction object.
   *
   * @return A pointer to a CryptoApproveAllowanceTransactionBody protobuf object filled with this
   *         AccountAllowanceApproveTransaction object's data.
   */
  [[nodiscard]] proto::CryptoApproveAllowanceTransactionBody* build() const;

  /**
   * The list of Hbar allowances to be approved.
   */
  std::vector<HbarAllowance> mHbarAllowances;

  /**
   * The list of fungible token allowances to be approved.
   */
  std::vector<TokenAllowance> mTokenAllowances;

  /**
   * The list of NFT allowances to be approved.
   */
  std::vector<TokenNftAllowance> mNftAllowances;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ACCOUNT_ALLOWANCE_APPROVE_TRANSACTION_H_
