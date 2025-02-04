// SPDX-License-Identifier: Apache-2.0
#include "MnemonicBIP39.h"
#include "PrivateKey.h"

#include <dotenv.h>
#include <iostream>
#include <string>

using namespace Hiero;

int main(int argc, char** argv)
{
  dotenv::init();
  const std::string passphrase = std::getenv("PASSPHRASE") != nullptr ? std::getenv("PASSPHRASE") : "passphrase";

  // Generate and print a 12-word BIP39 mnemonic
  MnemonicBIP39 mnemonicBip39 = MnemonicBIP39::generate12WordBIP39Mnemonic();
  std::cout << "Generated 12-word MnemonicBIP39: " << mnemonicBip39.toString() << std::endl;

  // Generate and print a ED25519PrivateKey and ECDSAsecp256k1PrivateKey from the mnemonic with no passphrase
  std::unique_ptr<PrivateKey> ed25519PrivateKey = mnemonicBip39.toStandardEd25519PrivateKey();
  std::unique_ptr<PrivateKey> ecdsaSecp256k1PrivateKey = mnemonicBip39.toStandardECDSAsecp256k1PrivateKey();
  std::cout << std::endl
            << "Generated ED25519PrivateKey from mnemonic with no passphrase: " << ed25519PrivateKey->toStringRaw()
            << std::endl;
  std::cout << "Generated ECDSAsecp256k1PrivateKey from mnemonic with no passphrase: "
            << ecdsaSecp256k1PrivateKey->toStringRaw() << std::endl;

  // Regenerate and print a ED25519PrivateKey and ECDSAsecp256k1PrivateKey from the mnemonic with no passphrase
  ed25519PrivateKey = mnemonicBip39.toStandardEd25519PrivateKey();
  ecdsaSecp256k1PrivateKey = mnemonicBip39.toStandardECDSAsecp256k1PrivateKey();
  std::cout << std::endl
            << "Regenerated ED25519PrivateKey from mnemonic with no passphrase: " << ed25519PrivateKey->toStringRaw()
            << std::endl;
  std::cout << "Regenerated ECDSAsecp256k1PrivateKey from mnemonic with no passphrase: "
            << ecdsaSecp256k1PrivateKey->toStringRaw() << std::endl;

  // Generate and print a ED25519PrivateKey and ECDSAsecp256k1PrivateKey from the mnemonic with a passphrase
  ed25519PrivateKey = mnemonicBip39.toStandardEd25519PrivateKey(passphrase);
  ecdsaSecp256k1PrivateKey = mnemonicBip39.toStandardECDSAsecp256k1PrivateKey(passphrase);
  std::cout << std::endl
            << "Generated ED25519PrivateKey from mnemonic with passphrase '" << passphrase
            << "': " << ed25519PrivateKey->toStringRaw() << std::endl;
  std::cout << "Generated ECDSAsecp256k1PrivateKey from mnemonic with passphrase '" << passphrase
            << "': " << ecdsaSecp256k1PrivateKey->toStringRaw() << std::endl;

  // Regenerate and print a ED25519PrivateKey and ECDSAsecp256k1PrivateKey from the mnemonic with a passphrase
  ed25519PrivateKey = mnemonicBip39.toStandardEd25519PrivateKey(passphrase);
  ecdsaSecp256k1PrivateKey = mnemonicBip39.toStandardECDSAsecp256k1PrivateKey(passphrase);
  std::cout << std::endl
            << "Regenerated ED25519PrivateKey from mnemonic with passphrase '" << passphrase
            << "': " << ed25519PrivateKey->toStringRaw() << std::endl;
  std::cout << "Regenerated ECDSAsecp256k1PrivateKey from mnemonic with passphrase '" << passphrase
            << "': " << ecdsaSecp256k1PrivateKey->toStringRaw() << std::endl;

  // Start a new section of printing
  std::cout << std::endl;
  std::cout << "-----------------------------------------------" << std::endl;

  // Generate and print a 24-word BIP39 mnemonic
  mnemonicBip39 = MnemonicBIP39::generate24WordBIP39Mnemonic();
  std::cout << "Generated 24-word MnemonicBIP39: " << mnemonicBip39.toString() << std::endl;

  // Generate and print a ED25519PrivateKey and ECDSAsecp256k1PrivateKey from the mnemonic with no passphrase
  ed25519PrivateKey = mnemonicBip39.toStandardEd25519PrivateKey();
  ecdsaSecp256k1PrivateKey = mnemonicBip39.toStandardECDSAsecp256k1PrivateKey();
  std::cout << std::endl
            << "Generated ED25519PrivateKey from mnemonic with no passphrase: " << ed25519PrivateKey->toStringRaw()
            << std::endl;
  std::cout << "Generated ECDSAsecp256k1PrivateKey from mnemonic with no passphrase: "
            << ecdsaSecp256k1PrivateKey->toStringRaw() << std::endl;

  // Regenerate and print a ED25519PrivateKey and ECDSAsecp256k1PrivateKey from the mnemonic with no passphrase
  ed25519PrivateKey = mnemonicBip39.toStandardEd25519PrivateKey();
  ecdsaSecp256k1PrivateKey = mnemonicBip39.toStandardECDSAsecp256k1PrivateKey();
  std::cout << std::endl
            << "Regenerated ED25519PrivateKey from mnemonic with no passphrase: " << ed25519PrivateKey->toStringRaw()
            << std::endl;
  std::cout << "Regenerated ECDSAsecp256k1PrivateKey from mnemonic with no passphrase: "
            << ecdsaSecp256k1PrivateKey->toStringRaw() << std::endl;

  // Generate and print a ED25519PrivateKey and ECDSAsecp256k1PrivateKey from the mnemonic with a passphrase
  ed25519PrivateKey = mnemonicBip39.toStandardEd25519PrivateKey(passphrase);
  ecdsaSecp256k1PrivateKey = mnemonicBip39.toStandardECDSAsecp256k1PrivateKey(passphrase);
  std::cout << std::endl
            << "Generated ED25519PrivateKey from mnemonic with passphrase '" << passphrase
            << "': " << ed25519PrivateKey->toStringRaw() << std::endl;
  std::cout << "Generated ECDSAsecp256k1PrivateKey from mnemonic with passphrase '" << passphrase
            << "': " << ecdsaSecp256k1PrivateKey->toStringRaw() << std::endl;

  // Regenerate and print a ED25519PrivateKey and ECDSAsecp256k1PrivateKey from the mnemonic with a passphrase
  ed25519PrivateKey = mnemonicBip39.toStandardEd25519PrivateKey(passphrase);
  ecdsaSecp256k1PrivateKey = mnemonicBip39.toStandardECDSAsecp256k1PrivateKey(passphrase);
  std::cout << std::endl
            << "Regenerated ED25519PrivateKey from mnemonic with passphrase '" << passphrase
            << "': " << ed25519PrivateKey->toStringRaw() << std::endl;
  std::cout << "Regenerated ECDSAsecp256k1PrivateKey from mnemonic with passphrase '" << passphrase
            << "': " << ecdsaSecp256k1PrivateKey->toStringRaw() << std::endl;

  return 0;
}
