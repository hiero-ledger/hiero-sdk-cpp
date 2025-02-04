// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FILE_APPEND_TRANSACTION_H_
#define HIERO_SDK_CPP_FILE_APPEND_TRANSACTION_H_

#include "ChunkedTransaction.h"
#include "FileId.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace proto
{
class FileAppendTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that appends new file content to the end of an existing file. The contents of the file can be viewed by
 * submitting a FileContentsQuery request.
 *
 * Transaction Signing Requirements:
 *  - The key on the file is required to sign the transaction if different than the client operator account key.
 */
class FileAppendTransaction : public ChunkedTransaction<FileAppendTransaction>
{
public:
  /**
   * The default chunk size for a FileAppendTransaction.
   */
  static constexpr unsigned int DEFAULT_CHUNK_SIZE = 4096U;

  /**
   * Default constructor. Sets the maximum transaction fee to 5 Hbars, chunk size to 2048 bytes, and sets the receipt
   * retrieval policy to always retrieve receipts between chunk submissions.
   */
  FileAppendTransaction();

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a FileAppend transaction.
   */
  explicit FileAppendTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit FileAppendTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the file to which to append.
   *
   * @param fileId The ID of the file to which to append.
   * @return A reference to this FileAppendTransaction object with the newly-set file ID.
   * @throws IllegalStateException If this FileAppendTransaction is frozen.
   */
  FileAppendTransaction& setFileId(const FileId& fileId);

  /**
   * Set the contents to append.
   *
   * @param contents The contents of the new file.
   * @return A reference to this FileAppendTransaction object with the newly-set contents.
   * @throws IllegalStateException If this FileAppendTransaction is frozen.
   */
  FileAppendTransaction& setContents(const std::vector<std::byte>& contents);
  FileAppendTransaction& setContents(std::string_view contents);

  /**
   * Get the ID of the file to which to append.
   *
   * @return The ID of the file to which to append.
   */
  [[nodiscard]] inline FileId getFileId() const { return mFileId; }

  /**
   * Get the contents to append.
   *
   * @return The contents to append.
   */
  [[nodiscard]] inline std::vector<std::byte> getContents() const { return getData(); }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this FileAppendTransaction's data to a
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
   * Derived from Transaction. Verify that all the checksums in this FileAppendTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This FileAppendTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add this FileAppendTransaction's protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Derived from ChunkedTransaction. Build and add this FileAppendTransaction's chunked protobuf representation to the
   * TransactionBody protobuf object.
   *
   * @param chunk The chunk number.
   * @param total The total number of chunks being created.
   * @param body  The TransactionBody protobuf object to which to add the chunked data.
   */
  void addToChunk(uint32_t chunk, uint32_t /*total*/, proto::TransactionBody& body) const override;

  /**
   * Initialize this FileAppendTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a FileAppendTransactionBody protobuf object from this FileAppendTransaction object. Optionally, build this
   * FileAppendTransaction for a specific chunk.
   *
   * @param chunk The chunk number for which to build this FileAppendTransaction. The default value (-1) indicates to
   *              build for all chunks (i.e. build with all data).
   * @return A pointer to a FileAppendTransactionBody protobuf object filled with this FileAppendTransaction object's
   *         data.
   */
  [[nodiscard]] proto::FileAppendTransactionBody* build(int chunk = -1) const;

  /**
   * The ID of the file to which to append.
   */
  FileId mFileId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FILE_APPEND_TRANSACTION_H_
