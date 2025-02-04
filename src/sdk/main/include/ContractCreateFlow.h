// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CONTRACT_CREATE_FLOW_H_
#define HIERO_SDK_CPP_CONTRACT_CREATE_FLOW_H_

#include "AccountId.h"
#include "Defaults.h"
#include "Hbar.h"
#include "Key.h"
#include "TransactionResponse.h"

#include <chrono>
#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace Hiero
{
class Client;
class ContractFunctionParameters;
class PrivateKey;
}

namespace Hiero
{
/**
 * A helper class to create a contract file bytecode directly. This will use FileCreateTransaction and
 * FileAppendTransaction as necessary to create the file followed by ContractCreateTransaction to create the contract,
 * and finally FileDeleteTransaction to delete the created file since it's no longer necessary.
 */
class ContractCreateFlow
{
public:
  /**
   * Execute the Transactions in this flow (FileCreateTransaction, FileAppendTransaction (if needed),
   * ContractCreateTransaction, and a FileDeleteTransaction).
   *
   * @param client The Client to use to submit these Transactions.
   * @return The TransactionResponse object of the ContractCreateTransaction.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionResponse execute(const Client& client);

  /**
   * Execute the Transactions in this flow (FileCreateTransaction, FileAppendTransaction (if needed),
   * ContractCreateTransaction, and a FileDeleteTransaction) with a specified timeout.
   *
   * @param client  The Client to use to submit these Transactions.
   * @param timeout The desired timeout for the execution of these Transactions.
   * @return The TransactionResponse object of the ContractCreateTransaction.
   * @throws MaxAttemptsExceededException If this Executable attempts to execute past the number of allowable attempts.
   * @throws PrecheckStatusException      If this Executable fails its pre-check.
   * @throws UninitializedException       If the input Client has not yet been initialized.
   */
  TransactionResponse execute(const Client& client, const std::chrono::system_clock::duration& timeout);

  /**
   * Set the bytes of the smart contract bytecode. If the bytecode is large (>5K), then it must be stored in a file.
   *
   * @param byteCode The bytecode for the new smart contract instance.
   * @return A reference to this ContractCreateFlow object with the newly-set bytecode.
   * @throws OpenSSLException If input bytecode string is not hex-encoded.
   */
  ContractCreateFlow& setBytecode(const std::vector<std::byte>& byteCode);
  ContractCreateFlow& setBytecode(std::string_view byteCode);

  /**
   * Set the admin key for the new smart contract instance. The state of the smart contract instance and its fields can
   * be modified arbitrarily if this key signs a transaction to modify it. If this is not set, then such modifications
   * are not possible, and there is no administrator that can override the normal operation of the new smart contract
   * instance. Note, if this smart contract instance is created with no admin keys, then there is no administrator to
   * authorize changing the admin keys, so there can never be any admin keys for that instance.
   *
   * @param key The admin key for the new smart contract instance.
   * @return A reference to this ContractCreateFlow object with the newly-set admin key.
   */
  ContractCreateFlow& setAdminKey(const std::shared_ptr<Key>& key);

  /**
   * Set the amount of gas required to run the constructor of the new smart contract instance.
   *
   * @param gas The amount of gas to run the constructor of the new smart contract instance.
   * @return A reference to this ContractCreateFlow object with the newly-set amount of gas.
   */
  ContractCreateFlow& setGas(const uint64_t& gas);

  /**
   * Set the initial amount of Hbar to put into the cryptocurrency account associated with and owned by the new smart
   * contract instance.
   *
   * @param initialBalance The initial balance for the cryptocurrency account owned by the new smart contract instance.
   * @return A reference to this ContractCreateFlow object with the newly-set initial balance.
   */
  ContractCreateFlow& setInitialBalance(const Hbar& initialBalance);

  /**
   * Set the auto renew period for the new smart contract instance. The smart contract instance will be charged to
   * extend its expiration date every renew period. If it doesn't have enough balance, it extends as long as possible.
   * If the balance is zero when it expires, then the smart contract instance is deleted.
   *
   * @param autoRenewPeriod The desired auto renew period for the new smart contract instance.
   * @return A reference to this ContractCreateFlow object with the newly-set auto renew period.
   */
  ContractCreateFlow& setAutoRenewPeriod(const std::chrono::system_clock::duration& autoRenewPeriod);

