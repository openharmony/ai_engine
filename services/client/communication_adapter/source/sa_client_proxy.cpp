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

#include "communication_adapter/include/sa_client_proxy.h"

#include <pthread.h>
#include <string>

#include "iproxy_client.h"
#include "liteipc.h"
#include "samgr_lite.h"

#include "protocol/ipc_interface/ai_service.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
SvcIdentity g_sid;

struct Notify {
    int retCode;
};

struct NotifyBuff {
    int ipcRetCode;
    int retCode;
    int outLen;
    unsigned char *outBuff;
};

extern "C" void __attribute__((weak)) HOS_SystemInit(void)
{
};

void HosInit()
{
    HOS_SystemInit();
}

IClientProxy *GetRemoteIUnknown(void)
{
    IUnknown *iUnknown = SAMGR_GetInstance()->GetDefaultFeatureApi(AI_SERVICE);
    if (iUnknown == nullptr) {
        HILOGE("[SaClientProxy][TID:0x%lx][GetDefaultFeatureApi S:%s]: error is null.",
            pthread_self(), AI_SERVICE);
        return nullptr;
    }
    IClientProxy *proxy = nullptr;
    (void)iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, (void **)&proxy);
    return proxy;
}

static int Callback(void *owner, int code, IpcIo *reply)
{
    HILOGI("[SaClientProxy]Callback start.");
    if (owner == nullptr) {
        HILOGE("[SaClientProxy]Callback owner is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    auto notify = reinterpret_cast<struct Notify *>(owner);
    notify->retCode = IpcIoPopInt32(reply);
    return RETCODE_SUCCESS;
}

static int CallbackBuff(void *owner, int code, IpcIo *reply)
{
    HILOGI("[SaClientProxy]CallbackBuff start.");
    if (owner == nullptr) {
        HILOGE("[SaClientProxy]SyncCallback owner is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    auto notify = reinterpret_cast<struct NotifyBuff *>(owner);
    notify->ipcRetCode = RETCODE_SUCCESS;
    notify->retCode = IpcIoPopInt32(reply);
    notify->outLen = IpcIoPopInt32(reply);
    if (notify->outLen <= 0) {
        HILOGI("[SaClientProxy]SyncCallback dataBuf is nullptr.");
        return RETCODE_SUCCESS;
    }
    BuffPtr *dataBuf = IpcIoPopDataBuff(reply);
    if (dataBuf == nullptr) {
        HILOGE("[SaClientProxy]SyncCallback dataBuf is nullptr.");
        notify->ipcRetCode = RETCODE_FAILURE;
        return RETCODE_NULL_PARAM;
    }

    notify->outBuff = reinterpret_cast<unsigned char *>(malloc(sizeof(unsigned char) * notify->outLen));
    if (notify->outBuff == nullptr) {
        HILOGE("[SaClientProxy]Failed to request memory.");
        notify->ipcRetCode = RETCODE_OUT_OF_MEMORY;
        FreeBuffer(nullptr, dataBuf->buff);
        return RETCODE_OUT_OF_MEMORY;
    }
    errno_t retCode = memcpy_s(notify->outBuff, notify->outLen, dataBuf->buff, dataBuf->buffSz);
    if (retCode != EOK) {
        HILOGE("[SaClientProxy]Failed to memory copy, retCode[%d].", retCode);
        notify->ipcRetCode = RETCODE_MEMORY_COPY_FAILURE;
        free(notify->outBuff);
    }
    FreeBuffer(nullptr, dataBuf->buff);
    return notify->ipcRetCode;
}

void ParcelConfigInfo(IpcIo *request, const ConfigInfo &configInfo)
{
    IpcIoPushString(request, configInfo.description);
}

void ParcelClientInfo(IpcIo *request, const ClientInfo &clientInfo)
{
    IpcIoPushInt64(request, clientInfo.clientVersion);
    IpcIoPushInt32(request, clientInfo.clientId);
    IpcIoPushInt32(request, clientInfo.sessionId);
    IpcIoPushInt32(request, clientInfo.extendLen);

    if ((clientInfo.extendLen > 0) && (clientInfo.extendMsg != nullptr)) {
        BuffPtr dataBuff = {
            .buffSz = static_cast<uint32_t>(clientInfo.extendLen),
            .buff = clientInfo.extendMsg,
        };
        IpcIoPushDataBuff(request, &dataBuff);
    }
}

void ParcelAlgorithmInfo(IpcIo *request, const AlgorithmInfo &algorithmInfo)
{
    IpcIoPushInt64(request, algorithmInfo.clientVersion);
    IpcIoPushBool(request, algorithmInfo.isAsync);
    IpcIoPushInt32(request, algorithmInfo.algorithmType);
    IpcIoPushInt64(request, algorithmInfo.algorithmVersion);
    IpcIoPushBool(request, algorithmInfo.isCloud);
    IpcIoPushInt32(request, algorithmInfo.operateId);
    IpcIoPushInt32(request, algorithmInfo.requestId);
    IpcIoPushInt32(request, algorithmInfo.extendLen);

    if (algorithmInfo.extendLen > 0 && algorithmInfo.extendMsg != nullptr) {
        BuffPtr dataBuff = {
            .buffSz = static_cast<uint32_t>(algorithmInfo.extendLen),
            .buff = algorithmInfo.extendMsg,
        };
        IpcIoPushDataBuff(request, &dataBuff);
    }
}

void ParcelDataInfo(IpcIo *request, const DataInfo &dataInfo)
{
    IpcIoPushInt32(request, dataInfo.length);
    if (dataInfo.length > 0 && dataInfo.data != nullptr) {
        BuffPtr dataBuff = {
            .buffSz = static_cast<uint32_t>(dataInfo.length),
            .buff = dataInfo.data,
        };
        IpcIoPushDataBuff(request, &dataBuff);
    }
}

int InitSaEngine(IClientProxy &proxy, const ConfigInfo &configInfo)
{
    HILOGI("[SaClientProxy]Begin to call InitSaEngine.");

    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, IPC_OBJECT_COUNTS);
    ParcelConfigInfo(&request, configInfo);

    struct Notify owner = {.retCode = RETCODE_FAILURE};
    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_INIT_ENGINE, &request, &owner, Callback);
    return owner.retCode;
}

int DestroyEngineProxy(IClientProxy &proxy, const ClientInfo &clientInfo)
{
    HILOGI("[SaClientProxy]Begin to call DestroyEngineProxy.");

    struct Notify owner = {.retCode = RETCODE_FAILURE};
    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, IPC_OBJECT_COUNTS);
    ParcelClientInfo(&request, clientInfo);

    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_DESTROY_ENGINE, &request, &owner, Callback);
    if (owner.retCode != RETCODE_SUCCESS) {
        HILOGE("[SaClientProxy]IPC data processing failed, error code is [%d].", owner.retCode);
    }
    return owner.retCode;
}

