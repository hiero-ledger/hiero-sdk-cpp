// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_PRNG_TRANSACTION_H_
#define HIERO_SDK_CPP_PRNG_TRANSACTION_H_

#include "Transaction.h"

namespace proto
{
class UtilPrngTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * A transaction that generates a pseudorandom number. When the pseudorandom number generate transaction executes, its
 * transaction record will contain the 384-bit array of pseudorandom bytes. The transaction has an optional range
 * parameter. If the parameter is given and is positive, then the record will contain a 32-bit pseudorandom integer r,
 * where 0 <= r < range instead of containing the 384 pseudorandom bits.
 *
 * When the nth transaction needs a pseudorandom number, it is given the running hash of all records up to and including
 * the record for transaction n-3. If it needs 384 bits, then it uses the entire hash. If it needs 256 bits, it uses the
 * first 256 bits of the hash. If it needs a random number r that is in the range 0 <= r < range, then it lets x be the
 * first 32 bits of the hash (interpreted as a signed integer).
 *
 * The choice of using the hash up to transaction n-3 rather than n-1 is to ensure the transactions can be processed
 * quickly. Because the thread calculating the hash will have more time to complete it before it is needed. The use of
 * n-3 rather than n-1000000 is to make it hard to predict the pseudorandom number in advance.
 */
class PrngTransaction : public Transaction<PrngTransaction>
{
public:
  PrngTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a UtilPrng.
   */
  explicit PrngTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit PrngTransaction(const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the range of the pseudorandom number to generate.
   *
   * @param range The range of the pseudorandom number to generate.
   * @return A reference to this PrngTransaction object with the newly-set range.
   * @throws IllegalStateException If this PrngTransaction is frozen.
   */
  PrngTransaction& setRange(int range);

  /**
   * Get the range of the pseudorandom number to generate.
   *
   * @return The the range of the pseudorandom number to generate.
   */
  [[nodiscard]] inline int getRange() const { return mRange; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this PrngTransaction's data to a Node.
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
   * Derived from Transaction. Verify that all the checksums in this PrngTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This PrngTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the PrngTransaction protobuf representation to the Transaction protobuf
   * object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this PrngTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a UtilPrngTransactionBody protobuf object from this PrngTransaction object.
   *
   * @return A pointer to a UtilPrngTransactionBody protobuf object filled with this PrngTransaction object's data.
   */
  [[nodiscard]] proto::UtilPrngTransactionBody* build() const;

  /**
   * The range from which to return the pseudorandom number. If this is , a 384-bit pseudorandom number will be returned
   * in the TransactionRecord. If this is set, a 32-bit pseudorandom number will be returned between 0 and the specified
   * range.
   */
  int mRange = 0;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_PRNG_TRANSACTION_H_
