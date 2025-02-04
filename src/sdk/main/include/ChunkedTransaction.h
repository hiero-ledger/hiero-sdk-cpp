// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CHUNKED_TRANSACTION_H_
#define HIERO_SDK_CPP_CHUNKED_TRANSACTION_H_

#include "Defaults.h"
#include "Transaction.h"
#include "TransactionId.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <future>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace Hiero
{
class Client;
class TransactionResponse;
}

namespace Hiero
{
/**
 * Base class for all transactions that require being divided into chunks.
 *
 * @tparam SdkRequestType The SDK request type.
 */
template<typename SdkRequestType>
class ChunkedTransaction : public Transaction<SdkRequestType>
{
public:
  /**
   * Derived from Executable. Execute this ChunkedTransaction. This should NOT be used if multiple chunks are being sent
   * (i.e. data.size() > mMaxChunkSize); opt to use `executeAll()` instead.
   *
   * @param client The Client to use to submit this ChunkedTransaction.
   * @return The TransactionResponse object sent from the Hiero network that contains the result of the first chunked
   *         request.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionResponse execute(const Client& client) override;

  /**
   * Derived from Executable. Execute this ChunkedTransaction with a specific timeout. This should NOT be used if
   * multiple chunks are being sent (i.e. data.size() > mMaxChunkSize); opt to use `executeAll()` instead.
   *
   * @param client The Client to use to submit this ChunkedTransaction.
   * @param timeout The desired timeout for the execution of this ChunkedTransaction.
   * @return The TransactionResponse object sent from the Hiero network that contains the result of the first chunked
   *         request.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionResponse execute(const Client& client, const std::chrono::system_clock::duration& timeout) override;

  /**
   * Derived from Executable. Execute this ChunkedTransaction asynchronously with a specific timeout.
   *
   * @param client  The Client to use to submit this ChunkedTransaction.
   * @param timeout The desired timeout for the execution of this ChunkedTransaction.
   * @return The future TransactionResponse object sent from the Hiero network that contains the result of the request.
   * @throws MaxAttemptsExceededException If this ChunkedTransaction attempts to execute past the number of allowable
   *                                      attempts.
   * @throws PrecheckStatusException      If this ChunkedTransaction fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<TransactionResponse> executeAsync(const Client& client,
                                                const std::chrono::system_clock::duration& timeout) override;

  /**
   * Execute all chunks of this ChunkedTransaction.
   *
   * @param client The Client to use to submit this ChunkedTransaction.
   * @return The list of TransactionResponse objects sent from the Hiero network that contains the result of the
   *         requests.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::vector<TransactionResponse> executeAll(const Client& client);

  /**
   * Execute all chunks of this ChunkedTransaction with a specific timeout.
   *
   * @param client The Client to use to submit this ChunkedTransaction.
   * @param timeout The desired timeout for the execution of this ChunkedTransaction.
   * @return The list of TransactionResponse objects sent from the Hiero network that contains the result of the
   *         requests.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::vector<TransactionResponse> executeAll(const Client& client, const std::chrono::system_clock::duration& timeout);

  /**
   * Execute all chunks of this ChunkedTransaction asynchronously.
   *
   * @param client The Client to use to submit this ChunkedTransaction.
   * @return The future list of TransactionResponse objects sent from the Hiero network that contains the result of the
   *         requests.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<std::vector<TransactionResponse>> executeAllAsync(const Client& client);

  /**
   * Execute all chunks of this ChunkedTransaction asynchronously with a specified timeout.
   *
   * @param client  The Client to use to submit this ChunkedTransaction.
   * @param timeout The desired timeout for the execution of this ChunkedTransaction.
   * @return The future list of TransactionResponse objects sent from the Hiero network that contains the result of the
   *         requests.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  std::future<std::vector<TransactionResponse>> executeAllAsync(const Client& client,
                                                                const std::chrono::system_clock::duration& timeout);

  /**
   * Execute all chunks of this ChunkedTransaction asynchronously and consume the response and/or exception with a
   * callback.
   *
   * @param client   The Client to use to submit this ChunkedTransaction.
   * @param callback The callback that should consume the response/exception.
   */
  void executeAllAsync(
    const Client& client,
    const std::function<void(const std::vector<TransactionResponse>&, const std::exception&)>& callback);

  /**
   * Execute all chunks of this ChunkedTransaction asynchronously with a specified timeout and consume the response
   * and/or exception with a callback.
   *
   * @param client   The Client to use to submit this ChunkedTransaction.
   * @param timeout  The desired timeout for the execution of this ChunkedTransaction.
   * @param callback The callback that should consume the response/exception.
   */
  void executeAllAsync(
    const Client& client,
    const std::chrono::system_clock::duration& timeout,
    const std::function<void(const std::vector<TransactionResponse>&, const std::exception&)>& callback);

