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

#include "iproxy_client.h"
#include "samgr_lite.h"

#include "platform/os_wrapper/ipc/include/aie_ipc.h"
#include "protocol/ipc_interface/ai_service.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
namespace {
SvcIdentity g_sid;
IpcObjectStub g_objStub;

struct Notify {
    int retCode;
};

typedef struct {
    int clientId;
    uid_t serverUid;
} NotificationInitServer;

struct NotifyBuff {
    int ipcRetCode;
    int retCode;
    int outLen;
    unsigned char *outBuff;
};

int Callback(void *owner, int code, IpcIo *reply)
{
    HILOGI("[SaClientProxy]Callback start.");
    if (owner == nullptr) {
        HILOGE("[SaClientProxy]Callback owner is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    auto notify = reinterpret_cast<NotificationInitServer *>(owner);
    ReadInt32(reply, &(notify->clientId));
    ReadUint32(reply, &(notify->serverUid));
    return RETCODE_SUCCESS;
}

int CallbackBuff(void *owner, int code, IpcIo *reply)
{
    HILOGI("[SaClientProxy]CallbackBuff start.");
    if (owner == nullptr) {
        HILOGE("[SaClientProxy]SyncCallback owner is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    auto notify = reinterpret_cast<struct NotifyBuff *>(owner);
    ReadInt32(reply, &(notify->retCode));

    DataInfo dataInfo {};
    notify->ipcRetCode = UnParcelDataInfo(reply, &dataInfo);
    if (notify->ipcRetCode == RETCODE_SUCCESS) {
        notify->outLen = dataInfo.length;
        notify->outBuff = dataInfo.data;
    } else {
        notify->outLen = 0;
        notify->outBuff = nullptr;
    }

    return notify->ipcRetCode;
}

void ParcelClientInfo(IpcIo *request, const ClientInfo &clientInfo)
{
    WriteInt64(request, clientInfo.clientVersion);
    WriteInt32(request, clientInfo.clientId);
    WriteInt32(request, clientInfo.sessionId);
    WriteUint32(request, clientInfo.serverUid);
    WriteUint32(request, clientInfo.clientUid);

    DataInfo dataInfo {clientInfo.extendMsg, clientInfo.extendLen};
    ParcelDataInfo(request, &dataInfo, clientInfo.serverUid);
}

void ParcelAlgorithmInfo(IpcIo *request, const AlgorithmInfo &algorithmInfo, const uid_t serverUid)
{
    WriteInt64(request, algorithmInfo.clientVersion);
    WriteBool(request, algorithmInfo.isAsync);
    WriteInt32(request, algorithmInfo.algorithmType);
    WriteInt64(request, algorithmInfo.algorithmVersion);
    WriteBool(request, algorithmInfo.isCloud);
    WriteInt32(request, algorithmInfo.operateId);
    WriteInt32(request, algorithmInfo.requestId);

    DataInfo dataInfo {algorithmInfo.extendMsg, algorithmInfo.extendLen};
    ParcelDataInfo(request, &dataInfo, serverUid);
}
} // anonymous namespace

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

int InitSaEngine(IClientProxy &proxy, const ConfigInfo &configInfo, ClientInfo &clientInfo)
{
    HILOGI("[SaClientProxy]Begin to call InitSaEngine.");

    IpcIo request;
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_OBJECT_COUNTS);
    WriteString(&request, configInfo.description);

    NotificationInitServer owner = {
        .clientId = INVALID_CLIENT_ID,
        .serverUid = 0
    };
    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_INIT_ENGINE, &request, &owner, Callback);
    clientInfo.clientId = owner.clientId;
    clientInfo.serverUid = owner.serverUid;
    return RETCODE_SUCCESS;
}

int DestroyEngineProxy(IClientProxy &proxy, const ClientInfo &clientInfo)
{
    HILOGI("[SaClientProxy]Begin to call DestroyEngineProxy.");

    struct Notify owner = {.retCode = RETCODE_FAILURE};
    IpcIo request;
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_OBJECT_COUNTS);
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
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    ParcelAlgorithmInfo(&request, algoInfo, clientInfo.serverUid);
    ParcelDataInfo(&request, &inputInfo, clientInfo.serverUid);

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
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_OBJECT_COUNTS);
    ParcelClientInfo(&request, clientInfo);
    ParcelAlgorithmInfo(&request, algoInfo, clientInfo.serverUid);
    ParcelDataInfo(&request, &inputInfo, clientInfo.serverUid);

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
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    ParcelAlgorithmInfo(&request, algoInfo, clientInfo.serverUid);
    ParcelDataInfo(&request, &inputInfo, clientInfo.serverUid);
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
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    ParcelAlgorithmInfo(&request, algoInfo, clientInfo.serverUid);
    ParcelDataInfo(&request, &inputInfo, clientInfo.serverUid);

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
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    WriteInt32(&request, optionType);
    ParcelDataInfo(&request, &inputInfo, clientInfo.serverUid);

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
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    WriteInt32(&request, optionType);
    ParcelDataInfo(&request, &inputInfo, clientInfo.serverUid);

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

int RegisterCallbackProxy(IClientProxy &proxy, const ClientInfo &clientInfo, OnRemoteRequest asyncCallback)
{
    HILOGI("[SaClientProxy]Begin to call RegisterCallbackProxy.");

    g_objStub.func = asyncCallback;
    g_objStub.args = nullptr;
    g_objStub.isRemote = false;

    g_sid.handle = IPC_INVALID_HANDLE;
    g_sid.token = SERVICE_TYPE_ANONYMOUS;
    g_sid.cookie = (uintptr_t)&g_objStub;

    struct Notify owner = {.retCode = RETCODE_FAILURE};
    IpcIo request;
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_OBJECT_COUNTS);
    bool writeRemote = WriteRemoteObject(&request, &g_sid);
    if (!writeRemote) {
        HILOGE("WriteRemoteObject failed.");
        return RETCODE_FAILURE;
    }
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
    char data[MAX_IO_SIZE];
    IpcIoInit(&request, data, MAX_IO_SIZE, IPC_OBJECT_COUNTS);

    ParcelClientInfo(&request, clientInfo);
    struct Notify owner = {.retCode = RETCODE_FAILURE};
    if (proxy.Invoke == nullptr) {
        HILOGE("[SaClientProxy]Function pointer proxy.Invoke is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    proxy.Invoke(&proxy, ID_UNREGISTER_CALLBACK, &request, &owner, Callback);
    return owner.retCode;
}
} // namespace AI
} // namespace OHOS
