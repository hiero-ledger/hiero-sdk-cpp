// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ETHEREUM_TRANSACTION_H_
#define HIERO_SDK_CPP_ETHEREUM_TRANSACTION_H_

#include "FileId.h"
#include "Hbar.h"
#include "Transaction.h"

#include <cstddef>
#include <optional>
#include <vector>

namespace proto
{
class EthereumTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * The raw Ethereum transaction (RLP encoded type 0, 1, and 2) will hold signed Ethereum transactions and execute them
 * as Hiero transactions in a prescribed manner.
 *
 * Transaction Signing Requirements:
 *  - The key of the transaction fee-paying account.
 */
class EthereumTransaction : public Transaction<EthereumTransaction>
{
public:
  EthereumTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent an EthereumTransaction transaction.
   */
  explicit EthereumTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit EthereumTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the raw Ethereum transaction (RLP encoded type 0, 1, and 2).
   *
   * @param ethereumData The raw Ethereum transaction.
   * @return A reference to this EthereumTransaction object with the newly-set ethereum data.
   * @throws IllegalStateException If this EthereumTransaction is frozen.
   */
  EthereumTransaction& setEthereumData(const std::vector<std::byte>& ethereumData);

  /**
   * Set the ID of the file that contains the call data.
   *
   * @param fileId The ID of the file that contains the call data.
   * @return A reference to this EthereumTransaction object with the newly-set call data file ID.
   * @throws IllegalStateException If this EthereumTransaction is frozen.
   */
  EthereumTransaction& setCallDataFileId(const FileId& fileId);

  /**
   * Set the maximum amount that the payer of the Hiero transaction is willing to pay to complete this
   * EthereumTransaction.
   *
   * @param maxGasAllowance The maximum amount that the payer of the Hiero transaction is willing to pay to complete
   *                        this EthereumTransaction.
   * @return A reference to this EthereumTransaction object with the newly-set transfer maximum gas allowance.
   * @throws IllegalStateException If this EthereumTransaction is frozen.
   */
  EthereumTransaction& setMaxGasAllowance(const Hbar& maxGasAllowance);

  /**
   * Get the raw Ethereum transaction.
   *
   * @return The raw Ethereum transaction.
   */
  [[nodiscard]] inline std::vector<std::byte> getEthereumData() const { return mEthereumData; }

  /**
   * Get the ID of the file that contains the call data.
   *
   * @return The ID of the file that contains the call data. Returns uninitialized if a value has not been set.
   */
  [[nodiscard]] inline std::optional<FileId> getCallDataFileId() const { return mCallDataFileId; }

  /**
   * Get the maximum amount that the payer of the Hiero transaction is willing to pay to complete this
   * EthereumTransaction.
   *
   * @return The maximum amount that the payer of the Hiero transaction is willing to pay to complete this
   *         EthereumTransaction.
   */
  [[nodiscard]] inline Hbar getMaxGasAllowance() const { return mMaxGasAllowance; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this EthereumTransaction's data to a
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
   * Derived from Transaction. Verify that all the checksums in this EthereumTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This EthereumTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the EthereumTransaction protobuf representation to the Transaction protobuf
   * object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this EthereumTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a EthereumTransactionBody protobuf object from this EthereumTransaction object.
   *
   * @return A pointer to a EthereumTransactionBody protobuf object filled with this EthereumTransaction object's data.
   */
  [[nodiscard]] proto::EthereumTransactionBody* build() const;

  /**
   * The raw Ethereum transaction (RLP encoded type 0, 1, and 2).
   */
  std::vector<std::byte> mEthereumData;

  /**
   * The ID of the file that contains the call data.
   *
   * For large transactions (for example contract creation) this should be used to set the FileId of an HFS file
   * containing the call data of the ethereum data. The data in the ethereum will be re-written with the call data
   * element as a zero-length string with the original contents in the referenced file at the time of execution. The
   * ethereum data will need to be "rehydrated" with the call data for signature validation to pass.
   */
  std::optional<FileId> mCallDataFileId;

  /**
   * The maximum amount that the payer of the Hiero transaction is willing to pay to complete the transaction.
   *
   * Ordinarily the account with the ECDSA alias corresponding to the public key that is extracted from the
   * ethereum data signature is responsible for fees that result from the execution of the transaction. If that amount
   * of authorized fees is not sufficient then the payer of the transaction can be charged, up to but not exceeding this
   * amount. If the ethereum data transaction authorized an amount that was insufficient then the payer will only be
   * charged the amount needed to make up the difference. If the gas price in the transaction was set to zero then the
   * payer will be assessed the entire fee.
   */
  Hbar mMaxGasAllowance = Hbar(0LL);
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ETHEREUM_TRANSACTION_H_
