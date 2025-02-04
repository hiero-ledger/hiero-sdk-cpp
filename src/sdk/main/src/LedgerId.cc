// SPDX-License-Identifier: Apache-2.0
#include "LedgerId.h"
#include "exceptions/OpenSSLException.h"
#include "impl/HexConverter.h"

#include <stdexcept>

namespace Hiero
{
//-----
const LedgerId LedgerId::MAINNET = LedgerId({ std::byte(0x00) });

//-----
const LedgerId LedgerId::TESTNET = LedgerId({ std::byte(0x01) });

//-----
const LedgerId LedgerId::PREVIEWNET = LedgerId({ std::byte(0x02) });

//-----
LedgerId::LedgerId(std::vector<std::byte> bytes)
  : mLedgerId(std::move(bytes))
{
}

//-----
bool LedgerId::operator==(const LedgerId& other) const
{
  return mLedgerId == other.mLedgerId;
}

//-----
LedgerId LedgerId::fromString(std::string_view id)
{
  if (id == "mainnet")
  {
    return MAINNET;
  }
  else if (id == "testnet")
  {
    return TESTNET;
  }
  else if (id == "previewnet")
  {
    return PREVIEWNET;
  }
  else
  {
    try
    {
      return LedgerId(internal::HexConverter::hexToBytes(id));
    }
    catch (const OpenSSLException&)
    {
      throw std::invalid_argument("LedgerId cannot be realized from input hex-encoded string");
    }
  }
}

//-----
std::string LedgerId::toString() const
{
  if (isMainnet())
  {
    return "mainnet";
  }
  else if (isTestnet())
  {
    return "testnet";
  }
  else if (isPreviewnet())
  {
    return "previewnet";
  }
  else
  {
    return internal::HexConverter::bytesToHex(mLedgerId);
  }
}

} // namespace Hiero
