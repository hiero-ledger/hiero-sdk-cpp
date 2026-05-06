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
 * The extra fee charged for a fee component, as defined by HIP-1261.
 */
struct FeeExtra
{
  /**
   * The unique name of this extra fee as defined in the fee schedule.
   */
  std::string mName;

  /**
   * The count of this "extra" that is included for free.
   */
  uint64_t mIncluded = 0;

  /**
   * The actual count of items received.
   */
  uint64_t mCount = 0;

  /**
   * The charged count of items as calculated by max(0, count - included).
   */
  uint64_t mCharged = 0;

  /**
   * The fee price per unit in tinycents. JSON key: "fee_per_unit".
   */
  uint64_t mFeePerUnit = 0;

  /**
   * The subtotal in tinycents for this extra fee (charged * feePerUnit).
   */
  uint64_t mSubtotal = 0;

  /**
   * Construct a FeeExtra from its JSON object representation.
   */
  [[nodiscard]] static FeeExtra fromJson(const nlohmann::json& json)
  {
    FeeExtra extra;
    if (json.contains("name"))
    {
      extra.mName = json["name"].get<std::string>();
    }
    if (json.contains("included"))
    {
      extra.mIncluded = json["included"].get<uint64_t>();
    }
    if (json.contains("count"))
    {
      extra.mCount = json["count"].get<uint64_t>();
    }
    if (json.contains("charged"))
    {
      extra.mCharged = json["charged"].get<uint64_t>();
    }
    if (json.contains("fee_per_unit"))
    {
      extra.mFeePerUnit = json["fee_per_unit"].get<uint64_t>();
    }
    if (json.contains("subtotal"))
    {
      extra.mSubtotal = json["subtotal"].get<uint64_t>();
    }
    return extra;
  }
};

/**
 * The fee estimate for a node or service component, including the base fee and any extras.
 */
struct FeeEstimate
{
  /**
   * The base fee price, in tinycents.
   */
  uint64_t mBase = 0;

  /**
   * The extra fees that apply for this fee component.
   */
  std::vector<FeeExtra> mExtras;

  /**
   * Compute the subtotal of this fee component: base plus the sum of every extra's subtotal.
   */
  [[nodiscard]] uint64_t subtotal() const
  {
    uint64_t total = mBase;
    for (const auto& extra : mExtras)
    {
      total += extra.mSubtotal;
    }
    return total;
  }

  /**
   * Construct a FeeEstimate from its JSON object representation.
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
 * The network fee component which covers the cost of gossip, consensus, signature
 * verifications, fee payment, and storage.
 */
struct NetworkFee
{
  /**
   * Multiplied by the node fee to determine the total network fee.
   */
  uint32_t mMultiplier = 0;

  /**
   * The subtotal in tinycents for the network fee component.
   */
  uint64_t mSubtotal = 0;

  /**
   * Construct a NetworkFee from its JSON object representation.
   */
  [[nodiscard]] static NetworkFee fromJson(const nlohmann::json& json)
  {
    NetworkFee networkFee;
    if (json.contains("multiplier"))
    {
      networkFee.mMultiplier = json["multiplier"].get<uint32_t>();
    }
    if (json.contains("subtotal"))
    {
      networkFee.mSubtotal = json["subtotal"].get<uint64_t>();
    }
    return networkFee;
  }
};

/**
 * The response returned from the mirror node fee estimation endpoint, as defined by HIP-1261.
 */
struct FeeEstimateResponse
{
  /**
   * The high-volume pricing multiplier per HIP-1313. A value of 1 indicates no high-volume
   * pricing. JSON key: "high_volume_multiplier".
   */
  uint64_t mHighVolumeMultiplier = 0;

  /**
   * The network fee component. JSON key: "network".
   */
  NetworkFee mNetwork;

  /**
   * The node fee component to be paid to the submitting node. JSON key: "node".
   */
  FeeEstimate mNode;

  /**
   * The service fee component for execution costs and state storage. JSON key: "service".
   */
  FeeEstimate mService;

  /**
   * The sum of the network, node, and service subtotals in tinycents.
   */
  uint64_t mTotal = 0;

  /**
   * Construct a FeeEstimateResponse from its JSON object representation.
   */
  [[nodiscard]] static FeeEstimateResponse fromJson(const nlohmann::json& json)
  {
    FeeEstimateResponse response;

    if (json.contains("high_volume_multiplier"))
    {
      response.mHighVolumeMultiplier = json["high_volume_multiplier"].get<uint64_t>();
    }
    if (json.contains("network"))
    {
      response.mNetwork = NetworkFee::fromJson(json["network"]);
    }
    if (json.contains("node"))
    {
      response.mNode = FeeEstimate::fromJson(json["node"]);
    }
    if (json.contains("service"))
    {
      response.mService = FeeEstimate::fromJson(json["service"]);
    }
    if (json.contains("total"))
    {
      response.mTotal = json["total"].get<uint64_t>();
    }

    return response;
  }
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_FEE_ESTIMATE_RESPONSE_H_
