// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_RLP_ITEM_H_
#define HIERO_SDK_CPP_IMPL_RLP_ITEM_H_

#include <cstddef>
#include <string_view>
#include <vector>

namespace Hiero
{
/**
 * Helper class used to hold RLP data and do RLP serialization for Ethereum data and transactions.
 */
class RLPItem
{
public:
  /**
   * Enumeration of possible RLP items. An RLPItem is defined as follows:
   *  - A value (i.e. a string or byte array) is an item
   *  - A list of items is an item
   */
  enum class RLPType
  {
    VALUE_TYPE,
    LIST_TYPE
  };

  RLPItem() = default;

  /**
   * Construct with a specified RLPType.
   *
   * @param type The type of RLPItem to set.
   */
  explicit RLPItem(RLPType type);

  /**
   * Construct with a specified RLPItem value.
   *
   * @param value The value of the RLPItem to set.
   */
  explicit RLPItem(std::vector<std::byte> value);
  explicit RLPItem(std::string_view value);

  /*
   * Get the raw byte-representation of a number.
   *
   * @param num The number of which to get the byte-representation.
   * @return The byte-representation of the input number.
   */
  static std::vector<std::byte> encodeBinary(size_t num);

  /*
   * Encode a number to its byte-representation, given an offset.
   *
   * @param num    The number to encode.
   * @param offset The offset of the length.
   * @return The byte-encoding of the number.
   */
  static std::vector<std::byte> encodeLength(size_t num, unsigned char offset);

  /**
   * Clear this RLPItem.
   */
  void clear();

  /**
   * Assign the value of this RLPItem.
   *
   * @param value The value to assign to this RLPItem.
   */
  void assign(const std::vector<std::byte>& value);
  void assign(std::string_view value);

  /**
   * Set the type of this RLPItem. This will clear values if a different type is set.
   *
   * @param type The RLPType to assign to this RLPItem.
   */
  void setType(RLPType type);

  /**
   * Determine if this RLPItem is a certain type.
   *
   * @param type The type to which to compare this RLPItem.
   * @return \c TRUE if this RLPItem is input type, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool isType(RLPType type) const { return mType == type; }

  /**
   * Get the value of this RLPItem.
   *
   * @return The value of this RLPItem. Returns empty if this RLPItem is of RLPType::LIST_TYPE.
   */
  [[nodiscard]] inline std::vector<std::byte> getValue() const { return mValue; }

  /**
   * Get the RLPItem values of this RLPItem.
   *
   * @return The RLPItems of this RLPItem. Returns empty if this RLPItem is of RLPType::VALUE_TYPE.
   */
  [[nodiscard]] inline std::vector<RLPItem> getValues() const { return mValues; }

  /**
   * Add a value to this RLPItem's values.
   *
   * @param value The RLPItem to add to this RLPItem.
   */
  void pushBack(const RLPItem& value);
  void pushBack(const std::vector<std::byte>& value);
  void pushBack(std::string_view value);

  /**
   * Get the total size in bytes of this RLPItem. This will be either the size of mValue, or the size of all the
   * RLPItems in mValues added together, depending on the type of this RLPItem.
   *
   * @return The size of this RLPItem, in bytes.
   */
  [[nodiscard]] size_t size() const;

  /**
   * Encode this RLPItem to a byte array.
   *
   * @return This RLPItem RLP-encoded to a byte array.
   */
  [[nodiscard]] std::vector<std::byte> write() const;

  /**
   * Decode a byte array to to this RLPItem.
   *
   * @param bytes The byte array to decode.
   * @throws std::invalid_argument If the input byte array is malformed and cannot be decoded.
   */
  void read(const std::vector<std::byte>& bytes);

private:
  /**
   * Decode a byte array and put its values in this RLPItem. Helper function used by 'read()'.
   *
   * @param bytes The byte array to decode.
   * @param index The index to begin decoding.
   */
  void decodeBytes(const std::vector<std::byte>& bytes, long& index);

  /**
   * The type of RLPItem this RLPItem is.
   */
  RLPType mType = RLPType::VALUE_TYPE;

  /**
   * The value of this RLPItem.
   */
  std::vector<std::byte> mValue;

  /**
   * The list of values of this RLPItem.
   */
  std::vector<RLPItem> mValues;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_IMPL_RLP_ITEM_H_
