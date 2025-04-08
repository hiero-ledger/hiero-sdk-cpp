// SPDX-License-Identifier: Apache-2.0
#include "TokenAllowance.h"
#include "impl/Utilities.h"

#include <services/crypto_approve_allowance.pb.h>

namespace Hiero
{
//-----
TokenAllowance::TokenAllowance(const TokenId& tokenId, AccountId owner, AccountId spender, int64_t amount)
  : mTokenId(tokenId)
  , mOwnerAccountId(std::move(owner))
  , mSpenderAccountId(std::move(spender))
  , mAmount(amount)
{
}

//-----
TokenAllowance TokenAllowance::fromProtobuf(const proto::TokenAllowance& proto)
{
  return { proto.has_tokenid() ? TokenId::fromProtobuf(proto.tokenid()) : TokenId(),
           proto.has_owner() ? AccountId::fromProtobuf(proto.owner()) : AccountId(),
           proto.has_spender() ? AccountId::fromProtobuf(proto.spender()) : AccountId(),
           proto.amount() };
}

//-----
TokenAllowance TokenAllowance::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::TokenAllowance tokenAllowance;
  tokenAllowance.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(tokenAllowance);
}

//-----
void TokenAllowance::validateChecksums(const Client& client) const
{
  mTokenId.validateChecksum(client);
  mOwnerAccountId.validateChecksum(client);
  mSpenderAccountId.validateChecksum(client);
}

//-----
std::unique_ptr<proto::TokenAllowance> TokenAllowance::toProtobuf() const
{
  auto proto = std::make_unique<proto::TokenAllowance>();

  if (!(mTokenId == TokenId()))
  {
    proto->set_allocated_tokenid(mTokenId.toProtobuf().release());
  }

  if (!(mOwnerAccountId == AccountId()))
  {
    proto->set_allocated_owner(mOwnerAccountId.toProtobuf().release());
  }

  if (!(mSpenderAccountId == AccountId()))
  {
    proto->set_allocated_spender(mSpenderAccountId.toProtobuf().release());
  }

  proto->set_amount(mAmount);
  return proto;
}

//-----
std::vector<std::byte> TokenAllowance::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

} // namespace Hiero