  /**
   * Execute all chunks of this ChunkedTransaction asynchronously and consume the response and/or exception with
   * separate callbacks.
   *
   * @param client            The Client to use to submit this Executable.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   */
  void executeAllAsync(const Client& client,
                       const std::function<void(const std::vector<TransactionResponse>&)>& responseCallback,
                       const std::function<void(const std::exception&)>& exceptionCallback);

  /**
   * Execute all chunks of this ChunkedTransaction asynchronously with a specific timeout and consume the response
   * and/or exception with separate callbacks.
   *
   * @param client            The Client to use to submit this Executable.
   * @param timeout           The desired timeout for the execution of this Executable.
   * @param responseCallback  The callback that should consume the response.
   * @param exceptionCallback The callback that should consume the exception.
   */
  void executeAllAsync(const Client& client,
                       const std::chrono::system_clock::duration& timeout,
                       const std::function<void(const std::vector<TransactionResponse>&)>& responseCallback,
                       const std::function<void(const std::exception&)>& exceptionCallback);

  /**
   * Derived from Transaction. Add a signature to this ChunkedTransaction.
   *
   * @param publicKey The associated PublicKey of the PrivateKey that generated the signature.
   * @param signature The signature to add.
   * @return A reference to this derived ChunkedTransaction object with the newly-added signature.
   * @throws IllegalStateException If there are multiple chunks in this ChunkedTransaction,there is not exactly one node
   *                               account ID set, or if this ChunkedTransaction is not frozen.
   */
  SdkRequestType& addSignature(const std::shared_ptr<PublicKey>& publicKey,
                               const std::vector<std::byte>& signature) override;

  /**
   * Derived from Transaction. Get the signatures of each potential Transaction protobuf object this ChunkedTransaction
   * may send.
   *
   * @return The map of node account IDs to their PublicKeys and signatures.
   */
  [[nodiscard]] std::map<AccountId, std::map<std::shared_ptr<PublicKey>, std::vector<std::byte>>> getSignatures()
    const override;

  /**
   * Get the signatures of all chunks of this ChunkedTransaction, for each potential node to which it may be sent.
   *
   * @return The list of signatures for each node account ID for each chunk.
   */
  [[nodiscard]] std::vector<std::map<AccountId, std::map<std::shared_ptr<PublicKey>, std::vector<std::byte>>>>
  getAllSignatures() const;

  /**
   * Derived from Transaction. Get the SHA384 hash of this ChunkedTransaction.
   *
   * @return The SHA384 hash of this Transaction.
   * @throws IllegalStateException If this ChunkedTransaction contains multiple chunks or this ChunkedTransaction is
   * not frozen.
   */
  [[nodiscard]] std::vector<std::byte> getTransactionHash() const override;

  /**
   * Derived from Transaction. Get the SHA384 hash of each potential Transaction protobuf object this ChunkedTransaction
   * may send.
   *
   * @return The map of node account IDs to the SHA384 hash of their Transaction.
   * @throws IllegalStateException If this ChunkedTransaction contains multiple chunks or this ChunkedTransaction is not
   *                               frozen.
   */
  [[nodiscard]] std::map<AccountId, std::vector<std::byte>> getTransactionHashPerNode() const override;

  /**
   * Get the SHA384 hashes of all Transaction protobuf objects for each chunk of this ChunkedTransaction, for each node
   * account ID.
   *
   * @return The list of Transaction protobuf object hashes for each node account ID for each chunk.
   * @throws IllegalStateException If this ChunkedTransaction isn't frozen.
   */
  [[nodiscard]] std::vector<std::map<AccountId, std::vector<std::byte>>> getAllTransactionHashesPerNode() const;

  /**
   * Set the maximum number of chunks for this ChunkedTransaction.
   *
   * @param chunks The maximum number of chunks for this ChunkedTransaction.
   * @return A reference to this derived ChunkedTransaction object with the newly-set chunk maximum.
   */
  SdkRequestType& setMaxChunks(unsigned int chunks);

  /**
   * Set the size of each chunk, in bytes, for this ChunkedTransaction.
   *
   * @param size The size of each chunk, in bytes, for this ChunkedTransaction.
   * @return A reference to this derived ChunkedTransaction object with the newly-set chunk size.
   */
  SdkRequestType& setChunkSize(unsigned int size);

  /**
   * Get the maximum number of chunks for this ChunkedTransaction.
   *
   * @return The maximum number of chunks for this ChunkedTransaction.
   */
  [[nodiscard]] unsigned int getMaxChunks() const;

