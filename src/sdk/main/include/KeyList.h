// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_KEYLIST_H_
#define HIERO_SDK_CPP_KEYLIST_H_

#include "Key.h"

#include <memory>
#include <string>
#include <vector>

namespace proto
{
class Key;
class KeyList;
class ThresholdKey;
}

namespace Hiero
{
/**
 * A key list key structure where all the keys in the list are required to sign transactions that modify accounts,
 * topics, tokens, smart contracts, or files. A key list can contain a ED25519 or ECDSAsecp256k1 key type.
 *
 * If all the keys in the key list key structure do not sign, the transaction will fail and return an
 * "INVALID_SIGNATURE" error. A key list can have repeated keys. A signature for the repeated key will count as many
 * times as the key is listed in the key list. For example, a key list has three keys. Two of the three public keys in
 * the list are the same. When a user signs a transaction with the repeated key it will account for two out of the three
 * keys required signature.
 */
class KeyList : public Key
{
public:
  /**
   * Construct a KeyList object from a KeyList protobuf object.
   *
   * @param proto     The KeyList protobuf object from which to create a KeyList object.
   * @param threshold The optional threshold of Keys that must sign.
   * @return The created KeyList object.
   * @throws BadKeyException If a key in the KeyList protobuf is unable to be created.
   */
  [[nodiscard]] static KeyList fromProtobuf(const proto::KeyList& proto);

  /**
   * Construct a KeyList object from a ThresholdKey protobuf object.
   *
   * @param proto     The ThresholdKey protobuf object from which to create a KeyList object.
   * @return The created KeyList object.
   * @throws BadKeyException If a key in the KeyList protobuf is unable to be created.
   */
  [[nodiscard]] static KeyList fromProtobuf(const proto::ThresholdKey& proto);

  /**
   * Construct a KeyList object from a list of Keys.
   *
   * @param keys The list of Keys to add to this KeyList.
   * @return The created KeyList object.
   */
  [[nodiscard]] static KeyList of(const std::vector<std::shared_ptr<Key>>& keys);

  /**
   * Construct a Keylist object with a threshold.
   *
   * @param threshold The number of Keys in the KeyList that must sign.
   * @param The created Keylist object.
   */
  [[nodiscard]] static KeyList withThreshold(uint32_t threshold);

  /**
   * Derived from Key. Create a clone of this KeyList object.
   *
   * @return A pointer to the created clone of this KeyList.
   */
  [[nodiscard]] std::unique_ptr<Key> clone() const override;

  /**
   * Derived from Key. Construct a Key protobuf object from this KeyList object.
   *
   * @return A pointer to the created Key protobuf object filled with this KeyList object's data.
   */
  [[nodiscard]] std::unique_ptr<proto::Key> toProtobufKey() const override;

  /**
   * Construct a KeyList protobuf object from this KeyList object.
   *
   * @return A pointer to a created KeyList protobuf object filled with this KeyList object's data.
   * @throws OpenSSLException If OpenSSL is unable to serialize any key in this KeyList.
   */
  [[nodiscard]] std::unique_ptr<proto::KeyList> toProtobuf() const;

  /**
   * Construct a string representation of this KeyList object.
   *
   * @return The string representation of this KeyList object.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Write this KeyList to an output stream.
   *
   * @param os   The output stream.
   * @param list The KeyList to print.
   * @return The output stream with this KeyList written to it.
   */
  friend std::ostream& operator<<(std::ostream& os, const KeyList& list);

  /**
   * Set the threshold for this KeyList.
   *
   * @param threshold The threshold for this KeyList.
   * @return A reference to this KeyList with the newly-set threshold.
   */
  KeyList& setThreshold(uint32_t threshold);

  /**
   * Get the threshold for this KeyList.
   *
   * @return The threshold number of Keys that must sign.
   */
  [[nodiscard]] inline uint32_t getThreshold() const { return mThreshold; }

  /**
   * Get the number of keys in this KeyList.
   *
   * @return The number of keys in this KeyList.
   */
  [[nodiscard]] size_t size() const;

  /**
   * Determine if this KeyList contains any keys.
   *
   * @return \c TRUE if this KeyList is empty, otherwise \c FALSE.
   */
  [[nodiscard]] bool empty() const;

  /**
   * Determine if this KeyList contains a certain key.
   *
   * @param key The key to determine if this KeyList contains.
   * @return \c TRUE if this KeyList contains the input key, otherwise \c FALSE.
   */
  [[nodiscard]] bool contains(const Key* key) const;

  /**
   * Add a key to this KeyList.
   *
   * @param key The key to add to this KeyList.
   */
  void push_back(const std::shared_ptr<Key>& key);

  /**
   * Remove a key from this KeyList. Does nothing if the input key is not a part of this KeyList.
   *
   * @param key The key to remove from this KeyList.
   */
  void remove(const Key* key);

  /**
   * Remove all keys from this KeyList.
   */
  void clear();

private:
  /**
   * The list of Keys that all must sign transactions.
   */
  std::vector<std::shared_ptr<Key>> mKeys;

  /**
   * The threshold number of keys that must sign a transaction. 0 means all keys must sign.
   */
  uint32_t mThreshold = 0;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_KEYLIST_H_
