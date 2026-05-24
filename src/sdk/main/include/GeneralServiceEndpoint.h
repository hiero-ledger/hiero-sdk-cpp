// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_GENERAL_SERVICE_ENDPOINT_H_
#define HIERO_SDK_CPP_GENERAL_SERVICE_ENDPOINT_H_

#include "RegisteredServiceEndpoint.h"

#include <nlohmann/json_fwd.hpp>
#include <optional>
#include <string>
#include <string_view>

namespace com::hedera::hapi::node::addressbook
{
class RegisteredServiceEndpoint;
}

namespace Hiero
{
/**
 * A registered service endpoint for a general-purpose service. Carries an optional human-readable description.
 * Defined as a distinct subtype so that general-service-specific fields can be added in future HIPs without breaking
 * changes.
 */
class GeneralServiceEndpoint : public RegisteredServiceEndpoint
{
public:
  GeneralServiceEndpoint() = default;

  /**
   * Construct a GeneralServiceEndpoint from a RegisteredServiceEndpoint protobuf object.
   *
   * @param proto The RegisteredServiceEndpoint protobuf object from which to construct.
   * @return The constructed GeneralServiceEndpoint.
   */
  [[nodiscard]] static GeneralServiceEndpoint fromProtobuf(
    const com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint& proto);

  /**
   * Construct a GeneralServiceEndpoint from a JSON object (mirror node REST response shape).
   *
   * @param json The JSON object from which to construct.
   * @return The constructed GeneralServiceEndpoint.
   */
  [[nodiscard]] static GeneralServiceEndpoint fromJson(const nlohmann::json& json);

  /**
   * Construct a RegisteredServiceEndpoint protobuf object from this GeneralServiceEndpoint.
   *
   * @return A unique_ptr to the constructed RegisteredServiceEndpoint protobuf object.
   */
  [[nodiscard]] std::unique_ptr<com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint> toProtobuf()
    const override;

  /**
   * Construct a string representation of this GeneralServiceEndpoint.
   *
   * @return The string representation of this GeneralServiceEndpoint.
   */
  [[nodiscard]] std::string toString() const override;

  /**
   * Set the description of this general service endpoint.
   *
   * @param description The description (max 100 bytes UTF-8).
   * @return A reference to this GeneralServiceEndpoint with the newly-set description.
   */
  GeneralServiceEndpoint& setDescription(std::string_view description);

  /**
   * Get the description of this general service endpoint.
   *
   * @return An optional string containing the description, or empty if not set.
   */
  [[nodiscard]] const std::optional<std::string>& getDescription() const { return mDescription; }

private:
  /**
   * An optional human-readable description (max 100 bytes UTF-8).
   */
  std::optional<std::string> mDescription;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_GENERAL_SERVICE_ENDPOINT_H_
