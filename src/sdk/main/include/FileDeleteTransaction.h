// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FILE_DELETE_TRANSACTION_H_
#define HIERO_SDK_CPP_FILE_DELETE_TRANSACTION_H_

#include "FileId.h"
#include "Transaction.h"

namespace proto
{
class FileDeleteTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that deletes a file from a Hiero network. When deleted, a file's contents are truncated to zero length
 * and it can no longer be updated or appended to, or its expiration time extended. When you request the contents or
 * info of a deleted file, the network will return FILE_DELETED.
 *
 * Transaction Signing Requirements:
 *  - The key(s) on the file are required to sign the transaction.
 *  - If you do not sign with the key(s) on the file, you will receive an INVALID_SIGNATURE network error.
 */
class FileDeleteTransaction : public Transaction<FileDeleteTransaction>
{
public:
  FileDeleteTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a FileDelete transaction.
   */
  explicit FileDeleteTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit FileDeleteTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the file to delete.
   *
   * @param fileId The ID of the file to delete.
   * @return A reference to this FileDeleteTransaction object with the newly-set file ID.
   * @throws IllegalStateException If this FileDeleteTransaction is frozen.
   */
  FileDeleteTransaction& setFileId(const FileId& fileId);

  /**
   * Get the ID of the file to delete.
   *
   * @return The ID of the file to delete.
   */
  [[nodiscard]] inline FileId getFileId() const { return mFileId; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this FileDeleteTransaction's data to a
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
   * Derived from Transaction. Verify that all the checksums in this FileDeleteTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This FileDeleteTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the FileDeleteTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this FileDeleteTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a FileDeleteTransactionBody protobuf object from this FileDeleteTransaction object.
   *
   * @return A pointer to a FileDeleteTransactionBody protobuf object filled with this FileDeleteTransaction object's
   *         data.
   */
  [[nodiscard]] proto::FileDeleteTransactionBody* build() const;

  /**
   * The ID of the file to delete.
   */
  FileId mFileId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FILE_DELETE_TRANSACTION_H_
