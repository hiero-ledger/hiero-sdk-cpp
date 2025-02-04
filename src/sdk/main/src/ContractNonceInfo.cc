// SPDX-License-Identifier: Apache-2.0
#include "ContractNonceInfo.h"
#include "impl/Utilities.h"

#include <contract_types.pb.h>

namespace Hiero
{
//-----
ContractNonceInfo::ContractNonceInfo(ContractId contractId, int64_t nonce)
  : mContractId(std::move(contractId))
  , mNonce(nonce)
{
}

//-----
bool ContractNonceInfo::operator==(const ContractNonceInfo& other) const
{
  return (mContractId == other.mContractId) && (mNonce == other.mNonce);
}

//-----
ContractNonceInfo ContractNonceInfo::fromProtobuf(const proto::ContractNonceInfo& proto)
{
  return { ContractId::fromProtobuf(proto.contract_id()), proto.nonce() };
}

//-----
ContractNonceInfo ContractNonceInfo::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ContractNonceInfo protoContractNonceInfo;
  protoContractNonceInfo.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(protoContractNonceInfo);
}

//-----
std::unique_ptr<proto::ContractNonceInfo> ContractNonceInfo::toProtobuf() const
{
  auto protoContractNonceInfo = std::make_unique<proto::ContractNonceInfo>();
  protoContractNonceInfo->set_allocated_contract_id(mContractId.toProtobuf().release());
  protoContractNonceInfo->set_nonce(mNonce);
  return protoContractNonceInfo;
}

//-----
std::vector<std::byte> ContractNonceInfo::toBytes() const
{
  return internal::Utilities::stringToByteVector(toProtobuf()->SerializeAsString());
}

//-----
std::string ContractNonceInfo::toString() const
{
  std::string str = mContractId.toString();

  if (mNonce)
  {
    str += '.' + std::to_string(mNonce);
  }

  return str;
}

//-----
std::ostream& operator<<(std::ostream& os, const ContractNonceInfo& nonceInfo)
{
  os << nonceInfo.toString();
  return os;
}

} // namespace Hiero
