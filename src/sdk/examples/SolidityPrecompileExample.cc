// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountId.h"
#include "Client.h"
#include "ContractHelper.h"
#include "ED25519PrivateKey.h"
#include "TransactionReceipt.h"
#include "TransactionResponse.h"
#include "impl/HexConverter.h"

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

  // Generate a new account.
  const std::shared_ptr<PrivateKey> privateKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PublicKey> publicKey = privateKey->getPublicKey();
  const AccountId accountId = AccountCreateTransaction()
                                .setKey(publicKey)
                                .setInitialBalance(Hbar(1000LL, HbarUnit::TINYBAR()))
                                .execute(client)
                                .getReceipt(client)
                                .mAccountId.value();
  std::cout << "Created new account with ID " << accountId.toString() << std::endl;

  // Instantiate the ContractHelper.
  ContractHelper contractHelper("precompile-example/PrecompileExample.json",
                                ContractFunctionParameters()
                                  .addAddress(operatorAccountId.toSolidityAddress())
                                  .addAddress(accountId.toSolidityAddress()),
                                client);

  /*
   * Step 0 tests pseudo random number generator (PRNG)
   * Step 1 creates a fungible token
   * Step 2 mints it
   * Step 3 associates Alice with it
   * Step 4 transfers it to Alice.
   * Step 5 approves an allowance of the fungible token with operator as the owner and Alice as the spender.
   * Steps 6 - 10 test misc functions on the fungible token (see PrecompileExample.sol for details).
   * Step 11 creates an NFT token with a custom fee, and with the admin and supply set to Alice's key
   * Step 12 mints some NFTs
   * Step 13 associates Alice with the NFT token
   * Step 14 transfers some NFTs to Alice
   * Step 15 approves an NFT allowance with operator as the owner and Alice as the spender [NOT WORKING]
   * Step 16 burn some NFTs
   */

  // Configure the steps in ContractHelper.
  contractHelper
    .setResultValidatorForStep(0,
                               [](const ContractFunctionResult& result)
                               {
                                 std::cout << "getPseudoRandomSeed() returned "
                                           << internal::HexConverter::bytesToHex(result.getBytes32(0)) << std::endl;
                                 return true;
                               })
    .setPayableAmountForStep(1, Hbar(20LL, HbarUnit::TINYBAR()))
    .addSignerForStep(3, privateKey)
    .addSignerForStep(5, privateKey)
    .setParameterSupplierForStep(11,
                                 [&privateKey]()
                                 {
                                   // Contracts work with the raw bytes of the public key.
                                   return ContractFunctionParameters().addBytes(
                                     privateKey->getPublicKey()->toBytesRaw());
                                 })
    .setPayableAmountForStep(11, Hbar(40LL, HbarUnit::TINYBAR()))
    .addSignerForStep(11, privateKey)
    .addSignerForStep(12, privateKey)
    .setParameterSupplierForStep(12,
                                 []()
                                 {
                                   return ContractFunctionParameters().addBytesArray(
                                     { { std::byte(0x01b) }, { std::byte(0x02b) }, { std::byte(0x03b) } });
                                 })
    .addSignerForStep(13, privateKey)
    .addSignerForStep(16, privateKey);

  // Execute all steps.
  contractHelper.executeSteps(/*from*/ 0, /*to*/ 16, client);

  // Everything worked!
  std::cout << "All steps completed with valid results" << std::endl;

  return 0;
}
