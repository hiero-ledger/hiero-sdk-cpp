// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CONTRACT_FUNCTION_RESULT_H_
#define HIERO_SDK_CPP_CONTRACT_FUNCTION_RESULT_H_

#include "AccountId.h"
#include "ContractId.h"
#include "ContractLogInfo.h"
#include "ContractNonceInfo.h"
#include "EvmAddress.h"
#include "Hbar.h"

#include <cstddef>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

namespace proto
{
class ContractFunctionResult;
}

namespace Hiero
{
/**
 * Class to represent the result of invoking a contract via ContractCallQuery, ContractExecuteTransaction, or the result
 * of a contract constructor being called by ContractCreateTransaction.
 */
class ContractFunctionResult
{
public:
  /**
   * The error prefix representing an error in contract execution.
   */
  static inline const std::vector<std::byte> ERROR_PREFIX = { std::byte(8),
                                                              std::byte(-61),
                                                              std::byte(121),
                                                              std::byte(-96) };

  /**
   * Construct a ContractFunctionResult object from a ContractFunctionResult protobuf object.
   *
   * @param proto The ContractFunctionResult protobuf object from which to construct a ContractFunctionResult object.
   * @return The constructed ContractFunctionResult object.
   */
  [[nodiscard]] static ContractFunctionResult fromProtobuf(const proto::ContractFunctionResult& proto);

  /**
   * Construct a ContractFunctionResult object from a byte array.
   *
   * @param bytes The byte array from which to construct a ContractFunctionResult object.
   * @return The constructed ContractFunctionResult object.
   */
  [[nodiscard]] static ContractFunctionResult fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a ContractFunctionResult protobuf object from this ContractFunctionResult object.
   *
   * @return A pointer to the created ContractFunctionResult protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::ContractFunctionResult> toProtobuf() const;

  /**
   * Construct a representative byte array from this ContractFunctionResult object.
   *
   * @return The byte array representing this ContractFunctionResult object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this ContractFunctionResult object.
   *
   * @return The string representation of this ContractFunctionResult object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this ContractFunctionResult to an output stream.
   *
   * @param os     The output stream.
   * @param result The ContractFunctionResult to print.
   * @return The output stream with this ContractFunctionResult written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const ContractFunctionResult& result);

  /**
   * Get the value at the input index as a string.
   *
   * @param index The index at which to retrieve the string.
   * @return The value at the input index as a string.
   */
  [[nodiscard]] std::string getString(int index) const;

  /**
   * Get the value at the input index as a string array.
   *
   * @param index The index at which to retrieve the string array.
   * @return The value at the input index as a string array.
   */
  [[nodiscard]] std::vector<std::string> getStringArray(int index) const;

  /**
   * Get the value at the input index as a byte array.
   *
   * @param index The index at which to retrieve the byte array.
   * @return The value at the input index as a byte array.
   */
  [[nodiscard]] std::vector<std::byte> getByteArray(int index) const;

  /**
   * Get the value at the input index as a Solidity 32-byte value.
   *
   * @param index The index at which to retrieve the 32-byte value.
   * @return The value at the input index as a 32-byte value.
   */
  [[nodiscard]] std::vector<std::byte> getBytes32(int index) const;

  /**
   * Get the value at the input index as a boolean.
   *
   * @param index The index at which to retrieve the boolean value.
   * @return The value at the input index as a bool value.
   */
  [[nodiscard]] bool getBool(int index) const;

  /**
   * Get the value at the input index as a signed 8-bit integer. If the actual value is wider, it will be truncated to
   * the last byte.
   *
   * @param index The index at which to retrieve the signed 8-bit integer value.
   * @return The value at the input index as a signed 8-bit integer value.
   */
  [[nodiscard]] int8_t getInt8(int index) const;

  /**
   * Get the value at the input index as a signed 32-bit integer. If the actual value is wider, it will be truncated to
   * the last 4 bytes.
   *
   * @param index The index at which to retrieve the signed 32-bit integer value.
   * @return The value at the input index as a signed 32-bit integer value.
   */
  [[nodiscard]] int32_t getInt32(int index) const;

  /**
   * Get the value at the input index as a signed 64-bit integer. If the actual value is wider, it will be truncated to
   * the last 8 bytes.
   *
   * @param index The index at which to retrieve the signed 64-bit integer value.
   * @return The value at the input index as a signed 64-bit integer value.
   */
  [[nodiscard]] int64_t getInt64(int index) const;

