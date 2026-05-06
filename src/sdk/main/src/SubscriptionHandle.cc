// SPDX-License-Identifier: Apache-2.0
#include "SubscriptionHandle.h"

namespace Hiero
{
//-----
SubscriptionHandle::~SubscriptionHandle()
{
  unsubscribe();
}

//-----
void SubscriptionHandle::unsubscribe() const
{
  if (mUnsubscribeFunc)
  {
    mUnsubscribeFunc();
  }
}

//-----
void SubscriptionHandle::setOnUnsubscribe(const std::function<void()>& onUnsubscribe)
{
  mUnsubscribeFunc = onUnsubscribe;
}

} // namespace Hiero
