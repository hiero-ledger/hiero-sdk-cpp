// SPDX-License-Identifier: Apache-2.0
#include "MnemonicBIP39.h"
#include "impl/HexConverter.h"

#include <gtest/gtest.h>
#include <vector>

using namespace Hiero;

class BIP39TestVectors : public testing::TestWithParam<std::tuple<std::string, std::string, std::string>>
{
protected:
  [[nodiscard]] inline const std::string& getPassphrase() const { return mPassphrase; }

private:
  // BIP39 spec provided passphrase
  const std::string mPassphrase = "TREZOR";
};

//-----
TEST_P(BIP39TestVectors, TestVectors)
{
  const auto& [inputEntropy, mnemonicString, seed] = GetParam();

  MnemonicBIP39 mnemonicFromEntropy = MnemonicBIP39::initializeBIP39Mnemonic(
    MnemonicBIP39::entropyToWordIndices(internal::HexConverter::hexToBytes(inputEntropy)));

  MnemonicBIP39 mnemonicFromString = MnemonicBIP39::initializeBIP39Mnemonic(mnemonicString);

  ASSERT_EQ(mnemonicFromEntropy.toString(), mnemonicFromString.toString());
  ASSERT_EQ(mnemonicFromEntropy.toSeed(getPassphrase()), internal::HexConverter::hexToBytes(seed));
}

