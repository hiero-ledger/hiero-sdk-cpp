// SPDX-License-Identifier: Apache-2.0
#include "TokenRejectFlow.h"
#include "AccountId.h"
#include "Client.h"
#include "NftId.h"
#include "TokenDissociateTransaction.h"
#include "TokenId.h"
#include "TokenRejectTransaction.h"
#include "TransactionReceipt.h"
#include "exceptions/UninitializedException.h"

namespace Hiero
{
//-----
TransactionResponse TokenRejectFlow::execute(const Client& client)
{
  return execute(client, client.getRequestTimeout());
}

//-----
TransactionResponse TokenRejectFlow::execute(const Client& client, const std::chrono::system_clock::duration& timeout)
{
  // Submit the TokenRejectTransaction.
  TransactionResponse txResponse =
    tokenRejectTransaction.freezeWith(&client).sign(mReceiverPrivateKey).execute(client, timeout);

  // Make sure the transaction reaches consensus.
  TransactionReceipt txReceipt = txResponse.getReceipt(client, timeout);

  // build dissociate vector from rejected nfts
  std::vector<TokenId> toDissociate;
  std::for_each(tokenRejectTransaction.getNfts().cbegin(),
                tokenRejectTransaction.getNfts().cend(),
                [&toDissociate](const NftId& nftId) { toDissociate.push_back(nftId.mTokenId); });

  // Make sure the transaction reaches consensus.
  txReceipt = tokenDissociateTransaction.setTokenIds(toDissociate)
                .freezeWith(&client)
                .sign(mReceiverPrivateKey)
                .execute(client, timeout)
                .getReceipt(client, timeout);

  return txResponse;
}

} // namespace Hiero