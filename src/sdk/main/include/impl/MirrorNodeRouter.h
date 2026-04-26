// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_MIRRORNODEROUTER_H
#define HIERO_SDK_CPP_MIRRORNODEROUTER_H

#include <string>
#include <string_view>
#include <unordered_map>

/**
 * Namespace for the internal classes related to the Mirror Node Gateway.
 */
namespace Hiero::internal::MirrorNodeGateway
{
/**
 * Represents different mirror node query types.
 */
static constexpr std::string_view ACCOUNT_INFO_QUERY = "accountInfoQuery";
static constexpr std::string_view CONTRACT_INFO_QUERY = "contractInfoQuery";
static constexpr std::string_view TOKEN_RELATIONSHIPS_QUERY = "tokenRelationshipsQuery";
static constexpr std::string_view TOKEN_BALANCES_QUERY = "tokenBalancesQuery";

/**
 * Class responsible for routing requests to different mirror node routes.
 */
class MirrorNodeRouter
{
public:
  /**
   * Retrieves the route for a specific mirror node query type.
   *
   * @param queryType The type of the mirror node query (e.g., "accountInfoQuery").
   * @return A string view representing the route for the specified mirror node query.
   */
  [[nodiscard]] std::string getRoute(std::string_view queryType) const;

private:
  /**
   * Internal mapping of mirror node query types to their respective routes.
   */
  const std::unordered_map<std::string, std::string> routes = {
    {std::string(ACCOUNT_INFO_QUERY),         "/api/v1/accounts/$"       },
    { std::string(CONTRACT_INFO_QUERY),       "/api/v1/contracts/$"      },
    { std::string(TOKEN_RELATIONSHIPS_QUERY), "/api/v1/accounts/$/tokens"},
    { std::string(TOKEN_BALANCES_QUERY),      "/api/v1/tokens/$/balances"}
  };
};

} // namespace Hiero::internal::MirrorNodeGateway

#endif // HIERO_SDK_CPP_MIRRORNODEROUTER_H
