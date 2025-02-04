// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CONTRACT_CREATE_TRANSACTION_H_
#define HIERO_SDK_CPP_CONTRACT_CREATE_TRANSACTION_H_

#include "AccountId.h"
#include "FileId.h"
#include "Hbar.h"
#include "Key.h"
#include "Transaction.h"

#include <chrono>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace proto
{
class ContractCreateTransactionBody;
class TransactionBody;
}

namespace Hiero
{
/**
 * Start a new smart contract instance. After the instance is created, the ContractId for it is in the receipt, and can
 * be retrieved by the Record or with a GetByKey query. The instance will run the bytecode, either stored in a
 * previously created file or in the transaction body itself for small contracts.
 *
 * The constructor will be executed using the given amount of gas, and any unspent gas will be refunded to the paying
 * account. Constructor inputs come from the given constructorParameters.
 *  - The instance will exist for autoRenewPeriod seconds. When that is reached, it will renew itself for another
 *    autoRenewPeriod seconds by charging its associated cryptocurrency account (which it creates here). If it has
 *    insufficient cryptocurrency to extend that long, it will extend as long as it can. If its balance is zero, the
 *    instance will be deleted.
 *
 *  - A smart contract instance normally enforces rules, so "the code is law". For example, an ERC-20 contract prevents
 *    a transfer from being undone without a signature by the recipient of the transfer. This is always enforced if the
 *    contract instance was created with the adminKeys being null. But for some uses, it might be desirable to create
 *    something like an ERC-20 contract that has a specific group of trusted individuals who can act as a "supreme
 *    court" with the ability to override the normal operation, when a sufficient number of them agree to do so. If
 *    adminKeys is not null, then they can sign a transaction that can change the state of the smart contract in
 *    arbitrary ways, such as to reverse a transaction that violates some standard of behavior that is not covered by
 *    the code itself. The admin keys can also be used to change the autoRenewPeriod, and change the adminKeys field
 *    itself. The API currently does not implement this ability. But it does allow the adminKeys field to be set and
 *    queried, and will in the future implement such admin abilities for any instance that has a non-null adminKeys.
 *
 *  - If this constructor stores information, it is charged gas to store it. There is a fee in Hbars to maintain that
 *    storage until the expiration time, and that fee is added as part of the transaction fee.
 *
 *  - An entity (account, file, or smart contract instance) must be created in a particular realm. If the realmID is
 *    left null, then a new realm will be created with the given admin key. If a new realm has a null adminKey, then
 *    anyone can create/modify/delete entities in that realm. But if an admin key is given, then any transaction to
 *    create/modify/delete an entity in that realm must be signed by that key, though anyone can still call functions on
 *    smart contract instances that exist in that realm. A realm ceases to exist when everything within it has expired
 *    and no longer exists.
 *
 *  - The current API ignores shardID, realmID, and newRealmAdminKey, and creates everything in shard 0 and realm 0,
 *    with a null key. Future versions of the API will support multiple realms and multiple shards.
 *
 *  - The optional memo field can contain a string whose length is up to 100 bytes. That is the size after Unicode NFD
 *    then UTF-8 conversion. This field can be used to describe the smart contract. It could also be used for other
 *    purposes. One recommended purpose is to hold a hexadecimal string that is the SHA-384 hash of a PDF file
 *    containing a human-readable legal contract. Then, if the admin keys are the public keys of human arbitrators, they
 *    can use that legal document to guide their decisions during a binding arbitration tribunal, convened to consider
 *    any changes to the smart contract in the future. The memo field can only be changed using the admin keys. If there
 *    are no admin keys, then it cannot be changed after the smart contract is created.
 *
 * Signing requirements: If an admin key is set, it must sign the transaction. If an auto-renew account is set, its key
 * must sign the transaction.
 */
class ContractCreateTransaction : public Transaction<ContractCreateTransaction>
{
public:
  /**
   * Default constructor. Sets the maximum transaction fee to 20 Hbars.
   */
  ContractCreateTransaction();

  /**
   * Construct from a TransactionBody protobuf object.
   *
   * @param transactionBody The TransactionBody protobuf object from which to construct.
   * @throws std::invalid_argument If the input TransactionBody does not represent a ContractCreateInstance transaction.
   */
  explicit ContractCreateTransaction(const proto::TransactionBody& transactionBody);

