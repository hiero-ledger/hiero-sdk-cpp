/*-
 *
 * Hedera C++ SDK
 *
 * Copyright (C) 2020 - 2024 Hedera Hashgraph, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License")
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#ifndef HEDERA_SDK_CPP_STATUS_H_
#define HEDERA_SDK_CPP_STATUS_H_

#include <string_view>
#include <unordered_map>
#include <string>

namespace proto
{
enum ResponseCodeEnum : int;
}

namespace Hedera
{
/**
 * Enumeration of possible transaction status types.
 *
 * The success variant is SUCCESS which is what a TransactionReceipt will contain for a successful transaction.
 */
enum class Status
{
  /**
   * The transaction passed the pre-check validations.
   */
  OK,

  /**
   * For any error not handled by specific error codes listed below.
   */
  INVALID_TRANSACTION,

  /**
   * Payer account does not exist.
   */
  PAYER_ACCOUNT_NOT_FOUND,

  /**
   * Node Account provided does not match the node account of the node the transaction was submitted
   * to.
   */
  INVALID_NODE_ACCOUNT,

  /**
   * Pre-Check error when TransactionValidStart + transactionValidDuration is less than current
   * consensus time.
   */
  TRANSACTION_EXPIRED,

  /**
   * Transaction start time is greater than current consensus time
   */
  INVALID_TRANSACTION_START,

  /**
   * The given transactionValidDuration was either non-positive, or greater than the maximum
   * valid duration of 180 secs.
   */
  INVALID_TRANSACTION_DURATION,

  /**
   * The transaction signature is not valid
   */
  INVALID_SIGNATURE,

  /**
   * Transaction memo size exceeded 100 bytes
   */
  MEMO_TOO_LONG,

  /**
   * The fee provided in the transaction is insufficient for this type of transaction
   */
  INSUFFICIENT_TX_FEE,

  /**
   * The payer account has insufficient cryptocurrency to pay the transaction fee
   */
  INSUFFICIENT_PAYER_BALANCE,

  /**
   * This transaction ID is a duplicate of one that was submitted to this node or reached consensus
   * in the last 180 seconds (receipt period)
   */
  DUPLICATE_TRANSACTION,

  /**
   * If API is throttled out
   */
  BUSY,

  /**
   * The API is not currently supported
   */
  NOT_SUPPORTED,

  /**
   * The file id is invalid or does not exist
   */
  INVALID_FILE_ID,

  /**
   * The account id is invalid or does not exist
   */
  INVALID_ACCOUNT_ID,

  /**
   * The contract id is invalid or does not exist
   */
  INVALID_CONTRACT_ID,

  /**
   * Transaction id is not valid
   */
  INVALID_TRANSACTION_ID,

  /**
   * Receipt for given transaction id does not exist
   */
  RECEIPT_NOT_FOUND,

  /**
   * Record for given transaction id does not exist
   */
  RECORD_NOT_FOUND,

  /**
   * The solidity id is invalid or entity with this solidity id does not exist
   */
  INVALID_SOLIDITY_ID,

  /**
   * The responding node has submitted the transaction to the network. Its final status is still
   * unknown.
   */
  UNKNOWN,

  /**
   * The transaction succeeded
   */
  SUCCESS,

  /**
   * There was a system error and the transaction failed because of invalid request parameters.
   */
  FAIL_INVALID,

  /**
   * There was a system error while performing fee calculation, reserved for future.
   */
  FAIL_FEE,

  /**
   * There was a system error while performing balance checks, reserved for future.
   */
  FAIL_BALANCE,

  /**
   * Key not provided in the transaction body
   */
  KEY_REQUIRED,

  /**
   * Unsupported algorithm/encoding used for keys in the transaction
   */
  BAD_ENCODING,

  /**
   * When the account balance is not sufficient for the transfer
   */
  INSUFFICIENT_ACCOUNT_BALANCE,

  /**
   * During an update transaction when the system is not able to find the Users Solidity address
   */
  INVALID_SOLIDITY_ADDRESS,

  /**
   * Not enough gas was supplied to execute transaction
   */
  INSUFFICIENT_GAS,

  /**
   * contract byte code size is over the limit
   */
  CONTRACT_SIZE_LIMIT_EXCEEDED,

  /**
   * local execution (query) is requested for a function which changes state
   */
  LOCAL_CALL_MODIFICATION_EXCEPTION,

  /**
   * Contract REVERT OPCODE executed
   */
  CONTRACT_REVERT_EXECUTED,

  /**
   * For any contract execution related error not handled by specific error codes listed above.
   */
  CONTRACT_EXECUTION_EXCEPTION,

  /**
   * In Query validation, account with +ve(amount) value should be Receiving node account, the
   * receiver account should be only one account in the list
   */
  INVALID_RECEIVING_NODE_ACCOUNT,

  /**
   * Header is missing in Query request
   */
  MISSING_QUERY_HEADER,

  /**
   * The update of the account failed
   */
  ACCOUNT_UPDATE_FAILED,

  /**
   * Provided key encoding was not supported by the system
   */
  INVALID_KEY_ENCODING,

  /**
   * null solidity address
   */
  NULL_SOLIDITY_ADDRESS,

  /**
   * update of the contract failed
   */
  CONTRACT_UPDATE_FAILED,

  /**
   * the query header is invalid
   */
  INVALID_QUERY_HEADER,

  /**
   * Invalid fee submitted
   */
  INVALID_FEE_SUBMITTED,

  /**
   * Payer signature is invalid
   */
  INVALID_PAYER_SIGNATURE,

  /**
   * The keys were not provided in the request.
   */
  KEY_NOT_PROVIDED,

  /**
   * Expiration time provided in the transaction was invalid.
   */
  INVALID_EXPIRATION_TIME,

  /**
   * WriteAccess Control Keys are not provided for the file
   */
  NO_WACL_KEY,

  /**
   * The contents of file are provided as empty.
   */
  FILE_CONTENT_EMPTY,

  /**
   * The crypto transfer credit and debit do not sum equal to 0
   */
  INVALID_ACCOUNT_AMOUNTS,

