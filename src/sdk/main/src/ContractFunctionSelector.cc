// SPDX-License-Identifier: Apache-2.0
#include "ContractFunctionSelector.h"
#include "exceptions/IllegalStateException.h"
#include "exceptions/OpenSSLException.h"
#include "impl/Utilities.h"
#include "impl/openssl_utils/OpenSSLUtils.h"

#include <openssl/evp.h>

namespace Hiero
{
//-----
ContractFunctionSelector& ContractFunctionSelector::addString()
{
  return addParameter("string");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addStringArray()
{
  return addParameter("string[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addBytes()
{
  return addParameter("bytes");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addBytesArray()
{
  return addParameter("bytes[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addBytes32()
{
  return addParameter("bytes32");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addBytes32Array()
{
  return addParameter("bytes32[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addBool()
{
  return addParameter("bool");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addInt8()
{
  return addParameter("int8");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addInt32()
{
  return addParameter("int32");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addInt64()
{
  return addParameter("int64");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addInt256()
{
  return addParameter("int256");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addInt8Array()
{
  return addParameter("int8[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addInt32Array()
{
  return addParameter("int32[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addInt64Array()
{
  return addParameter("int64[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addInt256Array()
{
  return addParameter("int256[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addUint8()
{
  return addParameter("uint8");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addUint32()
{
  return addParameter("uint32");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addUint64()
{
  return addParameter("uint64");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addUint256()
{
  return addParameter("uint256");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addUint8Array()
{
  return addParameter("uint8[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addUint32Array()
{
  return addParameter("uint32[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addUint64Array()
{
  return addParameter("uint64[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addUint256Array()
{
  return addParameter("uint256[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addAddress()
{
  return addParameter("address");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addAddressArray()
{
  return addParameter("address[]");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addFunction()
{
  return addParameter("function");
}

//-----
ContractFunctionSelector& ContractFunctionSelector::addParameter(std::string_view type)
{
  if (addComma)
  {
    mFunctionCall.push_back(std::byte(','));
  }

  mFunctionCall =
    internal::Utilities::concatenateVectors({ mFunctionCall, internal::Utilities::stringToByteVector(type) });
  addComma = true;

  return *this;
}

//-----
std::vector<std::byte> ContractFunctionSelector::finish(std::string_view name) const
{
  const std::vector<std::byte> keccak256Hash =
    internal::OpenSSLUtils::computeKECCAK256(internal::Utilities::concatenateVectors(
      { internal::Utilities::stringToByteVector(name), { std::byte('(') }, mFunctionCall, { std::byte(')') } }));
  return { keccak256Hash.cbegin(), keccak256Hash.cbegin() + 4 };
}

} // namespace Hiero
