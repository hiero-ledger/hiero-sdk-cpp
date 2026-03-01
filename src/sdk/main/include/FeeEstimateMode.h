// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FEE_ESTIMATE_MODE_H_
#define HIERO_SDK_CPP_FEE_ESTIMATE_MODE_H_

#include <string>
#include <unordered_map>

namespace Hiero
{
/**
 * Enum class representing the fee estimation mode.
 */
enum class FeeEstimateMode
{
  /**
   * STATE mode - Uses the current state of the network for fee estimation.
   */
  STATE,

  /**
   * TRANSIENT mode - Uses a transient simulation for fee estimation.
   */
  TRANSIENT
};

/**
 * Map of FeeEstimateMode to its corresponding string representation.
 */
[[maybe_unused]] static const std::unordered_map<FeeEstimateMode, std::string> gFeeEstimateModeToString = {
  {FeeEstimateMode::STATE,     "STATE"    },
  { FeeEstimateMode::TRANSIENT, "TRANSIENT"}
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_ESTIMATE_MODE_H_