  /**
   * Construct from a map of TransactionIds to node account IDs and their respective Transaction protobuf objects.
   *
   * @param transactions The map of TransactionIds to node account IDs and their respective Transaction protobuf
   *                     objects.
   */
  explicit ContractCreateTransaction(
    const std::map<TransactionId, std::map<AccountId, proto::Transaction>>& transactions);

  /**
   * Set the ID of the file that contains the smart contract bytecode. A copy will be made and held by the contract
   * instance, and have the same expiration time as the instance. If the bytecode is large (>5K), then it must be stored
   * in a file. This is mutually exclusive with mBytecode, and will reset the value of the mBytecode if it is set.
   *
   * @param fileId The ID of the file that contains the bytecode for the new smart contract instance.
   * @return A reference to this ContractCreateTransaction object with the newly-set file ID.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setBytecodeFileId(const FileId& fileId);

  /**
   * Set the bytes of the smart contract bytecode. If the bytecode is large (>5K), then it must be stored in a file.
   * This is mutually exclusive with mBytecodeFileId, and will reset the value of the mBytecodeFileId if it is set.
   *
   * @param initCode The bytecode for the new smart contract instance.
   * @return A reference to this ContractCreateTransaction object with the newly-set initcode.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setBytecode(const std::vector<std::byte>& initCode);

  /**
   * Set the admin key for the new smart contract instance. The state of the smart contract instance and its fields can
   * be modified arbitrarily if this key signs a transaction to modify it. If this is not set, then such modifications
   * are not possible, and there is no administrator that can override the normal operation of the new smart contract
   * instance. Note, if this smart contract instance is created with no admin keys, then there is no administrator to
   * authorize changing the admin keys, so there can never be any admin keys for that instance.
   *
   * @param key The admin key for the new smart contract instance.
   * @return A reference to this ContractCreateTransaction object with the newly-set admin key.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setAdminKey(const std::shared_ptr<Key>& key);

  /**
   * Set the amount of gas required to run the constructor of the new smart contract instance.
   *
   * @param gas The amount of gas to run the constructor of the new smart contract instance.
   * @return A reference to this ContractCreateTransaction object with the newly-set amount of gas.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setGas(const uint64_t& gas);

  /**
   * Set the initial amount of Hbar to put into the cryptocurrency account associated with and owned by the new smart
   * contract instance.
   *
   * @param initialBalance The initial balance for the cryptocurrency account owned by the new smart contract instance.
   * @return A reference to this ContractCreateTransaction object with the newly-set initial balance.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setInitialBalance(const Hbar& initialBalance);

  /**
   * Set the auto renew period for the new smart contract instance. The smart contract instance will be charged to
   * extend its expiration date every renew period. If it doesn't have enough balance, it extends as long as possible.
   * If the balance is zero when it expires, then the smart contract instance is deleted.
   *
   * @param autoRenewPeriod The desired auto renew period for the new smart contract instance.
   * @return A reference to this ContractCreateTransaction object with the newly-set auto renew period.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setAutoRenewPeriod(const std::chrono::system_clock::duration& autoRenewPeriod);

  /**
   * Set the parameters to pass to the new smart contract instance's constructor.
   *
   * @param constructorParameters The parameters to pass to the new smart contract instance's constructor.
   * @return A reference to this ContractCreateTransaction object with the newly-set constructor parameters.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setConstructorParameters(const std::vector<std::byte>& constructorParameters);

  /**
   * Set the memo for the new smart contract instance.
   *
   * @param memo The memo for the new smart contract instance.
   * @return A reference to this ContractCreateTransaction object with the newly-set memo.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setMemo(std::string_view memo);

  /**
   * Set the maximum number of tokens with which the new smart contract instance can be automatically associated.
   *
   * @param associations The maximum amount of token associations for the new smart contract instance.
   * @return A reference to this ContractCreateTransaction object with the newly-set maximum automatic token
   *         associations.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setMaxAutomaticTokenAssociations(int32_t associations);

  /**
   * Set the account to charge for auto-renewal of the new smart contract instance. If not set, or set to an account
   * with zero Hbar balance, the contract's own Hbar balance will be used to cover auto-renewal fees.
   *
   * @param autoRenewAccountId The ID of the account to charge for auto-renewal of the new smart contract instance.
   * @return A reference to this ContractCreateTransaction object with the newly-set auto-renewal account ID.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setAutoRenewAccountId(const AccountId& autoRenewAccountId);

  /**
   * Set the ID of the account to which the new smart contract instance should stake. This is mutually exclusive with
   * mStakedNodeId, and will reset the value of the mStakedNodeId if it is set.
   *
   * @param stakedAccountId The ID of the account to which the new smart contract instance will stake.
   * @return A reference to this ContractCreateTransaction object with the newly-set staked account ID.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setStakedAccountId(const AccountId& stakedAccountId);

  /**
   * Set the ID of the node to which the new smart contract instance should stake. This is mutually exclusive with
   * mStakedAccountId, and will reset the value of the mStakedAccountId if it is set.
   *
   * @param stakedNodeId The ID of the desired node to which the new smart contract instance will stake.
   * @return A reference to this ContractCreateTransaction object with the newly-set staked node ID.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setStakedNodeId(const uint64_t& stakedNodeId);

  /**
   * Set the staking reward reception policy for the new smart contract instance.
   *
   * @param declineReward \c TRUE if the new smart contract instance should decline receiving staking rewards, otherwise
   *                      \c FALSE.
   * @return A reference to this ContractCreateTransaction object with the newly-set staking rewards reception policy.
   * @throws IllegalStateException If this ContractCreateTransaction is frozen.
   */
  ContractCreateTransaction& setDeclineStakingReward(bool declineReward);

