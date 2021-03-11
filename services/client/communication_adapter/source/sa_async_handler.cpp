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

#include "communication_adapter/include/sa_async_handler.h"
#include "communication_adapter/include/sa_client.h"
#include "communication_adapter/include/sa_client_adapter.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
void SaClientCallback(int sessionId, const DataInfo &result, int resultCode, int requestId)
{
    AsyncHandler *asyncHandler = SaAsyncHandler::GetInstance();
    if (asyncHandler == nullptr) {
        HILOGE("[SaAsyncHandler][callback]AsyncHandler is nullptr. session id[%d]", sessionId);
        return;
    }
    HILOGD("[SaAsyncHandler][callback]End to get instance of asyncHandler, session id[%d]", sessionId);
    asyncHandler->OnResult(sessionId, result, resultCode, requestId);
}

void SaDeathCallback()
{
    SaClientAdapter *clientAdapter = SaClientAdapter::GetInstance();
    if (clientAdapter == nullptr) {
        HILOGE("[SaAsyncHandler][DeathCallback]The clientAdapter is nullptr");
        return;
    }
    clientAdapter->ResetClient();
    AsyncHandler *asyncHandler = SaAsyncHandler::GetInstance();
    if (asyncHandler == nullptr) {
        HILOGE("[SaAsyncHandler][DeathCallback]AsyncHandler is nullptr");
        return;
    }
    HILOGD("[SaAsyncHandler][DeathCallback]Push death callback to client executor");
    asyncHandler->OnDead();
}

std::mutex SaAsyncHandler::instance_mutex_;
SaAsyncHandler *SaAsyncHandler::instance_ = nullptr;

SaAsyncHandler::SaAsyncHandler()
{
}

SaAsyncHandler::~SaAsyncHandler()
{
}

SaAsyncHandler *SaAsyncHandler::GetInstance()
{
    CHK_RET(instance_ != nullptr, instance_);
    std::lock_guard<std::mutex> lock(instance_mutex_);
    CHK_RET(instance_ != nullptr, instance_);
    AIE_NEW(instance_, SaAsyncHandler);
    return instance_;
}

void SaAsyncHandler::ReleaseInstance()
{
    std::lock_guard<std::mutex> lock(instance_mutex_);
    AIE_DELETE(instance_);
}

int SaAsyncHandler::RegisterAsyncClientCb()
{
    SaClient *saClient = SaClient::GetInstance();
    CHK_RET(saClient == nullptr, RETCODE_NULL_PARAM);
    saClient->RegisterSaClientCb(SaClientCallback);
    return RETCODE_SUCCESS;
}

int SaAsyncHandler::UnRegisterAsyncClientCb()
{
    SaClient *saClient = SaClient::GetInstance();
    CHK_RET(saClient == nullptr, RETCODE_NULL_PARAM);
    saClient->UnRegisterSaClientCb();
    return RETCODE_SUCCESS;
}

int SaAsyncHandler::RegisterServiceDeathCb()
{
    SaClient *saClient = SaClient::GetInstance();
    CHK_RET(saClient == nullptr, RETCODE_NULL_PARAM);
    saClient->RegisterSaDeathCb(SaDeathCallback);
    return RETCODE_SUCCESS;
}

int SaAsyncHandler::UnRegisterServiceDeathCb()
{
    SaClient *saClient = SaClient::GetInstance();
    CHK_RET(saClient == nullptr, RETCODE_NULL_PARAM);
    saClient->UnRegisterSaDeathCb();
    return RETCODE_SUCCESS;
}
} // namespace AI
} // namespace OHOS