// SPDX-License-Identifier: Apache-2.0
#include "AssessedCustomFee.h"
#include "impl/Utilities.h"

#include <custom_fees.pb.h>
#include <nlohmann/json.hpp>

namespace Hiero
{
//-----
AssessedCustomFee AssessedCustomFee::fromProtobuf(const proto::AssessedCustomFee& proto)
{
  AssessedCustomFee assessedCustomFee;
  assessedCustomFee.mAmount = proto.amount();

  if (proto.has_token_id())
  {
    assessedCustomFee.mTokenId = TokenId::fromProtobuf(proto.token_id());
  }

  if (proto.has_fee_collector_account_id())
  {
    assessedCustomFee.mFeeCollectorAccountId = AccountId::fromProtobuf(proto.fee_collector_account_id());
  }

  for (int i = 0; i < proto.effective_payer_account_id_size(); ++i)
  {
    assessedCustomFee.mPayerAccountIdList.push_back(AccountId::fromProtobuf(proto.effective_payer_account_id(i)));
  }

  return assessedCustomFee;
}

//-----
AssessedCustomFee AssessedCustomFee::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::AssessedCustomFee assessedCustomFee;
  assessedCustomFee.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(assessedCustomFee);
}

//-----
std::unique_ptr<proto::AssessedCustomFee> AssessedCustomFee::toProtobuf() const
{
  auto assessedCustomFee = std::make_unique<proto::AssessedCustomFee>();
  assessedCustomFee->set_amount(mAmount);

  if (mTokenId.has_value())
  {
    assessedCustomFee->set_allocated_token_id(mTokenId->toProtobuf().release());
  }

  assessedCustomFee->set_allocated_fee_collector_account_id(mFeeCollectorAccountId.toProtobuf().release());

  for (const auto& accountId : mPayerAccountIdList)
  {
    *assessedCustomFee->add_effective_payer_account_id() = *accountId.toProtobuf();
  }

  return assessedCustomFee;
}

//-----
std::vector<std::byte> AssessedCustomFee::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string AssessedCustomFee::toString() const
{
  nlohmann::json json;
  json["mAmount"] = mAmount;

  if (mTokenId.has_value())
  {
    json["mTokenId"] = mTokenId->toString();
  }

  json["mFeeCollectorAccountId"] = mFeeCollectorAccountId.toString();
  std::for_each(mPayerAccountIdList.cbegin(),
                mPayerAccountIdList.cend(),
                [&json](const AccountId& accountId) { json["mPayerAccountIdList"].push_back(accountId.toString()); });
  return json.dump();
}

//-----
std::ostream& operator<<(std::ostream& os, const AssessedCustomFee& fee)
{
  os << fee.toString();
  return os;
}

} // namespace Hiero
