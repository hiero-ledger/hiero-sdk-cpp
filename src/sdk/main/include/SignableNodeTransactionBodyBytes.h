// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_SIGNABLE_NODE_TRANSACTION_BODY_BYTES_H_
#define HIERO_SDK_CPP_SIGNABLE_NODE_TRANSACTION_BODY_BYTES_H_

#include "AccountId.h"
#include "TransactionId.h"

#include <cstddef>
#include <vector>

namespace Hiero
{
/**
 * Represents a transaction body ready for external signing, explicitly
 * associated with a node account ID and transaction ID. This is used for
 * HSM (Hardware Security Module) signing workflows where the private key
 * never leaves the secure hardware.
 *
 * The workflow for HSM signing is:
 *   1. Create and freeze a transaction.
 *   2. Call getSignableNodeBodyBytesList() to get the canonical bodyBytes.
 *   3. Send each signableTransactionBodyBytes to an HSM for signing.
 *   4. Inject the signature back into the transaction via addSignature().
 *   5. Submit the signed transaction to the Hiero network.
 */
class SignableNodeTransactionBodyBytes
{
public:
  /**
   * The node account ID this signable body is targeted to.
   */
  AccountId mNodeAccountId;

  /**
   * The transaction ID of this signable body.
   */
  TransactionId mTransactionId;

  /**
   * The canonical body bytes that should be signed by the HSM or external
   * signer. Only these bytes should be signed — not the full transaction.
   */
  std::vector<std::byte> mSignableTransactionBodyBytes;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_SIGNABLE_NODE_TRANSACTION_BODY_BYTES_H_
