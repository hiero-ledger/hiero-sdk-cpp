// SPDX-License-Identifier: Apache-2.0
#include "AccountCreateTransaction.h"
#include "AccountDeleteTransaction.h"
#include "BaseIntegrationTest.h"
#include "ContractCreateTransaction.h"
#include "ContractId.h"
#include "ECDSAsecp256k1PrivateKey.h"
#include "ED25519PrivateKey.h"
#include "Hbar.h"
#include "NftId.h"
#include "Status.h"
#include "TokenAssociateTransaction.h"
#include "TokenCreateTransaction.h"
#include "TokenId.h"
#include "TokenMintTransaction.h"
#include "TokenType.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"
#include "TransactionRecord.h"
#include "TransactionResponse.h"
#include "TransferTransaction.h"
#include "exceptions/PrecheckStatusException.h"
#include "exceptions/ReceiptStatusException.h"
#include "hooks/EvmHookCall.h"
#include "hooks/EvmHookSpec.h"
#include "hooks/FungibleHookCall.h"
#include "hooks/FungibleHookType.h"
#include "hooks/HookCreationDetails.h"
#include "hooks/HookExtensionPoint.h"
#include "hooks/HookId.h"
#include "hooks/EvmHook.h"
#include "hooks/NftHookCall.h"
#include "hooks/NftHookType.h"
#include "impl/HexConverter.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace Hiero;

class TransferTransactionHooksIntegrationTests : public BaseIntegrationTest
{
protected:
  ContractId createHookContract(const std::string& hexBytecode, const Client& client)
  {
    return ContractCreateTransaction()
      .setBytecode(internal::HexConverter::hexToBytes(hexBytecode))
      .setGas(1000000ULL)
      .execute(client)
      .getReceipt(client)
      .mContractId.value();
  }

  AccountId createAccountWithHook(const std::shared_ptr<PrivateKey>& key,
                                  const ContractId& contractId,
                                  int64_t hookId,
                                  const Client& client)
  {
    EvmHook evmHook;
    evmHook.setEvmHookSpec(EvmHookSpec().setContractId(contractId));

    HookCreationDetails hookDetails;
    hookDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK)
      .setHookId(hookId)
      .setEvmHook(evmHook);

    return AccountCreateTransaction()
      .setKeyWithoutAlias(key->getPublicKey())
      .setInitialBalance(Hbar(2LL))
      .addHook(hookDetails)
      .freezeWith(&client)
      .sign(key)
      .execute(client)
      .getReceipt(client)
      .mAccountId.value();
  }

  [[nodiscard]] inline const Hbar& getTestHbarAmount() const { return mHbarAmount; }
  [[nodiscard]] inline int64_t getTestHookId1() const { return mHookId1; }
  [[nodiscard]] inline int64_t getTestHookId2() const { return mHookId2; }
  [[nodiscard]] inline const std::string& getTestHookBytecode() const { return mHookBytecode; }

