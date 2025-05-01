// SPDX-License-Identifier: Apache-2.0
#include <Client.h>

#include <dotenv.h>

namespace Hiero::SnippetsHelper
{
/**
 * Grab operator account ID and private key from the environment and initialize a Client with them.
 *
 * @return A Client initialized to communicate with the Hedera testnet with the environment operator parameters.
 */
Client initializeClient();

} // namespace Hiero::SnippetsHelper
