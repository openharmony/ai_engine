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

#include "platform/time/include/time_elapser.h"

namespace OHOS {
namespace AI {
TimeElapser::TimeElapser()
{
    Reset();
}

TimeElapser::~TimeElapser() = default;

void TimeElapser::Reset()
{
    startTime_ = std::chrono::system_clock::now();
}

int64_t TimeElapser::GetStartTime()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(startTime_.time_since_epoch()).count();
}

int64_t TimeElapser::ElapseMicro()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::microseconds>(now - startTime_).count();
}

int64_t TimeElapser::ElapseMilli()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_).count();
}

int64_t TimeElapser::ElapseSec()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::seconds>(now - startTime_).count();
}
} // namespace AI
} // namespace OHOS