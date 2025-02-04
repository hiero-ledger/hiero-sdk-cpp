// SPDX-License-Identifier: Apache-2.0
#include "FeeDataType.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
const std::unordered_map<proto::SubType, FeeDataType> gProtobufSubTypeToFeeDataType = {
  {proto::SubType::DEFAULT,                                     FeeDataType::DEFAULT                               },
  { proto::SubType::TOKEN_FUNGIBLE_COMMON,                      FeeDataType::TOKEN_FUNGIBLE_COMMON                 },
  { proto::SubType::TOKEN_NON_FUNGIBLE_UNIQUE,                  FeeDataType::TOKEN_NON_FUNGIBLE_UNIQUE             },
  { proto::SubType::TOKEN_FUNGIBLE_COMMON_WITH_CUSTOM_FEES,     FeeDataType::TOKEN_FUNGIBLE_COMMON_WITH_CUSTOM_FEES},
  { proto::SubType::TOKEN_NON_FUNGIBLE_UNIQUE_WITH_CUSTOM_FEES,
   FeeDataType::TOKEN_NON_FUNGIBLE_UNIQUE_WITH_CUSTOM_FEES                                                         },
  { proto::SubType::SCHEDULE_CREATE_CONTRACT_CALL,              FeeDataType::SCHEDULE_CREATE_CONTRACT_CALL         }
};

//-----
const std::unordered_map<FeeDataType, proto::SubType> gFeeDataTypeToProtobufSubType = {
  {FeeDataType::DEFAULT,                                     proto::SubType::DEFAULT                               },
  { FeeDataType::TOKEN_FUNGIBLE_COMMON,                      proto::SubType::TOKEN_FUNGIBLE_COMMON                 },
  { FeeDataType::TOKEN_NON_FUNGIBLE_UNIQUE,                  proto::SubType::TOKEN_NON_FUNGIBLE_UNIQUE             },
  { FeeDataType::TOKEN_FUNGIBLE_COMMON_WITH_CUSTOM_FEES,     proto::SubType::TOKEN_FUNGIBLE_COMMON_WITH_CUSTOM_FEES},
  { FeeDataType::TOKEN_NON_FUNGIBLE_UNIQUE_WITH_CUSTOM_FEES,
   proto::SubType::TOKEN_NON_FUNGIBLE_UNIQUE_WITH_CUSTOM_FEES                                                      },
  { FeeDataType::SCHEDULE_CREATE_CONTRACT_CALL,              proto::SubType::SCHEDULE_CREATE_CONTRACT_CALL         },
};

//-----
const std::unordered_map<FeeDataType, std::string> gFeeDataTypeToString = {
  {FeeDataType::DEFAULT,                                     "DEFAULT"                                   },
  { FeeDataType::TOKEN_FUNGIBLE_COMMON,                      "TOKEN_FUNGIBLE_COMMON"                     },
  { FeeDataType::TOKEN_NON_FUNGIBLE_UNIQUE,                  "TOKEN_NON_FUNGIBLE_UNIQUE"                 },
  { FeeDataType::TOKEN_FUNGIBLE_COMMON_WITH_CUSTOM_FEES,     "TOKEN_FUNGIBLE_COMMON_WITH_CUSTOM_FEES"    },
  { FeeDataType::TOKEN_NON_FUNGIBLE_UNIQUE_WITH_CUSTOM_FEES, "TOKEN_NON_FUNGIBLE_UNIQUE_WITH_CUSTOM_FEES"},
  { FeeDataType::SCHEDULE_CREATE_CONTRACT_CALL,              "SCHEDULE_CREATE_CONTRACT_CALL"             },
};

} // namespace Hiero
