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

#include "communication_adapter/include/future_listener.h"

#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
FutureListener::FutureListener(SaAsyncHandler *handler, int clientId)
    : handler_(handler), clientId_(clientId)
{
}

FutureListener::~FutureListener()
{
    handler_ = nullptr;
}

void FutureListener::OnReply(const IFuture *future)
{
    HILOGI("[FutureListener]Begin OnReply.");

    if (handler_ == nullptr) {
        HILOGE("[FutureListener]The handler_ is nullptr.");
        return;
    }

    if (future == nullptr) {
        HILOGE("[FutureListener]The future is nullptr.");
        return;
    }

    IResponse *response = future->GetResponse(0);
    if (response == nullptr) {
        HILOGE("[FutureListener][clientId:%d]Get response failed.", clientId_);
        return;
    }

    handler_->PushAsyncResponse(clientId_, response);
}
} // namespace AI
} // namespace OHOS