  /**
   * Get the value at the input index as an unsigned 8-bit integer. If the actual value is wider, it will be truncated
   * to the last byte.
   *
   * @param index The index at which to retrieve the unsigned 8-bit integer value.
   * @return The value at the input index as an unsigned 8-bit integer value.
   */
  [[nodiscard]] uint8_t getUint8(int index) const;

  /**
   * Get the value at the input index as an unsigned 32-bit integer. If the actual value is wider, it will be truncated
   * to the last 4 bytes.
   *
   * @param index The index at which to retrieve the unsigned 32-bit integer value.
   * @return The value at the input index as an unsigned 32-bit integer value.
   */
  [[nodiscard]] uint32_t getUint32(int index) const;

  /**
   * Get the value at the input index as an unsigned 64-bit integer. If the actual value is wider, it will be truncated
   * to the last 8 bytes.
   *
   * @param index The index at which to retrieve the unsigned 64-bit integer value.
   * @return The value at the input index as an unsigned 64-bit integer value.
   */
  [[nodiscard]] uint64_t getUint64(int index) const;

  /**
   * Get the value at the input index as a Solidity address.
   *
   * @param index The index at which to retrieve the Solidity address value.
   * @return The value at the input index as a Solidity address value.
   */
  [[nodiscard]] std::string getAddress(int index) const;

  /**
   * The ID of the contract whose functions was called.
   */
  ContractId mContractId;

  /**
   * The result returned by the function.
   */
  std::vector<std::byte> mContractCallResult;

  /**
   * The error message, in case there was an error during smart contract execution.
   */
  std::string mErrorMessage;

  /**
   * The bloom filter for the record.
   */
  std::vector<std::byte> mBloom;

  /**
   * The units of gas used to execute the contract.
   */
  uint64_t mGasUsed = 0ULL;

  /**
   * The log info for events returned by the function.
   */
  std::vector<ContractLogInfo> mLogs;

  /**
   * The created contract's 20-byte EVM address.
   */
  std::optional<EvmAddress> mEvmAddress;

  /**
   * The amount of gas available for the call, aka the gasLimit.
   *
   * This field should only be populated when the paired TransactionBody in the record stream is not a
   * ContractCreateTransactionBody or a ContractCallTransactionBody.
   */
  uint64_t mGas = 0ULL;

  /**
   * The amount sent (the function must be payable if this is nonzero).
   *
   * This field should only be populated when the paired TransactionBody in the record stream is not a
   * ContractCreateTransactionBody or a ContractCallTransactionBody.
   */
  Hbar mHbarAmount;

  /**
   * The parameters passed into the contract call.
   *
   * This field should only be populated when the paired TransactionBody in the record stream is not a
   * ContractCreateTransactionBody or a ContractCallTransactionBody.
   */
  std::vector<std::byte> mFunctionParameters;

  /**
   * The account that is the "sender." If not present it is the account ID from the transaction ID.
   *
   * This field should only be populated when the paired TransactionBody in the record stream is not a
   * ContractCreateTransactionBody or a ContractCallTransactionBody.
   */
  AccountId mSenderAccountId;

  /**
   * A vector of updated contract account nonces containing the new nonce value for each contract account. This is
   * always empty in a ContractCallLocalResponse#ContractFunctionResult message, since no internal creations can happen
   * in a static EVM call.
   */
  std::vector<ContractNonceInfo> mContractNonces;

  /**
   * If not null this field specifies what the value of the signer account nonce is post transaction execution.
   * For transactions that don't update the signer nonce, this field should be null.
   */
  std::optional<int64_t> mSignerNonce;

private:
  /**
   * Get bytes that are a dynamic size.
   *
   * @param index The index at which to find the bytes.
   * @return The bytes of the proper size.
   */
  [[nodiscard]] std::vector<std::byte> getDynamicBytes(int index) const;

  /**
   * Get the 32-bit integer value at a specific index.
   *
   * @param index The index from which to get the 32-bit integer.
   * @return The 32-bit integer at the input index.
   */
  [[nodiscard]] int getIntValueAt(int index) const;

  /**
   * Get the bytes at a specific start index all the way to a specific end index.
   *
   * @param start The starting index of the bytes to retrieve.
   * @param end   The ending index of the bytes to retrieve.
   * @return The bytes between the start and end indices.
   */
  [[nodiscard]] std::vector<std::byte> getByteString(int start, int end) const;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CONTRACT_FUNCTION_RESULT_H_
