// SPDX-License-Identifier: Apache-2.0
#include "impl/BaseNodeAddress.h"

#include <charconv>
#include <iostream>
#include <stdexcept>

namespace Hiero::internal
{
//-----
BaseNodeAddress::BaseNodeAddress(std::string_view name, std::string_view address, unsigned int port)
  : mName(name)
  , mAddress(address)
  , mPort(port)
{
}

//-----
BaseNodeAddress BaseNodeAddress::fromString(std::string_view address)
{
  if (const std::string_view inProcessPrefix = "in-process:"; address.find(inProcessPrefix) == 0ULL)
  {
    address.remove_prefix(inProcessPrefix.size());
    return BaseNodeAddress(address, "", 0U);
  }

  const size_t colonIndex = address.find(':');
  if (colonIndex == std::string::npos)
  {
    throw std::invalid_argument("Input BaseNodeAddress is malformed");
  }

  const std::string_view ipAddressV4 = address.substr(0, colonIndex);
  const std::string_view portStr = address.substr(colonIndex + 1, address.size() - colonIndex - 1);
  unsigned int port;
  if (const auto result = std::from_chars(portStr.data(), portStr.data() + portStr.size(), port);
      result.ptr != portStr.data() + portStr.size() || result.ec != std::errc())
  {
    throw std::invalid_argument("Input BaseNodeAddress is malformed");
  }

  return BaseNodeAddress("", ipAddressV4, port);
}

//-----
bool BaseNodeAddress::operator==(const BaseNodeAddress& other) const
{
  return (!mName.empty()) ? (mName == other.mName) : ((mAddress == other.mAddress) && (mPort == other.mPort));
}

//-----
BaseNodeAddress BaseNodeAddress::toSecure() const
{
  return BaseNodeAddress(
    mName, mAddress, (mPort == PORT_MIRROR_PLAIN || mPort == PORT_MIRROR_TLS) ? PORT_MIRROR_TLS : PORT_NODE_TLS);
}

//-----
BaseNodeAddress BaseNodeAddress::toInsecure() const
{
  return BaseNodeAddress(
    mName, mAddress, (mPort == PORT_MIRROR_PLAIN || mPort == PORT_MIRROR_TLS) ? PORT_MIRROR_PLAIN : PORT_NODE_PLAIN);
}

//-----
std::string BaseNodeAddress::toString() const
{
  return (!mName.empty()) ? "in-process:" + mName : mAddress + ':' + std::to_string(mPort);
}

} // namespace Hiero::internal
