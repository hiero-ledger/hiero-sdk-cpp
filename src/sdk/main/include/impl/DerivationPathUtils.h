// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_IMPL_DERIVATION_PATH_UTILS_H_
#define HIERO_SDK_CPP_IMPL_DERIVATION_PATH_UTILS_H_

#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * Utility functions dealing with wallet derivation paths, as described in BIP32, BIP44, and SLIP10.
 */
namespace Hiero::internal::DerivationPathUtils
{
/**
 * Check if an index falls into the "unhardened" or "hardened" category.
 * Unhardened: [0, 2147483647]; Hardened: [2147483648, UINT32_MAX]
 *
 * @param index The index to check.
 * @return \c TRUE if the index is in the hardened range, otherwise \c FALSE.
 */
bool isHardenedChildIndex(uint32_t index);

/**
 * Convert an index into its hardened counterpart (e.g. 0 -> 2147483648, 1-> 2147483649, etc.).
 *
 * @param index The index of which to get the hardened counterpart.
 * @return The hardened index.
 * @throws std::invalid_argument If the index is already hardened.
 */
uint32_t getHardenedIndex(uint32_t index);

/**
 * Converts a uint32_t index into a big endian byte array (direct implementation of ser32 function from BIP 32)
 *
 * @param childIndex The index to convert to a big endian byte array.
 * @return The big endian byte array representing the child index.
 */
std::vector<std::byte> indexToBigEndianArray(uint32_t childIndex);

}; // namespace Hiero::internal::DerivationPathUtils

#endif // HIERO_SDK_CPP_IMPL_DERIVATION_PATH_UTILS_H_