  /**
   * Set the parameters to pass to the new smart contract instance's constructor.
   *
   * @param constructorParameters The parameters to pass to the new smart contract instance's constructor.
   * @return A reference to this ContractCreateFlow object with the newly-set constructor parameters.
   */
  ContractCreateFlow& setConstructorParameters(const ContractFunctionParameters& constructorParameters);
  ContractCreateFlow& setConstructorParameters(const std::vector<std::byte>& constructorParameters);

  /**
   * Set the memo for the new smart contract instance.
   *
   * @param memo The memo for the new smart contract instance.
   * @return A reference to this ContractCreateFlow object with the newly-set memo.
   */
  ContractCreateFlow& setMemo(std::string_view memo);

  /**
   * Set the maximum number of tokens with which the new smart contract instance can be automatically associated.
   *
   * @param associations The maximum amount of token associations for the new smart contract instance.
   * @return A reference to this ContractCreateFlow object with the newly-set maximum automatic token associations.
   */
  ContractCreateFlow& setMaxAutomaticTokenAssociations(int32_t associations);

  /**
   * Set the account to charge for auto-renewal of the new smart contract instance. If not set, or set to an account
   * with zero Hbar balance, the contract's own Hbar balance will be used to cover auto-renewal fees.
   *
   * @param autoRenewAccountId The ID of the account to charge for auto-renewal of the new smart contract instance.
   * @return A reference to this ContractCreateFlow object with the newly-set auto-renewal account ID.
   */
  ContractCreateFlow& setAutoRenewAccountId(const AccountId& autoRenewAccountId);

  /**
   * Set the ID of the account to which the new smart contract instance should stake. This is mutually exclusive with
   * mStakedNodeId, and will reset the value of the mStakedNodeId if it is set.
   *
   * @param stakedAccountId The ID of the account to which the new smart contract instance will stake.
   * @return A reference to this ContractCreateFlow object with the newly-set staked account ID.
   */
  ContractCreateFlow& setStakedAccountId(const AccountId& stakedAccountId);

  /**
   * Set the ID of the node to which the new smart contract instance should stake. This is mutually exclusive with
   * mStakedAccountId, and will reset the value of the mStakedAccountId if it is set.
   *
   * @param stakedNodeId The ID of the desired node to which the new smart contract instance will stake.
   * @return A reference to this ContractCreateFlow object with the newly-set staked node ID.
   */
  ContractCreateFlow& setStakedNodeId(const uint64_t& stakedNodeId);

  /**
   * Set the staking reward reception policy for the new smart contract instance.
   *
   * @param declineReward \c TRUE if the new smart contract instance should decline receiving staking rewards, otherwise
   *                      \c FALSE.
   * @return A reference to this ContractCreateFlow object with the newly-set staking rewards reception policy.
   */
  ContractCreateFlow& setDeclineStakingReward(bool declineReward);

  /**
   * Set the desired account IDs of nodes to which this transaction will be submitted.
   *
   * @param nodeAccountIds The desired list of account IDs of nodes to submit this request.
   * @return A reference to this ContractCreateFlow object with the newly-set node account IDs.
   */
  ContractCreateFlow& setNodeAccountIds(const std::vector<AccountId>& nodeAccountIds);

  /**
   * Set the maximum number of chunks for the FileAppendTransaction that will be sent as a part of this
   * ContractCreateFlow.
   *
   * @param chunks The maximum number of chunks for the FileAppendTransaction.
   * @return A reference to this ContractCreateFlow object with the newly-set chunk maximum.
   */
  ContractCreateFlow& setMaxChunks(unsigned int chunks);

  /**
   * Freeze the ContractCreateTransaction with a Client. The Client's operator will be used to generate a transaction
   * ID, and the client's network will be used to generate a list of node account IDs.
   *
   * NOTE: Since Client's can't be copied, this ContractCreateFlow will store a pointer to the input Client. It is the
   * responsibility of the user to make sure that the Client does not go out of scope or get destroyed until this
   * ContractCreateFlow is done executing, otherwise this will crash upon execution.
   *
   * @param client The Client with which to freeze the ContractCreateTransaction.
   * @return A reference to this ContractCreateFlow object with the newly-set freeze Client.
   * @throws UninitializedException If Client operator has not been initialized.
   */
  ContractCreateFlow& freezeWith(const Client& client);

