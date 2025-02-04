// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TRANSACTION_RECORD_H_
#define HIERO_SDK_CPP_TRANSACTION_RECORD_H_

#include "AccountId.h"
#include "AssessedCustomFee.h"
#include "ContractFunctionResult.h"
#include "EvmAddress.h"
#include "Hbar.h"
#include "HbarTransfer.h"
#include "PendingAirdropRecord.h"
#include "PublicKey.h"
#include "TokenAssociation.h"
#include "TokenNftTransfer.h"
#include "TokenTransfer.h"
#include "TransactionId.h"
#include "TransactionReceipt.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

namespace proto
{
class TransactionGetRecordResponse;
class TransactionRecord;
}

namespace Hiero
{
/**
 * The complete record for a transaction on Hiero that has reached consensus. This is not-free to request and is
 * available for 1 hour after a transaction reaches consensus. A TransactionReceipt can be thought of as a light-weight
 * record which is free to ask for if you just need what it contains. A receipt however lasts for only 180 seconds.
 */
class TransactionRecord
{
public:
  /**
   * Construct a TransactionRecord object from a TransactionGetRecordResponse protobuf object.
   *
   * @param proto The TransactionGetRecordResponse protobuf object from which to construct a TransactionRecord object.
   * @return The constructed TransactionRecord object.
   */
  [[nodiscard]] static TransactionRecord fromProtobuf(const proto::TransactionGetRecordResponse& proto);

  /**
   * Construct a TransactionRecord object from a TransactionRecord protobuf object.
   *
   * @param proto The TransactionRecord protobuf object from which to construct a TransactionRecord object.
   * @return The constructed TransactionRecord object.
   */
  [[nodiscard]] static TransactionRecord fromProtobuf(const proto::TransactionRecord& proto);

  /**
   * Construct a TransactionRecord object from a byte array.
   *
   * @param bytes The byte array from which to construct a TransactionRecord object.
   * @return The constructed TransactionRecord object.
   */
  [[nodiscard]] static TransactionRecord fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a TransactionRecord protobuf object from this TransactionRecord object.
   *
   * @return A pointer to the created TransactionRecord protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::TransactionRecord> toProtobuf() const;

  /**
   * Construct a representative byte array from this TransactionRecord object.
   *
   * @return The byte array representing this TransactionRecord object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this TransactionRecord object.
   *
   * @return The string representation of this TransactionRecord object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this TransactionRecord to an output stream.
   *
   * @param os     The output stream.
   * @param record The TransactionRecord to print.
   * @return The output stream with this TransactionRecord written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const TransactionRecord& record);

  /**
   * The status (reach consensus, or failed, or is unknown) and the ID of any new account/file/instance created.
   */
  std::optional<TransactionReceipt> mReceipt;

  /**
   * The hash of the transaction that executed (not the hash of any transaction that failed for having a duplicate
   * transaction ID).
   */
  std::vector<std::byte> mTransactionHash;

  /**
   * The consensus timestamp (or uninitialized if the transaction hasn't reached consensus yet).
   */
  std::optional<std::chrono::system_clock::time_point> mConsensusTimestamp;

  /**
   * The ID of the transaction this record represents.
   */
  std::optional<TransactionId> mTransactionId;

  /**
   * The memo that was submitted as part of the transaction (max 100 bytes).
   */
  std::string mMemo;

  /**
   * The actual transaction fee charged, not the original transaction fee value from TransactionBody.
   */
  uint64_t mTransactionFee;

  /**
   * The result of the executed smart contract function or the result of the executed smart contract constructor.
   */
  std::optional<ContractFunctionResult> mContractFunctionResult;

  /**
   * All Hbar transfers as a result of this transaction, such as fees, or transfers performed by the transaction, or by
   * a smart contract it calls, or by the creation of threshold records that it triggers.
   */
  std::vector<HbarTransfer> mHbarTransferList;

  /**
   * All fungible token transfers as a result of this transaction.
   */
  std::vector<TokenTransfer> mTokenTransferList;

  /**
   * All NFT transfers as a result of this transaction.
   */
  std::vector<TokenNftTransfer> mNftTransferList;

  /**
   * The reference to the scheduled transaction ID that this TransactionRecord represents.
   */
  std::optional<ScheduleId> mScheduleRef;

  /**
   * All custom fees that were assessed during a CryptoTransfer, and must be paid if the transaction status resolved to
   * SUCCESS.
   */
  std::vector<AssessedCustomFee> mAssessedCustomFees;

  /**
   * All token associations implicitly created while handling this transaction.
   */
  std::vector<TokenAssociation> mAutomaticTokenAssociations;

  /**
   * In the record of an internal transaction, the consensus timestamp of the user transaction that spawned it.
   */
  std::optional<std::chrono::system_clock::time_point> mParentConsensusTimestamp;

  /**
   * In the record of an AccountCreateTransaction triggered by a user transaction with a (previously unused) alias, the
   * new account's alias.
   */
  std::shared_ptr<PublicKey> mAlias;

  /**
   * In the record of an EthereumTransaction, the KECCAK-256 hash of the ethereumData.
   */
  std::optional<std::vector<std::byte>> mEthereumHash;

  /**
   * The list of accounts with the corresponding staking rewards paid as a result of a transaction.
   */
  std::vector<HbarTransfer> mPaidStakingRewards;

  /**
   * In the record of a PrngTransaction with no range, a pseudorandom 384-bit string.
   */
  std::vector<std::byte> mPrngBytes;

  /**
   * In the record of a PrngTransaction with a range, the pseudorandom 32-bit number.
   */
  std::optional<int> mPrngNumber;

  /**
   * The new default EVM address of the account created by transaction with which this TransactionRecord is
   * associated. This field is populated only when the EVM address is not specified in the related transaction body.
   */
  std::optional<EvmAddress> mEvmAddress;

  /**
   * The records of processing all child transactions spawned by the transaction with the given top-level ID, in
   * consensus order. Always empty if the top-level status is UNKNOWN.
   */
  std::vector<TransactionRecord> mChildren;

  /**
   * The records of processing all consensus transaction with the same ID as the distinguished record, in chronological
   * order.
   */
  std::vector<TransactionRecord> mDuplicates;

  /**
   * A list of pending token airdrops.
   * Each pending airdrop represents a single requested transfer from a
   * sending account to a recipient account. These pending transfers are
   * issued unilaterally by the sending account, and MUST be claimed by the
   * recipient account before the transfer MAY complete.
   * A sender MAY cancel a pending airdrop before it is claimed.
   * An airdrop transaction SHALL emit a pending airdrop when the recipient has no
   * available automatic association slots available or when the recipient
   * has set `receiver_sig_required`.
   */
  std::vector<PendingAirdropRecord> mPendingAirdropRecords;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TRANSACTION_RECORD_H_
