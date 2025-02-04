// SPDX-License-Identifier: Apache-2.0
#include "TokenNftAllowance.h"
#include "impl/Utilities.h"

#include <crypto_approve_allowance.pb.h>
#include <crypto_delete_allowance.pb.h>

namespace Hiero
{
//-----
TokenNftAllowance::TokenNftAllowance(const std::optional<TokenId>& tokenId,
                                     const std::optional<AccountId>& owner,
                                     const std::optional<AccountId>& spender,
                                     const std::vector<uint64_t>& serialNumbers,
                                     const std::optional<bool>& allowAll,
                                     const std::optional<AccountId>& delegatingSpender)
  : mTokenId(tokenId)
  , mOwnerAccountId(owner)
  , mSpenderAccountId(spender)
  , mSerialNumbers(serialNumbers)
  , mApprovedForAll(allowAll)
  , mDelegatingSpenderAccountId(delegatingSpender)
{
}

//-----
TokenNftAllowance TokenNftAllowance::fromProtobuf(const proto::NftAllowance& proto)
{
  TokenNftAllowance allowance;

  if (proto.has_tokenid())
  {
    allowance.mTokenId = TokenId::fromProtobuf(proto.tokenid());
  }

  if (proto.has_owner())
  {
    allowance.mOwnerAccountId = AccountId::fromProtobuf(proto.owner());
  }

  if (proto.has_spender())
  {
    allowance.mSpenderAccountId = AccountId::fromProtobuf(proto.spender());
  }

  for (int i = 0; i < proto.serial_numbers_size(); ++i)
  {
    allowance.mSerialNumbers.push_back(static_cast<uint64_t>(proto.serial_numbers(i)));
  }

  if (proto.has_approved_for_all())
  {
    allowance.mApprovedForAll = proto.approved_for_all().value();
  }

  if (proto.has_delegating_spender())
  {
    allowance.mDelegatingSpenderAccountId = AccountId::fromProtobuf(proto.delegating_spender());
  }

  return allowance;
}

//-----
TokenNftAllowance TokenNftAllowance::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::NftAllowance nftAllowance;
  nftAllowance.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(nftAllowance);
}

//-----
void TokenNftAllowance::validateChecksums(const Client& client) const
{
  if (mTokenId.has_value())
  {
    mTokenId->validateChecksum(client);
  }

  if (mOwnerAccountId.has_value())
  {
    mOwnerAccountId->validateChecksum(client);
  }

  if (mSpenderAccountId.has_value())
  {
    mSpenderAccountId->validateChecksum(client);
  }
}

//-----
std::unique_ptr<proto::NftAllowance> TokenNftAllowance::toProtobuf() const
{
  auto proto = std::make_unique<proto::NftAllowance>();

  if (mTokenId.has_value())
  {
    proto->set_allocated_tokenid(mTokenId->toProtobuf().release());
  }

  if (mOwnerAccountId.has_value())
  {
    proto->set_allocated_owner(mOwnerAccountId->toProtobuf().release());
  }

  if (mSpenderAccountId.has_value())
  {
    proto->set_allocated_spender(mSpenderAccountId->toProtobuf().release());
  }

  for (const uint64_t& serialNumber : mSerialNumbers)
  {
    proto->add_serial_numbers(static_cast<int64_t>(serialNumber));
  }

  if (mApprovedForAll.has_value())
  {
    auto value = std::make_unique<google::protobuf::BoolValue>();
    value->set_value(*mApprovedForAll);
    proto->set_allocated_approved_for_all(value.release());
  }

  if (mDelegatingSpenderAccountId.has_value())
  {
    proto->set_allocated_delegating_spender(mDelegatingSpenderAccountId->toProtobuf().release());
  }

  return proto;
}

//-----
std::unique_ptr<proto::NftRemoveAllowance> TokenNftAllowance::toRemoveProtobuf() const
{
  auto proto = std::make_unique<proto::NftRemoveAllowance>();

  if (mTokenId.has_value())
  {
    proto->set_allocated_token_id(mTokenId->toProtobuf().release());
  }

  if (mOwnerAccountId.has_value())
  {
    proto->set_allocated_owner(mOwnerAccountId->toProtobuf().release());
  }

  for (const uint64_t& serialNumber : mSerialNumbers)
  {
    proto->add_serial_numbers(static_cast<int64_t>(serialNumber));
  }

  return proto;
}

//-----
std::vector<std::byte> TokenNftAllowance::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

} // namespace Hiero