  /**
   * Set the PrivateKey with which the ContractCreateTransaction will be signed.
   *
   * @param key The PrivateKey with which to sign the ContractCreateTransaction.
   * @return A reference to this ContractCreateFlow object with the newly-set signing PrivateKey.
   */
  ContractCreateFlow& sign(const std::shared_ptr<PrivateKey>& key);

  /**
   * Set the PublicKey and signer function with which the ContractCreateTransaction will be signed.
   *
   * @param key    The PublicKey with which to sign the ContractCreateTransaction.
   * @param signer The callback function to use to sign the ContractCreateTransaction.
   * @return A reference to this ContractCreateFlow object with the newly-set public key and signer function.
   */
  ContractCreateFlow& signWith(const std::shared_ptr<PublicKey>& key,
                               const std::function<std::vector<std::byte>(const std::vector<std::byte>&)>& signer);

  /**
   * Set the Client operator with which the ContractCreateTransaction will be signed.
   *
   * @param client The Client operator to sign the ContractCreateTransaction.
   * @return A reference to this ContractCreateFlow object with the newly-set signing operator.
   * @throws UninitializedException If the Client operator has not yet been set.
   */
  ContractCreateFlow& signWithOperator(const Client& client);

  /**
   * Get the bytes of the smart contract initcode.
   *
   * @return The bytes of the desired smart contract initcode.
   */
  [[nodiscard]] inline std::vector<std::byte> getBytecode() const { return mBytecode; }

  /**
   * Get the admin key for the new smart contract instance.
   *
   * @return A pointer to the admin key for the new smart contract instance. Return nullptr if the admin key has not yet
   *         been set.
   */
  [[nodiscard]] inline std::shared_ptr<Key> getAdminKey() const { return mAdminKey; }

  /**
   * Get the amount of gas required to run the constructor of the new smart contract instance.
   *
   * @return The amount of gas required to run the constructor of the new smart contract instance.
   */
  [[nodiscard]] inline uint64_t getGas() const { return mGas; }

  /**
   * Get the initial amount of Hbar to put into the cryptocurrency account associated with and owned by the new smart
   * contract instance.
   *
   * @return The initial amount of Hbar to put into the cryptocurrency account associated with and owned by the new
   *         smart contract instance.
   */
  [[nodiscard]] inline Hbar getInitialBalance() const { return mInitialBalance; }

  /**
   * Get the auto renew period for the new smart contract instance.
   *
   * @return The auto renew period for the new smart contract instance.
   */
  [[nodiscard]] inline std::chrono::system_clock::duration getAutoRenewPeriod() const { return mAutoRenewPeriod; }

  /**
   * Get the parameters to pass to the new smart contract instance's constructor.
   *
   * @return The parameters to pass to the new smart contract instance's constructor.
   */
  [[nodiscard]] inline std::vector<std::byte> getConstructorParameters() const { return mConstructorParameters; }

  /**
   * Get the memo for the new smart contract instance.
   *
   * @return The memo for the new smart contract instance.
   */
  [[nodiscard]] inline std::string getMemo() const { return mMemo; }

  /**
   * Get the maximum number of tokens with which the new smart contract instance can be automatically associated.
   *
   * @return The maximum number of tokens with which the new smart contract instance can be automatically associated.
   */
  [[nodiscard]] inline int32_t getMaxAutomaticTokenAssociations() const { return mMaxAutomaticTokenAssociations; }

  /**
   * Get the account to charge for auto-renewal of the new smart contract instance.
   *
   * @return The account to charge for auto-renewal of the new smart contract instance. Uninitialized if no auto-renew
   *         account ID has been set.
   */
  [[nodiscard]] inline std::optional<AccountId> getAutoRenewAccountId() const { return mAutoRenewAccountId; }

  /**
   * Get the ID of the account to which the new smart contract instance should stake.
   *
   * @return The ID of the account to which the new smart contract instance should stake. Returns uninitialized if a
   *         value has not yet been set, or if a staked node ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<AccountId> getStakedAccountId() const { return mStakedAccountId; }

  /**
   * Get the ID of the node to which the new smart contract instance should stake.
   *
   * @return The ID of the node to which the new smart contract instance should stake. Returns uninitialized if a value
   *         has not yet been set, or if a staked account ID has been set most recently.
   */
  [[nodiscard]] inline std::optional<uint64_t> getStakedNodeId() const { return mStakedNodeId; }

