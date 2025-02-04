// SPDX-License-Identifier: Apache-2.0
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "PublicKey.h"
#include "impl/DerivationPathUtils.h"
#include "impl/HexConverter.h"

#include <gtest/gtest.h>
#include <vector>

using namespace Hiero;

class SLIP10TestVectors : public testing::Test
{
protected:
  [[nodiscard]] inline const std::string& getTestVector1Seed() const { return mTestVector1Seed; }
  [[nodiscard]] inline const std::string& getTestVector2Seed() const { return mTestVector2Seed; }

private:
  // SLIP10 spec provided seeds
  const std::string mTestVector1Seed = "000102030405060708090a0b0c0d0e0f";
  const std::string mTestVector2Seed = "fffcf9f6f3f0edeae7e4e1dedbd8d5d2cfccc9c6c3c0bdbab7b4b1aeaba8a5a29f9c999693908d8"
                                       "a8784817e7b7875726f6c696663605d5a5754514e4b484542";
};

//-----
TEST_F(SLIP10TestVectors, ECDSAsecp256k1PrivateKeyTestVector1)
{
  std::unique_ptr<PrivateKey> privateKey =
    ECDSAsecp256k1PrivateKey::fromSeed(internal::HexConverter::hexToBytes(getTestVector1Seed()));

  std::string expectedChainCode = "873dff81c02f525623fd1fe5167eac3a55a049de3d314bb42ee227ffed37d508";
  std::string expectedPrivate = "e8f32e723decf4051aefac8e2c93c9c5b214313817cdb01a1494b917c8436b35";
  std::string expectedPublic = "0339a36013301597daef41fbe593a02cc513d0b55527ec2df1050e2e8ff49c85c2";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(internal::DerivationPathUtils::getHardenedIndex(0));
  expectedChainCode = "47fdacbd0f1097043b78c63c20c34ef4ed9a111d980047ad16282c7ae6236141";
  expectedPrivate = "edb2e14f9ee77d26dd93b4ecede8d16ed408ce149b6cd80b0715a2d911a0afea";
  expectedPublic = "035a784662a4a20a65bf6aab9ae98a6c068a81c52e4b032c0fb5400c706cfccc56";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(1);
  expectedChainCode = "2a7857631386ba23dacac34180dd1983734e444fdbf774041578e9b6adb37c19";
  expectedPrivate = "3c6cb8d0f6a264c91ea8b5030fadaa8e538b020f0a387421a12de9319dc93368";
  expectedPublic = "03501e454bf00751f24b1b489aa925215d66af2234e3891c3b21a52bedb3cd711c";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(internal::DerivationPathUtils::getHardenedIndex(2));
  expectedChainCode = "04466b9cc8e161e966409ca52986c584f07e9dc81f735db683c3ff6ec7b1503f";
  expectedPrivate = "cbce0d719ecf7431d88e6a89fa1483e02e35092af60c042b1df2ff59fa424dca";
  expectedPublic = "0357bfe1e341d01c69fe5654309956cbea516822fba8a601743a012a7896ee8dc2";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(2);
  expectedChainCode = "cfb71883f01676f587d023cc53a35bc7f88f724b1f8c2892ac1275ac822a3edd";
  expectedPrivate = "0f479245fb19a38a1954c5c7c0ebab2f9bdfd96a17563ef28a6a4b1a2a764ef4";
  expectedPublic = "02e8445082a72f29b75ca48748a914df60622a609cacfce8ed0e35804560741d29";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(1000000000);
  expectedChainCode = "c783e67b921d2beb8f6b389cc646d7263b4145701dadd2161548a8b078e65e9e";
  expectedPrivate = "471b76e389e528d6de6d816857e012c5455051cad6660850e58372a6c3e6e7c8";
  expectedPublic = "022a471424da5e657499d1ff51cb43c47481a03b1e77f951fe64cec9f5a48f7011";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));
}