  /**
   * Transaction body provided is empty
   */
  EMPTY_TRANSACTION_BODY,

  /**
   * Invalid transaction body provided
   */
  INVALID_TRANSACTION_BODY,

  /**
   * the type of key (base ed25519 key, KeyList, or ThresholdKey) does not match the type of
   * signature (base ed25519 signature, SignatureList, or ThresholdKeySignature)
   */
  INVALID_SIGNATURE_TYPE_MISMATCHING_KEY,

  /**
   * the number of key (KeyList, or ThresholdKey) does not match that of signature (SignatureList,
   * or ThresholdKeySignature). e.g. if a keyList has 3 base keys, then the corresponding
   * signatureList should also have 3 base signatures.
   */
  INVALID_SIGNATURE_COUNT_MISMATCHING_KEY,

  /**
   * the livehash body is empty
   */
  EMPTY_LIVE_HASH_BODY,

  /**
   * the livehash data is missing
   */
  EMPTY_LIVE_HASH,

  /**
   * the keys for a livehash are missing
   */
  EMPTY_LIVE_HASH_KEYS,

  /**
   * the livehash data is not the output of a SHA-384 digest
   */
  INVALID_LIVE_HASH_SIZE,

  /**
   * the query body is empty
   */
  EMPTY_QUERY_BODY,

  /**
   * the crypto livehash query is empty
   */
  EMPTY_LIVE_HASH_QUERY,

  /**
   * the livehash is not present
   */
  LIVE_HASH_NOT_FOUND,

  /**
   * the account id passed has not yet been created.
   */
  ACCOUNT_ID_DOES_NOT_EXIST,

  /**
   * the livehash already exists for a given account
   */
  LIVE_HASH_ALREADY_EXISTS,

  /**
   * File WACL keys are invalid
   */
  INVALID_FILE_WACL,

  /**
   * Serialization failure
   */
  SERIALIZATION_FAILED,

  /**
   * The size of the Transaction is greater than transactionMaxBytes
   */
  TRANSACTION_OVERSIZE,

  /**
   * The Transaction has more than 50 levels
   */
  TRANSACTION_TOO_MANY_LAYERS,

  /**
   * Contract is marked as deleted
   */
  CONTRACT_DELETED,

  /**
   * the platform node is either disconnected or lagging behind.
   */
  PLATFORM_NOT_ACTIVE,

  /**
   * one public key matches more than one prefixes on the signature map
   */
  KEY_PREFIX_MISMATCH,

  /**
   * transaction not created by platform due to large backlog
   */
  PLATFORM_TRANSACTION_NOT_CREATED,

  /**
   * auto renewal period is not a positive number of seconds
   */
  INVALID_RENEWAL_PERIOD,

  /**
   * the response code when a smart contract id is passed for a crypto API request
   */
  INVALID_PAYER_ACCOUNT_ID,

  /**
   * the account has been marked as deleted
   */
  ACCOUNT_DELETED,

  /**
   * the file has been marked as deleted
   */
  FILE_DELETED,

  /**
   * same accounts repeated in the transfer account list
   */
  ACCOUNT_REPEATED_IN_ACCOUNT_AMOUNTS,

  /**
   * attempting to set negative balance value for crypto account
   */
  SETTING_NEGATIVE_ACCOUNT_BALANCE,

  /**
   * when deleting smart contract that has crypto balance either transfer account or transfer smart
   * contract is required
   */
  OBTAINER_REQUIRED,

  /**
   * when deleting smart contract that has crypto balance you can not use the same contract id as
   * transferContractId as the one being deleted
   */
  OBTAINER_SAME_CONTRACT_ID,

  /**
   * transferAccountId or transferContractId specified for contract delete does not exist
   */
  OBTAINER_DOES_NOT_EXIST,

  /**
   * attempting to modify (update or delete a immutable smart contract, i.e. one created without a
   * admin key)
   */
  MODIFYING_IMMUTABLE_CONTRACT,

  /**
   * Unexpected exception thrown by file system functions
   */
  FILE_SYSTEM_EXCEPTION,

  /**
   * the duration is not a subset of [MINIMUM_AUTORENEW_DURATION,MAXIMUM_AUTORENEW_DURATION]
   */
  AUTORENEW_DURATION_NOT_IN_RANGE,

  /**
   * Decoding the smart contract binary to a byte array failed. Check that the input is a valid hex
   * string.
   */
  ERROR_DECODING_BYTESTRING,

  /**
   * File to create a smart contract was of length zero
   */
  CONTRACT_FILE_EMPTY,

  /**
   * Bytecode for smart contract is of length zero
   */
  CONTRACT_BYTECODE_EMPTY,

  /**
   * Attempt to set negative initial balance
   */
  INVALID_INITIAL_BALANCE,

  /**
   * Special Account Operations should be performed by only Genesis account, return this code if it
   * is not Genesis Account
   */
  ACCOUNT_IS_NOT_GENESIS_ACCOUNT,

  /**
   * The fee payer account doesn't have permission to submit such Transaction
   */
  PAYER_ACCOUNT_UNAUTHORIZED,

  /**
   * FreezeTransactionBody is invalid
   */
  INVALID_FREEZE_TRANSACTION_BODY,

  /**
   * FreezeTransactionBody does not exist
   */
  FREEZE_TRANSACTION_BODY_NOT_FOUND,

  /**
   * Exceeded the number of accounts (both from and to) allowed for crypto transfer list
   */
  TRANSFER_LIST_SIZE_LIMIT_EXCEEDED,

  /**
   * Smart contract result size greater than specified maxResultSize
   */
  RESULT_SIZE_LIMIT_EXCEEDED,

  /**
   * The payer account is not a special account(account 0.0.55)
   */
  NOT_SPECIAL_ACCOUNT,

  /**
   * Negative gas was offered in smart contract call
   */
  CONTRACT_NEGATIVE_GAS,

  /**
   * Negative value / initial balance was specified in a smart contract call / create
   */
  CONTRACT_NEGATIVE_VALUE,

