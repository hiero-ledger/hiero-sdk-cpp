// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_DEFAULTS_H_
#define HIERO_SDK_CPP_DEFAULTS_H_

#include "Hbar.h"

#include <chrono>

namespace Hiero
{
/**
 * The default duration of time to try and submit a request.
 */
constexpr auto DEFAULT_REQUEST_TIMEOUT = std::chrono::minutes(2);
/**
 * The default maximum number of times a request will attempt to be submitted before considering the execution failed.
 */
constexpr auto DEFAULT_MAX_ATTEMPTS = 10U;
/**
 * The default minimum duration of time to wait before retrying to submit a previously-failed request.
 */
constexpr auto DEFAULT_MIN_BACKOFF = std::chrono::milliseconds(250);
/**
 * The default maximum duration of time to wait before retrying to submit a previously-failed request.
 */
constexpr auto DEFAULT_MAX_BACKOFF = std::chrono::seconds(8);
/**
 * The default maximum amount of time to spend on one execution attempt
 */
constexpr auto DEFAULT_GRPC_DEADLINE = std::chrono::seconds(10);
/**
 * The default maximum number of times a node is allowed to return a bad gRPC status before it is permanently removed
 * from a network. 0 indicates there's no maximum.
 */
constexpr auto DEFAULT_MAX_NODE_ATTEMPTS = 0U;
/**
 * The default minimum duration of time to wait before retrying to submit a previously-failed request to the same node.
 */
constexpr auto DEFAULT_MIN_NODE_BACKOFF = std::chrono::seconds(8);
/**
 * The default maximum duration of time to wait before retrying to submit a previously-failed request to the same node.
 */
constexpr auto DEFAULT_MAX_NODE_BACKOFF = std::chrono::hours(1);
/**
 * The default amount of time to allow a node to gracefully close a gRPC connection before forcibly terminating it.
 */
constexpr auto DEFAULT_CLOSE_TIMEOUT = std::chrono::seconds(30);
/**
 * The default maximum transaction fee.
 */
constexpr auto DEFAULT_MAX_TRANSACTION_FEE = Hbar(2LL);
/**
 * The default maximum query payment
 */
constexpr auto DEFAULT_MAX_QUERY_PAYMENT = Hbar(1LL);
/**
 * The default auto-renew period.
 */
constexpr auto DEFAULT_AUTO_RENEW_PERIOD = std::chrono::hours(2160);
/**
 * The default duration of time a Transaction will remain valid.
 */
constexpr auto DEFAULT_TRANSACTION_VALID_DURATION = std::chrono::minutes(2);
/**
 * The default transaction ID regeneration policy.
 */
constexpr auto DEFAULT_REGENERATE_TRANSACTION_ID = true;
/**
 * The default size of a chunk for a ChunkedTransaction.
 */
constexpr auto DEFAULT_CHUNK_SIZE = 1024U;
/**
 * The default number of chunks for a ChunkedTransaction.
 */
constexpr auto DEFAULT_MAX_CHUNKS = 20U;
/**
 * The default amount of time to wait after a network update to update again.
 */
constexpr auto DEFAULT_NETWORK_UPDATE_PERIOD = std::chrono::hours(24);
/**
 * The default amount of time to wait after Client creation to update the network for the first time.
 */
constexpr auto DEFAULT_NETWORK_UPDATE_INITIAL_DELAY = std::chrono::seconds(10);
/**
 * The default name of Logger types.
 */
constexpr auto DEFAULT_LOGGER_NAME = "HIERO_CPP_SDK_LOGGER";
}

#endif // HIERO_SDK_CPP_DEFAULTS_H_