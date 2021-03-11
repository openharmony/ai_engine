/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "platform/semaphore/include/simple_event_notifier.h"

namespace OHOS {
namespace AI {
template<class T>
SimpleEventNotifier<T>::SimpleEventNotifier(FPDestruct destruct)
    : value_(nullptr), destruct_(destruct), producer_(ISemaphore::MakeShared(0))
{
}

template<class T>
SimpleEventNotifier<T>::~SimpleEventNotifier()
{
    CHK_RET_NONE(value_ == nullptr || destruct_ == nullptr);
    destruct_(value_);
}

template<class T>
void SimpleEventNotifier<T>::AddToBack(T *item)
{
    value_ = item;
    producer_->Signal();
}

template<class T>
bool SimpleEventNotifier<T>::GetFromFront(int timeOut, T *&item)
{
    bool ret = producer_->Wait(timeOut);
    if (ret) {
        item = value_;
        value_ = nullptr;
    }
    return ret;
}
} // namespace AI
} // namespace OHOS