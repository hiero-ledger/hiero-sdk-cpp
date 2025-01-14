// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_MIRROR_NODE_CONTRACT_ESTIMATE_GAS_QUERY_H_
#define HIERO_SDK_CPP_MIRROR_NODE_CONTRACT_ESTIMATE_GAS_QUERY_H_

#include "impl/MirrorNodeContractQuery.h"

namespace Hiero
{
class MirrorNodeContractEstimateGasQuery : public MirrorNodeContractQuery
{
public:
  /**
   * Executes the Mirror Node query.
   *
   * @param client The Client object used for network access.
   * @return The result of the execution in string format.
   */
  [[nodiscard]] std::string execute(const Client& client) const override;
};
} // namespace Hiero

#endif // HIERO_SDK_CPP_MIRROR_NODE_CONTRACT_ESTIMATE_GAS_QUERY_H_