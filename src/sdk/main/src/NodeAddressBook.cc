// SPDX-License-Identifier: Apache-2.0
#include "NodeAddressBook.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
NodeAddressBook NodeAddressBook::fromProtobuf(const proto::NodeAddressBook& proto)
{
  NodeAddressBook nodeAddressBook;
  for (int i = 0; i < proto.nodeaddress_size(); ++i)
  {
    nodeAddressBook.mNodeAddresses.push_back(NodeAddress::fromProtobuf(proto.nodeaddress(i)));
  }

  return nodeAddressBook;
}

//-----
NodeAddressBook NodeAddressBook::fromBytes(const std::vector<char>& bytes)
{
  proto::NodeAddressBook addressBook;
  addressBook.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(addressBook);
}

//-----
std::unique_ptr<proto::NodeAddressBook> NodeAddressBook::toProtobuf() const
{
  auto proto = std::make_unique<proto::NodeAddressBook>();
  for (const NodeAddress& address : mNodeAddresses)
  {
    *proto->add_nodeaddress() = *address.toProtobuf();
  }

  return proto;
}

//-----
std::vector<std::byte> NodeAddressBook::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
NodeAddressBook& NodeAddressBook::setNodeAddresses(const std::vector<NodeAddress>& addresses)
{
  mNodeAddresses = addresses;
  return *this;
}

} // namespace Hiero