  /**
   * Get the ID of the file that contains the smart contract initcode.
   *
   * @return The ID of the file that contains the desired smart contract initcode. Returns uninitialized if a value has
   *         not yet been set, or if mBytecode has been set more recently.
   */
  [[nodiscard]] inline std::optional<FileId> getFileId() const { return mBytecodeFileId; }

  /**
   * Get the bytes of the smart contract initcode.
   *
   * @return The bytes of the desired smart contract initcode. Returns uninitialized if a value has not yet been set, or
   *         if the mBytecodeFileId has been set more recently.
   */
  [[nodiscard]] inline std::optional<std::vector<std::byte>> getInitCode() const { return mBytecode; }

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

private:
  friend class WrappedTransaction;

  /**
   * Derived from Executable. Submit a Transaction protobuf object which contains this ContractCreateTransaction's data
   * to a Node.
   *
   * @param request  The Transaction protobuf object to submit.
   * @param node     The Node to which to submit the request.
   * @param deadline The deadline for submitting the request.
   * @param response Pointer to the ProtoResponseType object that gRPC should populate with the response information
   *                 from the gRPC server.
   * @return The gRPC status of the submission.
   */
  [[nodiscard]] grpc::Status submitRequest(const proto::Transaction& request,
                                           const std::shared_ptr<internal::Node>& node,
                                           const std::chrono::system_clock::time_point& deadline,
                                           proto::TransactionResponse* response) const override;

  /**
   * Derived from Transaction. Verify that all the checksums in this ContractCreateTransaction are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This ContractCreateTransaction's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Transaction. Build and add the ContractCreateTransaction protobuf representation to the Transaction
   * protobuf object.
   *
   * @param body The TransactionBody protobuf object being built.
   */
  void addToBody(proto::TransactionBody& body) const override;

  /**
   * Initialize this ContractCreateTransaction from its source TransactionBody protobuf object.
   */
  void initFromSourceTransactionBody();

  /**
   * Build a ContractCreateTransactionBody protobuf object from this ContractCreateTransaction object.
   *
   * @return A pointer to a ContractCreateTransactionBody protobuf object filled with this ContractCreateTransaction
   *         object's data.
   */
  [[nodiscard]] proto::ContractCreateTransactionBody* build() const;

  /**
   * The ID of the file that contains the smart contract bytecode.
   */
  std::optional<FileId> mBytecodeFileId;

  /**
   * The bytes of the smart contract bytecode.
   */
  std::optional<std::vector<std::byte>> mBytecode;

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
   * The ID of the node to which the new smart contract instance will be staked. Mutually exclusive with
   * mStakedAccountId.
   */
  std::optional<uint64_t> mStakedNodeId;

  /**
   * If \c TRUE, the new smart contract instance will decline receiving staking rewards.
   */
  bool mDeclineStakingReward = false;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CONTRACT_CREATE_TRANSACTION_H_
