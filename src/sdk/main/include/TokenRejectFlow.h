// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_REJECT_FLOW_H_
#define HIERO_SDK_CPP_TOKEN_REJECT_FLOW_H_

#include "AccountId.h"
#include "NftId.h"
#include "TokenDissociateTransaction.h"
#include "TokenId.h"
#include "TokenRejectTransaction.h"
#include "TransactionResponse.h"

namespace Hiero
{
class Client;
class PrivateKey;
}

namespace Hiero
{
/**
 * Reject undesired token(s) and dissociate in a single flow.
 */
class TokenRejectFlow
{
public:
  /**
   * The transaction used to reject tokens.
   */
  TokenRejectTransaction tokenRejectTransaction;

  /**
   * The transaction used to dissociate tokens.
   */
  TokenDissociateTransaction tokenDissociateTransaction;

  /**
   * Execute the Transactions in this flow (TokenRejectTransaction and TokenDissociateTransaction).
   *
   * @param client The Client to use to submit these Transactions.
   * @return The TransactionResponse object of the flow.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionResponse execute(const Client& client);

  /**
   * Execute the Transactions in this flow (TokenRejectTransaction and TokenDissociateTransaction) with timeout.
   *
   * @param client  The Client to use to submit these Transactions.
   * @param timeout The desired timeout for the execution of these Transactions.
   * @return The TransactionResponse object of the flow.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionResponse execute(const Client& client, const std::chrono::system_clock::duration& timeout);

  /**
   * Set the PrivateKey to be used for signing the transactions.
   *
   * @param privateKey A shared pointer to the `PrivateKey` object.
   */
  void setReceiverPrivateKey(const std::shared_ptr<PrivateKey>& privateKey) { mReceiverPrivateKey = privateKey; }

private:
  /**
   * The PrivateKey with which to sign the transactions.
   */
  std::shared_ptr<PrivateKey> mReceiverPrivateKey = nullptr;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_REJECT_FLOW_H_
