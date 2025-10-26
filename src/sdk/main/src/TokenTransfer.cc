// SPDX-License-Identifier: Apache-2.0
#include "TokenTransfer.h"
#include "hooks/FungibleHookType.h"
#include "impl/Utilities.h"

#include <nlohmann/json.hpp>

#include <services/basic_types.pb.h>

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
TokenTransfer::TokenTransfer(TokenId tokenId,
                             AccountId accountId,
                             int64_t amount,
                             bool isApproved,
                             const FungibleHookCall& hookCall)
  : mTokenId(std::move(tokenId))
  , mAccountId(std::move(accountId))
  , mAmount(amount)
  , mIsApproval(isApproved)
  , mHookCall(hookCall)
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
  TokenTransfer transfer(
    tokenId, AccountId::fromProtobuf(proto.accountid()), proto.amount(), decimals, proto.is_approval());

  if (proto.has_pre_tx_allowance_hook())
  {
    transfer.mHookCall =
      FungibleHookCall::fromProtobuf(proto.pre_tx_allowance_hook(), FungibleHookType::PRE_TX_ALLOWANCE_HOOK);
  }

  if (proto.has_pre_post_tx_allowance_hook())
  {
    transfer.mHookCall =
      FungibleHookCall::fromProtobuf(proto.pre_post_tx_allowance_hook(), FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK);
  }

  return transfer;
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

  if (mHookCall.getHookType() == FungibleHookType::PRE_TX_ALLOWANCE_HOOK)
  {
    accountAmount->set_allocated_pre_tx_allowance_hook(mHookCall.toProtobuf().release());
  }
  else if (mHookCall.getHookType() == FungibleHookType::PRE_POST_TX_ALLOWANCE_HOOK)
  {
    accountAmount->set_allocated_pre_post_tx_allowance_hook(mHookCall.toProtobuf().release());
  }

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
  json["mHookType"] = gFungibleHookTypeToString.at(mHookCall.getHookType());
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const TokenTransfer& transfer)
{
  os << transfer.toString();
  return os;
}

} // namespace Hiero
