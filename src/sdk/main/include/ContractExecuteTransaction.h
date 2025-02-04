// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CONTRACT_EXECUTE_TRANSACTION_H_
#define HIERO_SDK_CPP_CONTRACT_EXECUTE_TRANSACTION_H_

#include "ContractFunctionParameters.h"
#include "ContractId.h"
#include "Hbar.h"
#include "Transaction.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace proto
{
class ContractCallTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * The transaction calls a function of the given smart contract instance, giving it functionParameters as its input. The
 * call can use at maximum the given amount of gas – the paying account will not be charged for any unspent gas. If this
 * function results in data being stored, an amount of gas is calculated that reflects this storage burden. The amount
 * of gas used, as well as other attributes of the transaction, e.g. size, and number of signatures to be verified,
 * determine the fee for the transaction – which is charged to the paying account.
 *
 * Transaction Signing Requirements:
 *  - The key of the transaction fee-paying account.
 */
class ContractExecuteTransaction : public Transaction<ContractExecuteTransaction>
{
public:
  ContractExecuteTransaction() = default;

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a ContractCall transaction.
   */
  explicit ContractExecuteTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit ContractExecuteTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the contract to call.
   *
   * @param fileId The ID of the contract to call.
   * @return A reference to this ContractExecuteTransaction object with the newly-set contract ID.
   * @throws IllegalStateException If this ContractExecuteTransaction is frozen.
   */
  ContractExecuteTransaction& setContractId(const ContractId& contractId);

  /**
   * Set the maximum amount of gas to use for the function call.
   *
   * @param gas The maximum amount of gas to use for the function call.
   * @return A reference to this ContractExecuteTransaction object with the newly-set amount of gas.
   * @throws IllegalStateException If this ContractExecuteTransaction is frozen.
   */
  ContractExecuteTransaction& setGas(const uint64_t& gas);

  /**
   * Set the amount to pay for the function call.
   *
   * @param amount The amount to pay for the function call.
   * @return A reference to this ContractExecuteTransaction object with the newly-set payable amount.
   * @throws IllegalStateException If this ContractExecuteTransaction is frozen.
   */
  ContractExecuteTransaction& setPayableAmount(const Hbar& amount);

  /**
   * Set the function parameters for the function call.
   *
   * @param parameters The parameters to pass to function call.
   * @return A reference to this ContractExecuteTransaction object with the newly-set function parameters.
   * @throws IllegalStateException If this ContractExecuteTransaction is frozen.
   */
  ContractExecuteTransaction& setFunctionParameters(const std::vector<std::byte>& parameters);

  /**
   * Set the function to call, optionally with its parameters.
   *
   * @param name       The name of the function to call.
   * @param parameters The function parameters to pass to the function call.
   * @return A reference to this ContractExecuteTransaction object with the newly-set function name and parameters.
   * @throws IllegalStateException If this ContractExecuteTransaction is frozen.
   */
  ContractExecuteTransaction& setFunction(std::string_view name,
                                          const ContractFunctionParameters& parameters = ContractFunctionParameters());

  /**
   * Get the ID of the contract to call.
   *
   * @return The ID of the contract to call
   */
  [[nodiscard]] inline ContractId getContractId() const { return mContractId; }

  /**
   * Get the maximum amount of gas to use for the function call.
   *
   * @return The maximum amount of gas to use for the function call.
   */
  [[nodiscard]] inline uint64_t getGas() const { return mGas; }

  /**
   * Get the amount to pay for the function call.
   *
   * @return The amount to pay for the function call.
   */
  [[nodiscard]] inline Hbar getPayableAmount() const { return mPayableAmount; }

  /**
   * Get the function parameters for the function call.
   *
   * @return The function parameters for the function call.
   */
  [[nodiscard]] inline std::vector<std::byte> getFunctionParameters() const { return mFunctionParameters; }

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this ContractExecuteTransaction's data
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
   * Derived from Transaction. Verify that all the checksums in this ContractExecuteTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This ContractExecuteTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the ContractExecuteTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this ContractExecuteTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a ContractCallTransactionBody protobuf object from this ContractExecuteTransaction object.
   *
   * @return A pointer to a ContractCallTransactionBody protobuf object filled with this ContractExecuteTransaction
   *         object's data.
   */
  [[nodiscard]] proto::ContractCallTransactionBody* build() const;

  /**
   * The ID of the contract to call.
   */
  ContractId mContractId;

  /**
   * The maximum amount of gas to use for the function call.
   */
  uint64_t mGas = 0ULL;

  /**
   * The amount to pay for the function call
   */
  Hbar mPayableAmount = Hbar(0LL);

  /**
   * The function parameters for the function call.
   */
  std::vector<std::byte> mFunctionParameters;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CONTRACT_EXECUTE_TRANSACTION_H_
