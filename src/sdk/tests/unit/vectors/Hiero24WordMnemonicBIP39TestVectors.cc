// SPDX-License-Identifier: Apache-2.0
#include "MnemonicBIP39.h"
#include "PublicKey.h"
#include "impl/DerivationPathUtils.h"
#include "impl/HexConverter.h"

#include <gtest/gtest.h>
#include <vector>

using namespace Hiero;

class Hiero24WordMnemonicBIP39TestVectors
  : public testing::TestWithParam<std::tuple<uint32_t, std::string, std::string, std::string, std::string>>
{
protected:
  [[nodiscard]] inline const MnemonicBIP39& getTestMnemonicBIP39() const { return mMnemonicBIP39; }

private:
  const MnemonicBIP39 mMnemonicBIP39 =
    MnemonicBIP39::initializeBIP39Mnemonic("inmate flip alley wear offer often piece magnet surge toddler submit right "
                                           "radio absent pear floor belt raven price stove replace reduce plate home");
};

class Hiero24WordMnemonicBIP39ToECDSAsecp256k1PrivateKeyTestVectors : public Hiero24WordMnemonicBIP39TestVectors
{
};

class Hiero24WordMnemonicBIP39ToED25519PrivateKeyTestVectors : public Hiero24WordMnemonicBIP39TestVectors
{
};

//-----
TEST_P(Hiero24WordMnemonicBIP39ToECDSAsecp256k1PrivateKeyTestVectors, ECDSAsecp256k1PrivateKeyTest)
{
  const auto& [index, passphrase, chainCode, privateKeyBytes, publicKeyBytes] = GetParam();

  const std::unique_ptr<PrivateKey> ecdsaSecp256k1PrivateKey =
    getTestMnemonicBIP39().toStandardECDSAsecp256k1PrivateKey(passphrase, index);

  ASSERT_EQ(ecdsaSecp256k1PrivateKey->getChainCode(), internal::HexConverter::hexToBytes(chainCode));
  ASSERT_EQ(ecdsaSecp256k1PrivateKey->toBytesRaw(), internal::HexConverter::hexToBytes(privateKeyBytes));
  ASSERT_EQ(ecdsaSecp256k1PrivateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(publicKeyBytes));
}

//-----
TEST_P(Hiero24WordMnemonicBIP39ToED25519PrivateKeyTestVectors, ED25519PrivateKeyTest)
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
  Hiero24WordMnemonicBIP39ToECDSAsecp256k1PrivateKeyTestVectors,
  testing::Values(std::make_tuple(0,
                                  "",
                                  "7717bc71194c257d4b233e16cf48c24adef630052f874a262d19aeb2b527620d",
                                  "0fde7bfd57ae6ec310bdd8b95967d98e8762a2c02da6f694b152cf9860860ab8",
                                  "03b1c064b4d04d52e51f6c8e8bb1bff75d62fa7b1446412d5901d424f6aedd6fd4"),
                  std::make_tuple(internal::DerivationPathUtils::getHardenedIndex(0),
                                  "",
                                  "e333da4bd9e21b5dbd2b0f6d88bad02f0fa24cf4b70b2fb613368d0364cdf8af",
                                  "aab7d720a32c2d1ea6123f58b074c865bb07f6c621f14cb012f66c08e64996bb",
                                  "03a0ea31bb3562f8a309b1436bc4b2f537301778e8a5e12b68cec26052f567a235"),
                  std::make_tuple(0,
                                  "some pass",
                                  "0ff552587f6baef1f0818136bacac0bb37236473f6ecb5a8c1cc68a716726ed1",
                                  "6df5ed217cf6d5586fdf9c69d39c843eb9d152ca19d3e41f7bab483e62f6ac25",
                                  "0357d69bb36fee569838fe7b325c07ca511e8c1b222873cde93fc6bb541eb7ecea"),
                  std::make_tuple(internal::DerivationPathUtils::getHardenedIndex(0),
                                  "some pass",
                                  "3a5048e93aad88f1c42907163ba4dce914d3aaf2eea87b4dd247ca7da7530f0b",
                                  "80df01f79ee1b1f4e9ab80491c592c0ef912194ccca1e58346c3d35cb5b7c098",
                                  "039ebe79f85573baa065af5883d0509a5634245f7864ddead76a008c9e42aa758d"),
                  std::make_tuple(2147483647,
                                  "some pass",
                                  "e54254940db58ef4913a377062ac6e411daebf435ad592d262d5a66d808a8b94",
                                  "60cb2496a623e1201d4e0e7ce5da3833cd4ec7d6c2c06bce2bcbcbc9dfef22d6",
                                  "02b59f348a6b69bd97afa80115e2d5331749b3c89c61297255430c487d6677f404"),
                  std::make_tuple(internal::DerivationPathUtils::getHardenedIndex(2147483647),
                                  "some pass",
                                  "cb23165e9d2d798c85effddc901a248a1a273fab2a56fe7976df97b016e7bb77",
                                  "100477c333028c8849250035be2a0a166a347a5074a8a727bce1db1c65181a50",
                                  "03d10ebfa2d8ff2cd34aa96e5ef59ca2e69316b4c0996e6d5f54b6932fe51be560")));

//-----
INSTANTIATE_TEST_SUITE_P(
  ED25519PrivateKeyFromMnemonicBIP39Tests,
  Hiero24WordMnemonicBIP39ToED25519PrivateKeyTestVectors,
  testing::Values(std::make_tuple(0,
                                  "",
                                  "404914563637c92d688deb9d41f3f25cbe8d6659d859cc743712fcfac72d7eda",
                                  "f8dcc99a1ced1cc59bc2fee161c26ca6d6af657da9aa654da724441343ecd16f",
                                  "2e42c9f5a5cdbde64afa65ce3dbaf013d5f9ff8d177f6ef4eb89fbe8c084ec0d"),
                  std::make_tuple(2147483647,
                                  "",
                                  "9c2b0073ac934696cd0b52c6c521b9bd1902aac134380a737282fdfe29014bf1",
                                  "e978a6407b74a0730f7aeb722ad64ab449b308e56006c8bff9aad070b9b66ddf",
                                  "c4b33dca1f83509f17b69b2686ee46b8556143f79f4b9df7fe7ed3864c0c64d0"),
                  std::make_tuple(0,
                                  "some pass",
                                  "699344acc5e07c77eb63b154b4c5c3d33cab8bf85ee21bea4cc29ab7f0502259",
                                  "abeca64d2337db386e289482a252334c68c7536daaefff55dc169ddb77fbae28",
                                  "fd311925a7a04b38f7508931c6ae6a93e5dc4394d83dafda49b051c0017d3380"),
                  std::make_tuple(2147483647,
                                  "some pass",
                                  "e5af7c95043a912af57a6e031ddcad191677c265d75c39954152a2733c750a3b",
                                  "9a601db3e24b199912cec6573e6a3d01ffd3600d50524f998b8169c105165ae5",
                                  "cf525500706faa7752dca65a086c9381d30d72cc67f23bf334f330579074a890")));