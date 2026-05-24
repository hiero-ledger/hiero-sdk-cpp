// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FEE_ESTIMATE_MODE_H_
#define HIERO_SDK_CPP_FEE_ESTIMATE_MODE_H_

#include <string>
#include <unordered_map>

namespace Hiero
{
/**
 * Enum class representing the fee estimation mode used by FeeEstimateQuery.
 */
enum class FeeEstimateMode
{
  /**
   * STATE mode - Estimate using the transaction plus the mirror node's latest known state
   * (e.g. whether an alias already maps to an account, required token associations exist,
   * token types have custom fees, hooks/allowances apply).
   */
  STATE,

  /**
   * INTRINSIC mode - Default. Estimate from the payload alone (bytes, signatures, declared keys, gas),
   * ignoring any state-dependent costs.
   */
  INTRINSIC
};

/**
 * Map of FeeEstimateMode to its corresponding string representation as expected by the
 * mirror node REST API `mode` query parameter.
 */
[[maybe_unused]] static const std::unordered_map<FeeEstimateMode, std::string> gFeeEstimateModeToString = {
  {FeeEstimateMode::STATE,      "STATE"    },
  { FeeEstimateMode::INTRINSIC, "INTRINSIC"}
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_ESTIMATE_MODE_H_
