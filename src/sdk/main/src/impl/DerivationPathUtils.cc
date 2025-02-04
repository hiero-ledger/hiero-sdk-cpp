// SPDX-License-Identifier: Apache-2.0
#include "impl/DerivationPathUtils.h"

#include <iostream>
#include <stdexcept>

namespace Hiero::internal::DerivationPathUtils
{
//-----
bool isHardenedChildIndex(uint32_t index)
{
  return index >> 31 == 0x1;
}

//-----
uint32_t getHardenedIndex(uint32_t index)
{
  if (isHardenedChildIndex(index))
  {
    throw std::invalid_argument("Index is already hardened");
  }

  return ~(UINT32_MAX >> 1) | index;
}

//-----
std::vector<std::byte> indexToBigEndianArray(uint32_t childIndex)
{
  std::vector<std::byte> indexVector;
  for (int byteIndex = 3; byteIndex >= 0; --byteIndex)
  {
    indexVector.push_back(std::byte((childIndex >> (byteIndex << 3)) & 0xFF));
  }

  return indexVector;
}

} // namespace Hiero::internal::DerivationPathUtils