  /**
   * Failed to update fee file
   */
  INVALID_FEE_FILE,

  /**
   * Failed to update exchange rate file
   */
  INVALID_EXCHANGE_RATE_FILE,

  /**
   * Payment tendered for contract local call cannot cover both the fee and the gas
   */
  INSUFFICIENT_LOCAL_CALL_GAS,

  /**
   * Entities with Entity ID below 1000 are not allowed to be deleted
   */
  ENTITY_NOT_ALLOWED_TO_DELETE,

  /**
   * Violating one of these rules: 1) treasury account can update all entities below 0.0.1000, 2)
   * account 0.0.50 can update all entities from 0.0.51 - 0.0.80, 3) Network Function Master Account
   * A/c 0.0.50 - Update all Network Function accounts and perform all the Network Functions listed
   * below, 4) Network Function Accounts: i) A/c 0.0.55 - Update Address Book files (0.0.101/102),
   * ii) A/c 0.0.56 - Update Fee schedule (0.0.111), iii) A/c 0.0.57 - Update Exchange Rate
   * (0.0.112).
   */
  AUTHORIZATION_FAILED,

  /**
   * Fee Schedule Proto uploaded but not valid (append or update is required)
   */
  FILE_UPLOADED_PROTO_INVALID,

  /**
   * Fee Schedule Proto uploaded but not valid (append or update is required)
   */
  FILE_UPLOADED_PROTO_NOT_SAVED_TO_DISK,

  /**
   * Fee Schedule Proto File Part uploaded
   */
  FEE_SCHEDULE_FILE_PART_UPLOADED,

  /**
   * The change on Exchange Rate exceeds Exchange_Rate_Allowed_Percentage
   */
  EXCHANGE_RATE_CHANGE_LIMIT_EXCEEDED,

  /**
   * Contract permanent storage exceeded the currently allowable limit
   */
  MAX_CONTRACT_STORAGE_EXCEEDED,

  /**
   * Transfer Account should not be same as Account to be deleted
   */
  TRANSFER_ACCOUNT_SAME_AS_DELETE_ACCOUNT,

  TOTAL_LEDGER_BALANCE_INVALID,

  /**
   * The expiration date/time on a smart contract may not be reduced
   */
  EXPIRATION_REDUCTION_NOT_ALLOWED,

  /**
   * Gas exceeded currently allowable gas limit per transaction
   */
  MAX_GAS_LIMIT_EXCEEDED,

  /**
   * File size exceeded the currently allowable limit
   */
  MAX_FILE_SIZE_EXCEEDED,

  /**
   * When a valid signature is not provided for operations on account with receiverSigRequired=true
   */
  RECEIVER_SIG_REQUIRED,

  /**
   * The Topic ID specified is not in the system.
   */
  INVALID_TOPIC_ID,

  /**
   * A provided admin key was invalid. Verify the bytes for an ED25519PublicKey are exactly 32 bytes; and the bytes for
   * a compressed ECDSA(secp256k1) key are exactly 33 bytes, with the first byte either 0x02 or 0x03.
   */
  INVALID_ADMIN_KEY,

  /**
   * A provided submit key was invalid.
   */
  INVALID_SUBMIT_KEY,

  /**
   * An attempted operation was not authorized (ie - a deleteTopic for a topic with no adminKey).
   */
  UNAUTHORIZED,

  /**
   * A ConsensusService message is empty.
   */
  INVALID_TOPIC_MESSAGE,

  /**
   * The autoRenewAccount specified is not a valid, active account.
   */
  INVALID_AUTORENEW_ACCOUNT,

  /**
   * An adminKey was not specified on the topic, so there must not be an autoRenewAccount.
   */
  AUTORENEW_ACCOUNT_NOT_ALLOWED,

  /**
   * The topic has expired, was not automatically renewed, and is in a 7 day grace period before the
   * topic will be deleted unrecoverably. This error response code will not be returned until
   * autoRenew functionality is supported by HAPI.
   */
  TOPIC_EXPIRED,

  /**
   * chunk number must be from 1 to total (chunks) inclusive.
   */
  INVALID_CHUNK_NUMBER,

  /**
   * For every chunk, the payer account that is part of initialTransactionID must match the Payer Account of this
   * transaction. The entire initialTransactionID should match the transactionID of the first chunk, but this is not
   * checked or enforced by Hedera except when the chunk number is 1.
   */
  INVALID_CHUNK_TRANSACTION_ID,

  /**
   * Account is frozen and cannot transact with the token
   */
  ACCOUNT_FROZEN_FOR_TOKEN,

  /**
   * An involved account already has more than tokens.maxPerAccount associations with non-deleted tokens.
   */
  TOKENS_PER_ACCOUNT_LIMIT_EXCEEDED,

  /**
   * The token is invalid or does not exist
   */
  INVALID_TOKEN_ID,

  /**
   * Invalid token decimals
   */
  INVALID_TOKEN_DECIMALS,

  /**
   * Invalid token initial supply
   */
  INVALID_TOKEN_INITIAL_SUPPLY,

  /**
   * Treasury Account does not exist or is deleted
   */
  INVALID_TREASURY_ACCOUNT_FOR_TOKEN,

  /**
   * Token Symbol is not UTF-8 capitalized alphabetical string
   */
  INVALID_TOKEN_SYMBOL,

  /**
   * Freeze key is not set on token
   */
  TOKEN_HAS_NO_FREEZE_KEY,

  /**
   * Amounts in transfer list are not net zero
   */
  TRANSFERS_NOT_ZERO_SUM_FOR_TOKEN,

  /**
   * A token symbol was not provided
   */
  MISSING_TOKEN_SYMBOL,

  /**
   * The provided token symbol was too long
   */
  TOKEN_SYMBOL_TOO_LONG,

  /**
   * KYC must be granted and account does not have KYC granted
   */
  ACCOUNT_KYC_NOT_GRANTED_FOR_TOKEN,

  /**
   * KYC key is not set on token
   */
  TOKEN_HAS_NO_KYC_KEY,

  /**
   * Token balance is not sufficient for the transaction
   */
  INSUFFICIENT_TOKEN_BALANCE,

