// SPDX-License-Identifier: Apache-2.0
#include "AccountBalance.h"
#include "AccountBalanceQuery.h"
#include "AccountInfo.h"
#include "AccountInfoQuery.h"
#include "Client.h"
#include "ED25519PrivateKey.h"
#include "PublicKey.h"
#include "Status.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"

#include <dotenv.h>
#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const AccountId operatorAccountId = AccountId::fromString(std::getenv("OPERATOR_ID"));
  const std::shared_ptr<PrivateKey> operatorPrivateKey = ED25519PrivateKey::fromString(std::getenv("OPERATOR_KEY"));

  // Get a client for the Hiero testnet, and set the operator account ID and key such that all generated transactions
  // will be paid for by this account and be signed by this key.
  Client client = Client::forTestnet();
  client.setOperator(operatorAccountId, operatorPrivateKey);

  /*
   * Hiero supports a form of auto account creation.
   *
   * You can "create" an account by generating a private key, and then deriving the public key, without any need to
   * interact with the Hiero network. The public key more or less acts as the user's account ID. This public key is an
   * account's mAliasKey: a public key that aliases (or will eventually alias) to a Hiero account.
   *
   * An AccountId takes one of two forms: a normal AccountId with a null mAliasKey member takes the form 0.0.123, while
   * an AccountId with a non-null mAliasKey member takes the form
   * 0.0.302a300506032b6570032100114e6abc371b82dab5c15ea149f02d34a012087b163516dd70f44acafabf7777.
   * Note the prefix of "0.0." indicating the shard and realm. Also note that the mAliasKey is stringified as a
   * hex-encoded ASN1 DER representation of the key.
   *
   * An AccountId with a mAliasKey can be used just like a normal AccountId for the purposes of queries and
   * transactions, however most queries and transactions involving such an AccountId won't work until Hbar has been
   * transferred to the mAliasKey account.
   *
   * There is no record in the Hiero network of an account associated with a given mAliasKey until an amount of Hbar is
   * transferred to the account. The moment that Hbar is transferred to that mAliasKey AccountId is the moment that that
   * account actually begins to exist in the Hiero ledger.
   */

  // Generate a ED25519 private, public key pair
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();

  std::cout << "Generated private key: " << privateKey->toStringDer() << std::endl;
  std::cout << "Generated public key: " << publicKey->toStringDer() << std::endl;

  // Get an account ID from the generated public key (use default 0 for shared and realm).
  const AccountId aliasAccountId = publicKey->toAccountId();

  /*
   * Note that no queries or transactions have taken place yet. This account "creation" process is entirely local.
   *
   * AccountId.fromString() can construct an AccountId with an alias key. It expects a string of the form 0.0.123 in the
   * case of a normal AccountId, or of the form
   * 0.0.302a300506032b6570032100114e6abc371b82dab5c15ea149f02d34a012087b163516dd70f44acafabf7777
   * in the case of an AccountId with an alias key. Note the prefix of "0.0." to indicate the shard and realm.
   *
   * If the shard and realm are known, you may use PublicKey.fromString().toAccountId() to construct the alias key
   * AccountId.
   *
   * Now transfer some Hbar to the alias key account to officially create the account on the ledger.
   */
  std::cout << "Transfer Hbar to alias account key: "
            << gStatusToString.at(TransferTransaction()
                                    .addHbarTransfer(operatorAccountId, Hbar(10LL).negated())
                                    .addHbarTransfer(aliasAccountId, Hbar(10LL))
                                    .execute(client)
                                    .setValidateStatus(false)
                                    .getReceipt(client)
                                    .mStatus)
            << std::endl;

  std::cout << "Balance of the created account: "
            << AccountBalanceQuery().setAccountId(aliasAccountId).execute(client).mBalance.toString() << std::endl;

  /*
   * Note that once an account exists in the ledger, it is assigned a normal AccountId, which can be retrieved via an
   * AccountInfoQuery.
   *
   * Users may continue to refer to the account by its alias key AccountId, but they may also now refer to it by its
   * normal AccountId.
   */
  const AccountInfo accountInfo = AccountInfoQuery().setAccountId(aliasAccountId).execute(client);
  std::cout << "Created account ID: " << accountInfo.mAccountId.toString() << std::endl;
  std::cout << "Created account alias: " << accountInfo.mPublicKeyAlias->toStringDer() << std::endl;

  return 0;
}
