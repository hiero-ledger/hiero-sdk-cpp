// SPDX-License-Identifier: Apache-2.0
#include "ContractFunctionParameters.h"
#include "exceptions/OpenSSLException.h"
#include "impl/EntityIdHelper.h"
#include "impl/HexConverter.h"
#include "impl/Utilities.h"

#include <cmath>
#include <stdexcept>

namespace Hiero
{
namespace
{
// Padding that can substring without new allocations.
const std::vector<std::byte> POSITIVE_PADDING(31, std::byte(0x00));
const std::vector<std::byte> NEGATIVE_PADDING(31, std::byte(0xFF));

/**
 * Get the number of Solidity words there must be to contain the input byte array.
 *
 * @param bytes The bytes of which to determine the Solidity word count.
 * @return The number of Solidity words taken up by the byte array.
 */
[[nodiscard]] size_t getNumSolidityWords(const std::vector<std::byte>& bytes)
{
  return static_cast<size_t>(ceil(static_cast<double>(bytes.size()) / 32.0));
}

/**
 * Get the number of bytes that are required to pad the input byte array to be a full Solidity word.
 *
 * @param bytes The byte array to pad.
 * @return The number of bytes that are required to pad the input byte array to be a full Solidity word.
 */
[[nodiscard]] long getPaddingAmount(const std::vector<std::byte>& bytes)
{
  return static_cast<long>(getNumSolidityWords(bytes) * 32ULL - bytes.size());
}

/**
 * Pad the left of the input byte array to be a full Solidity word (or a multiple of).
 *
 * @param bytes    The byte array to pad.
 * @param negative \c TRUE if the bytes represent a negative integer value, otherwise \c FALSE.
 * @return The byte array padded to be a full Solidity word (or a multiple of).
 */
[[nodiscard]] std::vector<std::byte> leftPad(const std::vector<std::byte>& bytes, bool negative = false)
{
  const std::vector<std::byte>& padding = negative ? NEGATIVE_PADDING : POSITIVE_PADDING;
  return internal::Utilities::concatenateVectors({
    {padding.cbegin(), padding.cbegin() + getPaddingAmount(bytes)},
    bytes
  });
}

/**
 * Pad the right of the input byte array to be a full Solidity word (or a multiple of).
 *
 * @param bytes The byte array to pad.
 * @return The byte array padded to be a full Solidity word (or a multiple of).
 */
[[nodiscard]] std::vector<std::byte> rightPad(const std::vector<std::byte>& bytes, bool = false)
{
  return internal::Utilities::concatenateVectors({
    bytes, {POSITIVE_PADDING.cbegin(), POSITIVE_PADDING.cbegin() + getPaddingAmount(bytes)}
  });
}

/**
 * Check the length of a byte array and throw if larger than a Solidity word (32 bytes).
 *
 * @param bytes The byte array of which to check the length.
 * @return The input byte array.
 * @throws std::invalid_argument If the byte array is larger than 32 bytes.
 */
[[nodiscard]] std::vector<std::byte> checkByteArraySize(const std::vector<std::byte>& bytes)
{
  if (bytes.size() > 32ULL)
  {
    throw std::invalid_argument("Input byte array is larger than a Solidity word (32 bytes)");
  }

  return bytes;
}

/**
 * Encode a value to its representative Solidity byte array.
 *
 * @tparam ValType         The type of value to encode.
 * @tparam ToBytesFuncType The function type that can encode the value.
 * @tparam PaddingFuncType The function type that will pad the value.
 * @param val         The value to encode.
 * @param toBytesFunc The function to use to encode the value.
 * @param paddingFunc The function that will pad the encoded value.
 * @param negative    \c TRUE if the value is signed and should be padded as a negative number (i.e. padded with 0xFF
 *                    values instead of 0x00), otherwise \c FALSE.
 * @return The encoded and padded value.
 */
template<typename ValType, class ToBytesFuncType, class PaddingFuncType>
[[nodiscard]] std::vector<std::byte> encodeValue(const ValType& val,
                                                 const ToBytesFuncType& toBytesFunc,
                                                 const PaddingFuncType& paddingFunc,
                                                 bool negative = false)
{
  return paddingFunc(toBytesFunc(val), negative);
}

/**
 * Encode an array of non-integral types to its representative Solidity byte array.
 *
 * @tparam ValType         The type of values in the array to encode.
 * @tparam ToBytesFuncType The function type that can encode the values in the array.
 * @tparam PaddingFuncType The function type that will pad the values in the array.
 * @param valArray    The array to encode.
 * @param toBytesFunc The function to use to encode the values in the array.
 * @param paddingFunc The function that will pad the encoded values in the array.
 * @return The encoded byte array.
 */
template<typename ValType,
         typename ToBytesFuncType,
         typename PaddingFuncType,
         typename std::enable_if_t<!std::is_integral_v<ValType>, int> = 0>
[[nodiscard]] std::vector<std::byte> encodeArray(const std::vector<ValType>& valArray,
                                                 const ToBytesFuncType& toBytesFunc,
                                                 const PaddingFuncType& paddingFunc)
{
  // The first word of an array always contains the number of elements in the array.
  std::vector<std::byte> bytes = leftPad(internal::Utilities::getBytes(valArray.size()));
  for (const ValType& val : valArray)
  {
    bytes = internal::Utilities::concatenateVectors({ bytes, encodeValue(val, toBytesFunc, paddingFunc) });
  }

  return bytes;
}

/**
 * Encode an array of integral types to its representative Solidity byte array.
 *
 * @tparam ValType         The type of values in the array to encode.
 * @tparam ToBytesFuncType The function type that can encode the values in the array.
 * @tparam PaddingFuncType The function type that will pad the values in the array.
 * @param valArray    The array to encode.
 * @param toBytesFunc The function to use to encode the values in the array.
 * @param paddingFunc The function that will pad the encoded values in the array.
 * @return The encoded byte array.
 */
template<typename ValType,
         typename ToBytesFuncType,
         typename PaddingFuncType,
         typename std::enable_if_t<std::is_integral_v<ValType>, int> = 0>
[[nodiscard]] std::vector<std::byte> encodeArray(const std::vector<ValType>& valArray,
                                                 const ToBytesFuncType& toBytesFunc,
                                                 const PaddingFuncType& paddingFunc)
{
  // The first word of an array always contains the number of elements in the array.
  std::vector<std::byte> bytes = leftPad(internal::Utilities::getBytes(valArray.size()));
  for (const ValType& val : valArray)
  {
    bytes = internal::Utilities::concatenateVectors({ bytes, encodeValue(val, toBytesFunc, paddingFunc, val < 0) });
  }

  return bytes;
}

/**
 * Encode an array of dynamic types to its representative Solidity byte array.
 *
 * @tparam ValType         The type of values in the array to encode.
 * @tparam ToBytesFuncType The function type that can convert the values in the array.
 * @param valArray    The array to encode.
 * @param toBytesFunc The function to use to encode the values in the array.
 * @return The encoded byte array.
 */
template<typename ValType, typename ToBytesFuncType>
[[nodiscard]] std::vector<std::byte> encodeDynamicArray(const std::vector<ValType>& valArray,
                                                        const ToBytesFuncType& toBytesFunc)
{
  // Make one pass over all the values, keeping track of several things:
  //  - The offsets of each value's byte array.
  //  - The length of each value's byte array.
  //  - The value's byte array itself.
  std::vector<std::byte> offsets = leftPad(internal::Utilities::getBytes(valArray.size()));
  std::vector<std::byte> values;
  size_t offset = 32ULL * valArray.size();
  for (const auto& val : valArray)
  {
    // Add the current value offset to the offsets vector.
    offsets = internal::Utilities::concatenateVectors({ offsets, leftPad(internal::Utilities::getBytes(offset)) });

    // For a dynamic value, the encoded byte array should contain the encoded value's length, followed by the encoded
    // value itself.
    const std::vector<std::byte> valBytes = toBytesFunc(val);
    values = internal::Utilities::concatenateVectors({ values, valBytes });

    // Update the offset with the number of Solidity words in the encoded value.
    offset += 32ULL * getNumSolidityWords(valBytes);
  }

  // Concatenate the offsets, lengths, and values together.
  return internal::Utilities::concatenateVectors({ offsets, values });
}

/**
 * Encode a Solidity address to a byte array.
 *
 * @param address The Solidity address to encode.
 * @return The encoded Solidity address.
 */
[[nodiscard]] std::vector<std::byte> encodeAddress(std::string_view address)
{
  // Remove 0x prefix if it exists
  if (address.find("0x") == 0ULL)
  {
    address.remove_prefix(2);
  }

  if (address.size() != internal::EntityIdHelper::SOLIDITY_ADDRESS_LEN_HEX)
  {
    throw std::invalid_argument("Solidity addresses must be " +
                                std::to_string(internal::EntityIdHelper::SOLIDITY_ADDRESS_LEN_HEX) + " characters");
  }

  try
  {
    return internal::HexConverter::hexToBytes(address);
  }
  catch (const OpenSSLException&)
  {
    throw std::invalid_argument("Solidity address unable to be decoded");
  }
}

/**
 * Encode a byte array. This involves prepending the byte array with its length (in a Solidity word), and right padding
 * the byte array to a full Solidity word (or multiple of).
 *
 * @param bytes The byte array to encode.
 * @return The encoded byte array.
 */
[[nodiscard]] std::vector<std::byte> encodeBytes(const std::vector<std::byte>& bytes)
{
  return internal::Utilities::concatenateVectors(
    { leftPad(internal::Utilities::getBytes(bytes.size())), rightPad(bytes) });
}

/**
 * Encode a string to a byte array.
 *
 * @param str The string to encode.
 * @return The encoded string.
 */
[[nodiscard]] std::vector<std::byte> encodeString(std::string_view str)
{
  return encodeBytes(internal::Utilities::stringToByteVector(str));
}

} // namespace

//-----
ContractFunctionParameters& ContractFunctionParameters::addString(std::string_view param)
{
  mFunction.addString();
  mArguments.emplace_back(encodeString(param), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addStringArray(const std::vector<std::string>& param)
{
  mFunction.addStringArray();
  mArguments.emplace_back(encodeDynamicArray(param, encodeString), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addBytes(const std::vector<std::byte>& param)
{
  mFunction.addBytes();
  mArguments.emplace_back(encodeBytes(param), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addBytesArray(const std::vector<std::vector<std::byte>>& param)
{
  mFunction.addBytesArray();
  mArguments.emplace_back(encodeDynamicArray(param, encodeBytes), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addBytes32(const std::vector<std::byte>& param)
{
  mFunction.addBytes32();
  mArguments.emplace_back(encodeValue(param, checkByteArraySize, rightPad), false);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addBytes32Array(
  const std::vector<std::vector<std::byte>>& param)
{
  mFunction.addBytes32Array();
  mArguments.emplace_back(encodeArray(param, checkByteArraySize, rightPad), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addBool(bool param)
{
  mFunction.addBool();
  mArguments.emplace_back(leftPad(std::vector<std::byte>{ param ? std::byte(0x01) : std::byte(0x00) }), false);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addInt8(int8_t param)
{
  mFunction.addInt8();
  mArguments.emplace_back(leftPad(internal::Utilities::getBytes(param), param < 0), false);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addInt32(int32_t param)
{
  mFunction.addInt32();
  mArguments.emplace_back(leftPad(internal::Utilities::getBytes(param), param < 0), false);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addInt64(int64_t param)
{
  mFunction.addInt64();
  mArguments.emplace_back(leftPad(internal::Utilities::getBytes(param), param < 0), false);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addInt256(const std::vector<std::byte>& param)
{
  mFunction.addInt256();
  mArguments.emplace_back(
    encodeValue(
      param, checkByteArraySize, leftPad, (!param.empty()) && ((param.front() & std::byte(0x10)) == std::byte(0x10))),
    false);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addInt8Array(const std::vector<int8_t>& param)
{
  mFunction.addInt8Array();
  mArguments.emplace_back(encodeArray(param, internal::Utilities::getBytes<int8_t>, leftPad), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addInt32Array(const std::vector<int32_t>& param)
{
  mFunction.addInt32Array();
  mArguments.emplace_back(encodeArray(param, internal::Utilities::getBytes<int32_t>, leftPad), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addInt64Array(const std::vector<int64_t>& param)
{
  mFunction.addInt64Array();
  mArguments.emplace_back(encodeArray(param, internal::Utilities::getBytes<int64_t>, leftPad), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addInt256Array(const std::vector<std::vector<std::byte>>& param)
{
  mFunction.addInt256Array();
  mArguments.emplace_back(encodeArray(param, checkByteArraySize, leftPad), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addUint8(uint8_t param)
{
  mFunction.addUint8();
  mArguments.emplace_back(leftPad(internal::Utilities::getBytes(param)), false);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addUint32(uint32_t param)
{
  mFunction.addUint32();
  mArguments.emplace_back(leftPad(internal::Utilities::getBytes(param)), false);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addUint64(uint64_t param)
{
  mFunction.addUint64();
  mArguments.emplace_back(leftPad(internal::Utilities::getBytes(param)), false);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addUint256(const std::vector<std::byte>& param)
{
  mFunction.addUint256();
  mArguments.emplace_back(encodeValue(param, checkByteArraySize, leftPad), false);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addUint8Array(const std::vector<uint8_t>& param)
{
  mFunction.addUint8Array();
  mArguments.emplace_back(encodeArray(param, internal::Utilities::getBytes<uint8_t>, leftPad), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addUint32Array(const std::vector<uint32_t>& param)
{
  mFunction.addUint32Array();
  mArguments.emplace_back(encodeArray(param, internal::Utilities::getBytes<uint32_t>, leftPad), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addUint64Array(const std::vector<uint64_t>& param)
{
  mFunction.addUint64Array();
  mArguments.emplace_back(encodeArray(param, internal::Utilities::getBytes<uint64_t>, leftPad), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addUint256Array(
  const std::vector<std::vector<std::byte>>& param)
{
  mFunction.addUint256Array();
  mArguments.emplace_back(encodeArray(param, checkByteArraySize, leftPad), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addAddress(std::string_view param)
{
  mFunction.addAddress();
  mArguments.emplace_back(leftPad(encodeAddress(param)), false);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addAddressArray(const std::vector<std::string>& param)
{
  mFunction.addAddressArray();
  mArguments.emplace_back(encodeArray(param, encodeAddress, leftPad), true);
  return *this;
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addFunction(std::string_view address,
                                                                    const ContractFunctionSelector& selector)
{
  return addFunction(address, selector.finish());
}

//-----
ContractFunctionParameters& ContractFunctionParameters::addFunction(std::string_view address,
                                                                    const std::vector<std::byte>& selector)
{
  mFunction.addFunction();
  mArguments.emplace_back(rightPad(internal::Utilities::concatenateVectors({ encodeAddress(address), selector })),
                          false);
  return *this;
}

//-----
std::vector<std::byte> ContractFunctionParameters::toBytes(std::string_view name) const
{
  // Keep track of the offset for dynamic-length data. They come immediately after the value arguments.
  size_t dynamicOffset = mArguments.size() * 32ULL;

  std::vector<std::byte> paramsBytes;
  std::vector<std::byte> dynamicBytes;

  for (const Argument& arg : mArguments)
  {
    if (arg.mIsDynamic)
    {
      paramsBytes =
        internal::Utilities::concatenateVectors({ paramsBytes, leftPad(internal::Utilities::getBytes(dynamicOffset)) });
      dynamicBytes = internal::Utilities::concatenateVectors({ dynamicBytes, arg.mValue });
      dynamicOffset += arg.mValue.size();
    }
    else
    {
      paramsBytes = internal::Utilities::concatenateVectors({ paramsBytes, arg.mValue });
    }
  }

  // Encode the function name if one exists.
  std::vector<std::byte> functionBytes;
  if (!name.empty())
  {
    functionBytes = mFunction.finish(name);
  }

  return internal::Utilities::concatenateVectors({ functionBytes, paramsBytes, dynamicBytes });
}

} // namespace Hiero