  /**
   * Token transactions cannot be executed on deleted token
   */
  TOKEN_WAS_DELETED,

  /**
   * Supply key is not set on token
   */
  TOKEN_HAS_NO_SUPPLY_KEY,

  /**
   * Wipe key is not set on token
   */
  TOKEN_HAS_NO_WIPE_KEY,

  /**
   * The requested token mint amount would cause an invalid total supply
   */
  INVALID_TOKEN_MINT_AMOUNT,

  /**
   * The requested token burn amount would cause an invalid total supply
   */
  INVALID_TOKEN_BURN_AMOUNT,

  /**
   * A required token-account relationship is missing
   */
  TOKEN_NOT_ASSOCIATED_TO_ACCOUNT,

  /**
   * The target of a wipe operation was the token treasury account
   */
  CANNOT_WIPE_TOKEN_TREASURY_ACCOUNT,

  /**
   * The provided KYC key was invalid.
   */
  INVALID_KYC_KEY,

  /**
   * The provided wipe key was invalid.
   */
  INVALID_WIPE_KEY,

  /**
   * The provided freeze key was invalid.
   */
  INVALID_FREEZE_KEY,

  /**
   * The provided supply key was invalid.
   */
  INVALID_SUPPLY_KEY,

  /**
   * Token Name is not provided
   */
  MISSING_TOKEN_NAME,

  /**
   * Token Name is too long
   */
  TOKEN_NAME_TOO_LONG,

  /**
   * The provided wipe amount must not be negative, zero or bigger than the token holder balance
   */
  INVALID_WIPING_AMOUNT,

  /**
   * Token does not have Admin key set, thus update/delete transactions cannot be performed
   */
  TOKEN_IS_IMMUTABLE,

  /**
   * An associateToken operation specified a token already associated to the account
   */
  TOKEN_ALREADY_ASSOCIATED_TO_ACCOUNT,

  /**
   * An attempted operation is invalid until all token balances for the target account are zero
   */
  TRANSACTION_REQUIRES_ZERO_TOKEN_BALANCES,

  /**
   * An attempted operation is invalid because the account is a treasury
   */
  ACCOUNT_IS_TREASURY,

  /**
   * Same TokenIDs present in the token list
   */
  TOKEN_ID_REPEATED_IN_TOKEN_LIST,

  /**
   * Exceeded the number of token transfers (both from and to) allowed for token transfer list
   */
  TOKEN_TRANSFER_LIST_SIZE_LIMIT_EXCEEDED,

  /**
   * TokenTransfersTransactionBody has no TokenTransferList
   */
  EMPTY_TOKEN_TRANSFER_BODY,

  /**
   * TokenTransfersTransactionBody has a TokenTransferList with no AccountAmounts
   */
  EMPTY_TOKEN_TRANSFER_ACCOUNT_AMOUNTS,

  /**
   * The Scheduled entity does not exist; or has now expired, been deleted, or been executed
   */
  INVALID_SCHEDULE_ID,

  /**
   * The Scheduled entity cannot be modified. Admin key not set
   */
  SCHEDULE_IS_IMMUTABLE,

  /**
   * The provided Scheduled Payer does not exist
   */
  INVALID_SCHEDULE_PAYER_ID,

  /**
   * The Schedule Create Transaction TransactionID account does not exist
   */
  INVALID_SCHEDULE_ACCOUNT_ID,

  /**
   * The provided sig map did not contain any new valid signatures from required signers of the scheduled transaction
   */
  NO_NEW_VALID_SIGNATURES,

  /**
   * The required signers for a scheduled transaction cannot be resolved, for example because they do not exist or have
   * been deleted
   */
  UNRESOLVABLE_REQUIRED_SIGNERS,

  /**
   * Only whitelisted transaction types may be scheduled
   */
  SCHEDULED_TRANSACTION_NOT_IN_WHITELIST,

  /**
   * At least one of the signatures in the provided sig map did not represent a valid signature for any required signer
   */
  SOME_SIGNATURES_WERE_INVALID,

  /**
   * The scheduled field in the TransactionID may not be set to true
   */
  TRANSACTION_ID_FIELD_NOT_ALLOWED,

  /**
   * A schedule already exists with the same identifying fields of an attempted ScheduleCreate (that is, all fields
   * other than scheduledPayerAccountID)
   */
  IDENTICAL_SCHEDULE_ALREADY_CREATED,

  /**
   * A string field in the transaction has a UTF-8 encoding with the prohibited zero byte
   */
  INVALID_ZERO_BYTE_IN_STRING,

  /**
   * A schedule being signed or deleted has already been deleted
   */
  SCHEDULE_ALREADY_DELETED,

  /**
   * A schedule being signed or deleted has already been executed
   */
  SCHEDULE_ALREADY_EXECUTED,

  /**
   * ConsensusSubmitMessage request's message size is larger than allowed.
   */
  MESSAGE_SIZE_TOO_LARGE,

  /**
   * An operation was assigned to more than one throttle group in a given bucket
   */
  OPERATION_REPEATED_IN_BUCKET_GROUPS,

  /**
   * The capacity needed to satisfy all opsPerSec groups in a bucket overflowed a signed 8-byte integral type
   */
  BUCKET_CAPACITY_OVERFLOW,

  /**
   * Given the network size in the address book, the node-level capacity for an operation would never be enough to
   * accept a single request; usually means a bucket burstPeriod should be increased
   */
  NODE_CAPACITY_NOT_SUFFICIENT_FOR_OPERATION,

  /**
   * A bucket was defined without any throttle groups
   */
  BUCKET_HAS_NO_THROTTLE_GROUPS,

  /**
   * A throttle group was granted zero opsPerSec
   */
  THROTTLE_GROUP_HAS_ZERO_OPS_PER_SEC,

  /**
   * The throttle definitions file was updated, but some supported operations were not assigned a bucket
   */
  SUCCESS_BUT_MISSING_EXPECTED_OPERATION,

  /**
   * The new contents for the throttle definitions system file were not valid protobuf
   */
  UNPARSEABLE_THROTTLE_DEFINITIONS,

