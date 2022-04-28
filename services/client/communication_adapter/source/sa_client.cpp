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

#include "communication_adapter/include/sa_client.h"

#include "ipc_skeleton.h"

#include "communication_adapter/include/sa_client_proxy.h"
#include "platform/os_wrapper/ipc/include/aie_ipc.h"
#include "protocol/ipc_interface/ai_service.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/aie_macros.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
namespace {
int32_t AsyncCallback(uint32_t code, IpcIo *data, IpcIo *reply, MessageOption option)
{
    int asyncCallbackRet;
    ReadInt32(data, &asyncCallbackRet);
    int requestId;
    ReadInt32(data, &requestId);
    int sessionId;
    ReadInt32(data, &sessionId);
    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0,
    };
    int ipcUnParcelRet = UnParcelDataInfo(data, &outputInfo);
    SaClient *client = SaClient::GetInstance();
    if (client == nullptr) {
        HILOGE("[SaClient]The client is nullptr, maybe out of memory.");
        FreeDataInfo(&outputInfo);
        return RETCODE_FAILURE;
    }
    CallbackHandle callback = client->GetSaClientResultCb();
    if (callback == nullptr) {
        HILOGE("[SaClient]SA client callback is nullptr, maybe Release interface is called or the callback is deleted");
        FreeDataInfo(&outputInfo);
        return RETCODE_FAILURE;
    }
    // The asynchronous callback retCode is used only when the IPC is normal.
    int retCode = asyncCallbackRet;
    if (ipcUnParcelRet != RETCODE_SUCCESS) {
        HILOGE("[SaClient]AsyncCallback failed, UnParcelDataInfo retCode[%d].", ipcUnParcelRet);
        // The IPC is abnormal.
        retCode = RETCODE_FAILURE;
    }
    callback(sessionId, outputInfo, retCode, requestId);
    FreeDataInfo(&outputInfo);
    return retCode;
}

void OnAiDead(void *arg)
{
    SaClient *client = SaClient::GetInstance();
    if (client == nullptr) {
        HILOGE("[SaClient]callback is null.");
        return;
    }
    DeathCallbackHandle onDead = client->GetSaDeathResultCb();
    if (onDead == nullptr) {
        HILOGE("[SaClient]Dead callback is null.");
        return;
    }
    int clientId = *(reinterpret_cast<int *>(arg));
    HILOGW("[SaClient]OnAiDead for [clientId:%d].", clientId);
    onDead();
}
} // anonymous namespaces

std::mutex SaClient::instance_mutex_;
SaClient *SaClient::instance_ = nullptr;

SaClient::SaClient() = default;

SaClient::~SaClient() = default;

SaClient *SaClient::GetInstance()
{
    CHK_RET(instance_ != nullptr, instance_);

    std::lock_guard<std::mutex> lock(instance_mutex_);
    CHK_RET(instance_ != nullptr, instance_);

    SaClient *tempInstance = nullptr;
    AIE_NEW(tempInstance, SaClient);
    CHK_RET(tempInstance == nullptr, nullptr);

    instance_ = tempInstance;
    return instance_;
}

void SaClient::ReleaseInstance()
{
    std::lock_guard<std::mutex> lock(instance_mutex_);
    AIE_DELETE(instance_);
}

