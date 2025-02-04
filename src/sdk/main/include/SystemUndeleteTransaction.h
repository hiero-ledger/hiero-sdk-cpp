// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_SYSTEM_UNDELETE_TRANSACTION_H_
#define HIERO_SDK_CPP_SYSTEM_UNDELETE_TRANSACTION_H_

#include "ContractId.h"
#include "FileId.h"
#include "Transaction.h"

#include <optional>

namespace proto
{
class SystemUndeleteTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Undelete a file or smart contract that was deleted by SystemDelete; requires a Hiero administrative multisignature.
 *
 * Transaction Signing Requirements:
 *  - The Hiero administrative multisignature.
 */
class SystemUndeleteTransaction : public Transaction<SystemUndeleteTransaction>
{
public:
  SystemUndeleteTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a SystemUndelete.
   */
  explicit SystemUndeleteTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit SystemUndeleteTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the file to undelete. This is mutually exclusive with mContractId, and will reset the value of
   * mContractId if it is set.
   *
   * @param fileId The ID of the file to undelete.
   * @return A reference to this SystemUndeleteTransaction object with the newly-set file ID.
   * @throws IllegalStateException If this SystemUndeleteTransaction is frozen.
   */
  SystemUndeleteTransaction& setFileId(const FileId& fileId);

  /**
   * Set the ID of the contract to undelete. This is mutually exclusive with mFileId, and will reset the value of
   * mFileId if it is set.
   *
   * @param contractId The ID of the contract to undelete.
   * @return A reference to this SystemUndeleteTransaction object with the newly-set contract ID.
   * @throws IllegalStateException If this SystemUndeleteTransaction is frozen.
   */
  SystemUndeleteTransaction& setContractId(const ContractId& contractId);

  /**
   * Get the ID of the file this SystemUndeleteTransaction is currently configured to undelete.
   *
   * @return The ID of the file this SystemUndeleteTransaction is currently configured to undelete. Returns
   * uninitialized if a value has not yet been set, or if a contract ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<FileId> getFileId() const { return mFileId; }

  /**
   * Get the ID of the contract this SystemUndeleteTransaction is currently configured to undelete.
   *
   * @return The ID of the contract this SystemUndeleteTransaction is currently configured to undelete. Returns
   *         uninitialized if a value has not yet been set, or if a file ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<ContractId> getContractId() const { return mContractId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this SystemUndeleteTransaction's data
   * to a Node.
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
   * Derived from Transaction. Verify that all the checksums in this SystemUndeleteTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This SystemUndeleteTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the SystemUndeleteTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this SystemUndeleteTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a SystemUndeleteTransactionBody protobuf object from this SystemUndeleteTransaction object.
   *
   * @return A pointer to a SystemUndeleteTransactionBody protobuf object filled with this SystemUndeleteTransaction
   *         object's data.
   */
  [[nodiscard]] proto::SystemUndeleteTransactionBody* build() const;

  /**
   * The ID of the file to undelete. Mutually exclusive with mContractId.
   */
  std::optional<FileId> mFileId;

  /**
   * The ID of the contract to undelete. Mutually exclusive with mFileId.
   */
  std::optional<ContractId> mContractId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_SYSTEM_UNDELETE_TRANSACTION_H_