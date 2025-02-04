// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_TOKEN_NFT_INFO_QUERY_H_
#define HIERO_SDK_CPP_TOKEN_NFT_INFO_QUERY_H_

#include "NftId.h"
#include "Query.h"

namespace Hiero
{
class TokenNftInfo;
class TransactionRecord;
}

namespace Hiero
{
/**
 * A query that returns information about a non-fungible token (NFT). You request the info for an NFT by specifying the
 * NFT ID.
 *
 * Only when a spender is set on an explicit NFT ID of a token, the spender ID is returned for the respective NFT. If
 * mApproveTokenNftAllowanceAllSerials is used to approve all NFTs for a given token class and no NFT ID is specified,
 * the spender ID for all the serial numbers of that token will not be returned.
 */
class TokenNftInfoQuery : public Query<TokenNftInfoQuery, TokenNftInfo>
{
public:
  /**
   * Set the ID of the NFT of which to request the info.
   *
   * @param nft The ID of the NFT of which to request the info.
   * @return A reference to this TokenNftInfoQuery object with the newly-set NFT ID.
   */
  TokenNftInfoQuery& setNftId(const NftId& nft);

  /**
   * Get the ID of the NFT of which this query is currently configured to get the info.
   *
   * @return The ID of the NFT for which this query is meant.
   */
  [[nodiscard]] inline NftId getNftId() const { return mNftId; }

private:
  /**
   * Derived from Executable. Construct a TokenNftInfo object from a Response protobuf object.
   *
   * @param response The Response protobuf object from which to construct a TokenNftInfo object.
   * @return A TokenNftInfo object filled with the Response protobuf object's data
   */
  [[nodiscard]] TokenNftInfo mapResponse(const proto::Response& response) const override;

  /**
   * Derived from Executable. Submit a Query protobuf object which contains this TokenNftInfoQuery's data to a Node.
   *
   * @param request  The Query protobuf object to submit.
   * @param node     The Node to which to submit the request.
   * @param deadline The deadline for submitting the request.
   * @param response Pointer to the ProtoResponseType object that gRPC should populate with the response information
   *                 from the gRPC server.
   * @return The gRPC status of the submission.
   */
  [[nodiscard]] grpc::Status submitRequest(const proto::Query& request,
                                           const std::shared_ptr<internal::Node>& node,
                                           const std::chrono::system_clock::time_point& deadline,
                                           proto::Response* response) const override;

  /**
   * Derived from Query. Verify that all the checksums in this TokenNftInfoQuery are valid.
   *
   * @param client The Client that should be used to validate the checksums.
   * @throws BadEntityException This TokenNftInfoQuery's checksums are not valid.
   */
  void validateChecksums(const Client& client) const override;

  /**
   * Derived from Query. Build a Query protobuf object with this TokenNftInfoQuery's data, with the input QueryHeader
   * protobuf object.
   *
   * @param header A pointer to the QueryHeader protobuf object to add to the Query protobuf object.
   * @return The constructed Query protobuf object.
   */
  [[nodiscard]] proto::Query buildRequest(proto::QueryHeader* header) const override;

  /**
   * Derived from Query. Get the ResponseHeader protobuf object from the input Response protobuf object.
   *
   * @param response The Response protobuf object from which to get the ResponseHeader protobuf object.
   * @return The ResponseHeader protobuf object of the input Response protobuf object for this derived Query.
   */
  [[nodiscard]] proto::ResponseHeader mapResponseHeader(const proto::Response& response) const override;

  /**
   * The ID of the NFT of which this query should get the info.
   */
  NftId mNftId;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_TOKEN_NFT_INFO_QUERY_H_
