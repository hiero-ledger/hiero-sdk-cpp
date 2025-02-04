// SPDX-License-Identifier: Apache-2.0
#include "FreezeType.h"

#include <freeze_type.pb.h>

namespace Hiero
{
//-----
const std::unordered_map<proto::FreezeType, FreezeType> gProtobufFreezeTypeToFreezeType = {
  {proto::FreezeType::UNKNOWN_FREEZE_TYPE, FreezeType::UNKNOWN_FREEZE_TYPE},
  { proto::FreezeType::FREEZE_ONLY,        FreezeType::FREEZE_ONLY        },
  { proto::FreezeType::PREPARE_UPGRADE,    FreezeType::PREPARE_UPGRADE    },
  { proto::FreezeType::FREEZE_UPGRADE,     FreezeType::FREEZE_UPGRADE     },
  { proto::FreezeType::FREEZE_ABORT,       FreezeType::FREEZE_ABORT       },
  { proto::FreezeType::TELEMETRY_UPGRADE,  FreezeType::TELEMETRY_UPGRADE  },
};

//-----
const std::unordered_map<FreezeType, proto::FreezeType> gFreezeTypeToProtobufFreezeType = {
  {FreezeType::UNKNOWN_FREEZE_TYPE, proto::FreezeType::UNKNOWN_FREEZE_TYPE},
  { FreezeType::FREEZE_ONLY,        proto::FreezeType::FREEZE_ONLY        },
  { FreezeType::PREPARE_UPGRADE,    proto::FreezeType::PREPARE_UPGRADE    },
  { FreezeType::FREEZE_UPGRADE,     proto::FreezeType::FREEZE_UPGRADE     },
  { FreezeType::FREEZE_ABORT,       proto::FreezeType::FREEZE_ABORT       },
  { FreezeType::TELEMETRY_UPGRADE,  proto::FreezeType::TELEMETRY_UPGRADE  },
};

//-----
const std::unordered_map<FreezeType, const char*> gFreezeTypeToString = {
  {FreezeType::UNKNOWN_FREEZE_TYPE, "UNKNOWN_FREEZE_TYPE"},
  { FreezeType::FREEZE_ONLY,        "FREEZE_ONLY"        },
  { FreezeType::PREPARE_UPGRADE,    "PREPARE_UPGRADE"    },
  { FreezeType::FREEZE_UPGRADE,     "FREEZE_UPGRADE"     },
  { FreezeType::FREEZE_ABORT,       "FREEZE_ABORT"       },
  { FreezeType::TELEMETRY_UPGRADE,  "TELEMETRY_UPGRADE"  },
};

} // namespace Hiero
