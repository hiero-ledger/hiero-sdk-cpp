// SPDX-License-Identifier: Apache-2.0
#include "AccountStakersQuery.h"
#include "Client.h"
#include "ProxyStaker.h"
#include "TransactionRecord.h"
#include "TransferTransaction.h"
#include "impl/Node.h"

#include <crypto_get_stakers.pb.h>
#include <query.pb.h>
#include <query_header.pb.h>
#include <response.pb.h>

namespace Hiero
{
//-----
AccountStakersQuery& AccountStakersQuery::setAccountId(const AccountId& accountId)
{
  mAccountId = accountId;
  return *this;
}

//-----
AccountStakers AccountStakersQuery::mapResponse(const proto::Response& response) const
{
  AccountStakers accountStakers;
  for (int i = 0; i < response.cryptogetproxystakers().stakers().proxystaker_size(); ++i)
  {
    accountStakers.push_back(ProxyStaker::fromProtobuf(response.cryptogetproxystakers().stakers().proxystaker(i)));
  }

  return accountStakers;
}

//-----
grpc::Status AccountStakersQuery::submitRequest(const proto::Query& request,
                                                const std::shared_ptr<internal::Node>& node,
                                                const std::chrono::system_clock::time_point& deadline,
                                                proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kCryptoGetProxyStakers, request, deadline, response);
}

//-----
void AccountStakersQuery::validateChecksums(const Client& client) const
{
  mAccountId.validateChecksum(client);
}

//-----
proto::Query AccountStakersQuery::buildRequest(proto::QueryHeader* header) const
{
  auto accountStakersQuery = std::make_unique<proto::CryptoGetStakersQuery>();
  accountStakersQuery->set_allocated_header(header);
  accountStakersQuery->set_allocated_accountid(mAccountId.toProtobuf().release());

  proto::Query query;
  query.set_allocated_cryptogetproxystakers(accountStakersQuery.release());
  return query;
}

//-----
proto::ResponseHeader AccountStakersQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.cryptogetproxystakers().header());
  return response.cryptogetproxystakers().header();
}

} // namespace Hiero
