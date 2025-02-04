// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_BURN_TRANSACTION_H_
#define HIERO_SDK_CPP_TOKEN_BURN_TRANSACTION_H_

#include "TokenId.h"
#include "Transaction.h"

#include <cstdint>
#include <vector>

namespace proto
{
class TokenBurnTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Burns fungible and non-fungible tokens owned by the treasury account. If no supply key is defined, this transaction
 * will resolve to TOKEN_HAS_NO_SUPPLY_KEY. The operation decreases the total supply of the token. Total supply cannot
 * go below zero. The amount provided must be in the lowest denomination possible. Example: Token A has 2 decimals. In
 * order to burn 100 tokens, one must provide an amount of 10000. In order to burn 100.55 tokens, one must provide an
 * amount of 10055. This transaction accepts zero unit token burn operations for fungible tokens (HIP-564).
 *
 * Transaction Signing Requirements:
 *  - Supply key.
 *  - Transaction fee payer account key.
 */
class TokenBurnTransaction : public Transaction<TokenBurnTransaction>
{
public:
  TokenBurnTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a TokenBurn transaction.
   */
  explicit TokenBurnTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit TokenBurnTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the token to burn.
   *
   * @param tokenId The ID of the token to burn.
   * @return A reference to this TokenBurnTransaction object with the newly-set token ID.
   * @throws IllegalStateException If this TokenBurnTransaction is frozen.
   */
  TokenBurnTransaction& setTokenId(const TokenId& tokenId);

  /**
   * Set the amount of FUNGIBLE_COMMON tokens to burn. This should be in the lowest denomination possible.
   *
   * @param amount The amount of FUNGIBLE_COMMON tokens to burn.
   * @return A reference to this TokenBurnTransaction object with the newly-set amount.
   * @throws IllegalStateException If this TokenBurnTransaction is frozen.
   */
  TokenBurnTransaction& setAmount(uint64_t amount);

  /**
   * Set the serial numbers of NON_FUNGIBLE_UNIQUE tokens to burn.
   *
   * @param serialNumbers The serial numbers of NON_FUNGIBLE_UNIQUE tokens to burn.
   * @return A reference to this TokenBurnTransaction object with the newly-set serial numbers.
   * @throws IllegalStateException If this TokenBurnTransaction is frozen.
   */
  TokenBurnTransaction& setSerialNumbers(const std::vector<uint64_t>& serialNumbers);

  /**
   * Get the ID of the token to burn.
   *
   * @return The ID of the token to burn.
   */
  [[nodiscard]] inline TokenId getTokenId() const { return mTokenId; }

  /**
   * Get the amount of FUNGIBLE_COMMON tokens to burn.
   *
   * @return The amount of FUNGIBLE_COMMON tokens to burn.
   */
  [[nodiscard]] inline uint64_t getAmount() const { return mAmount; }

  /**
   * Get the serial numbers of the NON_FUNGIBLE_UNIQUE tokens to burn.
   *
   * @return The serial numbers of the NON_FUNGIBLE_UNIQUE tokens to burn.
   */
  [[nodiscard]] inline std::vector<uint64_t> getSerialNumbers() const { return mSerialNumbers; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this TokenBurnTransaction's data to a
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
   * Derived from Transaction. Verify that all the checksums in this TokenBurnTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenBurnTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the TokenBurnTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this TokenBurnTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a TokenBurnTransactionBody protobuf object from this TokenBurnTransaction object.
   *
   * @return A pointer to a TokenBurnTransactionBody protobuf object filled with this TokenBurnTransaction object's
   *         data.
   */
  [[nodiscard]] proto::TokenBurnTransactionBody* build() const;

  /**
   * The ID of the token to burn.
   */
  TokenId mTokenId;

  /**
   * Applicable to tokens of type FUNGIBLE_COMMON. The amount of tokens to burn from the treasury account. Amount must
   * be a positive non-zero number in the lowest denomination possible and not bigger than the token balance of the
   * treasury account.
   */
  uint64_t mAmount = 0ULL;

  /**
   * Applicable to tokens of type NON_FUNGIBLE_UNIQUE. The list of serial numbers to be burned.
   */
  std::vector<uint64_t> mSerialNumbers;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_BURN_TRANSACTION_H_