private:
  const std::string mHookBytecode =
    "6080604052348015600e575f5ffd5b506107d18061001c5f395ff3fe608060405260043610610033575f3560e01c8063124d8b301461003757"
    "806394112e2f14610067578063bd0dd0b614610097575b5f5ffd5b610051600480360381019061004c91906106f2565b6100c7565b60405161"
    "005e9190610782565b60405180910390f35b610081600480360381019061007c91906106f2565b6100d2565b60405161008e9190610782565b"
    "60405180910390f35b6100b160048036038101906100ac91906106f2565b6100dd565b6040516100be9190610782565b60405180910390f35b"
    "5f6001905092915050565b5f6001905092915050565b5f6001905092915050565b5f604051905090565b5f5ffd5b5f5ffd5b5f5ffd5b5f60a0"
    "8284031215610112576101116100f9565b5b81905092915050565b5f5ffd5b5f601f19601f8301169050919050565b7f4e487b710000000000"
    "00000000000000000000000000000000000000000000005f52604160045260245ffd5b6101658261011f565b810181811067ffffffffffffff"
    "ff821117156101845761018361012f565b5b80604052505050565b5f6101966100e8565b90506101a2828261015c565b919050565b5f5ffd5b"
    "5f5ffd5b5f67ffffffffffffffff8211156101c9576101c861012f565b5b602082029050602081019050919050565b5f5ffd5b5f73ffffffff"
    "ffffffffffffffffffffffffffffffff82169050919050565b5f610207826101de565b9050919050565b610217816101fd565b811461022157"
    "5f5ffd5b50565b5f813590506102328161020e565b92915050565b5f8160070b9050919050565b61024d81610238565b8114610257575f5ffd"
    "5b50565b5f8135905061026881610244565b92915050565b5f604082840312156102835761028261011b565b5b61028d604061018d565b9050"
    "5f61029c84828501610224565b5f8301525060206102af8482850161025a565b60208301525092915050565b5f6102cd6102c8846101af565b"
    "61018d565b905080838252602082019050604084028301858111156102f0576102ef6101da565b5b835b818110156103195780610305888261"
    "026e565b8452602084019350506040810190506102f2565b5050509392505050565b5f82601f830112610337576103366101ab565b5b813561"
    "03478482602086016102bb565b91505092915050565b5f67ffffffffffffffff82111561036a5761036961012f565b5b602082029050602081"
    "019050919050565b5f67ffffffffffffffff8211156103955761039461012f565b5b602082029050602081019050919050565b5f6060828403"
    "12156103bb576103ba61011b565b5b6103c5606061018d565b90505f6103d484828501610224565b5f8301525060206103e784828501610224"
    "565b60208301525060406103fb8482850161025a565b60408301525092915050565b5f6104196104148461037b565b61018d565b9050808382"
    "526020820190506060840283018581111561043c5761043b6101da565b5b835b81811015610465578061045188826103a6565b845260208401"
    "93505060608101905061043e565b5050509392505050565b5f82601f830112610483576104826101ab565b5b81356104938482602086016104"
    "07565b91505092915050565b5f606082840312156104b1576104b061011b565b5b6104bb606061018d565b90505f6104ca8482850161022456"
    "5b5f83015250602082013567ffffffffffffffff8111156104ed576104ec6101a7565b5b6104f984828501610323565b602083015250604082"
    "013567ffffffffffffffff81111561051d5761051c6101a7565b5b6105298482850161046f565b60408301525092915050565b5f6105476105"
    "4284610350565b61018d565b9050808382526020820190506020840283018581111561056a576105696101da565b5b835b818110156105b157"
    "803567ffffffffffffffff81111561058f5761058e6101ab565b5b80860161059c898261049c565b8552602085019450505060208101905061"
    "056c565b5050509392505050565b5f82601f8301126105cf576105ce6101ab565b5b81356105df848260208601610535565b91505092915050"
    "565b5f604082840312156105fd576105fc61011b565b5b610607604061018d565b90505f82013567ffffffffffffffff811115610626576106"
    "256101a7565b5b61063284828501610323565b5f83015250602082013567ffffffffffffffff811115610655576106546101a7565b5b610661"
    "848285016105bb565b60208301525092915050565b5f604082840312156106825761068161011b565b5b61068c604061018d565b90505f8201"
    "3567ffffffffffffffff8111156106ab576106aa6101a7565b5b6106b7848285016105e8565b5f83015250602082013567ffffffffffffffff"
    "8111156106da576106d96101a7565b5b6106e6848285016105e8565b60208301525092915050565b5f5f604083850312156107085761070761"
    "00f1565b5b5f83013567ffffffffffffffff811115610725576107246100f5565b5b610731858286016100fd565b925050602083013567ffff"
    "ffffffffffff811115610752576107516100f5565b5b61075e8582860161066d565b9150509250929050565b5f8115159050919050565b6107"
    "7c81610768565b82525050565b5f6020820190506107955f830184610773565b9291505056fea26469706673582212207dfe7723f6d6869419"
    "b1cb0619758b439da0cf4ffd9520997c40a3946299d4dc64736f6c634300081e0033";
  const Hbar mHbarAmount = Hbar(1LL);
  const int64_t mHookId1 = 1LL;
  const int64_t mHookId2 = 2LL;
};

