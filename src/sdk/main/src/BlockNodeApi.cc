// SPDX-License-Identifier: Apache-2.0
#include "BlockNodeApi.h"

#include <services/registered_service_endpoint.pb.h>

namespace Hiero
{

using ProtoApi = com::hedera::hapi::node::addressbook::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi;

//-----
const std::unordered_map<ProtoApi, BlockNodeApi> gProtobufBlockNodeApiToBlockNodeApi = {
  {ProtoApi::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_OTHER,            BlockNodeApi::OTHER           },
  {ProtoApi::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_STATUS,           BlockNodeApi::STATUS          },
  {ProtoApi::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_PUBLISH,          BlockNodeApi::PUBLISH         },
  {ProtoApi::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_SUBSCRIBE_STREAM, BlockNodeApi::SUBSCRIBE_STREAM},
  {ProtoApi::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_STATE_PROOF,      BlockNodeApi::STATE_PROOF     },
};

//-----
const std::unordered_map<BlockNodeApi, ProtoApi> gBlockNodeApiToProtobufBlockNodeApi = {
  {BlockNodeApi::OTHER,            ProtoApi::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_OTHER           },
  {BlockNodeApi::STATUS,           ProtoApi::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_STATUS          },
  {BlockNodeApi::PUBLISH,          ProtoApi::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_PUBLISH         },
  {BlockNodeApi::SUBSCRIBE_STREAM, ProtoApi::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_SUBSCRIBE_STREAM},
  {BlockNodeApi::STATE_PROOF,      ProtoApi::RegisteredServiceEndpoint_BlockNodeEndpoint_BlockNodeApi_STATE_PROOF     },
};

//-----
const std::unordered_map<BlockNodeApi, const char*> gBlockNodeApiToString = {
  {BlockNodeApi::OTHER,            "OTHER"           },
  {BlockNodeApi::STATUS,           "STATUS"          },
  {BlockNodeApi::PUBLISH,          "PUBLISH"         },
  {BlockNodeApi::SUBSCRIBE_STREAM, "SUBSCRIBE_STREAM"},
  {BlockNodeApi::STATE_PROOF,      "STATE_PROOF"     },
};

} // namespace Hiero
