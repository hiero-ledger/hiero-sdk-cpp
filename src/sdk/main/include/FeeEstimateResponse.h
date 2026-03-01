// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_FEE_ESTIMATE_RESPONSE_H_
#define HIERO_SDK_CPP_FEE_ESTIMATE_RESPONSE_H_

#include <cstdint>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

namespace Hiero
{
/**
 * Represents an extra fee component.
 */
struct FeeExtra
{
  /**
   * The amount of the extra fee in tinybars.
   */
  uint64_t mAmount = 0;

  /**
   * The description of the extra fee.
   */
  std::string mDescription;

  /**
   * Construct from JSON.
   *
   * @param json The JSON object.
   * @return The constructed FeeExtra.
   */
  [[nodiscard]] static FeeExtra fromJson(const nlohmann::json& json)
  {
    FeeExtra extra;
    if (json.contains("amount"))
    {
      extra.mAmount = json["amount"].get<uint64_t>();
    }
    if (json.contains("description"))
    {
      extra.mDescription = json["description"].get<std::string>();
    }
    return extra;
  }
};

/**
 * Represents a fee estimate with base fee and extras.
 */
struct FeeEstimate
{
  /**
   * The base fee in tinybars.
   */
  uint64_t mBase = 0;

  /**
   * The list of extra fee components.
   */
  std::vector<FeeExtra> mExtras;

  /**
   * Calculate the subtotal of the fee (base + all extras).
   *
   * @return The subtotal fee in tinybars.
   */
  [[nodiscard]] uint64_t subtotal() const
  {
    uint64_t total = mBase;
    for (const auto& extra : mExtras)
    {
      total += extra.mAmount;
    }
    return total;
  }

  /**
   * Construct from JSON.
   *
   * @param json The JSON object.
   * @return The constructed FeeEstimate.
   */
  [[nodiscard]] static FeeEstimate fromJson(const nlohmann::json& json)
  {
    FeeEstimate estimate;
    if (json.contains("base"))
    {
      estimate.mBase = json["base"].get<uint64_t>();
    }
    if (json.contains("extras") && json["extras"].is_array())
    {
      for (const auto& extra : json["extras"])
      {
        estimate.mExtras.push_back(FeeExtra::fromJson(extra));
      }
    }
    return estimate;
  }
};

/**
 * Represents the network fee component.
 */
struct NetworkFee
{
  /**
   * The multiplier for the network fee.
   */
  double mMultiplier = 0.0;

  /**
   * The subtotal of the network fee in tinybars.
   */
  uint64_t mSubtotal = 0;

  /**
   * Construct from JSON.
   *
   * @param json The JSON object.
   * @return The constructed NetworkFee.
   */
  [[nodiscard]] static NetworkFee fromJson(const nlohmann::json& json)
  {
    NetworkFee networkFee;
    if (json.contains("multiplier"))
    {
      networkFee.mMultiplier = json["multiplier"].get<double>();
    }
    if (json.contains("subtotal"))
    {
      networkFee.mSubtotal = json["subtotal"].get<uint64_t>();
    }
    return networkFee;
  }
};

/**
 * Represents the complete fee estimate response from the mirror node.
 */
struct FeeEstimateResponse
{
  /**
   * The node fee estimate.
   */
  FeeEstimate mNodeFee;

  /**
   * The service fee estimate.
   */
  FeeEstimate mServiceFee;

  /**
   * The network fee.
   */
  NetworkFee mNetworkFee;

  /**
   * The total estimated fee in tinybars.
   */
  uint64_t mTotal = 0;

  /**
   * Notes or messages from the fee estimation.
   */
  std::vector<std::string> mNotes;

  /**
   * Construct from JSON.
   *
   * @param json The JSON object.
   * @return The constructed FeeEstimateResponse.
   */
  [[nodiscard]] static FeeEstimateResponse fromJson(const nlohmann::json& json)
  {
    FeeEstimateResponse response;

    if (json.contains("nodeFee"))
    {
      response.mNodeFee = FeeEstimate::fromJson(json["nodeFee"]);
    }
    if (json.contains("serviceFee"))
    {
      response.mServiceFee = FeeEstimate::fromJson(json["serviceFee"]);
    }
    if (json.contains("networkFee"))
    {
      response.mNetworkFee = NetworkFee::fromJson(json["networkFee"]);
    }
    if (json.contains("total"))
    {
      response.mTotal = json["total"].get<uint64_t>();
    }
    if (json.contains("notes") && json["notes"].is_array())
    {
      for (const auto& note : json["notes"])
      {
        response.mNotes.push_back(note.get<std::string>());
      }
    }

    return response;
  }
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_ESTIMATE_RESPONSE_H_
