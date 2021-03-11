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

#ifndef SIMPLE_EVENT_NOTIFIER_H
#define SIMPLE_EVENT_NOTIFIER_H

#include "platform/semaphore/include/i_semaphore.h"

namespace OHOS {
namespace AI {
template<class T>
class SimpleEventNotifier {
public:
    using FPDestruct = void(*) (T *&t);
    explicit SimpleEventNotifier(FPDestruct destruct = nullptr);
    ~SimpleEventNotifier();

    /**
     * Return the execution result to the waiting party
     *
     * @param [in] item The task which wait for notification
     */
    void AddToBack(T *item);

    /**
     * Wait for notice within the time period.
     *
     * @param [in] timeOut The range of timeout
     * @param [out] item The result which is calculated from task
     * @return Returns true if the operation is successful, returns false otherwise.
     */
    bool GetFromFront(int timeOut, T *&item);

private:
    T *value_;
    FPDestruct destruct_;
    std::shared_ptr<ISemaphore> producer_;
};
} // namespace AI
} // namespace OHOS

#include "platform/semaphore/include/simple_event_notifier.inl"

#endif // SIMPLE_EVENT_NOTIFIER_H