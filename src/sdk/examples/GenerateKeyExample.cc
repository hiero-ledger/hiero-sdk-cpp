// SPDX-License-Identifier: Apache-2.0
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "PrivateKey.h"
#include "PublicKey.h"

#include <iostream>

using namespace Hiero;

int main(int argc, char** argv)
{
  // Generate an ED25519 private
  const std::unique_ptr<PrivateKey> ed25519Key = ED25519PrivateKey::generatePrivateKey();

  // Generate an ECDSA private
  const std::unique_ptr<PrivateKey> ecdsaKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

  // Print the ED25519 private key and public key in RAW and DER encoded formats
  std::cout << "Generated ED25519PrivateKey Raw: " << ed25519Key->toStringRaw() << std::endl;
  std::cout << "Generated ED25519PrivateKey DER encoded: " << ed25519Key->toStringDer() << std::endl;
  std::cout << "Generated ED25519PublicKey Raw: " << ed25519Key->getPublicKey()->toStringRaw() << std::endl;
  std::cout << "Generated ED25519PublicKey DER encoded: " << ed25519Key->getPublicKey()->toStringDer() << std::endl;
  std::cout << "\n";

  // Print the ECDSA private key and public key in RAW and DER encoded formats
  std::cout << "Generated ECDSAsecp256k1PrivateKey Raw: " << ecdsaKey->toStringRaw() << std::endl;
  std::cout << "Generated ECDSAsecp256k1PrivateKey DER encoded: " << ecdsaKey->toStringDer() << std::endl;
  std::cout << "Generated ECDSAsecp256k1PublicKey Raw: " << ecdsaKey->getPublicKey()->toStringRaw() << std::endl;
  std::cout << "Generated ECDSAsecp256k1PublicKey DER encoded: " << ecdsaKey->getPublicKey()->toStringDer()
            << std::endl;

  return 0;
}
