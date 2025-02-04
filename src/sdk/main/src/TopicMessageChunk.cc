// SPDX-License-Identifier: Apache-2.0
#include "TopicMessageChunk.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <mirror/consensus_service.pb.h>

namespace Hiero
{
//-----
TopicMessageChunk::TopicMessageChunk(const com::hedera::mirror::api::proto::ConsensusTopicResponse& proto)
  : mConsensusTimestamp(internal::TimestampConverter::fromProtobuf(proto.consensustimestamp()))
  , mContentSize(proto.message().size())
  , mRunningHash(internal::Utilities::stringToByteVector(proto.runninghash()))
  , mSequenceNumber(proto.sequencenumber())
{
}

} // namespace Hiero
