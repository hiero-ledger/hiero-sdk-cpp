// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_TCK_CPP_NFT_TRANSFER_PARAMS_H_
#define HIERO_TCK_CPP_NFT_TRANSFER_PARAMS_H_

#include "json/JsonUtils.h"

#include <optional>
#include <string>

namespace Hiero::TCK
{
/**
 * Struct that contains the parameters of an NFT transfer.
 */
struct NftTransferParams
{
  /**
   * The ID of the account sending the NFT.
   */
  std::string mSenderAccountId;

  /**
   * The ID of the account receiving the NFT.
   */
  std::string mReceiverAccountId;

  /**
   * The ID of the token associated with the transfer.
   */
  std::string mTokenId;

  /**
   * The serial number of the NFT being transferred.
   */
  std::string mSerialNumber;
};

} // namespace Hiero::TCK

namespace nlohmann
{
/**
 * JSON serializer template specialization required to convert NftTransferParams arguments properly.
 */
template<>
struct [[maybe_unused]] adl_serializer<Hiero::TCK::NftTransferParams>
{
  /**
   * Convert a JSON object to a NftTransferParams.
   *
   * @param jsonFrom The JSON object with which to fill the NftTransferParams.
   * @param params   The NftTransferParams to fill with the JSON object.
   */
  static void from_json(const json& jsonFrom, Hiero::TCK::NftTransferParams& params)
  {
    params.mSenderAccountId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "senderAccountId");
    params.mReceiverAccountId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "receiverAccountId");
    params.mTokenId = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "tokenId");
    params.mSerialNumber = Hiero::TCK::getRequiredJsonParameter<std::string>(jsonFrom, "serialNumber");
  }
};

} // namespace nlohmann

#endif // HIERO_TCK_CPP_NFT_TRANSFER_PARAMS_H_