//-----
TEST_F(SLIP10TestVectors, ECDSAsecp256k1PrivateKeyTestVector2)
{
  std::unique_ptr<PrivateKey> privateKey =
    ECDSAsecp256k1PrivateKey::fromSeed(internal::HexConverter::hexToBytes(getTestVector2Seed()));

  std::string expectedChainCode = "60499f801b896d83179a4374aeb7822aaeaceaa0db1f85ee3e904c4defbd9689";
  std::string expectedPrivate = "4b03d6fc340455b363f51020ad3ecca4f0850280cf436c70c727923f6db46c3e";
  std::string expectedPublic = "03cbcaa9c98c877a26977d00825c956a238e8dddfbd322cce4f74b0b5bd6ace4a7";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(0);
  expectedChainCode = "f0909affaa7ee7abe5dd4e100598d4dc53cd709d5a5c2cac40e7412f232f7c9c";
  expectedPrivate = "abe74a98f6c7eabee0428f53798f0ab8aa1bd37873999041703c742f15ac7e1e";
  expectedPublic = "02fc9e5af0ac8d9b3cecfe2a888e2117ba3d089d8585886c9c826b6b22a98d12ea";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(internal::DerivationPathUtils::getHardenedIndex(2147483647));
  expectedChainCode = "be17a268474a6bb9c61e1d720cf6215e2a88c5406c4aee7b38547f585c9a37d9";
  expectedPrivate = "877c779ad9687164e9c2f4f0f4ff0340814392330693ce95a58fe18fd52e6e93";
  expectedPublic = "03c01e7425647bdefa82b12d9bad5e3e6865bee0502694b94ca58b666abc0a5c3b";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(1);
  expectedChainCode = "f366f48f1ea9f2d1d3fe958c95ca84ea18e4c4ddb9366c336c927eb246fb38cb";
  expectedPrivate = "704addf544a06e5ee4bea37098463c23613da32020d604506da8c0518e1da4b7";
  expectedPublic = "03a7d1d856deb74c508e05031f9895dab54626251b3806e16b4bd12e781a7df5b9";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(internal::DerivationPathUtils::getHardenedIndex(2147483646));
  expectedChainCode = "637807030d55d01f9a0cb3a7839515d796bd07706386a6eddf06cc29a65a0e29";
  expectedPrivate = "f1c7c871a54a804afe328b4c83a1c33b8e5ff48f5087273f04efa83b247d6a2d";
  expectedPublic = "02d2b36900396c9282fa14628566582f206a5dd0bcc8d5e892611806cafb0301f0";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(2);
  expectedChainCode = "9452b549be8cea3ecb7a84bec10dcfd94afe4d129ebfd3b3cb58eedf394ed271";
  expectedPrivate = "bb7d39bdb83ecf58f2fd82b6d918341cbef428661ef01ab97c28a4842125ac23";
  expectedPublic = "024d902e1a2fc7a8755ab5b694c575fce742c48d9ff192e63df5193e4c7afe1f9c";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));
}

//-----
TEST_F(SLIP10TestVectors, ED25519PrivateKeyTestVector1)
{
  std::unique_ptr<PrivateKey> privateKey =
    ED25519PrivateKey::fromSeed(internal::HexConverter::hexToBytes(getTestVector1Seed()));

  std::string expectedChainCode = "90046a93de5380a72b5e45010748567d5ea02bbf6522f979e05c0d8d8ca9fffb";
  std::string expectedPrivate = "2b4be7f19ee27bbf30c667b642d5f4aa69fd169872f8fc3059c08ebae2eb19e7";
  std::string expectedPublic = "a4b2856bfec510abab89753fac1ac0e1112364e7d250545963f135f2a33188ed";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(0);
  expectedChainCode = "8b59aa11380b624e81507a27fedda59fea6d0b779a778918a2fd3590e16e9c69";
  expectedPrivate = "68e0fe46dfb67e368c75379acec591dad19df3cde26e63b93a8e704f1dade7a3";
  expectedPublic = "8c8a13df77a28f3445213a0f432fde644acaa215fc72dcdf300d5efaa85d350c";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(1);
  expectedChainCode = "a320425f77d1b5c2505a6b1b27382b37368ee640e3557c315416801243552f14";
  expectedPrivate = "b1d0bad404bf35da785a64ca1ac54b2617211d2777696fbffaf208f746ae84f2";
  expectedPublic = "1932a5270f335bed617d5b935c80aedb1a35bd9fc1e31acafd5372c30f5c1187";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(2);
  expectedChainCode = "2e69929e00b5ab250f49c3fb1c12f252de4fed2c1db88387094a0f8c4c9ccd6c";
  expectedPrivate = "92a5b23c0b8a99e37d07df3fb9966917f5d06e02ddbd909c7e184371463e9fc9";
  expectedPublic = "ae98736566d30ed0e9d2f4486a64bc95740d89c7db33f52121f8ea8f76ff0fc1";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(2);
  expectedChainCode = "8f6d87f93d750e0efccda017d662a1b31a266e4a6f5993b15f5c1f07f74dd5cc";
  expectedPrivate = "30d1dc7e5fc04c31219ab25a27ae00b50f6fd66622f6e9c913253d6511d1e662";
  expectedPublic = "8abae2d66361c879b900d204ad2cc4984fa2aa344dd7ddc46007329ac76c429c";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(1000000000);
  expectedChainCode = "68789923a0cac2cd5a29172a475fe9e0fb14cd6adb5ad98a3fa70333e7afa230";
  expectedPrivate = "8f94d394a8e8fd6b1bc2f3f49f5c47e385281d5c17e65324b0f62483e37e8793";
  expectedPublic = "3c24da049451555d51a7014a37337aa4e12d41e485abccfa46b47dfb2af54b7a";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));
}