//-----
INSTANTIATE_TEST_SUITE_P(
  MnemonicBIP39,
  BIP39TestVectors,
  ::testing::Values(
    std::make_tuple("00000000000000000000000000000000",
                    "abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon about",
                    "c55257c360c07c72029aebc1b53c05ed0362ada38ead3e3e9efa3708e53495531f09a6987599d18264c1e1c92f2cf14163"
                    "0c7a3c4ab7c81b2f001698e7463b04"),
    std::make_tuple("7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f",
                    "legal winner thank year wave sausage worth useful legal winner thank yellow",
                    "2e8905819b8723fe2c1d161860e5ee1830318dbf49a83bd451cfb8440c28bd6fa457fe1296106559a3c80937a1c1069be3"
                    "a3a5bd381ee6260e8d9739fce1f607"),
    std::make_tuple("80808080808080808080808080808080",
                    "letter advice cage absurd amount doctor acoustic avoid letter advice cage above",
                    "d71de856f81a8acc65e6fc851a38d4d7ec216fd0796d0a6827a3ad6ed5511a30fa280f12eb2e47ed2ac03b5c462a0358d1"
                    "8d69fe4f985ec81778c1b370b652a8"),
    std::make_tuple("ffffffffffffffffffffffffffffffff",
                    "zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo wrong",
                    "ac27495480225222079d7be181583751e86f571027b0497b5b5d11218e0a8a13332572917f0f8e5a589620c6f15b11c61d"
                    "ee327651a14c34e18231052e48c069"),
    std::make_tuple("0000000000000000000000000000000000000000000000000000000000000000",
                    "abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon "
                    "abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon abandon art",
                    "bda85446c68413707090a52022edd26a1c9462295029f2e60cd7c4f2bbd3097170af7a4d73245cafa9c3cca8d561a7c3de"
                    "6f5d4a10be8ed2a5e608d68f92fcc8"),
    std::make_tuple("7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f7f",
                    "legal winner thank year wave sausage worth useful legal winner thank year wave sausage worth "
                    "useful legal winner thank year wave sausage worth title",
                    "bc09fca1804f7e69da93c2f2028eb238c227f2e9dda30cd63699232578480a4021b146ad717fbb7e451ce9eb835f43620b"
                    "f5c514db0f8add49f5d121449d3e87"),
    std::make_tuple("8080808080808080808080808080808080808080808080808080808080808080",
                    "letter advice cage absurd amount doctor acoustic avoid letter advice cage absurd amount doctor "
                    "acoustic avoid letter advice cage absurd amount doctor acoustic bless",
                    "c0c519bd0e91a2ed54357d9d1ebef6f5af218a153624cf4f2da911a0ed8f7a09e2ef61af0aca007096df430022f7a2b6fb"
                    "91661a9589097069720d015e4e982f"),
    std::make_tuple("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff",
                    "zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo zoo vote",
                    "dd48c104698c30cfe2b6142103248622fb7bb0ff692eebb00089b32d22484e1613912f0a5b694407be899ffd31ed3992c4"
                    "56cdf60f5d4564b8ba3f05a69890ad"),
    std::make_tuple("9e885d952ad362caeb4efe34a8e91bd2",
                    "ozone drill grab fiber curtain grace pudding thank cruise elder eight picnic",
                    "274ddc525802f7c828d8ef7ddbcdc5304e87ac3535913611fbbfa986d0c9e5476c91689f9c8a54fd55bd38606aa6a8595a"
                    "d213d4c9c9f9aca3fb217069a41028"),
    std::make_tuple("68a79eaca2324873eacc50cb9c6eca8cc68ea5d936f98787c60c7ebc74e6ce7c",
                    "hamster diagram private dutch cause delay private meat slide toddler razor book happy fancy "
                    "gospel tennis maple dilemma loan word shrug inflict delay length",
                    "64c87cde7e12ecf6704ab95bb1408bef047c22db4cc7491c4271d170a1b213d20b385bc1588d9c7b38f1b39d415665b8a9"
                    "030c9ec653d75e65f847d8fc1fc440"),
    std::make_tuple("c0ba5a8e914111210f2bd131f3d5e08d",
                    "scheme spot photo card baby mountain device kick cradle pact join borrow",
                    "ea725895aaae8d4c1cf682c1bfd2d358d52ed9f0f0591131b559e2724bb234fca05aa9c02c57407e04ee9dc3b454aa63fb"
                    "ff483a8b11de949624b9f1831a9612"),
    std::make_tuple("9f6a2878b2520799a44ef18bc7df394e7061a224d2c33cd015b157d746869863",
                    "panda eyebrow bullet gorilla call smoke muffin taste mesh discover soft ostrich alcohol speed "
                    "nation flash devote level hobby quick inner drive ghost inside",
                    "72be8e052fc4919d2adf28d5306b5474b0069df35b02303de8c1729c9538dbb6fc2d731d5f832193cd9fb6aeecbc469594"
                    "a70e3dd50811b5067f3b88b28c3e8d"),
    std::make_tuple("23db8160a31d3e0dca3688ed941adbf3",
                    "cat swing flag economy stadium alone churn speed unique patch report train",
                    "deb5f45449e615feff5640f2e49f933ff51895de3b4381832b3139941c57b59205a42480c52175b6efcffaa58a2503887c"
                    "1e8b363a707256bdd2b587b46541f5"),
    std::make_tuple("066dca1a2bb7e8a1db2832148ce9933eea0f3ac9548d793112d9a95c9407efad",
                    "all hour make first leader extend hole alien behind guard gospel lava path output census museum "
                    "junior mass reopen famous sing advance salt reform",
                    "26e975ec644423f4a4c4f4215ef09b4bd7ef924e85d1d17c4cf3f136c2863cf6df0a475045652c57eb5fb41513ca2a2d67"
                    "722b77e954b4b3fc11f7590449191d"),
    std::make_tuple("f30f8c1da665478f49b001d94c5fc452",
                    "vessel ladder alter error federal sibling chat ability sun glass valve picture",
                    "2aaa9242daafcee6aa9d7269f17d4efe271e1b9a529178d7dc139cd18747090bf9d60295d0ce74309a78852a9caadf0af4"
                    "8aae1c6253839624076224374bc63f"),
    std::make_tuple("f585c11aec520db57dd353c69554b21a89b20fb0650966fa0a9d6f74fd989d8f",
                    "void come effort suffer camp survey warrior heavy shoot primary clutch crush open amazing screen "
                    "patrol group space point ten exist slush involve unfold",
                    "01f5bced59dec48e362f2c45b5de68b9fd6c92c6634f44d6d40aab69056506f0e35524a518034ddc1192e1dacd32c1ed3e"
                    "aa3c3b131c88ed8e7e54c49a5d0998")));
