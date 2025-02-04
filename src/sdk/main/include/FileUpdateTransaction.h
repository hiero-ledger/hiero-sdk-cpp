// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FILE_UPDATE_TRANSACTION_H_
#define HIERO_SDK_CPP_FILE_UPDATE_TRANSACTION_H_

#include "FileId.h"
#include "Key.h"
#include "KeyList.h"
#include "Transaction.h"

#include <chrono>
#include <cstddef>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class FileUpdateTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that updates the state of an existing file on a Hiero network. Once the transaction has been
 * processed, the network will be updated with the new field values of the file. If you need to access a previous state
 * of the file, you can query a mirror node.
 *
 * Transaction Signing Requirements:
 *  - The key or keys on the file are required to sign this transaction to modify the file properties.
 *  - If you are updating the keys on the file, you must sign with the old key and the new key.
 *  - If you do not sign with the key(s) on the file, you will receive an INVALID_SIGNATURE network error.
 */
class FileUpdateTransaction : public Transaction<FileUpdateTransaction>
{
public:
  FileUpdateTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a FileUpdate transaction.
   */
  explicit FileUpdateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit FileUpdateTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the file to update.
   *
   * @param fileId The ID of the file to update.
   * @return A reference to this FileUpdateTransaction object with the newly-set file ID.
   * @throws IllegalStateException If this FileUpdateTransaction is frozen.
   */
  FileUpdateTransaction& setFileId(const FileId& fileId);

  /**
   * Set the new time at which the file will expire.
   *
   * @param expirationTime The new time at which the file will expire.
   * @return A reference to this FileUpdateTransaction object with the newly-set expiration time.
   * @throws IllegalStateException If this FileUpdateTransaction is frozen.
   */
  FileUpdateTransaction& setExpirationTime(const std::chrono::system_clock::time_point& expirationTime);

  /**
   * Set the new key(s) that must sign when mutating the file.
   *
   * @param keys The keys that must sign any Transaction that edits the created file.
   * @return A reference to this FileUpdateTransaction object with the newly-set keys.
   * @throws IllegalStateException If this FileUpdateTransaction is frozen.
   */
  FileUpdateTransaction& setKeys(const std::vector<std::shared_ptr<Key>>& keys);
  FileUpdateTransaction& setKeys(const KeyList& keys);

  /**
   * Set the new contents of the file. The contents cannot exceed 4096 bytes. A FileAppendTransaction must be used to
   * set larger contents.
   *
   * @param contents The new contents of the file.
   * @return A reference to this FileUpdateTransaction object with the newly-set contents.
   * @throws std::invalid_argument If the number of bytes exceeds 4096.
   * @throws IllegalStateException If this FileUpdateTransaction is frozen.
   */
  FileUpdateTransaction& setContents(const std::vector<std::byte>& contents);
  FileUpdateTransaction& setContents(std::string_view contents);

  /**
   * Set the new memo for the file.
   *
   * @param memo The new memo of the file.
   * @return A reference to this FileUpdateTransaction object with the newly-set memo.
   * @throws IllegalStateException If this FileUpdateTransaction is frozen.
   */
  FileUpdateTransaction& setFileMemo(std::string_view memo);

  /**
   * Get the ID of the file to update.
   *
   * @return The ID of the file to update.
   */
  [[nodiscard]] inline FileId getFileId() const { return mFileId; }

  /**
   * Get the new time at which the file will expire.
   *
   * @return The new time at which the file will expire.
   */
  [[nodiscard]] inline std::optional<std::chrono::system_clock::time_point> getExpirationTime() const
  {
    return mExpirationTime;
  }

  /**
   * Get the new keys to be associated with the file.
   *
   * @return The new keys to be associated with the file.
   */
  [[nodiscard]] inline std::optional<KeyList> getKeys() const { return mKeys; }

  /**
   * Get the new contents of the file.
   *
   * @return The new contents of the file
   */
  [[nodiscard]] inline std::optional<std::vector<std::byte>> getContents() const { return mContents; }

  /**
   * Get the new memo for the file.
   *
   * @return The new memo for the file.
   */
  [[nodiscard]] inline std::optional<std::string> getFileMemo() const { return mFileMemo; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this FileUpdateTransaction's data to a
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
   * Derived from Transaction. Verify that all the checksums in this FileUpdateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This FileUpdateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the FileUpdateTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this FileUpdateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a FileUpdateTransactionBody protobuf object from this FileUpdateTransaction object.
   *
   * @return A pointer to a FileUpdateTransactionBody protobuf object filled with this FileUpdateTransaction object's
   *         data.
   */
  [[nodiscard]] proto::FileUpdateTransactionBody* build() const;

  /**
   * The ID of the file to update.
   */
  FileId mFileId;

  /**
   * The new time at which the file will expire.
   */
  std::optional<std::chrono::system_clock::time_point> mExpirationTime;

  /**
   * The new keys that must sign Transactions to mutate the file.
   */
  std::optional<KeyList> mKeys;

  /**
   * The new contents of the file.
   */
  std::optional<std::vector<std::byte>> mContents;

  /**
   * The new memo for the file.
   */
  std::optional<std::string> mFileMemo;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FILE_UPDATE_TRANSACTION_H_
