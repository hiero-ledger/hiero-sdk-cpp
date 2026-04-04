// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_REGISTERED_SERVICE_ENDPOINT_H_
#define HIERO_SDK_CPP_REGISTERED_SERVICE_ENDPOINT_H_

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace com::hedera::hapi::node::addressbook
{
class RegisteredServiceEndpoint;
}

namespace Hiero
{
/**
 * A service endpoint published by a registered node. Each endpoint declares an address (IP or FQDN), port, TLS
 * requirement, and the type of node service it provides.
 *
 * This is an abstract base class. Concrete subtypes are BlockNodeServiceEndpoint, MirrorNodeServiceEndpoint, and
 * RpcRelayServiceEndpoint.
 */
class RegisteredServiceEndpoint
{
public:
  virtual ~RegisteredServiceEndpoint() = default;

  /**
   * Construct a RegisteredServiceEndpoint subtype from a RegisteredServiceEndpoint protobuf object. Dispatches on the
   * endpoint_type oneof to construct the correct concrete subtype.
   *
   * @param proto The RegisteredServiceEndpoint protobuf object from which to construct.
   * @return A unique_ptr to the constructed RegisteredServiceEndpoint subtype.
   * @throws std::invalid_argument If the endpoint_type oneof is not set.
   */
  [[nodiscard]] static std::unique_ptr<RegisteredServiceEndpoint> fromProtobuf(
    const com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto);

  /**
   * Construct a RegisteredServiceEndpoint protobuf object from this RegisteredServiceEndpoint object.
   *
   * @return A unique_ptr to the constructed RegisteredServiceEndpoint protobuf object.
   */
  [[nodiscard]] virtual std::unique_ptr<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint> toProtobuf()
    const = 0;

  /**
   * Construct a string representation of this RegisteredServiceEndpoint object.
   *
   * @return The string representation of this RegisteredServiceEndpoint object.
   */
  [[nodiscard]] virtual std::string toString() const = 0;

  /**
   * Set the IP address of this endpoint. Clears any previously set domain name.
   *
   * @param ipAddress A vector of bytes representing the IP address (4 bytes for IPv4, 16 bytes for IPv6).
   * @return A reference to this RegisteredServiceEndpoint with the newly-set IP address.
   */
  RegisteredServiceEndpoint& setIpAddress(const std::vector<std::byte>& ipAddress);

  /**
   * Set the fully qualified domain name of this endpoint. Clears any previously set IP address.
   *
   * @param domainName The fully qualified domain name.
   * @return A reference to this RegisteredServiceEndpoint with the newly-set domain name.
   */
  RegisteredServiceEndpoint& setDomainName(std::string_view domainName);

  /**
   * Set the port of this endpoint.
   *
   * @param port The port number (0-65535).
   * @return A reference to this RegisteredServiceEndpoint with the newly-set port.
   */
  RegisteredServiceEndpoint& setPort(uint32_t port);

  /**
   * Set whether this endpoint requires TLS.
   *
   * @param requiresTls true if TLS is required; false otherwise.
   * @return A reference to this RegisteredServiceEndpoint with the newly-set TLS requirement.
   */
  RegisteredServiceEndpoint& setRequiresTls(bool requiresTls);

  /**
   * Get the IP address of this endpoint.
   *
   * @return An optional vector of bytes representing the IP address, or empty if a domain name is set.
   */
  [[nodiscard]] const std::optional<std::vector<std::byte>>& getIpAddress() const { return mIpAddress; }

  /**
   * Get the fully qualified domain name of this endpoint.
   *
   * @return An optional string containing the domain name, or empty if an IP address is set.
   */
  [[nodiscard]] const std::optional<std::string>& getDomainName() const { return mDomainName; }

  /**
   * Get the port of this endpoint.
   *
   * @return The port number.
   */
  [[nodiscard]] uint32_t getPort() const { return mPort; }

  /**
   * Get whether this endpoint requires TLS.
   *
   * @return true if TLS is required; false otherwise.
   */
  [[nodiscard]] bool getRequiresTls() const { return mRequiresTls; }

protected:
  /**
   * Fill the common fields of a RegisteredServiceEndpoint protobuf object from this object's fields.
   *
   * @param proto The RegisteredServiceEndpoint protobuf object to fill.
   */
  void fillCommonFields(com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto) const;

  /**
   * Read the common fields of a RegisteredServiceEndpoint protobuf object into this object's fields.
   *
   * @param proto The RegisteredServiceEndpoint protobuf object from which to read.
   */
  void readCommonFields(const com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto);

  /**
   * An IP address (IPv4 = 4 bytes, IPv6 = 16 bytes). Mutually exclusive with mDomainName.
   */
  std::optional<std::vector<std::byte>> mIpAddress;

  /**
   * A fully qualified domain name. Mutually exclusive with mIpAddress.
   */
  std::optional<std::string> mDomainName;

  /**
   * The port number for this endpoint.
   */
  uint32_t mPort = 0;

  /**
   * Whether this endpoint requires TLS.
   */
  bool mRequiresTls = false;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_REGISTERED_SERVICE_ENDPOINT_H_
