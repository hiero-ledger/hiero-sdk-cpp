// SPDX-License-Identifier: Apache-2.0
#include "DelegateContractId.h"
#include "impl/EntityIdHelper.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
DelegateContractId::DelegateContractId(uint64_t num)
  : ContractId(num)
{
}

//-----
DelegateContractId::DelegateContractId(uint64_t shard, uint64_t realm, uint64_t num, std::string_view checksum)
  : ContractId(shard, realm, num, checksum)
{
}

//-----
bool DelegateContractId::operator==(const DelegateContractId& other) const
{
  return ContractId::operator==(other);
}

//-----
DelegateContractId DelegateContractId::fromString(std::string_view id)
{
  return DelegateContractId(internal::EntityIdHelper::getShardNum(id),
                            internal::EntityIdHelper::getRealmNum(id),
                            internal::EntityIdHelper::getEntityNum(id),
                            internal::EntityIdHelper::getChecksum(id));
}

//-----
DelegateContractId DelegateContractId::fromSolidityAddress(std::string_view address)
{
  return internal::EntityIdHelper::fromSolidityAddress<DelegateContractId>(
    internal::EntityIdHelper::decodeSolidityAddress(address));
}

//-----
DelegateContractId DelegateContractId::fromProtobuf(const proto::ContractID& proto)
{
  return DelegateContractId(static_cast<uint64_t>(proto.shardnum()),
                            static_cast<uint64_t>(proto.realmnum()),
                            static_cast<uint64_t>(proto.contractnum()));
}

//-----
DelegateContractId DelegateContractId::fromBytes(const std::vector<std::byte>& bytes)
{
  proto::ContractID proto;
  proto.ParseFromArray(bytes.data(), static_cast<int>(bytes.size()));
  return fromProtobuf(proto);
}

//-----
std::unique_ptr<Key> DelegateContractId::clone() const
{
  return std::make_unique<DelegateContractId>(*this);
}

//-----
std::unique_ptr<proto::Key> DelegateContractId::toProtobufKey() const
{
  auto proto = std::make_unique<proto::Key>();
  proto->set_allocated_delegatable_contract_id(toProtobuf().release());
  return proto;
}

} // namespace Hiero
