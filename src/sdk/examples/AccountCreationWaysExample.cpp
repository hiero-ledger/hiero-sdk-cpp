// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "ECDSAsecp256k1PrivateKey.h"

#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  /**
   * Reference: [HIP-583 Expand alias support in CryptoCreate & CryptoTransfer
   * Transactions](https://hips.hedera.com/hip/hip-583)
   * In Hiero we have the concept of 4 different account representations:
   *  - An account can have an account ID in shard.realm.accountNumber format (0.0.10).
   *  - An account can have a public key alias in
   *    0.0.302D300706052B8104000A032200036847776633520568B5B4B1D074C647BE63579B3D7DC9E4B638042CB4E041C8B8 format.
   *  - An account can have an AccountId that is represented in 0x000000000000000000000000000000000000000A (for account
   *    ID 0.0.10) long zero format.
   *  - An account can be represented by an Ethereum public address 0xB794F5EA0BA39494CE839613FFFBA74279579268.
   */

  // An Account ID in shard.realm.number format, i.e. `0.0.10` with the corresponding
  // `0x000000000000000000000000000000000000000A` ethereum address.
  const AccountId hieroFormat = AccountId::fromString("0.0.10");
  std::cout << "Account ID: " + hieroFormat.toString() << std::endl;
  std::cout << "Account " + hieroFormat.toString() + " corresponding Long-Zero address: "
            << hieroFormat.toSolidityAddress() << std::endl;

  // The Hiero Long-Form Account ID: 0.0.aliasPublicKey, i.e.
  // `0.0.302D300706052B8104000A032200036847776633520568B5B4B1D074C647BE63579B3D7DC9E4B638042CB4E041C8B8`
  const std::unique_ptr<ECDSAsecp256k1PrivateKey> privateKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  const AccountId aliasAccountId = privateKey->getPublicKey()->toAccountId();
  std::cout << "Hiero Long-Form Account ID: " + aliasAccountId.toString() << std::endl;

  // The Hiero Account Long-Zero address `0x000000000000000000000000000000000000000a` (for accountId 0.0.10)
  const AccountId longZeroAddress = AccountId::fromString("0x000000000000000000000000000000000000000a");
  std::cout << "Hiero Account Long-Zero address: " << longZeroAddress.toString() << std::endl;

  // The Ethereum Account Address / public-address `0xb794f5ea0ba39494ce839613fffba74279579268`.
  const AccountId evmAddress = AccountId::fromString("0xb794f5ea0ba39494ce839613fffba74279579268");
  std::cout << "Ethereum Account Address / public-address: " << evmAddress.toString() << std::endl;

  return 0;
}
