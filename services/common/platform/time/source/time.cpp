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

#include "platform/time/include/time.h"

namespace OHOS {
namespace AI {
void StepSleepMs(uint32_t milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

time_t GetCurTimeSec()
{
    std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
    std::chrono::seconds sec = std::chrono::duration_cast<std::chrono::seconds>(d);
    return sec.count();
}

time_t GetCurTimeMillSec()
{
    std::chrono::system_clock::duration d = std::chrono::system_clock::now().time_since_epoch();
    std::chrono::milliseconds sec = std::chrono::duration_cast<std::chrono::milliseconds>(d);
    return sec.count();
}
} // namespace AI
} // namespace OHOS