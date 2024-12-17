// SPDX-License-Identifier: Apache-2.0
#include "TopicMessage.h"
#include "impl/TimestampConverter.h"
#include "impl/Utilities.h"

#include <mirror/consensus_service.pb.h>

#include <utility>

namespace Hiero
{
//-----
TopicMessage::TopicMessage(const std::chrono::system_clock::time_point& lastConsensusTimestamp,
                           const std::vector<std::byte>& message,
                           const std::vector<std::byte>& lastRunningHash,
                           uint64_t lastSequenceNumber,
                           const std::vector<TopicMessageChunk>& chunks,
                           TransactionId transactionId)
  : mConsensusTimestamp(lastConsensusTimestamp)
  , mContents(message)
  , mRunningHash(lastRunningHash)
  , mSequenceNumber(lastSequenceNumber)
  , mChunks(chunks)
  , mTransactionId(std::move(transactionId))
{
}

//-----
TopicMessage TopicMessage::ofSingle(const com::hedera::mirror::api::proto::ConsensusTopicResponse& proto)
{
  return TopicMessage(internal::TimestampConverter::fromProtobuf(proto.consensustimestamp()),
                      internal::Utilities::stringToByteVector(proto.message()),
                      internal::Utilities::stringToByteVector(proto.runninghash()),
                      proto.sequencenumber(),
                      { TopicMessageChunk(proto) },
                      proto.has_chunkinfo() ? TransactionId::fromProtobuf(proto.chunkinfo().initialtransactionid())
                                            : TransactionId());
}

//-----
TopicMessage TopicMessage::ofMany(const std::vector<com::hedera::mirror::api::proto::ConsensusTopicResponse>& protos)
{
  // Place the responses in order from oldest to newest.
  std::vector<TopicMessageChunk> chunks(protos.size(), TopicMessageChunk());
  std::vector<std::vector<std::byte>> contents(protos.size(), std::vector<std::byte>());
  TransactionId transactionId;

  for (const auto& proto : protos)
  {
    if (proto.has_chunkinfo() && proto.chunkinfo().has_initialtransactionid())
    {
      transactionId = TransactionId::fromProtobuf(proto.chunkinfo().initialtransactionid());
    }

    const int32_t index = proto.chunkinfo().number() - 1;
    chunks[index] = TopicMessageChunk(proto);
    contents[index] = internal::Utilities::stringToByteVector(proto.message());
  }

  return { chunks.back().mConsensusTimestamp,
           internal::Utilities::concatenateVectors(contents),
           chunks.back().mRunningHash,
           chunks.back().mSequenceNumber,
           chunks,
           transactionId };
}

} // namespace Hiero
