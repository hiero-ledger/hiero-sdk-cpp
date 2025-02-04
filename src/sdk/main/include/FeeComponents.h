// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FEE_COMPONENTS_H_
#define HIERO_SDK_CPP_FEE_COMPONENTS_H_

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace proto
{
class FeeComponents;
}

namespace Hiero
{
/**
 * A set of prices the nodes use in determining transaction and query fees, and constants involved in fee calculations.
 * Nodes multiply the amount of resources consumed by a transaction or query by the corresponding price to calculate the
 * appropriate fee. Units are one-thousandth of a tinyCent.
 */
class FeeComponents
{
public:
  /**
   * Construct an FeeComponents object from a FeeComponents protobuf object.
   *
   * @param proto The FeeComponents protobuf object from which to construct an FeeComponents object.
   * @return The constructed FeeComponents object.
   */
  [[nodiscard]] static FeeComponents fromProtobuf(const proto::FeeComponents& proto);

  /**
   * Construct an FeeComponents object from a byte array.
   *
   * @param bytes The byte array from which to construct an FeeComponents object.
   * @return The constructed FeeComponents object.
   */
  [[nodiscard]] static FeeComponents fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a FeeComponents protobuf object from this FeeComponents object.
   *
   * @return A pointer to the created FeeComponents protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::FeeComponents> toProtobuf() const;

  /**
   * Construct a representative byte array from this FeeComponents object.
   *
   * @return The byte array representing this FeeComponents object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this FeeComponents object.
   *
   * @return The string representation of this FeeComponents object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Set the minimum component.
   *
   * @param min The minimum component to set.
   * @return A reference to this FeeComponents with the newly-set minimum.
   */
  inline FeeComponents& setMin(int64_t min)
  {
    mMin = min;
    return *this;
  }

  /**
   * Set the maximum component.
   *
   * @param max The maximum component to set.
   * @return A reference to this FeeComponents with the newly-set maximum.
   */
  inline FeeComponents& setMax(int64_t max)
  {
    mMax = max;
    return *this;
  }

  /**
   * Set the constant component.
   *
   * @param constant The constant component to set.
   * @return A reference to this FeeComponents with the newly-set constant.
   */
  inline FeeComponents& setConstant(int64_t constant)
  {
    mConstant = constant;
    return *this;
  }

  /**
   * Set the transaction bandwidth bytes.
   *
   * @param bytes The transaction bandwidth bytes to set.
   * @return A reference to this FeeComponents with the newly-set transaction bandwidth bytes.
   */
  inline FeeComponents& setTransactionBandwidthBytes(int64_t bytes)
  {
    mTransactionBandwidthBytes = bytes;
    return *this;
  }

  /**
   * Set the transaction verification price per signature.
   *
   * @param price The transaction verification price per signature to set.
   * @return A reference to this FeeComponents with the newly-set transaction verification price per signature.
   */
  inline FeeComponents& setTransactionVerification(int64_t price)
  {
    mTransactionVerification = price;
    return *this;
  }

  /**
   * Set the price of RAM consumed in byte hours.
   *
   * @param price The price of RAM consumed in byte hours to set.
   * @return A reference to this FeeComponents with the newly-set price of RAM consumed in byte hours.
   */
  inline FeeComponents& setTransactionRamByteHour(int64_t price)
  {
    mTransactionRamByteHour = price;
    return *this;
  }

  /**
   * Set the price of storage consumed in byte hours.
   *
   * @param price The price of storage consumed in byte hours to set.
   * @return A reference to this FeeComponents with the newly-set price of storage consumed in byte hours.
   */
  inline FeeComponents& setTransactionStorageByteHour(int64_t price)
  {
    mTransactionStorageByteHour = price;
    return *this;
  }

  /**
   * Set the price of computation in gas.
   *
   * @param price The price of computation in gas to set.
   * @return A reference to this FeeComponents with the newly-set the price of computation in gas.
   */
  inline FeeComponents& setContractTransactionGas(int64_t price)
  {
    mContractTransactionGas = price;
    return *this;
  }

  /**
   * Set the price per hbar transferred.
   *
   * @param price The price per hbar transferred to set.
   * @return A reference to this FeeComponents with the newly-set the price per hbar transferred.
   */
  inline FeeComponents& setTransferVolumeHbar(int64_t price)
  {
    mTransferVolumeHbar = price;
    return *this;
  }

