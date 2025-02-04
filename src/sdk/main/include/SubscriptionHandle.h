// SPDX-License-Identifier: Apache-2.0
#ifndef HIERO_SDK_CPP_SUBSCRIPTION_HANDLE_H_
#define HIERO_SDK_CPP_SUBSCRIPTION_HANDLE_H_

#include <functional>
#include <mutex>

namespace Hiero
{
class SubscriptionHandle
{
public:
  /**
   * Destructor unsubscribes.
   */
  ~SubscriptionHandle();

  /**
   * Unsubscribe from the subscription this SubscriptionHandle is holding.
   */
  void unsubscribe() const;

  /**
   * Set the function to run when unsubscribing.
   *
   * @param onUnsubscribe The function to run when unsubscribing.
   */
  void setOnUnsubscribe(const std::function<void(void)>& onUnsubscribe);

private:
  /**
   * The function to run when attempting to unsubscribe.
   */
  std::function<void(void)> mUnsubscribeFunc;
};

} // namespace Hiero

#endif // HIERO_SDK_CPP_SUBSCRIPTION_HANDLE_H_