//-----
TEST_F(TransferTransactionHooksIntegrationTests, ShouldTransferHbarToAccountWithPreTransactionAllowanceHook)
{
  // Given
  const std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  const AccountId operatorId = AccountId(2ULL); // Operator account ID

  // Create account with a pre allowance hook on receiving side
  ContractId hookContractId = createHookContract(getTestHookBytecode(), getTestClient());

  EvmHook evmHook;
  evmHook.setEvmHookSpec(EvmHookSpec().setContractId(hookContractId));

  HookCreationDetails hookDetails;
  hookDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK)
    .setHookId(getTestHookId2())
    .setEvmHook(evmHook);

  AccountId receiverId = AccountCreateTransaction()
                           .setKeyWithoutAlias(receiverKey->getPublicKey())
                           .setInitialBalance(Hbar(1LL))
                           .addHook(hookDetails)
                           .freezeWith(&getTestClient())
                           .sign(receiverKey)
                           .execute(getTestClient())
                           .getReceipt(getTestClient())
                           .mAccountId.value();

  // Create hook call for receiver
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId2());
  hookCall.setEvmHookCall(EvmHookCall().setGasLimit(25000));

  // When - Operator sends TO the account with hook (receiver hook)
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = TransferTransaction()
                                 .addHbarTransfer(operatorId, getTestHbarAmount().negated())
                                 .addHbarTransferWithHook(receiverId, getTestHbarAmount(), hookCall)
                                 .execute(getTestClient()));

  // Then - Verify the transaction succeeded
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(receiverId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(receiverKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(TransferTransactionHooksIntegrationTests, ShouldExecuteHooksOnMultipleAccountsInSameTransfer)
{
  // Given
  const std::shared_ptr<PrivateKey> receiverKey1 = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> receiverKey2 = ED25519PrivateKey::generatePrivateKey();
  const AccountId operatorId = AccountId(2ULL); // Operator account ID

  // Create hook contract
  ContractId hookContractId = createHookContract(getTestHookBytecode(), getTestClient());

  // Create first account with hook
  EvmHook evmHook1;
  evmHook1.setEvmHookSpec(EvmHookSpec().setContractId(hookContractId));

  HookCreationDetails hookDetails1;
  hookDetails1.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK)
    .setHookId(getTestHookId2())
    .setEvmHook(evmHook1);

  AccountId receiverId1 = AccountCreateTransaction()
                            .setKeyWithoutAlias(receiverKey1->getPublicKey())
                            .setInitialBalance(Hbar(1LL))
                            .addHook(hookDetails1)
                            .freezeWith(&getTestClient())
                            .sign(receiverKey1)
                            .execute(getTestClient())
                            .getReceipt(getTestClient())
                            .mAccountId.value();

  // Create second account with hook
  EvmHook evmHook2;
  evmHook2.setEvmHookSpec(EvmHookSpec().setContractId(hookContractId));

  HookCreationDetails hookDetails2;
  hookDetails2.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK)
    .setHookId(getTestHookId2())
    .setEvmHook(evmHook2);

  AccountId receiverId2 = AccountCreateTransaction()
                            .setKeyWithoutAlias(receiverKey2->getPublicKey())
                            .setInitialBalance(Hbar(1LL))
                            .addHook(hookDetails2)
                            .freezeWith(&getTestClient())
                            .sign(receiverKey2)
                            .execute(getTestClient())
                            .getReceipt(getTestClient())
                            .mAccountId.value();

  // Create hook calls for both receivers
  FungibleHookCall hookCall1;
  hookCall1.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall1.setHookId(getTestHookId2());
  hookCall1.setEvmHookCall(EvmHookCall().setGasLimit(25000));

  FungibleHookCall hookCall2;
  hookCall2.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall2.setHookId(getTestHookId2());
  hookCall2.setEvmHookCall(EvmHookCall().setGasLimit(25000));

  // When - Transfer to both accounts - both hooks must approve
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = TransferTransaction()
                                 .addHbarTransfer(operatorId, Hbar(2LL).negated())
                                 .addHbarTransferWithHook(receiverId1, getTestHbarAmount(), hookCall1)
                                 .addHbarTransferWithHook(receiverId2, getTestHbarAmount(), hookCall2)
                                 .execute(getTestClient()));

  // Then - Verify the transaction succeeded
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(receiverId1)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(receiverKey1)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(receiverId2)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(receiverKey2)
                    .execute(getTestClient()));
}

