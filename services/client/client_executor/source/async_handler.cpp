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

#include "client_executor/include/async_handler.h"

#include "client_executor/include/i_client_cb.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
AsyncHandler::AsyncHandler() = default;

AsyncHandler::~AsyncHandler() = default;

int AsyncHandler::RegisterCb(int sessionId, IClientCb *cb)
{
    if (cb != nullptr) {
        mapCbMsg_.insert(std::make_pair(sessionId, cb));
    }
    return RETCODE_SUCCESS;
}

std::mutex AsyncHandler::cbMutex_;

int AsyncHandler::UnRegisterCb(int sessionId)
{
    std::lock_guard<std::mutex> guard(cbMutex_);
    if (mapCbMsg_.erase(sessionId) == 0) {
        HILOGE("[AsyncHandler][sessionId:%d]session does not exist", sessionId);
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int AsyncHandler::RegisterDeadCb(int sessionId, IServiceDeadCb *cb)
{
    if (cb != nullptr) {
        deadCbs_.insert(std::make_pair(sessionId, cb));
    }
    return RETCODE_SUCCESS;
}

int AsyncHandler::UnRegisterDeadCb(int sessionId)
{
    std::lock_guard<std::mutex> guard(deadCbMutex_);
    deadCbs_.erase(sessionId); // nullptr deadCb won't register in RegisterDeadCb, ignore return value
    return RETCODE_SUCCESS;
}

void AsyncHandler::OnResult(int sessionId, const DataInfo &result, int resultCode, int requestId)
{
    IClientCb *cb = nullptr;
    FindCb(sessionId, cb);
    if (cb == nullptr) {
        HILOGE("[AsyncHandler]Can't find callback, throw away the msg of session id[%d]", sessionId);
        return;
    }
    cb->OnResult(result, resultCode, requestId);
}

void AsyncHandler::OnDead()
{
    for (auto &iter : deadCbs_) {
        IServiceDeadCb *deadCb = iter.second;
        if (deadCb != nullptr) {
            deadCb->OnServiceDead();
        }
    }
    HILOGW("[AsyncHandler]On Service Dead");
}

void AsyncHandler::FindCb(int sessionId, IClientCb *&cb)
{
    std::lock_guard<std::mutex> guard(cbMutex_);
    ClientCbs::iterator iter = mapCbMsg_.find(sessionId);
    if (iter == mapCbMsg_.end()) {
        HILOGW("[AsyncHandler][sessionId:%d]Can't find callback.", sessionId);
        return;
    }
    cb = iter->second;
}

bool AsyncHandler::IsCallbackEmpty()
{
    return mapCbMsg_.empty();
}

int AsyncHandler::GetAsyncCbSize()
{
    return mapCbMsg_.size();
}
} // namespace AI
} // namespace OHOS