//-----
TEST_F(SLIP10TestVectors, ED25519PrivateKeyTestVector2)
{
  std::unique_ptr<PrivateKey> privateKey =
    ED25519PrivateKey::fromSeed(internal::HexConverter::hexToBytes(getTestVector2Seed()));

  std::string expectedChainCode = "ef70a74db9c3a5af931b5fe73ed8e1a53464133654fd55e7a66f8570b8e33c3b";
  std::string expectedPrivate = "171cb88b1b3c1db25add599712e36245d75bc65a1a5c9e18d76f9f2b1eab4012";
  std::string expectedPublic = "8fe9693f8fa62a4305a140b9764c5ee01e455963744fe18204b4fb948249308a";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(0);
  expectedChainCode = "0b78a3226f915c082bf118f83618a618ab6dec793752624cbeb622acb562862d";
  expectedPrivate = "1559eb2bbec5790b0c65d8693e4d0875b1747f4970ae8b650486ed7470845635";
  expectedPublic = "86fab68dcb57aa196c77c5f264f215a112c22a912c10d123b0d03c3c28ef1037";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(2147483647);
  expectedChainCode = "138f0b2551bcafeca6ff2aa88ba8ed0ed8de070841f0c4ef0165df8181eaad7f";
  expectedPrivate = "ea4f5bfe8694d8bb74b7b59404632fd5968b774ed545e810de9c32a4fb4192f4";
  expectedPublic = "5ba3b9ac6e90e83effcd25ac4e58a1365a9e35a3d3ae5eb07b9e4d90bcf7506d";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(1);
  expectedChainCode = "73bd9fff1cfbde33a1b846c27085f711c0fe2d66fd32e139d3ebc28e5a4a6b90";
  expectedPrivate = "3757c7577170179c7868353ada796c839135b3d30554bbb74a4b1e4a5a58505c";
  expectedPublic = "2e66aa57069c86cc18249aecf5cb5a9cebbfd6fadeab056254763874a9352b45";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(2147483646);
  expectedChainCode = "0902fe8a29f9140480a00ef244bd183e8a13288e4412d8389d140aac1794825a";
  expectedPrivate = "5837736c89570de861ebc173b1086da4f505d4adb387c6a1b1342d5e4ac9ec72";
  expectedPublic = "e33c0f7d81d843c572275f287498e8d408654fdf0d1e065b84e2e6f157aab09b";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));

  privateKey = privateKey->derive(2);
  expectedChainCode = "5d70af781f3a37b829f0d060924d5e960bdc02e85423494afc0b1a41bbe196d4";
  expectedPrivate = "551d333177df541ad876a60ea71f00447931c0a9da16f227c11ea080d7391b8d";
  expectedPublic = "47150c75db263559a70d5778bf36abbab30fb061ad69f69ece61a72b0cfa4fc0";

  EXPECT_EQ(privateKey->getChainCode(), internal::HexConverter::hexToBytes(expectedChainCode));
  EXPECT_EQ(privateKey->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPrivate));
  EXPECT_EQ(privateKey->getPublicKey()->toBytesRaw(), internal::HexConverter::hexToBytes(expectedPublic));
}