//-----
TEST_F(TransferTransactionHooksIntegrationTests, DISABLED_ShouldTransferFungibleTokensWithAllowanceHook)
{
  // Given
  const std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();
  const AccountId operatorId = AccountId(2ULL); // Operator account ID

  ContractId hookContractId = createHookContract(getTestHookBytecode(), getTestClient());

  EvmHook evmHook;
  evmHook.setEvmHookSpec(EvmHookSpec().setContractId(hookContractId));

  HookCreationDetails hookDetails;
  hookDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK)
    .setHookId(getTestHookId1())
    .setEvmHook(evmHook);

  AccountId receiverId = AccountCreateTransaction()
                           .setKeyWithoutAlias(receiverKey->getPublicKey())
                           .setInitialBalance(Hbar(2LL))
                           .addHook(hookDetails)
                           .freezeWith(&getTestClient())
                           .sign(receiverKey)
                           .execute(getTestClient())
                           .getReceipt(getTestClient())
                           .mAccountId.value();

  // Create token
  TokenId tokenId = TokenCreateTransaction()
                      .setTokenName("FT-HOOK")
                      .setTokenSymbol("FTH")
                      .setDecimals(2)
                      .setInitialSupply(10000)
                      .setTreasuryAccountId(operatorId)
                      .setAdminKey(getTestClient().getOperatorPublicKey())
                      .setSupplyKey(getTestClient().getOperatorPublicKey())
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mTokenId.value();

  // Associate token with receiver account
  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(receiverId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(receiverKey)
                    .execute(getTestClient()));

  // Create hook call for receiver
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId1());
  hookCall.setEvmHookCall(EvmHookCall().setGasLimit(25000));

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = TransferTransaction()
                                 .addTokenTransfer(tokenId, operatorId, -1000)
                                 .addTokenTransferWithHook(tokenId, receiverId, 1000, hookCall)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(receiverId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(receiverKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(TransferTransactionHooksIntegrationTests, ShouldTransferNftWithSenderAndReceiverAllowanceHooks)
{
  // Given
  const std::shared_ptr<PrivateKey> senderKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> receiverKey = ED25519PrivateKey::generatePrivateKey();

  // Create hook contract
  ContractId hookContractId = createHookContract(getTestHookBytecode(), getTestClient());

  // Create sender account with hook
  EvmHook senderHook;
  senderHook.setEvmHookSpec(EvmHookSpec().setContractId(hookContractId));

  HookCreationDetails senderHookDetails;
  senderHookDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK)
    .setHookId(getTestHookId2())
    .setEvmHook(senderHook);

  AccountId senderId = AccountCreateTransaction()
                         .setKeyWithoutAlias(senderKey->getPublicKey())
                         .setInitialBalance(Hbar(2LL))
                         .addHook(senderHookDetails)
                         .freezeWith(&getTestClient())
                         .sign(senderKey)
                         .execute(getTestClient())
                         .getReceipt(getTestClient())
                         .mAccountId.value();

  // Create receiver account with hook
  EvmHook receiverHook;
  receiverHook.setEvmHookSpec(EvmHookSpec().setContractId(hookContractId));

  HookCreationDetails receiverHookDetails;
  receiverHookDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK)
    .setHookId(getTestHookId2())
    .setEvmHook(receiverHook);

  AccountId receiverId = AccountCreateTransaction()
                           .setKeyWithoutAlias(receiverKey->getPublicKey())
                           .setInitialBalance(Hbar(2LL))
                           .addHook(receiverHookDetails)
                           .freezeWith(&getTestClient())
                           .sign(receiverKey)
                           .execute(getTestClient())
                           .getReceipt(getTestClient())
                           .mAccountId.value();

  // Create NFT token
  TokenId tokenId = TokenCreateTransaction()
                      .setTokenName("NFT-HOOK")
                      .setTokenSymbol("NHK")
                      .setTokenType(TokenType::NON_FUNGIBLE_UNIQUE)
                      .setInitialSupply(0)
                      .setTreasuryAccountId(senderId)
                      .setAdminKey(senderKey->getPublicKey())
                      .setSupplyKey(senderKey->getPublicKey())
                      .freezeWith(&getTestClient())
                      .sign(senderKey)
                      .execute(getTestClient())
                      .getReceipt(getTestClient())
                      .mTokenId.value();

  // Mint NFT
  ASSERT_NO_THROW(TokenMintTransaction()
                    .setTokenId(tokenId)
                    .setMetadata({ std::vector<std::byte>{ std::byte(1) } })
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient())
                    .getReceipt(getTestClient()));

  // Associate receiver with token
  ASSERT_NO_THROW(TokenAssociateTransaction()
                    .setAccountId(receiverId)
                    .setTokenIds({ tokenId })
                    .freezeWith(&getTestClient())
                    .sign(receiverKey)
                    .execute(getTestClient()));

  // Create hook calls for sender and receiver
  NftHookCall senderCall;
  senderCall.setHookType(NftHookType::PRE_HOOK);
  senderCall.setHookId(getTestHookId2());
  senderCall.setEvmHookCall(EvmHookCall().setGasLimit(25000));

  NftHookCall receiverCall;
  receiverCall.setHookType(NftHookType::PRE_HOOK);
  receiverCall.setHookId(getTestHookId2());
  receiverCall.setEvmHookCall(EvmHookCall().setGasLimit(25000));

  NftId nftId(tokenId, 1);

  // When - Transfer NFT with both sender and receiver hooks
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = TransferTransaction()
                                 .addNftTransferWithHook(nftId, senderId, receiverId, senderCall, receiverCall)
                                 .freezeWith(&getTestClient())
                                 .sign(senderKey)
                                 .execute(getTestClient()));

  // Then - Verify the transaction succeeded
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(senderId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(receiverId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(receiverKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(TransferTransactionHooksIntegrationTests, DISABLED_ShouldCallPreTransactionAllowanceHookAndApproveTransfer)
{
  // Given
  const std::shared_ptr<PrivateKey> senderKey = ED25519PrivateKey::generatePrivateKey();
  const AccountId operatorId = AccountId(2ULL); // Operator account ID

  ContractId hookContractId = createHookContract(getTestHookBytecode(), getTestClient());

  EvmHook evmHook;
  evmHook.setEvmHookSpec(EvmHookSpec().setContractId(hookContractId));

  HookCreationDetails hookDetails;
  hookDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK)
    .setHookId(getTestHookId1())
    .setEvmHook(evmHook);

  AccountId senderId = AccountCreateTransaction()
                         .setKeyWithoutAlias(senderKey->getPublicKey())
                         .setInitialBalance(Hbar(2LL))
                         .addHook(hookDetails)
                         .freezeWith(&getTestClient())
                         .sign(senderKey)
                         .execute(getTestClient())
                         .getReceipt(getTestClient())
                         .mAccountId.value();

  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId1());
  hookCall.setEvmHookCall(EvmHookCall().setGasLimit(100000));

  // When
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = TransferTransaction()
                                 .addHbarTransferWithHook(senderId, getTestHbarAmount().negated(), hookCall)
                                 .addHbarTransfer(operatorId, getTestHbarAmount())
                                 .freezeWith(&getTestClient())
                                 .sign(senderKey)
                                 .execute(getTestClient()));

  // Then
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(senderId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(TransferTransactionHooksIntegrationTests, ShouldCallPreAndPostAllowanceHooksAroundSuccessfulTransfer)
{
  // Given
  const std::shared_ptr<PrivateKey> senderKey = ED25519PrivateKey::generatePrivateKey();
  const AccountId operatorId = AccountId(2ULL); // Operator account ID

  // Create account with a pre/post allowance hook (hookId = 2)
  ContractId hookContractId = createHookContract(getTestHookBytecode(), getTestClient());

  EvmHook evmHook;
  evmHook.setEvmHookSpec(EvmHookSpec().setContractId(hookContractId));

  HookCreationDetails hookDetails;
  hookDetails.setExtensionPoint(HookExtensionPoint::ACCOUNT_ALLOWANCE_HOOK)
    .setHookId(getTestHookId2())
    .setEvmHook(evmHook);

  AccountId senderId = AccountCreateTransaction()
                         .setKeyWithoutAlias(senderKey->getPublicKey())
                         .setInitialBalance(Hbar(2LL))
                         .addHook(hookDetails)
                         .freezeWith(&getTestClient())
                         .sign(senderKey)
                         .execute(getTestClient())
                         .getReceipt(getTestClient())
                         .mAccountId.value();

  // Create hook call with medium gas limit
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId2());
  hookCall.setEvmHookCall(EvmHookCall().setGasLimit(500000));

  // When - Execute transfer with multiple hook calls in single transaction
  TransactionResponse txResponse;
  EXPECT_NO_THROW(txResponse = TransferTransaction()
                                 .addHbarTransferWithHook(senderId, getTestHbarAmount().negated(), hookCall)
                                 .addHbarTransfer(operatorId, getTestHbarAmount())
                                 .addHbarTransferWithHook(senderId, getTestHbarAmount().negated(), hookCall)
                                 .addHbarTransfer(operatorId, getTestHbarAmount())
                                 .freezeWith(&getTestClient())
                                 .sign(senderKey)
                                 .execute(getTestClient()));

  // Then - Verify the transaction succeeded
  TransactionReceipt txReceipt;
  ASSERT_NO_THROW(txReceipt = txResponse.getReceipt(getTestClient()));
  EXPECT_EQ(txReceipt.mStatus, Status::SUCCESS);

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(senderId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(TransferTransactionHooksIntegrationTests, ShouldFailWhenHookIdDoesNotExistOnAccount)
{
  // Given
  const std::shared_ptr<PrivateKey> senderKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> receiverKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

  // Create hook contract with bytecode (simplified version)

  ContractId hookContractId = createHookContract(getTestHookBytecode(), getTestClient());
  AccountId senderId = createAccountWithHook(senderKey, hookContractId, getTestHookId1(), getTestClient());
  AccountId receiverId = AccountCreateTransaction()
                           .setKeyWithoutAlias(receiverKey->getPublicKey())
                           .setInitialBalance(Hbar(10LL))
                           .execute(getTestClient())
                           .getReceipt(getTestClient())
                           .mAccountId.value();

  // Create hook call with non-existent hook ID
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(99LL); // Non-existent hook ID
  hookCall.setEvmHookCall(EvmHookCall().setGasLimit(25000));

  // When/Then - Should fail because hook ID doesn't exist
  EXPECT_THROW(TransferTransaction()
                 .addHbarTransferWithHook(senderId, getTestHbarAmount().negated(), hookCall)
                 .addHbarTransfer(receiverId, getTestHbarAmount())
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // HOOK_NOT_FOUND

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(senderId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(receiverId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(receiverKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(TransferTransactionHooksIntegrationTests, ShouldFailWithInsufficientGasForHookExecution)
{
  // Given
  const std::shared_ptr<PrivateKey> senderKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> receiverKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

  // Create hook contract with bytecode (simplified version)
  ContractId hookContractId = createHookContract(getTestHookBytecode(), getTestClient());
  AccountId senderId = createAccountWithHook(senderKey, hookContractId, getTestHookId1(), getTestClient());
  AccountId receiverId = AccountCreateTransaction()
                           .setKeyWithoutAlias(receiverKey->getPublicKey())
                           .setInitialBalance(Hbar(10LL))
                           .execute(getTestClient())
                           .getReceipt(getTestClient())
                           .mAccountId.value();

  // Create hook call with insufficient gas
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId1());
  hookCall.setEvmHookCall(EvmHookCall().setGasLimit(0)); // Insufficient gas

  // When/Then - Should fail due to insufficient gas
  EXPECT_THROW(TransferTransaction()
                 .addHbarTransferWithHook(senderId, getTestHbarAmount().negated(), hookCall)
                 .addHbarTransfer(receiverId, getTestHbarAmount())
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // INSUFFICIENT_GAS

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(senderId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(receiverId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(receiverKey)
                    .execute(getTestClient()));
}

//-----
TEST_F(TransferTransactionHooksIntegrationTests, ShouldNotTransferWhenHookReturnsFalse)
{
  // Given
  const std::shared_ptr<PrivateKey> senderKey = ED25519PrivateKey::generatePrivateKey();
  const std::shared_ptr<PrivateKey> receiverKey = ECDSAsecp256k1PrivateKey::generatePrivateKey();

  // Create hook contract with bytecode (simplified version that returns false)
  const std::string bytecode =
    "6080604052348015600e575f5ffd5b506107d18061001c5f395ff3fe608060405260043610610033575f3560e01c8063124d8b301461003757"
    "806394112e2f14610067578063bd0dd0b614610097575b5f5ffd5b610051600480360381019061004c91906106f2565b6100c7565b60405161"
    "005e9190610782565b60405180910390f35b610081600480360381019061007c91906106f2565b6100d2565b60405161008e9190610782565b"
    "60405180910390f35b6100b160048036038101906100ac91906106f2565b6100dd565b6040516100be9190610782565b60405180910390f35b"
    "5f6001905092915050565b5f6001905092915050565b5f6001905092915050565b5f604051905090565b5f5ffd5b5f5ffd5b5f5ffd5b5f60a0"
    "8284031215610112576101116100f9565b5b81905092915050565b5f5ffd5b5f601f19601f8301169050919050565b7f4e487b710000000000"
    "00000000000000000000000000000000000000000000005f52604160045260245ffd5b6101658261011f565b810181811067ffffffffffffff"
    "ff821117156101845761018361012f565b5b80604052505050565b5f6101966100e8565b90506101a2828261015c565b919050565b5f5ffd5b"
    "5f5ffd5b5f67ffffffffffffffff8211156101c9576101c861012f565b5b602082029050602081019050919050565b5f5ffd5b5f73ffffffff"
    "ffffffffffffffffffffffffffffffff82169050919050565b5f610207826101de565b9050919050565b610217816101fd565b811461022157"
    "5f5ffd5b50565b5f813590506102328161020e565b92915050565b5f8160070b9050919050565b61024d81610238565b8114610257575f5ffd"
    "5b50565b5f8135905061026881610244565b92915050565b5f604082840312156102835761028261011b565b5b61028d604061018d565b9050"
    "5f61029c84828501610224565b5f8301525060206102af8482850161025a565b60208301525092915050565b5f6102cd6102c8846101af565b"
    "61018d565b905080838252602082019050604084028301858111156102f0576102ef6101da565b5b835b818110156103195780610305888261"
    "026e565b8452602084019350506040810190506102f2565b5050509392505050565b5f82601f830112610337576103366101ab565b5b813561"
    "03478482602086016102bb565b91505092915050565b5f67ffffffffffffffff82111561036a5761036961012f565b5b602082029050602081"
    "019050919050565b5f67ffffffffffffffff8211156103955761039461012f565b5b602082029050602081019050919050565b5f6060828403"
    "12156103bb576103ba61011b565b5b6103c5606061018d565b90505f6103d484828501610224565b5f8301525060206103e784828501610224"
    "565b60208301525060406103fb8482850161025a565b60408301525092915050565b5f6104196104148461037b565b61018d565b9050808382"
    "526020820190506060840283018581111561043c5761043b6101da565b5b835b81811015610465578061045188826103a6565b845260208401"
    "93505060608101905061043e565b5050509392505050565b5f82601f830112610483576104826101ab565b5b81356104938482602086016104"
    "07565b91505092915050565b5f606082840312156104b1576104b061011b565b5b6104bb606061018d565b90505f6104ca8482820161022456"
    "5b5f83015250602082013567ffffffffffffffff8111156104ed576104ec6101a7565b5b6104f984828501610323565b602083015250604082"
    "013567ffffffffffffffff81111561051d5761051c6101a7565b5b6105298482850161046f565b60408301525092915050565b5f6105476105"
    "4284610350565b61018d565b90508083825260208201905060208402830185811115610567576105666101d7565b5b835b818110156105ae57"
    "803567ffffffffffffffff81111561058c5761058b6101a8565b5b8086016105998982610499565b8552602085019450505060208101905061"
    "0569565b5050509392505050565b5f82601f8301126105cc576105cb6101a8565b5b81356105dc848260208601610532565b91505092915050"
    "565b5f604082840312156105fa576105f9610118565b5b610604604061018a565b90505f82013567ffffffffffffffff811115610623576106"
    "226101a4565b5b61062f84828501610320565b5f83015250602082013567ffffffffffffffff811115610652576106516101a4565b5b61065e"
    "848285016105b8565b60208301525092915050565b5f6040828403121561067f5761067e610118565b5b610689604061018a565b90505f8201"
    "3567ffffffffffffffff8111156106a8576106a76101a4565b5b6106b4848285016105e5565b5f83015250602082013567ffffffffffffffff"
    "8111156106d7576106d66101a4565b5b6106e3848285016105e5565b60208301525092915050565b5f5f604083850312156107055761070461"
    "00ee565b5b5f83013567ffffffffffffffff811115610722576107216100f2565b5b61072e858286016100fa565b925050602083013567ffff"
    "ffffffffffff81111561074f5761074e6100f2565b5b61075b8582860161066a565b9150509250929050565b5f8115159050919050565b6107"
    "7981610765565b82525050565b5f6020820190506107925f830184610770565b9291505056fea26469706673582212206ee6e21549dc017713"
    "ebcaf2cc8da2267cab2518f23f1b8818b87b97b4b8389064736f6c634300081e0033";

  ContractId hookContractId = createHookContract(bytecode, getTestClient());
  AccountId senderId = createAccountWithHook(senderKey, hookContractId, getTestHookId1(), getTestClient());
  AccountId receiverId = AccountCreateTransaction()
                           .setKeyWithoutAlias(receiverKey->getPublicKey())
                           .setInitialBalance(Hbar(10LL))
                           .execute(getTestClient())
                           .getReceipt(getTestClient())
                           .mAccountId.value();

  // Create hook call with sufficient gas
  FungibleHookCall hookCall;
  hookCall.setHookType(FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  hookCall.setHookId(getTestHookId1());
  hookCall.setEvmHookCall(EvmHookCall().setGasLimit(25000));

  // When/Then - Should fail because hook returns false
  EXPECT_THROW(TransferTransaction()
                 .addHbarTransferWithHook(senderId, getTestHbarAmount().negated(), hookCall)
                 .addHbarTransfer(receiverId, getTestHbarAmount())
                 .execute(getTestClient())
                 .getReceipt(getTestClient()),
               ReceiptStatusException); // REJECTED_BY_ACCOUNT_ALLOWANCE_HOOK

  // Clean up
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(senderId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(senderKey)
                    .execute(getTestClient()));
  ASSERT_NO_THROW(AccountDeleteTransaction()
                    .setDeleteAccountId(receiverId)
                    .setTransferAccountId(AccountId(2ULL))
                    .freezeWith(&getTestClient())
                    .sign(receiverKey)
                    .execute(getTestClient()));
}