  /**
   * Set the price for data retrieved in bytes.
   *
   * @param price The price for data retrieved in bytes to set.
   * @return A reference to this FeeComponents with the newly-set the price for data retrieved in bytes.
   */
  inline FeeComponents& setResponseMemoryByte(int64_t price)
  {
    mResponseMemoryByte = price;
    return *this;
  }

  /**
   * Set the price for data retrieved from disk.
   *
   * @param price The price for data retrieved from disk to set.
   * @return A reference to this FeeComponents with the newly-set the price for data retrieved from disk.
   */
  inline FeeComponents& setResponseDiskByte(int64_t price)
  {
    mResponseDiskByte = price;
    return *this;
  }

  /**
   * Get the minimum component.
   *
   * @return The minimum component.
   */
  [[nodiscard]] inline int64_t getMin() const { return mMin; }

  /**
   * Get the maximum component.
   *
   * @return The maximum component.
   */
  [[nodiscard]] inline int64_t getMax() const { return mMax; }

  /**
   * Get the constant component.
   *
   * @return The constant component.
   */
  [[nodiscard]] inline int64_t getConstant() const { return mConstant; }

  /**
   * Get the transaction bandwidth bytes.
   *
   * @return The transaction bandwidth bytes.
   */
  [[nodiscard]] inline int64_t getTransactionBandwidthBytes() const { return mTransactionBandwidthBytes; }

  /**
   * Get the transaction verification price per signature.
   *
   * @return The transaction verification price per signature.
   */
  [[nodiscard]] inline int64_t getTransactionVerification() const { return mTransactionVerification; }

  /**
   * Get the price of ram consumed in byte hours.
   *
   * @return The price of ram consumed in byte hours.
   */
  [[nodiscard]] inline int64_t getTransactionRamByteHour() const { return mTransactionRamByteHour; }

  /**
   * Get the price of storage consumed in byte hours.
   *
   * @return The price of storage consumed in byte hours.
   */
  [[nodiscard]] inline int64_t getTransactionStorageByteHour() const { return mTransactionStorageByteHour; }

  /**
   * Get the price of computation in gas.
   *
   * @return The price of computation in gas.
   */
  [[nodiscard]] inline int64_t getContractTransactionGas() const { return mContractTransactionGas; }

  /**
   * Get the price per hbar transferred.
   *
   * @return The price per hbar transferred.
   */
  [[nodiscard]] inline int64_t getTransferVolumeHbar() const { return mTransferVolumeHbar; }

  /**
   * Get the price for data retrieved in bytes.
   *
   * @return The price for data retrieved in bytes.
   */
  [[nodiscard]] inline int64_t getResponseMemoryByte() const { return mResponseMemoryByte; }

  /**
   * Get the price for data retrieved from disk.
   *
   * @return The price for data retrieved from disk.
   */
  [[nodiscard]] inline int64_t getResponseDiskByte() const { return mResponseDiskByte; }

private:
  /**
   * A minimum, the calculated fee must be greater than this value.
   */
  int64_t mMin = 0LL;

  /**
   * A maximum, the calculated fee must be less than this value.
   */
  int64_t mMax = 0LL;

  /**
   * A constant contribution to the fee.
   */
  int64_t mConstant = 0LL;

  /**
   * The price of bandwidth consumed by a transaction, measured in bytes.
   */
  int64_t mTransactionBandwidthBytes = 0LL;

  /**
   * The price per signature verification for a transaction.
   */
  int64_t mTransactionVerification = 0LL;

  /**
   * The price of RAM consumed by a transaction, measured in byte-hours.
   */
  int64_t mTransactionRamByteHour = 0LL;

  /**
   * The price of storage consumed by a transaction, measured in byte-hours.
   */
  int64_t mTransactionStorageByteHour = 0LL;

  /**
   * The price of computation for a smart contract transaction, measured in gas.
   */
  int64_t mContractTransactionGas = 0LL;

  /**
   * The price per hbar transferred for a transfer.
   */
  int64_t mTransferVolumeHbar = 0LL;

  /**
   * The price of bandwidth for data retrieved from memory for a response, measured in bytes.
   */
  int64_t mResponseMemoryByte = 0LL;

  /**
   * The price of bandwidth for data retrieved from disk for a response, measured in bytes.
   */
  int64_t mResponseDiskByte = 0LL;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_COMPONENTS_H_
