// SPDX-License-Identifier: Apache-2.0
#include "MnemonicBIP39.h"
#include "PublicKey.h"
#include "impl/DerivationPathUtils.h"
#include "impl/HexConverter.h"

#include <gtest/gtest.h>
#include <vector>

using namespace Hiero;

class Hiero12WordMnemonicBIP39TestVectors
  : public testing::TestWithParam<std::tuple<uint32_t, std::string, std::string, std::string, std::string>>
{
protected:
  [[nodiscard]] inline const MnemonicBIP39& getTestMnemonicBIP39() const { return mMnemonicBIP39; }

private:
  const MnemonicBIP39 mMnemonicBIP39 = MnemonicBIP39::initializeBIP39Mnemonic(
    "finish furnace tomorrow wine mass goose festival air palm easy region guilt");
};

class Hiero12WordMnemonicBIP39ToECDSAsecp256k1PrivateKeyTestVectors : public Hiero12WordMnemonicBIP39TestVectors
{
};

class Hiero12WordMnemonicBIP39ToED25519PrivateKeyTestVectors : public Hiero12WordMnemonicBIP39TestVectors
{
};

//-----
TEST_P(Hiero12WordMnemonicBIP39ToECDSAsecp256k1PrivateKeyTestVectors, ECDSAsecp256k1PrivateKeyTest)
{
  const auto& [index, passphrase, chainCode, privateKeyBytes, publicKeyBytes] = GetParam();

  const std::unique_ptr<PrivateKey> ecdsaSecp256k1PrivateKey =
    getTestMnemonicBIP39().toStandardECDSAsecp256k1PrivateKey(passphrase, index);

  ASSERT_EQ(ecdsaSecp256k1PrivateKey->getChainCode(), internal::HexConverter::hexToBytes(chainCode));
  ASSERT_EQ(ecdsaSecp256k1PrivateKey->toBytesRaw(), internal::HexConverter::hexToBytes(privateKeyBytes));
  ASSERT_EQ(ecdsaSecp256k1PrivateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(publicKeyBytes));
}

//-----
TEST_P(Hiero12WordMnemonicBIP39ToED25519PrivateKeyTestVectors, ED25519PrivateKeyTest)
{
  const auto& [index, passphrase, chainCode, privateKeyBytes, publicKeyBytes] = GetParam();

  const std::unique_ptr<PrivateKey> ed25519PrivateKey =
    getTestMnemonicBIP39().toStandardEd25519PrivateKey(passphrase, index);

  ASSERT_EQ(ed25519PrivateKey->getChainCode(), internal::HexConverter::hexToBytes(chainCode));
  ASSERT_EQ(ed25519PrivateKey->toBytesRaw(), internal::HexConverter::hexToBytes(privateKeyBytes));
  ASSERT_EQ(ed25519PrivateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(publicKeyBytes));
}

//-----
INSTANTIATE_TEST_SUITE_P(
  ECDSAsecp256k1PrivateKeyFromMnemonicBIP39Tests,
  Hiero12WordMnemonicBIP39ToECDSAsecp256k1PrivateKeyTestVectors,
  testing::Values(std::make_tuple(0,
                                  "",
                                  "e76e0480faf2790e62dc1a7bac9dce51db1b3571fd74d8e264abc0d240a55d09",
                                  "f033824c20dd9949ad7a4440f67120ee02a826559ed5884077361d69b2ad51dd",
                                  "0294bf84a54806989a74ca4b76291d386914610b40b610d303162b9e495bc06416"),
                  std::make_tuple(internal::DerivationPathUtils::getHardenedIndex(0),
                                  "",
                                  "60c39c6a77bd68c0aaabfe2f4711dc9c2247214c4f4dae15ad4cb76905f5f544",
                                  "962f549dafe2d9c8091ac918cb4fc348ab0767353f37501067897efbc84e7651",
                                  "027123855357fd41d28130fbc59053192b771800d28ef47319ef277a1a032af78f"),
                  std::make_tuple(0,
                                  "some pass",
                                  "911a1095b64b01f7f3a06198df3d618654e5ed65862b211997c67515e3167892",
                                  "c139ebb363d7f441ccbdd7f58883809ec0cc3ee7a122ef67974eec8534de65e8",
                                  "0293bdb1507a26542ed9c1ec42afe959cf8b34f39daab4bf842cdac5fa36d50ef7"),
                  std::make_tuple(internal::DerivationPathUtils::getHardenedIndex(0),
                                  "some pass",
                                  "64173f2dcb1d65e15e787ef882fa15f54db00209e2dab16fa1661244cd98e95c",
                                  "87c1d8d4bb0cebb4e230852f2a6d16f6847881294b14eb1d6058b729604afea0",
                                  "03358e7761a422ca1c577f145fe845c77563f164b2c93b5b34516a8fa13c2c0888"),
                  std::make_tuple(2147483647,
                                  "some pass",
                                  "a7250c2b07b368a054f5c91e6a3dbe6ca3bbe01eb0489fe8778304bd0a19c711",
                                  "2583170ee745191d2bb83474b1de41a1621c47f6e23db3f2bf413a1acb5709e4",
                                  "03f9eb27cc73f751e8e476dd1db79037a7df2c749fa75b6cc6951031370d2f95a5"),
                  std::make_tuple(internal::DerivationPathUtils::getHardenedIndex(2147483647),
                                  "some pass",
                                  "66a1175e7690e3714d53ffce16ee6bb4eb02065516be2c2ad6bf6c9df81ec394",
                                  "f2d008cd7349bdab19ed85b523ba218048f35ca141a3ecbc66377ad50819e961",
                                  "027b653d04958d4bf83dd913a9379b4f9a1a1e64025a691830a67383bc3157c044")));

//-----
INSTANTIATE_TEST_SUITE_P(
  ED25519PrivateKeyFromMnemonicBIP39Tests,
  Hiero12WordMnemonicBIP39ToED25519PrivateKeyTestVectors,
  testing::Values(std::make_tuple(0,
                                  "",
                                  "48c89d67e9920e443f09d2b14525213ff83b245c8b98d63747ea0801e6d0ff3f",
                                  "020487611f3167a68482b0f4aacdeb02cc30c52e53852af7b73779f67eeca3c5",
                                  "2d047ff02a2091f860633f849ea2024b23e7803cfd628c9bdd635010cbd782d3"),
                  std::make_tuple(2147483647,
                                  "",
                                  "c0bcdbd9df6d8a4f214f20f3e5c7856415b68be34a1f406398c04690818bea16",
                                  "d0c4484480944db698dd51936b7ecc81b0b87e8eafc3d5563c76339338f9611a",
                                  "a1a2573c2c45bd57b0fd054865b5b3d8f492a6e1572bf04b44471e07e2f589b2"),
                  std::make_tuple(0,
                                  "some pass",
                                  "998a156855ab5398afcde06164b63c5523ff2c8900db53962cc2af191df59e1c",
                                  "d06630d6e4c17942155819bbbe0db8306cd989ba7baf3c29985c8455fbefc37f",
                                  "6bd0a51e0ca6fcc8b13cf25efd0b4814978bcaca7d1cf7dbedf538eb02969acb"),
                  std::make_tuple(2147483647,
                                  "some pass",
                                  "19d99506a5ce2dc0080092068d278fe29b85ffb8d9c26f8956bfca876307c79c",
                                  "a095ef77ee88da28f373246e9ae143f76e5839f680746c3f921e90bf76c81b08",
                                  "35be6a2a37ff6bbb142e9f4d9b558308f4f75d7c51d5632c6a084257455e1461")));