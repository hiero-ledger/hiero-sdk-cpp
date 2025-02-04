// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_BASE_NODE_ADDRESS_H_
#define HIERO_SDK_CPP_IMPL_BASE_NODE_ADDRESS_H_

#include <string>
#include <string_view>

namespace Hiero::internal
{
class BaseNodeAddress
{
public:
  /**
   * Port numbers for various node types and security.
   */
  static constexpr unsigned int PORT_MIRROR_PLAIN = 5600U;
  static constexpr unsigned int PORT_MIRROR_TLS = 443U;
  static constexpr unsigned int PORT_NODE_PLAIN = 50211U;
  static constexpr unsigned int PORT_NODE_TLS = 50212U;

  /**
   * Construct with values for name, address, and port.
   *
   * @param name    The name with which to construct.
   * @param address The address with which to construct.
   * @param port    The port with which to construct.
   */
  explicit BaseNodeAddress(std::string_view name, std::string_view address, unsigned int port);

  /**
   * Construct a BaseNodeAddress from an address string that contains the IP and port.
   *
   * @param address The address of the BaseNodeAddress.
   * @return The constructed BaseNodeAddress.
   * @throws std::invalid_argument If the provided address is malformed.
   */
  [[nodiscard]] static BaseNodeAddress fromString(std::string_view address);

  /**
   * Compare this BaseNodeAddress to another BaseNodeAddress and determine if they represent the same node address.
   *
   * @param other The other BaseNodeAddress with which to compare this BaseNodeAddress.
   * @return \c TRUE if this BaseNodeAddress is the same as the input BaseNodeAddress, otherwise \c FALSE.
   */
  bool operator==(const BaseNodeAddress& other) const;

  /**
   * Create a copy of this BaseNodeAddress but using TLS.
   *
   * @return The copy of this BoseNodeAddress using TLS.
   */
  [[nodiscard]] BaseNodeAddress toSecure() const;

  /**
   * Create a copy of this BaseNodeAddress but not using TLS.
   *
   * @return The copy of this BoseNodeAddress not using TLS.
   */
  [[nodiscard]] BaseNodeAddress toInsecure() const;

  /**
   * Get the string representation of this BaseNodeAddress.
   *
   * @return The string representation of this BaseNodeAddress.
   */
  [[nodiscard]] std::string toString() const;

  /**
   * Is this BaseNodeAddress in process?
   *
   * @return \c TRUE if this BaseNodeAddress is in process, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool isInProcess() const { return !mName.empty(); }

  /**
   * Is this BaseNodeAddress using transport security?
   *
   * @param \c TRUE if this BaseNodeAddress is using TLS, otherwise \c FALSE.
   */
  [[nodiscard]] inline bool isTransportSecurity() const
  {
    return (mPort == PORT_NODE_TLS) || (mPort == PORT_MIRROR_TLS);
  }

  /**
   * Get the name of this BaseNodeAddress.
   *
   * @return The name of this BaseNodeAddress.
   */
  [[nodiscard]] inline std::string getName() const { return mName; }

  /**
   * Get the IP address of this BaseNodeAddress.
   *
   * @return The IP address of this BaseNodeAddress.
   */
  [[nodiscard]] inline std::string getAddress() const { return mAddress; }

  /**
   * Get the port of this BaseNodeAddress.
   *
   * @return The port of this BaseNodeAddress.
   */
  [[nodiscard]] inline unsigned int getPort() const { return mPort; }

private:
  /**
   * The name of the address. This is used for "in-process" addresses.
   */
  std::string mName;

  /**
   * The IP address.
   */
  std::string mAddress;

  /**
   * The port.
   */
  unsigned int mPort = 0U;
};

} // namespace Hiero::internal

namespace std
{
template<>
struct hash<Hiero::internal::BaseNodeAddress>
{
  /**
   * Operator override to enable use of BaseNodeAddress as map key.
   */
  size_t operator()(const Hiero::internal::BaseNodeAddress& address) const
  {
    return hash<string>()(address.toString());
  }
};

} // namespace std

#endif // HIERO_SDK_CPP_IMPL_BASE_NODE_ADDRESS_H_
