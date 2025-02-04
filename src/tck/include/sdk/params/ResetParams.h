// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_RESET_PARAMS_H_
#define HIERO_TCK_CPP_RESET_PARAMS_H_

#include <nlohmann/json_fwd.hpp>

namespace Hiero::TCK::SdkClient
{
/**
 * Struct to hold the arguments for a `reset` JSON-RPC method call. It takes no arguments
 */
struct ResetParams
{
};

} // namespace Hiero::TCK::SdkClient

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert ResetParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::SdkClient::ResetParams>
{
  /**
   * Convert a JSON object to a ResetParams.
   *
   * @param jsonFrom The JSON object with which to fill the ResetParams.
   * @param params   The ResetParams to fill with the JSON object.
   */
  static void from_json(const json&, Hiero::TCK::SdkClient::ResetParams&) {}
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_RESET_PARAMS_H_