  /**
   * Get the size of each chunk, in bytes, for this ChunkedTransaction.
   *
   * @return The size of each chunk, in bytes, for this ChunkedTransaction.
   */
  [[nodiscard]] unsigned int getChunkSize() const;

protected:
  ChunkedTransaction();
  ~ChunkedTransaction();
  ChunkedTransaction(const ChunkedTransaction&);
  ChunkedTransaction& operator=(const ChunkedTransaction&);
  ChunkedTransaction(ChunkedTransaction&&) noexcept;
  ChunkedTransaction& operator=(ChunkedTransaction&&) noexcept;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param txBody The TransactionBody protobuf object from which to construct.
   */
  explicit ChunkedTransaction(const proto::TransactionBody& txBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects. This
   * will ignore the first TransactionId entry (as its information will be stored in the base Transaction
   * implementation) and construct additional Transaction protobuf objects for each additional chunk of the Transaction
   * that must be sent.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit ChunkedTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the data for this ChunkedTransaction.
   *
   * @param data The data for this ChunkedTransaction.
   * @return A reference to this derived ChunkedTransaction object with the newly-set data.
   */
  SdkRequestType& setData(const std::vector<std::byte>& data);
  SdkRequestType& setData(std::string_view data);

  /**
   * Get the data for this ChunkedTransaction.
   *
   * @return The data for this ChunkedTransaction.
   */
  [[nodiscard]] std::vector<std::byte> getData() const;

  /**
   * Get the data contained in the input chunk of this ChunkedTransaction.
   *
   * @param chunk The chunk number of which to get the data.
   * @return The data contained in the input chunk number.
   */
  [[nodiscard]] std::vector<std::byte> getDataForChunk(unsigned int chunk) const;

  /**
   * Set the receipt retrieval policy for this ChunkedTransaction.
   *
   * @param retrieveReceipt \c TRUE if this ChunkedTransaction should retrieve a receipt after each submitted chunk,
   *                        otherwise \c FALSE.
   */
  void setShouldGetReceipt(bool retrieveReceipt);

  /**
   * Get the receipt retrieval policy for this ChunkedTransaction.
   *
   * @retrun retrieveReceipt \c TRUE if this ChunkedTransaction should retrieve a receipt after each submitted chunk,
   *                         otherwise \c FALSE.
   */
  [[nodiscard]] bool getShouldGetReceipt() const;

private:
  /**
   * Build and add the derived ChunkedTransaction's chunked protobuf representation to the TransactionBody protobuf
   * object.
   *
   * @param chunk The chunk number.
   * @param total The total number of chunks being created.
   * @param body  The TransactionBody protobuf object to which to add the chunked data.
   */
  virtual void addToChunk(uint32_t chunk, [[maybe_unused]] uint32_t total, proto::TransactionBody& body) const = 0;

  /**
   * Derived from Executable. Construct a Transaction protobuf object from this ChunkedTransaction, based on the attempt
   * number. This will take into account the current chunk of this ChunkedTransaction trying to be sent.
   *
   * @param index The index of the node account ID that's associated with the Node being used to execute this
   *              ChunkedTransaction.
   * @return A Transaction protobuf object filled with this ChunkedTransaction's data, based on the node account ID at
   *         the given index.
   */
  [[nodiscard]] proto::Transaction makeRequest(unsigned int index) const override;

  /**
   * Derived from Transaction. Regenerate the SignedTransaction protobuf objects for this ChunkedTransaction.
   *
   * @param client A pointer to the Client to use to regenerate the SignedTransaction protobuf objects.
   */
  void regenerateSignedTransactions(const Client* client) const override;

  /**
   * Derived from Transaction. Clear the SignedTransaction and Transaction protobuf objects held by this
   * ChunkedTransaction.
   */
  void clearTransactions() const override;

  /**
   * Derived from Transaction. Get the ID of the previously-executed ChunkedTransaction.
   *
   * @return The ID of the previously-executed ChunkedTransaction.
   */
  [[nodiscard]] TransactionId getCurrentTransactionId() const override;

  /**
   * Get the number of chunks that will be required to send this full ChunkedTransaction.
   *
   * @return The number of chunks that will be required to send this full ChunkedTransaction.
   */
  [[nodiscard]] unsigned int getNumberOfChunksRequired() const;

  /**
   * Implementation object used to hide implementation details and internal headers.
   */
  struct ChunkedTransactionImpl;
  std::unique_ptr<ChunkedTransactionImpl> mImpl;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CHUNKED_TRANSACTION_H_