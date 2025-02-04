// SPDX-License-Identifier: Apache-2.0
#include "TokenAssociation.h"
#include "impl/Utilities.h"

#include <custom_fees.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
TokenAssociation TokenAssociation::fromProtobuf(const proto::TokenAssociation& proto)
{
  TokenAssociation tokenAssociation;

  if (proto.has_account_id())
  {
    tokenAssociation.mAccountId = AccountId::fromProtobuf(proto.account_id());
  }

  if (proto.has_token_id())
  {
    tokenAssociation.mTokenId = TokenId::fromProtobuf(proto.token_id());
  }

  return tokenAssociation;
}

//-----
TokenAssociation TokenAssociation::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::TokenAssociation tokenAssociation;
  tokenAssociation.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(tokenAssociation);
}

//-----
std::unique_ptr<proto::TokenAssociation> TokenAssociation::toProtobuf() const
{
  auto tokenAssociation = std::make_unique<proto::TokenAssociation>();
  tokenAssociation->set_allocated_account_id(mAccountId.toProtobuf().release());
  tokenAssociation->set_allocated_token_id(mTokenId.toProtobuf().release());
  return tokenAssociation;
}

//-----
std::vector<std::byte> TokenAssociation::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string TokenAssociation::toString() const
{
  nlohmann::json json;
  json["mAccountId"] = mAccountId.toString();
  json["mTokenId"] = mTokenId.toString();
  return json.dump();
}

} // namespace Hiero