void ReleaseIUnknown(IUnknown &proxy)
{
    HILOGI("[SaClientProxy]Begin to call ReleaseIUnknown.");
    if (proxy.Release == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Release is nullptr.");
        return;
    }
    proxy.Release(&proxy);
}

int SyncExecAlgorithmProxy(IClientProxy &proxy, const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    HILOGI("[SaClientProxy]Begin to call SyncExecAlgorithmProxy.");

    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    ParcelAlgorithmInfo(&request, algoInfo);
    ParcelDataInfo(&request, inputInfo);

    struct NotifyBuff owner = {
        .ipcRetCode = RETCODE_SUCCESS,
        .retCode = RETCODE_FAILURE,
        .outLen = 0,
        .outBuff = nullptr,
    };
    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_SYNC_EXECUTE_ALGORITHM, &request, &owner, CallbackBuff);

    if (owner.ipcRetCode != RETCODE_SUCCESS) {
        HILOGE("[SaClientProxy]IPC data processing failed, error code is [%d].", owner.ipcRetCode);
        return owner.ipcRetCode;
    }
    outputInfo.data = owner.outBuff;
    outputInfo.length = owner.outLen;
    return owner.retCode;
}

int AsyncExecuteAlgorithmProxy(IClientProxy &proxy, const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo)
{
    HILOGI("[SaClientProxy]Begin to call AsyncExecuteAlgorithmProxy.");

    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, IPC_OBJECT_COUNTS);
    ParcelClientInfo(&request, clientInfo);
    ParcelAlgorithmInfo(&request, algoInfo);
    ParcelDataInfo(&request, inputInfo);
    struct Notify owner = {.retCode = RETCODE_FAILURE};
    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_ASYNC_EXECUTE_ALGORITHM, &request, &owner, Callback);
    return owner.retCode;
}

