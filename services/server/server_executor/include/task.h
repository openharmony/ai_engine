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

#ifndef TASK_H
#define TASK_H

#include "platform/semaphore/include/simple_event_notifier.h"
#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class IHandler;
struct Task {
    IHandler *handler;
    IRequest *request;
    SimpleEventNotifier<IResponse> *notifier;

    /**
     * Empty constructor.
     */
    Task() : handler(nullptr), request(nullptr), notifier(nullptr) {}

    /**
     * Constructor with parameters.
     *
     * @param handler Handle info.
     * @param request Request info.
     * @param notifier Notifier info.
     */
    Task(IHandler *handler, IRequest *request, SimpleEventNotifier<IResponse> *notifier)
        : handler(handler), request(request), notifier(notifier)
    {
    }
};
} // namespace AI
} // namespace OHOS

#endif // TASK_H