  /**
   * The new throttle definitions system file were invalid, and no more specific error could be divined
   */
  INVALID_THROTTLE_DEFINITIONS,

  /**
   * The transaction references an account which has passed its expiration without renewal funds available, and
   * currently remains in the ledger only because of the grace period given to expired entities
   */
  ACCOUNT_EXPIRED_AND_PENDING_REMOVAL,

  /**
   * Invalid token max supply
   */
  INVALID_TOKEN_MAX_SUPPLY,

  /**
   * Invalid token nft serial number
   */
  INVALID_TOKEN_NFT_SERIAL_NUMBER,

  /**
   * Invalid nft id
   */
  INVALID_NFT_ID,

  /**
   * Nft metadata is too long
   */
  METADATA_TOO_LONG,

  /**
   * Repeated operations count exceeds the limit
   */
  BATCH_SIZE_LIMIT_EXCEEDED,

  /**
   * The range of data to be gathered is out of the set boundaries
   */
  INVALID_QUERY_RANGE,

  /**
   * A custom fractional fee set a denominator of zero
   */
  FRACTION_DIVIDES_BY_ZERO,

  /**
   * More than 10 custom fees were specified
   */
  CUSTOM_FEES_LIST_TOO_LONG,

  /**
   * Any of the feeCollector accounts for customFees is invalid
   */
  INVALID_CUSTOM_FEE_COLLECTOR,

  /**
   * Any of the token Ids in customFees is invalid
   */
  INVALID_TOKEN_ID_IN_CUSTOM_FEES,

  /**
   * Any of the token Ids in customFees are not associated to feeCollector
   */
  TOKEN_NOT_ASSOCIATED_TO_FEE_COLLECTOR,

  /**
   * A token cannot have more units minted due to its configured supply ceiling
   */
  TOKEN_MAX_SUPPLY_REACHED,

  /**
   * The transaction attempted to move an NFT serial number from an account other than its owner
   */
  SENDER_DOES_NOT_OWN_NFT_SERIAL_NO,

  /**
   * A custom fee schedule entry did not specify either a fixed or fractional fee
   */
  CUSTOM_FEE_NOT_FULLY_SPECIFIED,

  /**
   * Only positive fees may be assessed at this time
   */
  CUSTOM_FEE_MUST_BE_POSITIVE,

  /**
   * Fee schedule key is not set on token
   */
  TOKEN_HAS_NO_FEE_SCHEDULE_KEY,

  /**
   * A fractional custom fee exceeded the range of a 64-bit signed integer
   */
  CUSTOM_FEE_OUTSIDE_NUMERIC_RANGE,

  /**
   * A royalty cannot exceed the total fungible value exchanged for an NFT
   */
  ROYALTY_FRACTION_CANNOT_EXCEED_ONE,

  /**
   * Each fractional custom fee must have its maximum_amount, if specified, at least its minimum_amount
   */
  FRACTIONAL_FEE_MAX_AMOUNT_LESS_THAN_MIN_AMOUNT,

  /**
   * A fee schedule update tried to clear the custom fees from a token whose fee schedule was already empty
   */
  CUSTOM_SCHEDULE_ALREADY_HAS_NO_FEES,

  /**
   * Only tokens of type FUNGIBLE_COMMON can be used to as fee schedule denominations
   */
  CUSTOM_FEE_DENOMINATION_MUST_BE_FUNGIBLE_COMMON,

  /**
   * Only tokens of type FUNGIBLE_COMMON can have fractional fees
   */
  CUSTOM_FRACTIONAL_FEE_ONLY_ALLOWED_FOR_FUNGIBLE_COMMON,

  /**
   * The provided custom fee schedule key was invalid
   */
  INVALID_CUSTOM_FEE_SCHEDULE_KEY,

  /**
   * The requested token mint metadata was invalid
   */
  INVALID_TOKEN_MINT_METADATA,

  /**
   * The requested token burn metadata was invalid
   */
  INVALID_TOKEN_BURN_METADATA,

  /**
   * The treasury for a unique token cannot be changed until it owns no NFTs
   */
  CURRENT_TREASURY_STILL_OWNS_NFTS,

  /**
   * An account cannot be dissociated from a unique token if it owns NFTs for the token
   */
  ACCOUNT_STILL_OWNS_NFTS,

  /**
   * A NFT can only be burned when owned by the unique token's treasury
   */
  TREASURY_MUST_OWN_BURNED_NFT,

  /**
   * An account did not own the NFT to be wiped
   */
  ACCOUNT_DOES_NOT_OWN_WIPED_NFT,

  /**
   * An AccountAmount token transfers list referenced a token type other than FUNGIBLE_COMMON
   */
  ACCOUNT_AMOUNT_TRANSFERS_ONLY_ALLOWED_FOR_FUNGIBLE_COMMON,

  /**
   * All the NFTs allowed in the current price regime have already been minted
   */
  MAX_NFTS_IN_PRICE_REGIME_HAVE_BEEN_MINTED,

  /**
   * The payer account has been marked as deleted
   */
  PAYER_ACCOUNT_DELETED,

  /**
   * The reference chain of custom fees for a transferred token exceeded the maximum length of 2
   */
  CUSTOM_FEE_CHARGING_EXCEEDED_MAX_RECURSION_DEPTH,

  /**
   * More than 20 balance adjustments were to satisfy a CryptoTransfer and its implied custom fee payments
   */
  CUSTOM_FEE_CHARGING_EXCEEDED_MAX_ACCOUNT_AMOUNTS,

  /**
   * The sender account in the token transfer transaction could not afford a custom fee
   */
  INSUFFICIENT_SENDER_ACCOUNT_BALANCE_FOR_CUSTOM_FEE,

  /**
   * Currently no more than 4,294,967,295 NFTs may be minted for a given unique token type
   */
  SERIAL_NUMBER_LIMIT_REACHED,

  /**
   * Only tokens of type NON_FUNGIBLE_UNIQUE can have royalty fees
   */
  CUSTOM_ROYALTY_FEE_ONLY_ALLOWED_FOR_NON_FUNGIBLE_UNIQUE,