int LoadAlgorithmProxy(IClientProxy &proxy, const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    HILOGI("[SaClientProxy]Begin to call LoadAlgorithmProxy.");
    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    ParcelAlgorithmInfo(&request, algoInfo);
    ParcelDataInfo(&request, inputInfo);
    struct NotifyBuff owner = {
        .ipcRetCode = RETCODE_SUCCESS,
        .retCode = RETCODE_FAILURE,
        .outLen = 0,
        .outBuff = nullptr,
    };
    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_LOAD_ALGORITHM, &request, &owner, CallbackBuff);
    if (owner.ipcRetCode != RETCODE_SUCCESS) {
        HILOGE("[SaClientProxy]IPC data processing failed, error code is [%d].", owner.ipcRetCode);
        return owner.ipcRetCode;
    }
    outputInfo.data = owner.outBuff;
    outputInfo.length = owner.outLen;
    return owner.retCode;
}

int UnloadAlgorithmProxy(IClientProxy &proxy, const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo)
{
    HILOGI("[SaClientProxy]Begin to call UnloadAlgorithmProxy.");

    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    ParcelAlgorithmInfo(&request, algoInfo);
    ParcelDataInfo(&request, inputInfo);

    struct Notify owner = {.retCode = RETCODE_FAILURE};
    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_UNLOAD_ALGORITHM, &request, &owner, Callback);
    return owner.retCode;
}

int SetOptionProxy(IClientProxy &proxy, const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo)
{
    HILOGI("[SaClientProxy]Begin to call SetOptionProxy.");

    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    IpcIoPushInt32(&request, optionType);
    ParcelDataInfo(&request, inputInfo);

    struct Notify owner = {.retCode = RETCODE_FAILURE};
    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_SET_OPTION, &request, &owner, Callback);
    return owner.retCode;
}

int GetOptionProxy(IClientProxy &proxy, const ClientInfo &clientInfo, int optionType,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    HILOGI("[SaClientProxy]Begin to call GetOptionProxy.");

    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    IpcIoPushInt32(&request, optionType);
    ParcelDataInfo(&request, inputInfo);

    struct NotifyBuff owner = {
        .ipcRetCode = RETCODE_SUCCESS,
        .retCode = RETCODE_FAILURE,
        .outLen = 0,
        .outBuff = nullptr,
    };
    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_GET_OPTION, &request, &owner, CallbackBuff);

    if (owner.ipcRetCode != RETCODE_SUCCESS) {
        HILOGE("[SaClientProxy]IPC data processing failed, error code is [%d].", owner.ipcRetCode);
        return owner.ipcRetCode;
    }
    outputInfo.data = owner.outBuff;
    outputInfo.length = owner.outLen;
    return owner.retCode;
}

int RegisterCallbackProxy(IClientProxy &proxy, const ClientInfo &clientInfo, IpcMsgHandler asyncCallback)
{
    HILOGI("[SaClientProxy]Begin to call RegisterCallbackProxy.");

    int ret = RegisterIpcCallback(asyncCallback, ONCE, IPC_WAIT_FOREVER, &g_sid, nullptr);
    if (ret != 0) {
        HILOGE("[SaClientProxy]Function RegisterIpcCallback failed.");
        return RETCODE_FAILURE;
    }

    struct Notify owner = {.retCode = RETCODE_FAILURE};
    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, IPC_OBJECT_COUNTS);
    IpcIoPushSvc(&request, &g_sid);
    ParcelClientInfo(&request, clientInfo);
    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_REGISTER_CALLBACK, &request, &owner, Callback);
    return owner.retCode;
}

int UnregisterCallbackProxy(IClientProxy &proxy, const ClientInfo &clientInfo)
{
    HILOGI("[SaClientProxy]Begin to call UnregisterSaCallbackProxy.");

    IpcIo request;
    char data[IPC_IO_DATA_MAX];
    IpcIoInit(&request, data, IPC_IO_DATA_MAX, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    struct Notify owner = {.retCode = RETCODE_FAILURE};
    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_UNREGISTER_CALLBACK, &request, &owner, Callback);
    UnregisterIpcCallback(g_sid);
    return owner.retCode;
}
} // namespace AI
} // namespace OHOS
