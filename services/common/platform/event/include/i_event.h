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

#ifndef I_EVENT_H
#define I_EVENT_H

#include <memory>

#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class IEvent {
    FORBID_COPY_AND_ASSIGN(IEvent);
    FORBID_CREATE_BY_SELF(IEvent);
public:
    /**
     * Plugins prohibit the use of new, so add a create method here.
     */
    static std::shared_ptr<IEvent> MakeShared();

    /**
     * Wait for the event to wake up within the specified time.
     *
     * @param [in] milliSeconds Waiting time (ms).
     * @return Whether it is awakened successfully or not within the specified time.
     */
    bool Wait(const int milliSeconds = -1) const;

    /**
     * Wake up the thread waiting for the event.
     *
     * @return Whether the event is in reset state before wake-up. true: reset state, false: set state.
     */
    bool Signal();

    /**
     * Reset event flag to false.
     *
     * @return Event setting status before reset.
     */
    bool Reset();

    /**
     * Determine whether the event is set.
     */
    bool IsSet() const;
};
} // namespace AI
} // namespace OHOS

#endif // I_EVENT_H