  /**
   * The account has reached the limit on the automatic associations count.
   */
  NO_REMAINING_AUTOMATIC_ASSOCIATIONS,

  /**
   * Already existing automatic associations are more than the new maximum automatic associations.
   */
  EXISTING_AUTOMATIC_ASSOCIATIONS_EXCEED_GIVEN_LIMIT,

  /**
   * Cannot set the number of automatic associations for an account more than the maximum allowed
   * token associations tokens.maxPerAccount.
   */
  REQUESTED_NUM_AUTOMATIC_ASSOCIATIONS_EXCEEDS_ASSOCIATION_LIMIT,

  /**
   * Token is paused. This Token cannot be a part of any kind of Transaction until unpaused.
   */
  TOKEN_IS_PAUSED,

  /**
   * Pause key is not set on token
   */
  TOKEN_HAS_NO_PAUSE_KEY,

  /**
   * The provided pause key was invalid
   */
  INVALID_PAUSE_KEY,

  /**
   * The update file in a freeze transaction body must exist.
   */
  FREEZE_UPDATE_FILE_DOES_NOT_EXIST,

  /**
   * The hash of the update file in a freeze transaction body must match the in-memory hash.
   */
  FREEZE_UPDATE_FILE_HASH_DOES_NOT_MATCH,

  /**
   * A FREEZE_UPGRADE transaction was handled with no previous update prepared.
   */
  NO_UPGRADE_HAS_BEEN_PREPARED,

  /**
   * A FREEZE_ABORT transaction was handled with no scheduled freeze.
   */
  NO_FREEZE_IS_SCHEDULED,

  /**
   * The update file hash when handling a FREEZE_UPGRADE transaction differs from the file
   * hash at the time of handling the PREPARE_UPGRADE transaction.
   */
  UPDATE_FILE_HASH_CHANGED_SINCE_PREPARE_UPGRADE,

  /**
   * The given freeze start time was in the (consensus) past.
   */
  FREEZE_START_TIME_MUST_BE_FUTURE,

  /**
   * The prepared update file cannot be updated or appended until either the upgrade has
   * been completed, or a FREEZE_ABORT has been handled.
   */
  PREPARED_UPDATE_FILE_IS_IMMUTABLE,

  /**
   * Once a freeze is scheduled, it must be aborted before any other type of freeze can
   * can be performed.
   */
  FREEZE_ALREADY_SCHEDULED,

  /**
   * If an NMT upgrade has been prepared, the following operation must be a FREEZE_UPGRADE.
   * (To issue a FREEZE_ONLY, submit a FREEZE_ABORT first.)
   */
  FREEZE_UPGRADE_IN_PROGRESS,

  /**
   * If an NMT upgrade has been prepared, the subsequent FREEZE_UPGRADE transaction must
   * confirm the id of the file to be used in the upgrade.
   */
  UPDATE_FILE_ID_DOES_NOT_MATCH_PREPARED,

  /**
   * If an NMT upgrade has been prepared, the subsequent FREEZE_UPGRADE transaction must
   * confirm the hash of the file to be used in the upgrade.
   */
  UPDATE_FILE_HASH_DOES_NOT_MATCH_PREPARED,

  /**
   * Consensus throttle did not allow execution of this transaction. System is throttled at
   * consensus level.
   */
  CONSENSUS_GAS_EXHAUSTED,

  /**
   * A precompiled contract succeeded, but was later reverted.
   */
  REVERTED_SUCCESS,

  /**
   * All contract storage allocated to the current price regime has been consumed.
   */
  MAX_STORAGE_IN_PRICE_REGIME_HAS_BEEN_USED,

  /**
   * An alias used in a CryptoTransfer transaction is not the serialization of a primitive Key
   * message--that is, a Key with a single Ed25519 or ECDSA(secp256k1) public key and no
   * unknown protobuf fields.
   */
  INVALID_ALIAS_KEY,

  /**
   * A fungible token transfer expected a different number of decimals than the involved
   * type actually has.
   */
  UNEXPECTED_TOKEN_DECIMALS,

  /**
   * The transfer account id in CryptoDelete transaction is invalid or does not exist.
   */
  INVALID_TRANSFER_ACCOUNT_ID,

  /**
   * The fee collector account id in TokenFeeScheduleUpdate is invalid or does not exist.
   */
  INVALID_FEE_COLLECTOR_ACCOUNT_ID,

  /**
   * The alias already set on an account cannot be updated using CryptoUpdate transaction.
   */
  ALIAS_IS_IMMUTABLE,

  /**
   * An approved allowance specifies a spender account that is the same as the hbar/token
   * owner account.
   */
  SPENDER_ACCOUNT_SAME_AS_OWNER,

  /**
   * The establishment or adjustment of an approved allowance cause the token allowance
   * to exceed the token maximum supply.
   */
  AMOUNT_EXCEEDS_TOKEN_MAX_SUPPLY,

  /**
   * The specified amount for an approved allowance cannot be negative.
   */
  NEGATIVE_ALLOWANCE_AMOUNT,

  /**
   * The spender does not have an existing approved allowance with the hbar/token owner.
   */
  SPENDER_DOES_NOT_HAVE_ALLOWANCE,

  /**
   * The transfer amount exceeds the current approved allowance for the spender account.
   */
  AMOUNT_EXCEEDS_ALLOWANCE,

  /**
   * The payer account of an approveAllowances or adjustAllowance transaction is attempting
   * to go beyond the maximum allowed number of allowances.
   */
  MAX_ALLOWANCES_EXCEEDED,

  /**
   * No allowances have been specified in the approval transaction.
   */
  EMPTY_ALLOWANCES,

  /**
   * Fungible common token used in NFT allowances
   */
  FUNGIBLE_TOKEN_IN_NFT_ALLOWANCES,

  /**
   * Non fungible token used in fungible token allowances
   */
  NFT_IN_FUNGIBLE_TOKEN_ALLOWANCES,

  /**
   * The account id specified as the owner is invalid or does not exist.
   */
  INVALID_ALLOWANCE_OWNER_ID,

