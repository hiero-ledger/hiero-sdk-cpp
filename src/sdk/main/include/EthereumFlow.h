// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_ETHEREUM_FLOW_H_
#define HIERO_SDK_CPP_ETHEREUM_FLOW_H_

#include "EthereumTransactionData.h"
#include "Hbar.h"

#include <chrono>
#include <memory>
#include <optional>

namespace Hiero
{
class Client;
class TransactionResponse;
}

namespace Hiero
{
/**
 * A helper class to execute an EthereumTransaction. This will use FileCreateTransaction and FileAppendTransaction as
 * necessary to create a file with the call data followed by an EthereumTransaction to execute the EthereumData.
 */
class EthereumFlow
{
public:
  /**
   * Execute the Transactions in this flow (FileCreateTransaction and FileAppendTransaction (if needed), and an
   * EthereumTransaction).
   *
   * @param client The Client to use to submit these Transactions.
   * @return The TransactionResponse object of the EthereumTransaction.
   * @throws MaxAttemptsExceededException If any Transaction attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If any Transaction fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionResponse execute(const Client& client);

  /**
   * Execute the Transactions in this flow (FileCreateTransaction and FileAppendTransaction (if needed), and an
   * EthereumTransaction) with a specified timeout.
   *
   * @param client  The Client to use to submit these Transactions.
   * @param timeout The desired timeout for the execution of these Transactions.
   * @return The TransactionResponse object of the EthereumTransaction.
   * @throws IllegalStateException        If the EthereumData is not set.
   * @throws MaxAttemptsExceededException If any Transaction attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If any Transaction fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionResponse execute(const Client& client, const std::chrono::system_clock::duration& timeout);

  /**
   * Set the bytes of the raw EthereumTransaction (RLP encoded type 0, 1, or 2).
   *
   * @param data The ethereum data to set.
   * @return A reference to this EthereumFlow object with the newly-set ethereum data.
   * @throws std::invalid_argument If the input data is not RLP-encoded Ethereum data.
   */
  EthereumFlow& setEthereumData(const std::vector<std::byte>& data);

  /**
   * Set the maximum amount that the payer of the Hiero transaction is willing to pay to complete the
   * EthereumTransaction.
   *
   * @param maxGasAllowance The maximum amount that the payer of the Hiero transaction is willing to pay to complete
   *                        the EthereumTransaction.
   * @return A reference to this EthereumFlow object with the newly-set maximum gas allowance.
   */
  EthereumFlow& setMaxGasAllowance(const Hbar& maxGasAllowance);

  /**
   * Get the raw EthereumTransaction data.
   *
   * @return The raw EthereumTransaction data.
   */
  [[nodiscard]] inline std::shared_ptr<EthereumTransactionData> getEthereumData() const { return mEthereumData; }

  /**
   * Get the maximum amount that the payer of the Hiero transaction is willing to pay to complete the
   * EthereumTransaction.
   *
   * @return The maximum amount that the payer of the Hiero transaction is willing to pay to complete the
   *         EthereumTransaction. Returns uninitialized in no max gas allowance has been set.
   */
  [[nodiscard]] inline std::optional<Hbar> getMaxGasAllowance() const { return mMaxGasAllowance; }

private:
  /**
   * The maximum size for a EthereumTransaction call data.
   */
  static constexpr unsigned int MAX_ETHEREUM_DATA_SIZE = 5120U;

  /**
   * The data to be submitted as a part of the EthereumTransaction.
   */
  std::shared_ptr<EthereumTransactionData> mEthereumData = nullptr;

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
  std::optional<Hbar> mMaxGasAllowance;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_ETHEREUM_FLOW_H_
