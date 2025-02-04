// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_SYSTEM_DELETE_TRANSACTION_H_
#define HIERO_SDK_CPP_SYSTEM_DELETE_TRANSACTION_H_

#include "ContractId.h"
#include "FileId.h"
#include "Transaction.h"

#include <optional>

namespace proto
{
class SystemDeleteTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Delete a file or smart contract - can only be done with a Hiero administrative multisignature. When it is deleted,
 * it immediately disappears from the system as seen by the user, but is still stored internally until the expiration
 * time, at which time it is truly and permanently deleted. Until that time, it can be undeleted by the Hiero
 * administrative multisignature. When a smart contract is deleted, the cryptocurrency account within it continues to
 * exist, and is not affected by the expiration time here.
 *
 * Transaction Signing Requirements:
 *  - The Hiero administrative multisignature.
 */
class SystemDeleteTransaction : public Transaction<SystemDeleteTransaction>
{
public:
  SystemDeleteTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a SystemDelete.
   */
  explicit SystemDeleteTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit SystemDeleteTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the file to delete. This is mutually exclusive with mContractId, and will reset the value of
   * mContractId if it is set.
   *
   * @param fileId The ID of the file to delete.
   * @return A reference to this SystemDeleteTransaction object with the newly-set file ID.
   * @throws IllegalStateException If this SystemDeleteTransaction is frozen.
   */
  SystemDeleteTransaction& setFileId(const FileId& fileId);

  /**
   * Set the ID of the contract to delete. This is mutually exclusive with mFileId, and will reset the value of mFileId
   * if it is set.
   *
   * @param contractId The ID of the contract to delete.
   * @return A reference to this SystemDeleteTransaction object with the newly-set contract ID.
   * @throws IllegalStateException If this SystemDeleteTransaction is frozen.
   */
  SystemDeleteTransaction& setContractId(const ContractId& contractId);

  /**
   * Set the timestamp at which the deleted file will truly be permanently deleted.
   *
   * @param timestamp The timestamp at which the deleted file will truly be permanently deleted.
   * @return A reference to this SystemDeleteTransaction object with the newly-set expiration time.
   * @throws IllegalStateException If this SystemDeleteTransaction is frozen.
   */
  SystemDeleteTransaction& setExpirationTime(const std::chrono::system_clock::time_point& timestamp);

  /**
   * Get the ID of the file this SystemDeleteTransaction is currently configured to delete.
   *
   * @return The ID of the file this SystemDeleteTransaction is currently configured to delete. Returns uninitialized if
   *         a value has not yet been set, or if a contract ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<FileId> getFileId() const { return mFileId; }

  /**
   * Get the ID of the contract this SystemDeleteTransaction is currently configured to delete.
   *
   * @return The ID of the contract this SystemDeleteTransaction is currently configured to delete. Returns
   *         uninitialized if a value has not yet been set, or if a file ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<ContractId> getContractId() const { return mContractId; }

  /**
   * Get the timestamp at which the deleted file will truly be permanently deleted.
   *
   * @return The timestamp at which the deleted file will truly be permanently deleted.
   */
  [[nodiscard]] inline std::chrono::system_clock::time_point getExpirationTime() const { return mExpirationTime; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this SystemDeleteTransaction's data to
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
   * Derived from Transaction. Verify that all the checksums in this SystemDeleteTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This SystemDeleteTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the SystemDeleteTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this SystemDeleteTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a SystemDeleteTransactionBody protobuf object from this SystemDeleteTransaction object.
   *
   * @return A pointer to a SystemDeleteTransactionBody protobuf object filled with this SystemDeleteTransaction
   *         object's data.
   */
  [[nodiscard]] proto::SystemDeleteTransactionBody* build() const;

  /**
   * The ID of the file to delete. Mutually exclusive with mContractId.
   */
  std::optional<FileId> mFileId;

  /**
   * The ID of the contract to delete. Mutually exclusive with mFileId.
   */
  std::optional<ContractId> mContractId;

  /**
   * The timestamp at which the deleted file will truly be permanently deleted
   */
  std::chrono::system_clock::time_point mExpirationTime;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_SYSTEM_DELETE_TRANSACTION_H_
