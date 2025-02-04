// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TRANSACTION_FEE_SCHEDULE_H_
#define HIERO_SDK_CPP_TRANSACTION_FEE_SCHEDULE_H_

#include "FeeData.h"
#include "RequestType.h"

#include <cstddef>
#include <string>
#include <vector>

namespace proto
{
class TransactionFeeSchedule;
}

namespace Hiero
{
/**
 * The fees for a specific transaction or query based on the fee data.
 */
class TransactionFeeSchedule
{
public:
  /**
   * Construct a TransactionFeeSchedule object from a TransactionFeeSchedule protobuf object.
   *
   * @param proto The TransactionFeeSchedule protobuf object from which to construct an TransactionFeeSchedule object.
   * @return The constructed TransactionFeeSchedule object.
   */
  [[nodiscard]] static TransactionFeeSchedule fromProtobuf(const proto::TransactionFeeSchedule& proto);

  /**
   * Construct a TransactionFeeSchedule object from a byte array.
   *
   * @param bytes The byte array from which to construct an TransactionFeeSchedule object.
   * @return The constructed TransactionFeeSchedule object.
   */
  [[nodiscard]] static TransactionFeeSchedule fromBytes(const std::vector<std::byte>& bytes);

  /**
   * Construct a TransactionFeeSchedule protobuf object from this TransactionFeeSchedule object.
   *
   * @return A pointer to the created TransactionFeeSchedule protobuf object.
   */
  [[nodiscard]] std::unique_ptr<proto::TransactionFeeSchedule> toProtobuf() const;

  /**
   * Construct a representative byte array from this TransactionFeeSchedule object.
   *
   * @return The byte array representing this TransactionFeeSchedule object.
   */
  [[nodiscard]] std::vector<std::byte> toBytes() const;

  /**
   * Construct a string representation of this TransactionFeeSchedule object.
   *
   * @return The string representation of this TransactionFeeSchedule object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Set the request type.
   *
   * @param type The request type to set.
   * @return A reference to this TransactionFeeSchedule with the newly-set request type.
   */
  inline TransactionFeeSchedule& setRequestType(RequestType type)
  {
    mRequestType = type;
    return *this;
  }

  /**
   * Add a fee to this TransactionFeeSchedule.
   *
   * @param fee The fee to add.
   * @return A reference to this TransactionFeeSchedule with the newly-added fee.
   */
  inline TransactionFeeSchedule& addFee(const FeeData& fee)
  {
    mFees.push_back(fee);
    return *this;
  }

  /**
   * Get the request type.
   *
   * @return The request type.
   */
  [[nodiscard]] inline RequestType getRequestType() const { return mRequestType; }

  /**
   * Get the fees.
   *
   * @return The fees.
   */
  [[nodiscard]] inline std::vector<FeeData> getFees() const { return mFees; }

private:
  /**
   * The request to which this fee schedule applies.
   */
  RequestType mRequestType;

  /**
   * The resource price coefficients.
   */
  std::vector<FeeData> mFees;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TRANSACTION_FEE_SCHEDULE_H_
