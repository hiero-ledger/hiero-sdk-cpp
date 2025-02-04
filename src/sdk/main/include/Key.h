// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_KEY_H_
#define HIERO_SDK_CPP_KEY_H_

#include <cstddef>
#include <memory>
#include <vector>

namespace proto
{
class Key;
}

namespace Hiero
{
/**
 * A generic base class for the signing authority or key that entities in Hiero may have.
 */
class Key
{
public:
  virtual ~Key() = default;

  /**
   * Construct a Key object from a Key protobuf object.
   *
   * @param proto The Key protobuf object from which to create a Key object.
   * @return A pointer to the created PublicKey object.
   * @throws std::invalid_argument If the Key protobuf type is not recognized.
   */
  [[nodiscard]] static std::unique_ptr<Key> fromProtobuf(const proto::Key& proto);

  /**
   * Create a clone of this Key object.
   *
   * @return A pointer to the created clone of this Key.
   */
  [[nodiscard]] virtual std::unique_ptr<Key> clone() const = 0;

  /**
   * Construct a Key protobuf object from this Key object.
   *
   * @return A pointer to the created Key protobuf object filled with this Key object's data.
   */
  [[nodiscard]] virtual std::unique_ptr<proto::Key> toProtobufKey() const = 0;

  /**
   * Get the byte representation of this Key.
   *
   * @return The byte representation of this Key.
   */
  [[nodiscard]] virtual std::vector<std::byte> toBytes() const;

protected:
  Key() = default;

  /**
   * Prevent public copying and moving to prevent slicing. Use the 'clone()' virtual method instead.
   */
  Key(const Key&) = default;
  Key& operator=(const Key&) = default;
  Key(Key&&) = default;
  Key& operator=(Key&&) = default;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_KEY_H_
