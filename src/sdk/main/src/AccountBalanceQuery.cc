// SPDX-License-Identifier: Apache-2.0
#include "AccountBalanceQuery.h"
#include "AccountBalance.h"
#include "TokenId.h"
#include "exceptions/UninitializedException.h"
#include "impl/Node.h"

#include <nlohmann/json.hpp>

#include <services/crypto_get_account_balance.pb.h>
#include <services/query.pb.h>
#include <services/query_header.pb.h>
#include <services/response.pb.h>

#include <cstddef>
#include <string>

using json = nlohmann::json;

namespace Hiero
{
//-----
AccountBalanceQuery& AccountBalanceQuery::setAccountId(const AccountId& accountId)
{
  mAccountId = accountId;
  mContractId.reset();

  return *this;
}

//-----
AccountBalanceQuery& AccountBalanceQuery::setContractId(const ContractId& contractId)
{
  mContractId = contractId;
  mAccountId.reset();

  return *this;
}

//-----
AccountBalance AccountBalanceQuery::mapResponse(const proto::Response& response) const
{
  AccountBalance accountBalance = AccountBalance::fromProtobuf(response.cryptogetaccountbalance());
  return accountBalance;
}

//-----
grpc::Status AccountBalanceQuery::submitRequest(const proto::Query& request,
                                                const std::shared_ptr<internal::Node>& node,
                                                const std::chrono::system_clock::time_point& deadline,
                                                proto::Response* response) const
{
  return node->submitQuery(proto::Query::QueryCase::kCryptogetAccountBalance, request, deadline, response);
}

//-----
void AccountBalanceQuery::validateChecksums(const Client& client) const
{
  if (mAccountId.has_value())
  {
    mAccountId->validateChecksum(client);
  }

  if (mContractId.has_value())
  {
    mContractId->validateChecksum(client);
  }
}

//-----
proto::Query AccountBalanceQuery::buildRequest(proto::QueryHeader* header) const
{
  auto accountBalanceQuery = std::make_unique<proto::CryptoGetAccountBalanceQuery>();
  accountBalanceQuery->set_allocated_header(header);

  if (mAccountId)
  {
    accountBalanceQuery->set_allocated_accountid(mAccountId->toProtobuf().release());
  }

  if (mContractId)
  {
    accountBalanceQuery->set_allocated_contractid(mContractId->toProtobuf().release());
  }

  proto::Query query;
  query.set_allocated_cryptogetaccountbalance(accountBalanceQuery.release());
  return query;
}

//-----
proto::ResponseHeader AccountBalanceQuery::mapResponseHeader(const proto::Response& response) const
{
  saveCostFromHeader(response.cryptogetaccountbalance().header());
  return response.cryptogetaccountbalance().header();
}

} // namespace Hiero
