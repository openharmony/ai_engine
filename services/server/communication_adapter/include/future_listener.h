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

#ifndef FUTURE_LISTENER_H
#define FUTURE_LISTENER_H

#include "server_executor/include/i_future.h"

#include "communication_adapter/include/sa_async_handler.h"
#include "server_executor/include/i_future_listener.h"

namespace OHOS {
namespace AI {
class FutureListener : public IFutureListener {
public:
    FutureListener(SaAsyncHandler *handler, int clientId);

    ~FutureListener() override;

    /**
     * Reply callback. When receive reply, save the response in client listener handler.
     *
     * @param future The async result of the client listener handler thread.
     */
    void OnReply(const IFuture *future) override;

private:
    SaAsyncHandler *handler_;

    int clientId_;
};
} // namespace AI
} // namespace OHOS

#endif // FUTURE_LISTENER_H