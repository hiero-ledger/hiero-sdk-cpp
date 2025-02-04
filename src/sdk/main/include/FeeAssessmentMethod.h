// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FEE_ASSESSMENT_METHOD_H_
#define HIERO_SDK_CPP_FEE_ASSESSMENT_METHOD_H_

#include <string>
#include <unordered_map>

namespace Hiero
{
/**
 * An enumeration the describes how a fee should be assessed.
 */
enum class FeeAssessmentMethod
{
  /**
   * If Alice is paying Bob and an INCLUSIVE fractional fee is collected to be sent to Charlie, the amount Alice
   * declares she will pay in the transfer transaction includes the fee amount. In other words, Bob receives the amount
   * that Alice intended to send minus the fee.
   */
  INCLUSIVE,
  /**
   * If Alice is paying Bob, and an EXCLUSIVE fractional fee is collected to be sent to Charlie, the amount Alice
   * declares she will pay in the transfer transaction does not include the fee amount. In other words, Alice is charged
   * the fee in addition to the amount she intended to send to Bob.
   */
  EXCLUSIVE
};

/**
 * Map of FeeAssessmentMethod to its corresponding string.
 */
const std::unordered_map<FeeAssessmentMethod, std::string> gFeeAssessmentMethodToString = {
  {FeeAssessmentMethod::INCLUSIVE,  "INCLUSIVE"},
  { FeeAssessmentMethod::EXCLUSIVE, "EXCLUSIVE"}
};

/**
 * Map of FeeAssessmentMethod string representation to its corresponding enum value.
 */
[[maybe_unused]] const std::unordered_map<std::string, FeeAssessmentMethod> gStringToFeeAssessmentMethod = {
  {"INCLUSIVE",  FeeAssessmentMethod::INCLUSIVE},
  { "EXCLUSIVE", FeeAssessmentMethod::EXCLUSIVE}
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_ASSESSMENT_METHOD_H_
