// SPDX-License-Identifier: Apache-2.0
#include "TokenKeyValidation.h"

#include <basic_types.pb.h>

namespace Hiero
{
//-----
const std::unordered_map<proto::TokenKeyValidation, TokenKeyValidation>
  gProtobufTokenKeyValidationToTokenKeyValidation = {
    {proto::TokenKeyValidation::FULL_VALIDATION, TokenKeyValidation::FULL_VALIDATION},
    { proto::TokenKeyValidation::NO_VALIDATION,  TokenKeyValidation::NO_VALIDATION  }
};

//-----
const std::unordered_map<TokenKeyValidation, proto::TokenKeyValidation>
  gTokenKeyValidationToProtobufTokenKeyValidation = {
    {TokenKeyValidation::FULL_VALIDATION, proto::TokenKeyValidation::FULL_VALIDATION},
    { TokenKeyValidation::NO_VALIDATION,  proto::TokenKeyValidation::NO_VALIDATION  }
};

//-----
const std::unordered_map<TokenKeyValidation, const char*> gTokenKeyValidationToString = {
  {TokenKeyValidation::FULL_VALIDATION, "FULL_VALIDATION"},
  { TokenKeyValidation::NO_VALIDATION,  "NO_VALIDATION"  }
};
} // namespace Hiero