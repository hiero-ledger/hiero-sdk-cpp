// SPDX-License-Identifier: Apache-2.0
#include "impl/Utilities.h"

#include <algorithm>
#include <charconv>
#include <fstream>
#include <iostream>
#include <random>

using json = nlohmann::json;

namespace Hiero::internal::Utilities
{
//-----
bool isPrefixOf(const std::vector<std::byte>& bytes, const std::vector<std::byte>& prefix)
{
  if (prefix.size() > bytes.size())
  {
    return false;
  }

  for (std::size_t i = 0; i < prefix.size(); ++i)
  {
    if (bytes.at(i) != prefix.at(i))
    {
      return false;
    }
  }

  return true;
}

//-----
std::vector<std::byte> removePrefix(const std::vector<std::byte>& bytes, long num)
{
  return { bytes.cbegin() + num, bytes.cend() };
}

//-----
std::vector<std::byte> concatenateVectors(const std::vector<std::vector<std::byte>>& vectors)
{
  std::vector<std::byte> bytes;
  for (const std::vector<std::byte>& vec : vectors)
  {
    bytes.insert(bytes.end(), vec.cbegin(), vec.cend());
  }

  return bytes;
}

//-----
std::vector<std::byte> stringToByteVector(std::string_view str)
{
  std::vector<std::byte> bytes;
  std::transform(str.cbegin(), str.cend(), std::back_inserter(bytes), [](char c) { return std::byte(c); });
  return bytes;
}

//-----
std::byte stringToByte(std::string_view str)
{
  unsigned char byte;
  if (const auto result = std::from_chars(str.data(), str.data() + str.size(), byte);
      result.ptr != str.data() + str.size() || result.ec != std::errc())
  {
    throw std::invalid_argument("Cannot convert string to byte");
  }

  return std::byte(byte);
}

//-----
std::string byteVectorToString(const std::vector<std::byte>& bytes)
{
  std::string str;
  std::transform(
    bytes.cbegin(), bytes.cend(), std::back_inserter(str), [](std::byte byte) { return static_cast<char>(byte); });
  return str;
}

//-----
unsigned int getRandomNumber(unsigned int lowerBound, unsigned int upperBound)
{
  static std::random_device rand;
  static std::mt19937 eng(rand());

  std::uniform_int_distribution dis(lowerBound, upperBound);
  return dis(eng);
}

//-----
json fromConfigFile(std::string_view path)
{
  std::ifstream infile(path.data());
  if (!infile.is_open())
  {
    throw std::invalid_argument(std::string("File cannot be found at ") + path.data());
  }

  // Make sure the input file is valid JSON.
  nlohmann::json jsonObj;
  try
  {
    jsonObj = nlohmann::json::parse(infile);
  }
  catch (const std::exception& ex)
  {
    throw std::invalid_argument(std::string("Cannot parse JSON: ") + ex.what());
  }

  return jsonObj;
}

} // namespace Hiero::internal::Utilities
