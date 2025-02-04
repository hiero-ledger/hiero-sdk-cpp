// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FILE_CREATE_TRANSACTION_H_
#define HIERO_SDK_CPP_FILE_CREATE_TRANSACTION_H_

#include "Defaults.h"
#include "Key.h"
#include "KeyList.h"
#include "Transaction.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class FileCreateTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that creates a new file on a Hiero network. The file is referenced by its file ID which can be
 * obtained from the receipt or record once the transaction reaches consensus on a Hiero network. The file does not
 * have a file name. If the file is too big to create with a single FileCreateTransaction(), the file can be appended
 * with the remaining content multiple times using the FileAppendTransaction().
 *
 * Transaction Signing Requirements:
 *  - The key on the file is required to sign the transaction if different than the client operator account key.
 */
class FileCreateTransaction : public Transaction<FileCreateTransaction>
{
public:
  /**
   * Default constructor. Sets the maximum transaction fee to 5 Hbars.
   */
  FileCreateTransaction();

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a FileCreate transaction.
   */
  explicit FileCreateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit FileCreateTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the time at which the new file will expire. When the file expires, it will be deleted. To prevent the file from
   * being deleted, use a FileUpdateTransaction to update with the new expiration time.
   *
   * @param expirationTime The time at which the new file will expire.
   * @return A reference to this FileCreateTransaction object with the newly-set expiration time.
   * @throws IllegalStateException If this FileCreateTransaction is frozen.
   */
  FileCreateTransaction& setExpirationTime(const std::chrono::system_clock::time_point& expirationTime);

  /**
   * Set the Keys that must sign when mutating the new file via FileAppendTransactions or FileUpdateTransactions. If no
   * keys are provided, then the file is immutable and any of the aforementioned transactions will fail.
   *
   * @param keys The keys that must sign any Transaction that edits the created file.
   * @return A reference to this FileCreateTransaction object with the newly-set keys.
   * @throws IllegalStateException If this FileCreateTransaction is frozen.
   */
  FileCreateTransaction& setKeys(const std::vector<std::shared_ptr<Key>>& keys);
  FileCreateTransaction& setKeys(const KeyList& keys);

  /**
   * Set the contents of the new file. The contents cannot exceed 4096 bytes. A FileAppendTransaction must be used to
   * set larger contents.
   *
   * @param contents The contents of the new file.
   * @return A reference to this FileCreateTransaction object with the newly-set contents.
   * @throws std::invalid_argument If the number of bytes exceeds 4096.
   * @throws IllegalStateException If this FileCreateTransaction is frozen.
   */
  FileCreateTransaction& setContents(const std::vector<std::byte>& contents);
  FileCreateTransaction& setContents(std::string_view contents);

  /**
   * Set the memo for the new file. The memo cannot exceed 100 bytes.
   *
   * @param contents The contents of the new file.
   * @return A reference to this FileCreateTransaction object with the newly-set contents.
   * @throws std::invalid_argument If the number of bytes exceeds 4096.
   * @throws IllegalStateException If this FileCreateTransaction is frozen.
   */
  FileCreateTransaction& setFileMemo(std::string_view memo);

  /**
   * Get the time at which the new file will expire.
   *
   * @return The time at which the new file will expire
   */
  [[nodiscard]] inline std::chrono::system_clock::time_point getExpirationTime() const { return mExpirationTime; }

  /**
   * Get the Keys that must sign Transactions to mutate the new file.
   *
   * @return The Keys that must sign Transactions to mutate the new file.
   */
  [[nodiscard]] inline KeyList getKeys() const { return mKeys; }

  /**
   * Get the contents of the new file.
   *
   * @return The contents of the new file
   */
  [[nodiscard]] inline std::vector<std::byte> getContents() const { return mContents; }

  /**
   * Get the memo for the new file.
   *
   * @return The memo for the new file.
   */
  [[nodiscard]] inline std::string getFileMemo() const { return mFileMemo; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this FileCreateTransaction's data to a
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
   * Derived from Transaction. Verify that all the checksums in this FileCreateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This FileCreateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the FileCreateTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this FileCreateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a FileCreateTransactionBody protobuf object from this FileCreateTransaction object.
   *
   * @return A pointer to a FileCreateTransactionBody protobuf object filled with this FileCreateTransaction object's
   *         data.
   */
  [[nodiscard]] proto::FileCreateTransactionBody* build() const;

  /**
   * The time at which the new file will expire.
   */
  std::chrono::system_clock::time_point mExpirationTime = std::chrono::system_clock::now() + DEFAULT_AUTO_RENEW_PERIOD;

  /**
   * The keys that must sign Transactions to mutate the new file.
   */
  KeyList mKeys;

  /**
   * The contents of the new file.
   */
  std::vector<std::byte> mContents;

  /**
   * The memo for the new file.
   */
  std::string mFileMemo;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FILE_CREATE_TRANSACTION_H_