  /**
   * Get the staking rewards reception policy for the new smart contract instance.
   *
   * @return \c TRUE if the new smart contract instance should decline from receiving staking rewards, otherwise \c
   *         FALSE.
   */
  [[nodiscard]] inline bool getDeclineStakingReward() const { return mDeclineStakingReward; }

  /**
   * Get the list of account IDs for nodes with which execution will be attempted.
   *
   * @return The list of account IDs of nodes this ContractCreateFlow would attempt request submission.
   */
  [[nodiscard]] inline std::vector<AccountId> getNodeAccountIds() const { return mNodeAccountIds; }

  /**
   * Get the maximum number of chunks for the FileAppendTransaction that will be sent as a part of this
   * ContractCreateFlow.
   *
   * @return The maximum number of chunks for the FileAppendTransaction that will be sent as a part of this
   * ContractCreateFlow.
   */
  [[nodiscard]] inline unsigned int getMaxChunks() const { return mMaxChunks; }

private:
  /**
   * The bytes of the smart contract bytecode.
   */
  std::vector<std::byte> mBytecode;

  /**
   * The maximum number of chunks into which the FileAppendTransaction that will be sent as a part of this
   * ContractCreateFlow will get broken up.
   */
  unsigned int mMaxChunks = DEFAULT_MAX_CHUNKS;

  /**
   * The admin key for the new smart contract instance.
   */
  std::shared_ptr<Key> mAdminKey = nullptr;

  /**
   * The amount of gas required to run the constructor of the new smart contract instance.
   */
  uint64_t mGas = 0ULL;

  /**
   * The initial amount of Hbar to put into the cryptocurrency account associated with and owned by the new smart
   * contract instance.
   */
  Hbar mInitialBalance = Hbar(0LL);

  /**
   * A Hiero smart contract instance is charged to extend its expiration date every renew period. If it doesn't have
   * enough balance, it extends as long as possible. If the balance is zero when it expires, then the smart contract
   * instance is deleted.
   */
  std::chrono::system_clock::duration mAutoRenewPeriod = DEFAULT_AUTO_RENEW_PERIOD;

  /**
   * The parameters to pass to the new smart contract instance's constructor.
   */
  std::vector<std::byte> mConstructorParameters;

  /**
   * The memo to be associated with the smart contract instance (UTF-8 encoding max 100 bytes).
   */
  std::string mMemo;

  /**
   * The maximum number of tokens with which the new smart contract instance can be implicitly associated.
   */
  int32_t mMaxAutomaticTokenAssociations = 0;

  /**
   * The account to charge for auto-renewal of the new smart contract instance.
   */
  std::optional<AccountId> mAutoRenewAccountId;

  /**
   * The ID of the account to which the new smart contract instance will be staked. Mutually exclusive with
   * mStakedNodeId.
   */
  std::optional<AccountId> mStakedAccountId;

  /**
   * If \c TRUE, the new smart contract instance will decline receiving staking rewards.
   */
  bool mDeclineStakingReward = false;

  /**
   * The Client with which to freeze the ContractCreateTransaction.
   */
  const Client* mFreezeWithClient = nullptr;

  /**
   * The PrivateKey with which to sign the ContractCreateTransaction.
   */
  std::shared_ptr<PrivateKey> mPrivateKey = nullptr;

  /**
   * The PublicKey associated with the signer function to sign the ContractCreateTransaction.
   */
  std::shared_ptr<PublicKey> mPublicKey = nullptr;

  /**
   * The signer function to use to sign the ContractCreateTransaction.
   */
  std::optional<std::function<std::vector<std::byte>(const std::vector<std::byte>&)>> mSigner;

  /**
   * The ID of the node to which the new smart contract instance will be staked. Mutually exclusive with
   * mStakedAccountId.
   */
  std::optional<uint64_t> mStakedNodeId;

  /**
   * The list of account IDs of the nodes with which execution should be attempted.
   */
  std::vector<AccountId> mNodeAccountIds;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CONTRACT_CREATE_FLOW_H_
