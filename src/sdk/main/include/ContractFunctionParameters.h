// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_CONTRACT_FUNCTION_PARAMETERS_H_
#define HIERO_SDK_CPP_CONTRACT_FUNCTION_PARAMETERS_H_

#include "ContractFunctionSelector.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace Hiero
{
class ContractFunctionParameters
{
public:
  /**
   * Add a string parameter type.
   *
   * @param param The string parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added string parameter.
   */
  ContractFunctionParameters& addString(std::string_view param);

  /**
   * Add a string array parameter type.
   *
   * @param param The string array parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added string array parameter.
   */
  ContractFunctionParameters& addStringArray(const std::vector<std::string>& param);

  /**
   * Add a bytes parameter type.
   *
   * @param param The bytes parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added bytes parameter.
   */
  ContractFunctionParameters& addBytes(const std::vector<std::byte>& param);

  /**
   * Add a bytes array parameter type.
   *
   * @param param The bytes array parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added bytes array parameter.
   */
  ContractFunctionParameters& addBytesArray(const std::vector<std::vector<std::byte>>& param);

  /**
   * Add a bytes32 parameter type.
   *
   * @param param The bytes32 parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added bytes32 parameter.
   * @throws std::invalid_argument If the byte array length is greater than 32.
   */
  ContractFunctionParameters& addBytes32(const std::vector<std::byte>& param);

  /**
   * Add a bytes32 array parameter type.
   *
   * @param param The bytes32 array parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added bytes32 array parameter.
   * @throws std::invalid_argument If any of the byte array lengths are greater than 32.
   */
  ContractFunctionParameters& addBytes32Array(const std::vector<std::vector<std::byte>>& param);

  /**
   * Add a bool parameter type.
   *
   * @param param The bool parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added bool parameter.
   */
  ContractFunctionParameters& addBool(bool param);

  /**
   * Add an int8 parameter type.
   *
   * @param param The int8 parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added int8 parameter.
   */
  ContractFunctionParameters& addInt8(int8_t param);

  /**
   * Add an int32 parameter type.
   *
   * @param param The int32 parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added int32 parameter.
   */
  ContractFunctionParameters& addInt32(int32_t param);

  /**
   * Add an int64 parameter type.
   *
   * @param param The int64 parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added int64 parameter.
   */
  ContractFunctionParameters& addInt64(int64_t param);

  /**
   * Add an int256 parameter type.
   *
   * @param param The int256 parameter to add, as a byte array.
   * @return A reference to this ContractFunctionParameters with the newly-added int256 parameter.
   * @throws std::invalid_argument If in the input byte array isn't 32 bytes.
   */
  ContractFunctionParameters& addInt256(const std::vector<std::byte>& param);

  /**
   * Add an int8 array parameter type.
   *
   * @param param The int8 array parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added int8 array parameter.
   */
  ContractFunctionParameters& addInt8Array(const std::vector<int8_t>& param);

  /**
   * Add an int32 array parameter type.
   *
   * @param param The int32 array parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added int32 array parameter.
   */
  ContractFunctionParameters& addInt32Array(const std::vector<int32_t>& param);

  /**
   * Add an int64 array parameter type.
   *
   * @param param The int64 array parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added int64 array parameter.
   */
  ContractFunctionParameters& addInt64Array(const std::vector<int64_t>& param);

  /**
   * Add an int256 array parameter type.
   *
   * @param param The int256 array parameter to add, as an array of byte arrays.
   * @return A reference to this ContractFunctionParameters with the newly-added int256 array parameter.
   */
  ContractFunctionParameters& addInt256Array(const std::vector<std::vector<std::byte>>& param);

  /**
   * Add a uint8 parameter type.
   *
   * @param param The uint8 parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added uint8 parameter.
   */
  ContractFunctionParameters& addUint8(uint8_t param);

  /**
   * Add a uint32 parameter type.
   *
   * @param param The uint32 parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added uint32 parameter.
   */
  ContractFunctionParameters& addUint32(uint32_t param);

  /**
   * Add a uint64 parameter type.
   *
   * @param param The uint64 parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added uint64 parameter.
   */
  ContractFunctionParameters& addUint64(uint64_t param);

  /**
   * Add a uint256 parameter type.
   *
   * @param param The uint256 parameter to add, as a byte array.
   * @return A reference to this ContractFunctionParameters with the newly-added uint256 parameter.
   * @throws std::invalid_argument If in the input byte array isn't 32 bytes.
   */
  ContractFunctionParameters& addUint256(const std::vector<std::byte>& param);

  /**
   * Add a uint8 array parameter type.
   *
   * @param param The uint8 array parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added uint8 array parameter.
   */
  ContractFunctionParameters& addUint8Array(const std::vector<uint8_t>& param);

  /**
   * Add a uint32 array parameter type.
   *
   * @param param The uint32 array parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added uint32 array parameter.
   */
  ContractFunctionParameters& addUint32Array(const std::vector<uint32_t>& param);

  /**
   * Add a uint64 array parameter type.
   *
   * @param param The uint64 array parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added uint64 array parameter.
   */
  ContractFunctionParameters& addUint64Array(const std::vector<uint64_t>& param);

  /**
   * Add a uint256 array parameter type.
   *
   * @param param The uint256 array parameter to add, as an array of byte arrays.
   * @return A reference to this ContractFunctionParameters with the newly-added uint256 array parameter.
   */
  ContractFunctionParameters& addUint256Array(const std::vector<std::vector<std::byte>>& param);

  /**
   * Add a Solidity address parameter type.
   *
   * @param param The Solidity address parameter to add, as a string.
   * @return A reference to this ContractFunctionParameters with the newly-added address parameter.
   * @throws std::invalid_argument If the input address is malformed.
   */
  ContractFunctionParameters& addAddress(std::string_view param);

  /**
   * Add a Solidity address array parameter type.
   *
   * @param param The Solidity address array parameter to add.
   * @return A reference to this ContractFunctionParameters with the newly-added address array parameter.
   * @throws std::invalid_argument If any input address is malformed.
   */
  ContractFunctionParameters& addAddressArray(const std::vector<std::string>& param);

  /**
   * Add a Solidity function parameter type.
   *
   * @param address  The Solidity address of the function.
   * @param selector The ContractFunctionSelector to use to call the function, or the selector bytes.
   * @return A reference to this ContractFunctionParameters with the newly-added function parameter.
   * @throws std::invalid_argument If the input address is malformed.
   */
  ContractFunctionParameters& addFunction(std::string_view address, const ContractFunctionSelector& selector);
  ContractFunctionParameters& addFunction(std::string_view address, const std::vector<std::byte>& selector);

  /**
   * Get the encoding of the currently-added parameters.
   *
   * @param name The name of the function to which these parameters are being passed.
   * @return The encoding of the function parameters.
   */
  [[nodiscard]] std::vector<std::byte> toBytes(std::string_view name = "") const;

private:
  /**
   * Wrapper struct to hold argument data.
   */
  struct Argument
  {
    Argument(std::vector<std::byte> value, bool dynamic)
      : mValue(std::move(value))
      , mIsDynamic(dynamic)
    {
    }

    /**
     * The argument data.
     */
    std::vector<std::byte> mValue;

    /**
     * Is this argument dynamic?
     */
    bool mIsDynamic = false;
  };

  /**
   * The list of arguments to add ot the function call.
   */
  std::vector<Argument> mArguments;

  /**
   * The contract function to call.
   */
  ContractFunctionSelector mFunction;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_CONTRACT_FUNCTION_PARAMETERS_H_
