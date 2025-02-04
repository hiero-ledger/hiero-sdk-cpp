// SPDX-License-Identifier: Apache-2.0
#include "AccountId.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "PublicKey.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <gtest/gtest.h>

using namespace Hiero;

class AccountIdUnitTests : public ::testing::Test
{
protected:
  [[nodiscard]] inline const uint64_t& getTestShardNum() const { return mShardNum; }
  [[nodiscard]] inline const uint64_t& getTestRealmNum() const { return mRealmNum; }
  [[nodiscard]] inline const uint64_t& getTestAccountNum() const { return mAccountNum; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestEd25519Alias() const { return mEd25519Alias; }
  [[nodiscard]] inline const std::shared_ptr<PublicKey>& getTestEcdsaSecp256k1Alias() const
  {
    return mEcdsaSecp256k1Alias;
  }
  [[nodiscard]] inline const EvmAddress& getTestEvmAddressAlias() const { return mEvmAddressAlias; }

private:
  const uint64_t mShardNum = 8ULL;
  const uint64_t mRealmNum = 90ULL;
  const uint64_t mAccountNum = 1000ULL;
  const std::shared_ptr<PublicKey> mEd25519Alias = ED25519PrivateKey::generatePrivateKey()->getPublicKey();
  const std::shared_ptr<PublicKey> mEcdsaSecp256k1Alias =
    ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey();
  const EvmAddress mEvmAddressAlias = EvmAddress::fromString("0x0123456789abcdef0123456789abcdef01234567");
};

//-----
TEST_F(AccountIdUnitTests, ConstructWithAccountNum)
{
  // Given / When
  const AccountId accountId(getTestAccountNum());

  // Then
  EXPECT_EQ(accountId.mShardNum, 0ULL);
  EXPECT_EQ(accountId.mRealmNum, 0ULL);
  EXPECT_TRUE(accountId.mAccountNum.has_value());
  EXPECT_EQ(*accountId.mAccountNum, getTestAccountNum());
  EXPECT_EQ(accountId.mPublicKeyAlias, nullptr);
  EXPECT_FALSE(accountId.mEvmAddressAlias.has_value());
}

//-----
TEST_F(AccountIdUnitTests, ConstructWithAccountAlias)
{
  // Given / When
  const AccountId ed25519AliasAccountId(getTestEd25519Alias());
  const AccountId ecdsaAliasAccountId(getTestEcdsaSecp256k1Alias());

  // Then
  EXPECT_EQ(ed25519AliasAccountId.mShardNum, 0ULL);
  EXPECT_EQ(ed25519AliasAccountId.mRealmNum, 0ULL);
  EXPECT_FALSE(ed25519AliasAccountId.mAccountNum.has_value());
  EXPECT_NE(ed25519AliasAccountId.mPublicKeyAlias, nullptr);
  EXPECT_EQ(ed25519AliasAccountId.mPublicKeyAlias->toStringDer(), getTestEd25519Alias()->toStringDer());
  EXPECT_FALSE(ed25519AliasAccountId.mEvmAddressAlias.has_value());

  EXPECT_EQ(ecdsaAliasAccountId.mShardNum, 0ULL);
  EXPECT_EQ(ecdsaAliasAccountId.mRealmNum, 0ULL);
  EXPECT_FALSE(ecdsaAliasAccountId.mAccountNum.has_value());
  EXPECT_NE(ecdsaAliasAccountId.mPublicKeyAlias, nullptr);
  EXPECT_EQ(ecdsaAliasAccountId.mPublicKeyAlias->toStringDer(), getTestEcdsaSecp256k1Alias()->toStringDer());
  EXPECT_FALSE(ecdsaAliasAccountId.mEvmAddressAlias.has_value());
}

//-----
TEST_F(AccountIdUnitTests, ConstructWithEvmAddress)
{
  // Given / When
  const AccountId accountId(getTestEvmAddressAlias());

  // Then
  EXPECT_EQ(accountId.mShardNum, 0ULL);
  EXPECT_EQ(accountId.mRealmNum, 0ULL);
  EXPECT_FALSE(accountId.mAccountNum.has_value());
  EXPECT_EQ(accountId.mPublicKeyAlias, nullptr);
  EXPECT_TRUE(accountId.mEvmAddressAlias.has_value());
  EXPECT_EQ(accountId.mEvmAddressAlias->toString(), getTestEvmAddressAlias().toString());
}

//-----
TEST_F(AccountIdUnitTests, ConstructWithShardRealmAccountNum)
{
  // Given / When
  const AccountId accountId(getTestShardNum(), getTestRealmNum(), getTestAccountNum());

  // Then
  EXPECT_EQ(accountId.mShardNum, getTestShardNum());
  EXPECT_EQ(accountId.mRealmNum, getTestRealmNum());
  EXPECT_TRUE(accountId.mAccountNum.has_value());
  EXPECT_EQ(*accountId.mAccountNum, getTestAccountNum());
  EXPECT_EQ(accountId.mPublicKeyAlias, nullptr);
  EXPECT_FALSE(accountId.mEvmAddressAlias.has_value());
}

//-----
TEST_F(AccountIdUnitTests, ConstructWithShardRealmAccountAlias)
{
  // Given / When
  const AccountId ed25519AliasAccountId(getTestShardNum(), getTestRealmNum(), getTestEd25519Alias());
  const AccountId ecdsaAliasAccountId(getTestShardNum(), getTestRealmNum(), getTestEcdsaSecp256k1Alias());

  // Then
  EXPECT_EQ(ed25519AliasAccountId.mShardNum, getTestShardNum());
  EXPECT_EQ(ed25519AliasAccountId.mRealmNum, getTestRealmNum());
  EXPECT_FALSE(ed25519AliasAccountId.mAccountNum.has_value());
  EXPECT_NE(ed25519AliasAccountId.mPublicKeyAlias, nullptr);
  EXPECT_EQ(ed25519AliasAccountId.mPublicKeyAlias->toStringDer(), getTestEd25519Alias()->toStringDer());
  EXPECT_FALSE(ed25519AliasAccountId.mEvmAddressAlias.has_value());

  EXPECT_EQ(ecdsaAliasAccountId.mShardNum, getTestShardNum());
  EXPECT_EQ(ecdsaAliasAccountId.mRealmNum, getTestRealmNum());
  EXPECT_FALSE(ecdsaAliasAccountId.mAccountNum.has_value());
  EXPECT_NE(ecdsaAliasAccountId.mPublicKeyAlias, nullptr);
  EXPECT_EQ(ecdsaAliasAccountId.mPublicKeyAlias->toStringDer(), getTestEcdsaSecp256k1Alias()->toStringDer());
  EXPECT_FALSE(ecdsaAliasAccountId.mEvmAddressAlias.has_value());
}

//-----
TEST_F(AccountIdUnitTests, ConstructWithShardRealmEvmAddress)
{
  // Given / When
  const AccountId accountId(getTestShardNum(), getTestRealmNum(), getTestEvmAddressAlias());

  // Then
  EXPECT_EQ(accountId.mShardNum, getTestShardNum());
  EXPECT_EQ(accountId.mRealmNum, getTestRealmNum());
  EXPECT_FALSE(accountId.mAccountNum.has_value());
  EXPECT_EQ(accountId.mPublicKeyAlias, nullptr);
  EXPECT_TRUE(accountId.mEvmAddressAlias.has_value());
  EXPECT_EQ(accountId.mEvmAddressAlias->toString(), getTestEvmAddressAlias().toString());
}

//-----
TEST_F(AccountIdUnitTests, CompareAccountIds)
{
  EXPECT_TRUE(AccountId() == AccountId());
  EXPECT_TRUE(AccountId(getTestAccountNum()) == AccountId(getTestAccountNum()));
  EXPECT_TRUE(AccountId(getTestEd25519Alias()) == AccountId(getTestEd25519Alias()));
  EXPECT_TRUE(AccountId(getTestEcdsaSecp256k1Alias()) == AccountId(getTestEcdsaSecp256k1Alias()));
  EXPECT_TRUE(AccountId(getTestEvmAddressAlias()) == AccountId(getTestEvmAddressAlias()));
  EXPECT_TRUE(AccountId(getTestShardNum(), getTestRealmNum(), getTestAccountNum()) ==
              AccountId(getTestShardNum(), getTestRealmNum(), getTestAccountNum()));
  EXPECT_TRUE(AccountId(getTestShardNum(), getTestRealmNum(), getTestEd25519Alias()) ==
              AccountId(getTestShardNum(), getTestRealmNum(), getTestEd25519Alias()));
  EXPECT_TRUE(AccountId(getTestShardNum(), getTestRealmNum(), getTestEcdsaSecp256k1Alias()) ==
              AccountId(getTestShardNum(), getTestRealmNum(), getTestEcdsaSecp256k1Alias()));
  EXPECT_TRUE(AccountId(getTestShardNum(), getTestRealmNum(), getTestEvmAddressAlias()) ==
              AccountId(getTestShardNum(), getTestRealmNum(), getTestEvmAddressAlias()));

  EXPECT_FALSE(AccountId(getTestAccountNum()) == AccountId(getTestAccountNum() - 1ULL));
  EXPECT_FALSE(AccountId(getTestEd25519Alias()) == AccountId(ED25519PrivateKey::generatePrivateKey()->getPublicKey()));
  EXPECT_FALSE(AccountId(getTestEcdsaSecp256k1Alias()) ==
               AccountId(ECDSAsecp256k1PrivateKey::generatePrivateKey()->getPublicKey()));
  EXPECT_FALSE(AccountId(getTestEvmAddressAlias()) ==
               AccountId(EvmAddress::fromString("abcdef1234567890abcdef1234567890abcdef12")));

  EXPECT_FALSE(AccountId(getTestShardNum(), getTestRealmNum(), getTestAccountNum()) ==
               AccountId(getTestShardNum() - 1ULL, getTestRealmNum(), getTestAccountNum()));
  EXPECT_FALSE(AccountId(getTestShardNum(), getTestRealmNum(), getTestAccountNum()) ==
               AccountId(getTestShardNum(), getTestRealmNum() - 1ULL, getTestAccountNum()));

  EXPECT_FALSE(AccountId(getTestShardNum(), getTestRealmNum(), getTestAccountNum()) ==
               AccountId(getTestShardNum(), getTestRealmNum(), getTestEd25519Alias()));
  EXPECT_FALSE(AccountId(getTestShardNum(), getTestRealmNum(), getTestAccountNum()) ==
               AccountId(getTestShardNum(), getTestRealmNum(), getTestEcdsaSecp256k1Alias()));
  EXPECT_FALSE(AccountId(getTestShardNum(), getTestRealmNum(), getTestAccountNum()) ==
               AccountId(getTestShardNum(), getTestRealmNum(), getTestEvmAddressAlias()));
  EXPECT_FALSE(AccountId(getTestShardNum(), getTestRealmNum(), getTestEd25519Alias()) ==
               AccountId(getTestShardNum(), getTestRealmNum(), getTestEcdsaSecp256k1Alias()));
  EXPECT_FALSE(AccountId(getTestShardNum(), getTestRealmNum(), getTestEd25519Alias()) ==
               AccountId(getTestShardNum(), getTestRealmNum(), getTestEvmAddressAlias()));
  EXPECT_FALSE(AccountId(getTestShardNum(), getTestRealmNum(), getTestEcdsaSecp256k1Alias()) ==
               AccountId(getTestShardNum(), getTestRealmNum(), getTestEvmAddressAlias()));
}

//-----
TEST_F(AccountIdUnitTests, ConstructFromString)
{
  const std::string testShardNumStr = std::to_string(getTestShardNum());
  const std::string testRealmNumStr = std::to_string(getTestRealmNum());
  const std::string testAccountNumStr = std::to_string(getTestAccountNum());

  AccountId accountId;
  EXPECT_NO_THROW(accountId = AccountId::fromString(testShardNumStr + '.' + testRealmNumStr + '.' + testAccountNumStr));
  EXPECT_EQ(accountId.mShardNum, getTestShardNum());
  EXPECT_EQ(accountId.mRealmNum, getTestRealmNum());
  EXPECT_TRUE(accountId.mAccountNum.has_value());
  EXPECT_EQ(*accountId.mAccountNum, getTestAccountNum());

  EXPECT_THROW(accountId = AccountId::fromString(testShardNumStr + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString('.' + testShardNumStr + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString(testShardNumStr + '.' + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString(testShardNumStr + testRealmNumStr + '.' + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString(testShardNumStr + testRealmNumStr + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString(".." + testShardNumStr + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString('.' + testShardNumStr + testRealmNumStr + '.' + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString('.' + testShardNumStr + testRealmNumStr + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString(testShardNumStr + ".." + testRealmNumStr + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString(testShardNumStr + '.' + testRealmNumStr + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString(testShardNumStr + testRealmNumStr + ".." + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString(testShardNumStr + testRealmNumStr + '.' + testAccountNumStr + '.'),
               std::invalid_argument);
  EXPECT_THROW(accountId =
                 AccountId::fromString('.' + testShardNumStr + '.' + testRealmNumStr + '.' + testAccountNumStr + '.'),
               std::invalid_argument);

  EXPECT_THROW(accountId = AccountId::fromString("abc"), std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString("o.o.e"), std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString("0.0.1!"), std::invalid_argument);

  const std::string ed25519AliasStr = getTestEd25519Alias()->toStringDer();
  EXPECT_NO_THROW(accountId = AccountId::fromString(testShardNumStr + '.' + testRealmNumStr + '.' + ed25519AliasStr));
  EXPECT_EQ(accountId.mShardNum, getTestShardNum());
  EXPECT_EQ(accountId.mRealmNum, getTestRealmNum());
  EXPECT_NE(accountId.mPublicKeyAlias, nullptr);
  EXPECT_EQ(accountId.mPublicKeyAlias->toStringDer(), ed25519AliasStr);

  EXPECT_THROW(accountId = AccountId::fromString(ed25519AliasStr + '.' + testRealmNumStr + '.' + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString(testShardNumStr + '.' + ed25519AliasStr + '.' + testAccountNumStr),
               std::invalid_argument);

  const std::string ecdsaAliasStr = getTestEcdsaSecp256k1Alias()->toStringDer();
  EXPECT_NO_THROW(accountId = AccountId::fromString(testShardNumStr + '.' + testRealmNumStr + '.' + ecdsaAliasStr));
  EXPECT_EQ(accountId.mShardNum, getTestShardNum());
  EXPECT_EQ(accountId.mRealmNum, getTestRealmNum());
  EXPECT_NE(accountId.mPublicKeyAlias, nullptr);
  EXPECT_EQ(accountId.mPublicKeyAlias->toStringDer(), ecdsaAliasStr);

  EXPECT_THROW(accountId = AccountId::fromString(ecdsaAliasStr + '.' + testRealmNumStr + '.' + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString(testShardNumStr + '.' + ecdsaAliasStr + '.' + testAccountNumStr),
               std::invalid_argument);

  const std::string evmAddressStr = getTestEvmAddressAlias().toString();
  EXPECT_NO_THROW(accountId = AccountId::fromString(testShardNumStr + '.' + testRealmNumStr + '.' + evmAddressStr));
  EXPECT_EQ(accountId.mShardNum, getTestShardNum());
  EXPECT_EQ(accountId.mRealmNum, getTestRealmNum());
  EXPECT_TRUE(accountId.mEvmAddressAlias);
  EXPECT_EQ(accountId.mEvmAddressAlias->toString(), evmAddressStr);

  EXPECT_THROW(accountId = AccountId::fromString(evmAddressStr + '.' + testRealmNumStr + '.' + testAccountNumStr),
               std::invalid_argument);
  EXPECT_THROW(accountId = AccountId::fromString(testShardNumStr + '.' + evmAddressStr + '.' + testAccountNumStr),
               std::invalid_argument);
}

//-----
TEST_F(AccountIdUnitTests, FromEvmAddress)
{
  // Given / When
  const AccountId accountIdFromEvmAddress =
    AccountId::fromEvmAddress(getTestEvmAddressAlias(), getTestShardNum(), getTestRealmNum());
  const AccountId accountIdFromEvmAddressStr =
    AccountId::fromEvmAddress(getTestEvmAddressAlias().toString(), getTestShardNum(), getTestRealmNum());

  // Then
  EXPECT_EQ(accountIdFromEvmAddress.mShardNum, getTestShardNum());
  EXPECT_EQ(accountIdFromEvmAddress.mRealmNum, getTestRealmNum());
  EXPECT_FALSE(accountIdFromEvmAddress.mAccountNum.has_value());
  EXPECT_EQ(accountIdFromEvmAddress.mPublicKeyAlias, nullptr);
  ASSERT_TRUE(accountIdFromEvmAddress.mEvmAddressAlias.has_value());
  EXPECT_EQ(accountIdFromEvmAddress.mEvmAddressAlias->toBytes(), getTestEvmAddressAlias().toBytes());

  EXPECT_EQ(accountIdFromEvmAddress.mShardNum, accountIdFromEvmAddressStr.mShardNum);
  EXPECT_EQ(accountIdFromEvmAddress.mRealmNum, accountIdFromEvmAddressStr.mRealmNum);
  EXPECT_EQ(accountIdFromEvmAddress.mAccountNum, accountIdFromEvmAddressStr.mAccountNum);
  EXPECT_EQ(accountIdFromEvmAddress.mPublicKeyAlias, accountIdFromEvmAddressStr.mPublicKeyAlias);
  EXPECT_EQ(accountIdFromEvmAddress.mEvmAddressAlias->toBytes(),
            accountIdFromEvmAddressStr.mEvmAddressAlias->toBytes());
}

//-----
TEST_F(AccountIdUnitTests, ProtobufAccountId)
{
  AccountId accountId;
  accountId.mShardNum = getTestShardNum();
  accountId.mRealmNum = getTestRealmNum();
  accountId.mAccountNum = getTestAccountNum();

  // Serialize shard, realm, account number
  std::unique_ptr<proto::AccountID> protoAccountId = accountId.toProtobuf();
  EXPECT_EQ(protoAccountId->shardnum(), getTestShardNum());
  EXPECT_EQ(protoAccountId->realmnum(), getTestRealmNum());
  EXPECT_EQ(protoAccountId->account_case(), proto::AccountID::AccountCase::kAccountNum);
  EXPECT_EQ(protoAccountId->accountnum(), getTestAccountNum());

  // Adjust protobuf fields
  const uint64_t adjustment = 3ULL;
  const uint64_t newShard = getTestShardNum() + adjustment;
  const uint64_t newRealm = getTestRealmNum() - adjustment;
  const uint64_t newAccount = getTestAccountNum() * adjustment;

  protoAccountId->set_shardnum(static_cast<int64_t>(newShard));
  protoAccountId->set_realmnum(static_cast<int64_t>(newRealm));
  protoAccountId->set_accountnum(static_cast<int64_t>(newAccount));

  // Deserialize shard, realm, account number
  accountId = AccountId::fromProtobuf(*protoAccountId);
  EXPECT_EQ(accountId.mShardNum, newShard);
  EXPECT_EQ(accountId.mRealmNum, newRealm);
  EXPECT_TRUE(accountId.mAccountNum.has_value());
  EXPECT_EQ(*accountId.mAccountNum, newAccount);

  // Serialize ED25519 alias
  accountId.mAccountNum.reset();
  accountId.mPublicKeyAlias = getTestEd25519Alias();
  protoAccountId = accountId.toProtobuf();
  EXPECT_EQ(protoAccountId->account_case(), proto::AccountID::AccountCase::kAlias);

  // Adjust protobuf fields
  std::unique_ptr<PrivateKey> key = ED25519PrivateKey::generatePrivateKey();
  std::vector<std::byte> testBytes =
    internal::Utilities::stringToByteVector(key->getPublicKey()->toProtobufKey()->SerializeAsString());
  protoAccountId->set_alias(internal::Utilities::byteVectorToString(testBytes));

  // Deserialize ED25519 alias
  accountId = AccountId::fromProtobuf(*protoAccountId);
  EXPECT_NE(accountId.mPublicKeyAlias, nullptr);
  EXPECT_EQ(accountId.mPublicKeyAlias->toBytesDer(), key->getPublicKey()->toBytesDer());

  // Serialize ECDSA alias
  accountId.mPublicKeyAlias = getTestEcdsaSecp256k1Alias();
  protoAccountId = accountId.toProtobuf();
  EXPECT_EQ(protoAccountId->account_case(), proto::AccountID::AccountCase::kAlias);

  // Adjust protobuf fields
  key = ECDSAsecp256k1PrivateKey::generatePrivateKey();
  testBytes = internal::Utilities::stringToByteVector(key->getPublicKey()->toProtobufKey()->SerializeAsString());
  protoAccountId->set_alias(internal::Utilities::byteVectorToString(testBytes));

  // Deserialize ECDSA alias
  accountId = AccountId::fromProtobuf(*protoAccountId);
  EXPECT_NE(accountId.mPublicKeyAlias, nullptr);
  EXPECT_EQ(accountId.mPublicKeyAlias->toBytesDer(), key->getPublicKey()->toBytesDer());

  // Serialize EVM address
  accountId.mPublicKeyAlias = nullptr;
  accountId.mEvmAddressAlias = getTestEvmAddressAlias();
  protoAccountId = accountId.toProtobuf();
  EXPECT_EQ(protoAccountId->account_case(), proto::AccountID::AccountCase::kAlias);

  // Adjust protobuf fields
  testBytes = { std::byte('0'), std::byte('1'), std::byte('2'), std::byte('3'), std::byte('4'),
                std::byte('5'), std::byte('6'), std::byte('7'), std::byte('8'), std::byte('9'),
                std::byte('a'), std::byte('b'), std::byte('c'), std::byte('d'), std::byte('e'),
                std::byte('f'), std::byte('g'), std::byte('h'), std::byte('i'), std::byte('j') };
  protoAccountId->set_allocated_alias(new std::string(internal::Utilities::byteVectorToString(testBytes)));

  // Deserialize EVM address
  accountId = AccountId::fromProtobuf(*protoAccountId);
  EXPECT_TRUE(accountId.mEvmAddressAlias.has_value());
  EXPECT_EQ(accountId.mEvmAddressAlias->toBytes(), testBytes);
}

//-----
TEST_F(AccountIdUnitTests, ToString)
{
  AccountId accountId;
  EXPECT_EQ(accountId.toString(), "0.0.0");

  accountId.mShardNum = getTestShardNum();
  accountId.mRealmNum = getTestRealmNum();
  accountId.mAccountNum = getTestAccountNum();
  EXPECT_EQ(accountId.toString(),
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              std::to_string(getTestAccountNum()));

  accountId.mAccountNum.reset();
  accountId.mPublicKeyAlias = getTestEd25519Alias();
  EXPECT_EQ(accountId.toString(),
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              getTestEd25519Alias()->toStringDer());

  accountId.mPublicKeyAlias = getTestEcdsaSecp256k1Alias();
  EXPECT_EQ(accountId.toString(),
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              getTestEcdsaSecp256k1Alias()->toStringDer());

  accountId.mPublicKeyAlias = nullptr;
  accountId.mEvmAddressAlias = getTestEvmAddressAlias();
  EXPECT_EQ(accountId.toString(),
            std::to_string(getTestShardNum()) + '.' + std::to_string(getTestRealmNum()) + '.' +
              getTestEvmAddressAlias().toString());
}