  /**
   * The account id specified as the spender is invalid or does not exist.
   */
  INVALID_ALLOWANCE_SPENDER_ID,

  /**
   * If the account Id specified as the delegating spender is invalid or does not exist.
   */
  INVALID_DELEGATING_SPENDER,

  /**
   * The delegating Spender cannot grant approveForAll allowance on a NFT token type for another spender.
   */
  DELEGATING_SPENDER_CANNOT_GRANT_APPROVE_FOR_ALL,

  /**
   * The delegating Spender cannot grant allowance on a NFT serial for another spender as it doesnt not have
   * approveForAll granted on token-owner.
   */
  DELEGATING_SPENDER_DOES_NOT_HAVE_APPROVE_FOR_ALL,

  /**
   * The scheduled transaction could not be created because it's expiration_time was too far in the future.
   */
  SCHEDULE_EXPIRATION_TIME_TOO_FAR_IN_FUTURE,

  /**
   * The scheduled transaction could not be created because it's expiration_time was less than or equal to the consensus
   * time.
   */
  SCHEDULE_EXPIRATION_TIME_MUST_BE_HIGHER_THAN_CONSENSUS_TIME,

  /**
   * The scheduled transaction could not be created because it would cause throttles to be violated on the specified
   * expiration_time.
   */
  SCHEDULE_FUTURE_THROTTLE_EXCEEDED,

  /**
   * The scheduled transaction could not be created because it would cause the gas limit to be violated on the specified
   * expiration_time.
   */
  SCHEDULE_FUTURE_GAS_LIMIT_EXCEEDED,

  /**
   * The ethereum transaction either failed parsing or failed signature validation, or some other EthereumTransaction
   * error not covered by another response code.
   */
  INVALID_ETHEREUM_TRANSACTION,

  /**
   * EthereumTransaction was signed against a chainId that this network does not support.
   */
  WRONG_CHAIN_ID,

  /**
   * This transaction specified an ethereumNonce that is not the current ethereumNonce of the account.
   */
  WRONG_NONCE,

  /**
   * The ethereum transaction specified an access list, which the network does not support.
   */
  ACCESS_LIST_UNSUPPORTED,

  /**
   * A schedule being signed or deleted has passed it's expiration date and is pending execution if needed and then
   * expiration.
   */
  SCHEDULE_PENDING_EXPIRATION,

  /**
   * A selfdestruct or ContractDelete targeted a contract that is a token treasury.
   */
  CONTRACT_IS_TOKEN_TREASURY,

  /**
   * A selfdestruct or ContractDelete targeted a contract with non-zero token balances.
   */
  CONTRACT_HAS_NON_ZERO_TOKEN_BALANCES,

  /**
   * A contract referenced by a transaction is "detached"; that is, expired and lacking any
   * hbar funds for auto-renewal payment---but still within its post-expiry grace period.
   */
  CONTRACT_EXPIRED_AND_PENDING_REMOVAL,

  /**
   * A ContractUpdate requested removal of a contract's auto-renew account, but that contract has
   * no auto-renew account.
   */
  CONTRACT_HAS_NO_AUTO_RENEW_ACCOUNT,

  /**
   * A delete transaction submitted via HAPI set permanent_removal=true
   */
  PERMANENT_REMOVAL_REQUIRES_SYSTEM_INITIATION,

  /**
   * A CryptoCreate or ContractCreate used the deprecated proxyAccountID field.
   */
  PROXY_ACCOUNT_ID_FIELD_IS_DEPRECATED,

  /**
   * An account set the staked_account_id to itself in CryptoUpdate or ContractUpdate transactions.
   */
  SELF_STAKING_IS_NOT_ALLOWED,

  /**
   * The staking account id or staking node id given is invalid or does not exist.
   */
  INVALID_STAKING_ID,

  /**
   * Native staking, while implemented, has not yet enabled by the council.
   */
  STAKING_NOT_ENABLED,

  /**
   * The range provided in UtilPrng transaction is negative.
   */
  INVALID_PRNG_RANGE,

  /**
   * The maximum number of entities allowed in the current price regime have been created.
   */
  MAX_ENTITIES_IN_PRICE_REGIME_HAVE_BEEN_CREATED,

  /**
   * The full prefix signature for precompile is not valid
   */
  INVALID_FULL_PREFIX_SIGNATURE_FOR_PRECOMPILE,

  /**
   * The combined balances of a contract and its auto-renew account (if any) did not cover
   * the rent charged for net new storage used in a transaction.
   */
  INSUFFICIENT_BALANCES_FOR_STORAGE_RENT,

  /**
   * A contract transaction tried to use more than the allowed number of child records, via
   * either system contract records or internal contract creations.
   */
  MAX_CHILD_RECORDS_EXCEEDED,

  /**
   * The combined balances of a contract and its auto-renew account (if any) or balance of an account did not cover
   * the auto-renewal fees in a transaction.
   */
  INSUFFICIENT_BALANCES_FOR_RENEWAL_FEES,

  /**
   * A transaction's protobuf message includes unknown fields; could mean that a client expects not-yet-released
   * functionality to be available.
   */
  TRANSACTION_HAS_UNKNOWN_FIELDS,

  /**
   * The account cannot be modified. Account's key is not set
   */
  ACCOUNT_IS_IMMUTABLE,

  /**
   * An alias that is assigned to an account or contract cannot be assigned to another account or contract.
   */
  ALIAS_ALREADY_ASSIGNED,

  /**
   * A provided metadata key was invalid. Verification includes, for example, checking the size of Ed25519 and
   * ECDSA(secp256k1) public keys.
   */
  INVALID_METADATA_KEY,

  /**
   * Token Metadata is not provided
   */
  MISSING_TOKEN_METADATA,

  /**
   * NFT serial numbers are missing in the TokenUpdateNftsTransactionBody
   */
  MISSING_SERIAL_NUMBERS,

  /**
   * The maximum automatic associations value is not valid. The most common cause for this error is a value less than
   * `-1`.
   */
  INVALID_MAX_AUTO_ASSOCIATIONS,

