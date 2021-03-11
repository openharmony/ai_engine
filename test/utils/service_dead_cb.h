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

#ifndef SERVICE_DEAD_CB_H
#define SERVICE_DEAD_CB_H

#include "client_executor/include/i_client_cb.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
class ServiceDeadCb : public IServiceDeadCb {
public:
    ServiceDeadCb() = default;
    ~ServiceDeadCb() override = default;

    void OnServiceDead() override
    {
        HILOGI("[ServiceDeadCb]OnServiceDead Callback happens");
    }
};
} // namespace AI
} // namespace OHOS

#endif // SERVICE_DEAD_CB_H