// SPDX-License-Identifier: Apache-2.0
#include "KeyList.h"
#include "impl/HexConverter.h"

#include <algorithm>
#include <basic_types.pb.h>
#include <cstddef>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
KeyList KeyList::fromProtobuf(const proto::KeyList& proto)
{
  KeyList keyList;

  for (int i = 0; i < proto.keys_size(); ++i)
  {
    keyList.mKeys.emplace_back(Key::fromProtobuf(proto.keys(i)).release());
  }

  return keyList;
}

//-----
KeyList KeyList::fromProtobuf(const proto::ThresholdKey& proto)
{
  KeyList keyList;
  keyList.mThreshold = proto.threshold();

  for (int i = 0; i < proto.keys().keys_size(); ++i)
  {
    keyList.mKeys.emplace_back(Key::fromProtobuf(proto.keys().keys(i)).release());
  }

  return keyList;
}

//-----
KeyList KeyList::of(const std::vector<std::shared_ptr<Key>>& keys)
{
  KeyList keyList;
  keyList.mKeys = keys;
  return keyList;
}

//-----
KeyList KeyList::withThreshold(uint32_t threshold)
{
  KeyList keyList;
  keyList.mThreshold = threshold;
  return keyList;
}

//-----
std::unique_ptr<Key> KeyList::clone() const
{
  return std::make_unique<KeyList>(*this);
}

//-----
std::unique_ptr<proto::Key> KeyList::toProtobufKey() const
{
  auto key = std::make_unique<proto::Key>();
  if (mThreshold > 0)
  {
    key->mutable_thresholdkey()->set_allocated_keys(toProtobuf().release());
    key->mutable_thresholdkey()->set_threshold(static_cast<uint32_t>(mThreshold));
  }
  else
  {
    key->set_allocated_keylist(toProtobuf().release());
  }

  return key;
}

//-----
std::unique_ptr<proto::KeyList> KeyList::toProtobuf() const
{
  auto keyList = std::make_unique<proto::KeyList>();

  for (const auto& key : mKeys)
  {
    *keyList->add_keys() = *key->toProtobufKey();
  }

  return keyList;
}

//-----
std::string KeyList::toString() const
{
  nlohmann::json json;

  if (mThreshold > 0)
  {
    json["mThreshold"] = mThreshold;
  }

  std::for_each(mKeys.cbegin(),
                mKeys.cend(),
                [&json](const std::shared_ptr<Key>& key)
                { json["mKeys"].push_back(internal::HexConverter::bytesToHex(key->toBytes())); });
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const KeyList& list)
{
  os << list.toString();
  return os;
}

//-----
KeyList& KeyList::setThreshold(uint32_t threshold)
{
  mThreshold = threshold;
  return *this;
}

//-----
size_t KeyList::size() const
{
  return mKeys.size();
}

//-----
bool KeyList::empty() const
{
  return mKeys.empty();
}

//-----
bool KeyList::contains(const Key* key) const
{
  const std::vector<std::byte> keyBytes = key->toBytes();
  return std::any_of(mKeys.cbegin(),
                     mKeys.cend(),
                     [&keyBytes](const std::shared_ptr<Key>& listKey) { return listKey->toBytes() == keyBytes; });
}

//-----
void KeyList::push_back(const std::shared_ptr<Key>& key)
{
  mKeys.push_back(key);
}

//-----
void KeyList::remove(const Key* key)
{
  const std::vector<std::byte> keyBytes = key->toBytes();
  const auto eraseIter =
    std::remove_if(mKeys.begin(),
                   mKeys.end(),
                   [&keyBytes](const std::shared_ptr<Key>& listKey) { return listKey->toBytes() == keyBytes; });

  // Erasing an element pointed to by an iterator that equals end() is undefined, so this check is necessary.
  if (eraseIter != mKeys.end())
  {
    mKeys.erase(eraseIter, mKeys.end());
  }
}

//-----
void KeyList::clear()
{
  mKeys.clear();
}

} // namespace Hiero