  /**
   * The transaction attempted to use duplicate `TokenReference`.<br/>
   * This affects `TokenReject` attempting to reject same token reference more than once.
   */
  TOKEN_REFERENCE_REPEATED,

  /**
   * The account id specified as the owner in `TokenReject` is invalid or does not exist.
   */
  INVALID_OWNER_ID,

  /**
   * The transaction attempted to use empty `TokenReference` list.
   */
  EMPTY_TOKEN_REFERENCE_LIST,

  /**
   * The transaction attempted to the use an empty List of `PendingAirdropId`.
   */
  EMPTY_PENDING_AIRDROP_ID_LIST,

  /**
   * The transaction attempted to the same `PendingAirdropId` twice.
   */
  PENDING_AIRDROP_ID_REPEATED,

  /**
   * The transaction attempted to use more than the allowed number of `PendingAirdropId`.
   */
  PENDING_AIRDROP_ID_LIST_TOO_LONG,

  /**
   * A pending airdrop already exists for the specified NFT.
   */
  PENDING_NFT_AIRDROP_ALREADY_EXISTS,

  /**
   * The identified account is sender for one or more pending airdrop(s)
   * and cannot be deleted.
   * Requester should cancel all pending airdrops before resending
   * this transaction.
   */
  ACCOUNT_HAS_PENDING_AIRDROPS,

  /**
   * Consensus throttle did not allow execution of this transaction.
   * The transaction should be retried after a modest delay.
   */
  THROTTLED_AT_CONSENSUS,

  /**
   * The provided pending airdrop id is invalid.
   * This pending airdrop MAY already be claimed or cancelled.
   * The client SHOULD query a mirror node to determine the current status of
   * the pending airdrop.
   */
  INVALID_PENDING_AIRDROP_ID,

  /**
   * The token to be airdropped has a fallback royalty fee and cannot be
   * sent or claimed via an airdrop transaction.
   */
  TOKEN_AIRDROP_WITH_FALLBACK_ROYALTY,

  /**
   * This airdrop claim is for a pending airdrop with an invalid token.
   * The token might be deleted, or the sender may not have enough tokens
   * to fulfill the offer.
   * The client SHOULD query mirror node to determine the status of the pending
   * airdrop and whether the sender can fulfill the offer.
   */
  INVALID_TOKEN_IN_PENDING_AIRDROP,

  /**
   * A transaction failed because the consensus node identified is
   * deleted from the address book.
   */
  NODE_DELETED,

  /**
   * A transaction failed because the consensus node identified is not valid or
   * does not exist in state.
   */
  INVALID_NODE_ID,

  /**
   * A transaction failed because one or more entries in the list of
   * service endpoints for the `gossip_endpoint` field is invalid.<br/>
   * The most common cause for this response is a service endpoint that has
   * the domain name (DNS) set rather than address and port.
   */
  INVALID_GOSSIP_ENDPOINT,

  /**
   * A transaction failed because the node account identifier provided
   * does not exist or is not valid.<br/>
   * One common source of this error is providing a node account identifier
   * using the "alias" form rather than "numeric" form.
   */
  INVALID_NODE_ACCOUNT_ID,

  /**
   * A transaction failed because the description field cannot be encoded
   * as UTF-8 or is more than 100 bytes when encoded.
   */
  INVALID_NODE_DESCRIPTION,

  /**
   * A transaction failed because one or more entries in the list of
   * service endpoints for the `service_endpoint` field is invalid.<br/>
   * The most common cause for this response is a service endpoint that has
   * the domain name (DNS) set rather than address and port.
   */
  INVALID_SERVICE_ENDPOINT,

  /**
   * A transaction failed because the TLS certificate provided for the
   * node is missing or invalid.<br/>
   * The certificate MUST be a TLS certificate of a type permitted for gossip
   * signatures.<br/>
   * The value presented MUST be a UTF-8 NFKD encoding of the TLS
   * certificate.<br/>
   * The certificate encoded MUST be in PEM format.<br/>
   * The `gossip_ca_certificate` field is REQUIRED and MUST NOT be empty.
   */
  INVALID_GOSSIP_CA_CERTIFICATE,

  /**
   * A transaction failed because the hash provided for the gRPC certificate
   * is present but invalid.<br/>
   * The `grpc_certificate_hash` MUST be a SHA-384 hash.<br/>
   * The input hashed MUST be a UTF-8 NFKD encoding of the actual TLS
   * certificate.<br/>
   * The certificate to be encoded MUST be in PEM format.
   */
  INVALID_GRPC_CERTIFICATE,

  /**
   * The maximum number of nodes allowed in the address book have been created.
   */
  MAX_NODES_CREATED,

  /**
   * In ServiceEndpoint, domain_name and ipAddressV4 are mutually exclusive
   */
  IP_FQDN_CANNOT_BE_SET_FOR_SAME_ENDPOINT,

  /**
   *  Fully qualified domain name is not allowed in gossip_endpoint
   */
  GOSSIP_ENDPOINT_CANNOT_HAVE_FQDN,

  /**
   * In ServiceEndpoint, domain_name size too large
   */
  FQDN_SIZE_TOO_LARGE,

  /**
   * ServiceEndpoint is invalid
   */
  INVALID_ENDPOINT,

  /**
   * The number of gossip endpoints exceeds the limit
   */
  GOSSIP_ENDPOINTS_EXCEEDED_LIMIT

};

/**
 * Map of protobuf ResponseCodeEnums to the corresponding Status.
 */
extern const std::unordered_map<proto::ResponseCodeEnum, Status> gProtobufResponseCodeToStatus;

/**
 * Map of Status to its corresponding ResponseCodeEnum protobuf.
 */
extern const std::unordered_map<Status, proto::ResponseCodeEnum> gStatusToProtobufResponseCode;

/**
 * Map of Status to its corresponding string.
 */
[[maybe_unused]] extern const std::unordered_map<Status, std::string> gStatusToString;

} // namespace Hedera

#endif // HEDERA_SDK_CPP_STATUS_H_
