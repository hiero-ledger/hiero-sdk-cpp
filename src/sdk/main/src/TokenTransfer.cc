// SPDX-License-Identifier: Apache-2.0
#include "TokenTransfer.h"
#include "impl/Utilities.h"

#include <basic_types.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
TokenTransfer::TokenTransfer(TokenId tokenId, AccountId accountId, int64_t amount, bool isApproved)
  : mTokenId(std::move(tokenId))
  , mAccountId(std::move(accountId))
  , mAmount(amount)
  , mIsApproval(isApproved)
{
}

//-----
TokenTransfer::TokenTransfer(TokenId tokenId, AccountId accountId, int64_t amount, uint32_t decimals, bool isApproved)
  : mTokenId(std::move(tokenId))
  , mAccountId(std::move(accountId))
  , mAmount(amount)
  , mExpectedDecimals(decimals)
  , mIsApproval(isApproved)
{
}

//-----
TokenTransfer TokenTransfer::fromProtobuf(const proto::AccountAmount& proto, const TokenId& tokenId, uint32_t decimals)
{
  return { tokenId, AccountId::fromProtobuf(proto.accountid()), proto.amount(), decimals, proto.is_approval() };
}

//-----
TokenTransfer TokenTransfer::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::AccountAmount proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto, TokenId(), 0U);
}

//-----
void TokenTransfer::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);
  mAccountId.validateChecksum(client);
}

//-----
std::unique_ptr<proto::AccountAmount> TokenTransfer::toProtobuf() const
{
  auto accountAmount = std::make_unique<proto::AccountAmount>();
  accountAmount->set_allocated_accountid(mAccountId.toProtobuf().release());
  accountAmount->set_amount(mAmount);
  accountAmount->set_is_approval(mIsApproval);
  return accountAmount;
}

//-----
std::vector<std::byte> TokenTransfer::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string TokenTransfer::toString() const
{
  nlohmann::json json;
  json["mTokenId"] = mTokenId.toString();
  json["mAccountId"] = mAccountId.toString();
  json["mAmount"] = mAmount;
  json["mExpectedDecimals"] = mExpectedDecimals;
  json["mIsApproval"] = mIsApproval;
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const TokenTransfer& transfer)
{
  os << transfer.toString();
  return os;
}

} // namespace Hiero