int SaClient::Init(const ConfigInfo &configInfo, ClientInfo &clientInfo)
{
    HosInit();
    proxy_ = GetRemoteIUnknown();
    if (proxy_ == nullptr) {
        HILOGE("[SaClient]Fail to get server proxy, server exception.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }

    int retCode = InitSaEngine(*proxy_, configInfo, clientInfo);
    if (retCode != RETCODE_SUCCESS || clientInfo.clientId == INVALID_CLIENT_ID) {
        HILOGE("[SaClient]InitServer result failed, clientId: %d, errorCode: [%d]", clientInfo.clientId, retCode);
        ReleaseIUnknown(*((IUnknown *)proxy_));
        proxy_ = nullptr;
        return RETCODE_FAILURE;
    }

    // Register SA Death Callback
    svc_ = SAMGR_GetRemoteIdentity(AI_SERVICE, nullptr);
    int32_t resultCode = AddDeathRecipient(svc_, OnAiDead, &clientInfo.clientId, &deadId_);
    if (resultCode != 0) {
        HILOGE("[SaClient]Register SA Death Callback failed, errorCode[%d]", resultCode);
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int SaClient::LoadAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    if (proxy_ == nullptr) {
        HILOGE("[SaClient]Service is nullptr, need reconnect server");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }
    return LoadAlgorithmProxy(*proxy_, clientInfo, algorithmInfo, inputInfo, outputInfo);
}

int SaClient::SyncExecuteAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    if (proxy_ == nullptr) {
        HILOGE("[SaClient]Service is nullptr, need reconnect server.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }
    return SyncExecAlgorithmProxy(*proxy_, clientInfo, algorithmInfo, inputInfo, outputInfo);
}

int SaClient::AsyncExecuteAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo)
{
    if (proxy_ == nullptr) {
        HILOGE("[SaClient]Fail to get server proxy, retry to prepare.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }

    return AsyncExecuteAlgorithmProxy(*proxy_, clientInfo, algorithmInfo, inputInfo);
}

int SaClient::UnloadAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo)
{
    if (proxy_ == nullptr) {
        HILOGE("[SaClient]Service is nullptr, need reconnect server.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }

    return UnloadAlgorithmProxy(*proxy_, clientInfo, algorithmInfo, inputInfo);
}

int SaClient::Destroy(const ClientInfo &clientInfo)
{
    if (proxy_ == nullptr) {
        HILOGE("[SaClient]The proxy_ is nullptr. No need to destroy.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }

    int retCode = DestroyEngineProxy(*proxy_, clientInfo);
    (void)RemoveDeathRecipient(svc_, deadId_);
    ReleaseIUnknown(*((IUnknown *)proxy_));
    proxy_ = nullptr;
    return retCode;
}

int SaClient::SetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo)
{
    if (proxy_ == nullptr) {
        HILOGE("[SaClient]Service is nullptr. need reconnect server.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }
    return SetOptionProxy(*proxy_, clientInfo, optionType, inputInfo);
}

int SaClient::GetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo,
    DataInfo &outputInfo)
{
    if (proxy_ == nullptr) {
        HILOGE("[SaClient]Service is nullptr. need reconnect server.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }
    return GetOptionProxy(*proxy_, clientInfo, optionType, inputInfo, outputInfo);
}

int SaClient::RegisterCallback(const ClientInfo &clientInfo)
{
    if (proxy_ == nullptr) {
        HILOGE("[SaClient]The proxy_ is nullptr.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }

    return RegisterCallbackProxy(*proxy_, clientInfo, AsyncCallback);
}

int SaClient::UnregisterCallback(const ClientInfo &clientInfo)
{
    if (proxy_ == nullptr) {
        HILOGE("[SaClient]Service is nullptr, need reconnect server.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }

    return UnregisterCallbackProxy(*proxy_, clientInfo);
}

void SaClient::RegisterSaClientCb(CallbackHandle resultCb)
{
    ResultCb_ = resultCb;
}

void SaClient::UnRegisterSaClientCb()
{
    ResultCb_ = nullptr;
}

CallbackHandle SaClient::GetSaClientResultCb()
{
    return ResultCb_;
}

void SaClient::RegisterSaDeathCb(DeathCallbackHandle deathCb)
{
    deathCb_ = deathCb;
}

void SaClient::UnRegisterSaDeathCb()
{
    deathCb_ = nullptr;
}

DeathCallbackHandle SaClient::GetSaDeathResultCb()
{
    return deathCb_;
}
} // namespace AI
} // namespace OHOS
