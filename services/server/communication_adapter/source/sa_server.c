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

#include <stddef.h>
#include <stdlib.h>

#include "iproxy_server.h"
#include "ohos_errno.h"
#include "ohos_init.h"
#include "samgr_lite.h"
#include "service.h"

#include "communication_adapter/include/adapter_wrapper.h"
#include "platform/os_wrapper/ipc/include/aie_ipc.h"
#include "protocol/ipc_interface/ai_service.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

static const int STACK_SIZE = 0x800;
static const int QUEUE_SIZE = 20;

typedef struct AiEngineService {
    INHERIT_SERVICE;
    INHERIT_IUNKNOWNENTRY(AiInterface);
    Identity identity;
} AiEngineService;

static const char *GetName(Service *service)
{
    return AI_SERVICE;
}

static BOOL Initialize(Service *service, Identity identity)
{
    if (service == NULL) {
        return FALSE;
    }
    AiEngineService *hiAiService = (AiEngineService *)service;
    hiAiService->identity = identity;
    return TRUE;
}

static BOOL MessageHandle(Service *service, Request *msg)
{
    return TRUE;
}

static TaskConfig GetTaskConfig(Service *service)
{
    TaskConfig config = {LEVEL_HIGH, PRI_NORMAL, STACK_SIZE, QUEUE_SIZE, SINGLE_TASK};
    return config;
}

static int UnParcelClientInfo(IpcIo *request, ClientInfo *clientInfo)
{
    if (request == NULL) {
        HILOGE("[SaServer]The request is NULL.");
        return RETCODE_FAILURE;
    }
    if (clientInfo == NULL) {
        HILOGE("[SaServer]The clientInfo is NULL.");
        return RETCODE_FAILURE;
    }
    clientInfo->clientVersion = IpcIoPopInt64(request);
    clientInfo->clientId = IpcIoPopInt32(request);
    clientInfo->sessionId = IpcIoPopInt32(request);
    clientInfo->serverUid = IpcIoPopUint32(request);
    clientInfo->clientUid = IpcIoPopUint32(request);

    DataInfo dataInfo = {NULL, 0};
    int retCode = UnParcelDataInfo(request, &dataInfo);
    if (retCode == RETCODE_SUCCESS) {
        clientInfo->extendLen = dataInfo.length;
        clientInfo->extendMsg = dataInfo.data;
    } else {
        clientInfo->extendLen = 0;
        clientInfo->extendMsg = NULL;
    }
    return retCode;
}

static int UnParcelAlgorithmInfo(IpcIo *request, AlgorithmInfo *algorithmInfo)
{
    if (request == NULL) {
        HILOGE("[SaServer]The request is NULL.");
        return RETCODE_FAILURE;
    }
    if (algorithmInfo == NULL) {
        HILOGE("[SaServer]The algorithmInfo is NULL.");
        return RETCODE_FAILURE;
    }
    algorithmInfo->clientVersion = IpcIoPopInt64(request);
    algorithmInfo->isAsync = IpcIoPopBool(request);
    algorithmInfo->algorithmType = IpcIoPopInt32(request);
    algorithmInfo->algorithmVersion = IpcIoPopInt64(request);
    algorithmInfo->isCloud = IpcIoPopBool(request);
    algorithmInfo->operateId = IpcIoPopInt32(request);
    algorithmInfo->requestId = IpcIoPopInt32(request);

    DataInfo dataInfo = {NULL, 0};
    int retCode = UnParcelDataInfo(request, &dataInfo);
    if (retCode == RETCODE_SUCCESS) {
        algorithmInfo->extendLen = dataInfo.length;
        algorithmInfo->extendMsg = dataInfo.data;
    } else {
        algorithmInfo->extendLen = 0;
        algorithmInfo->extendMsg = NULL;
    }
    return retCode;
}

static void FreeClientInfo(ClientInfo *clientInfo)
{
    if (clientInfo != NULL && clientInfo->extendMsg != NULL) {
        free(clientInfo->extendMsg);
        clientInfo->extendMsg = NULL;
        clientInfo->extendLen = 0;
    }
}

static void FreeAlgorithmInfo(AlgorithmInfo *algorithmInfo)
{
    if (algorithmInfo != NULL && algorithmInfo->extendMsg != NULL) {
        free(algorithmInfo->extendMsg);
        algorithmInfo->extendMsg = NULL;
        algorithmInfo->extendLen = 0;
    }
}

static int UnParcelInfo(IpcIo *req, ClientInfo *clientInfo, AlgorithmInfo *algorithmInfo, DataInfo *dataInfo)
{
    int retCode = UnParcelClientInfo(req, clientInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelClientInfo failed, retCode[%d].", retCode);
        return retCode;
    }

    retCode = UnParcelAlgorithmInfo(req, algorithmInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelAlgorithmInfo failed, retCode[%d].", retCode);
        FreeClientInfo(clientInfo);
        return retCode;
    }

    retCode = UnParcelDataInfo(req, dataInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelDataInfo failed, retCode[%d].", retCode);
        FreeClientInfo(clientInfo);
        FreeAlgorithmInfo(algorithmInfo);
    }
    return retCode;
}

static int InitEngine(const ConfigInfo *configInfo)
{
    int clientId = GenerateClient();
    if (clientId <= 0) {
        HILOGE("[SaServer]Fail to generate client id.");
        return INVALID_CLIENT_ID;
    }
    return clientId;
}

static int SyncExecuteAlgorithm(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo, const DataInfo *inputInfo,
    DataInfo *outputInfo)
{
    if (clientInfo == NULL || algoInfo == NULL) {
        HILOGE("[SaServer]Fail to SyncExecuteAlgorithm, because parameter verification failed.");
        return RETCODE_NULL_PARAM;
    }
    int retCode = SyncExecAlgoWrapper(clientInfo, algoInfo, inputInfo, outputInfo);
    HILOGD("[SaServer][clientId:%d,sessionId:%d]SyncExecAlgoWrapper finished, retCode is [%d]",
        clientInfo->clientId, clientInfo->sessionId, retCode);
    return retCode;
}

static int AsyncExecuteAlgorithm(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo,
    const DataInfo *inputInfo)
{
    if (clientInfo == NULL || algoInfo == NULL) {
        HILOGE("[SaServer]Fail to AsyncExecuteAlgorithm, because parameter verification failed.");
        return RETCODE_NULL_PARAM;
    }

    int retCode = AsyncExecAlgoWrapper(clientInfo, algoInfo, inputInfo);
    HILOGD("[SaServer][clientId:%d,sessionId:%d]AsyncExecAlgoWrapper finished, retCode is [%d]",
        clientInfo->clientId, clientInfo->sessionId, retCode);
    return retCode;
}

static int DestroyEngine(const ClientInfo *clientInfo)
{
    if (clientInfo == NULL) {
        HILOGE("[SaServer]Fail to DestroyEngine, because parameter verification failed.");
        return RETCODE_NULL_PARAM;
    }
    int retCode = RemoveAdapterWrapper(clientInfo);
    HILOGD("[SaServer][clientId:%d]RemoveAdapterWrapper finished, retCode is [%d].", clientInfo->clientId, retCode);
    return retCode;
}

static int SetOption(const ClientInfo *clientInfo, int optionType, const DataInfo *inputInfo)
{
    if (clientInfo == NULL) {
        HILOGE("[SaServer]Fail to SetOption, because parameter verification failed.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }
    int retCode = SetOptionWrapper(clientInfo, optionType, inputInfo);
    HILOGD("[SaServer][clientId:%d]SetOptionWrapper finished, retCode is [%d].", clientInfo->clientId, retCode);
    return retCode;
}

static int GetOption(const ClientInfo *clientInfo, int optionType, const DataInfo *inputInfo, DataInfo *outputInfo)
{
    if (clientInfo == NULL) {
        HILOGE("[SaServer]Fail to GetOption, because parameter verification failed.");
        return RETCODE_NULL_PARAM;
    }
    int retCode = GetOptionWrapper(clientInfo, optionType, inputInfo, outputInfo);
    HILOGD("[SaServer][clientId:%d,sessionId:%d]GetOptionWrapper finished, retCode is [%d]",
        clientInfo->clientId, clientInfo->sessionId, retCode);
    return retCode;
}

static int InvokeInitSaEngine(AiInterface *aiInterface, IpcIo *req, IpcIo *reply)
{
    HILOGI("[SaServer]InvokeInitSaEngine start.");
    ConfigInfo configInfo;
    size_t len = 0;
    configInfo.description = (char*)IpcIoPopString(req, &len);
    int32_t clientId = aiInterface->InitEngine(&configInfo);
    IpcIoPushInt32(reply, clientId);
    IpcIoPushUint32(reply, getuid());
    return clientId;
}

static int LoadAlgorithm(const ClientInfo *clientInfo, const AlgorithmInfo *algorithmInfo, const DataInfo *inputInfo,
    DataInfo *outputInfo)
{
    int retCode = LoadAlgoWrapper(clientInfo, algorithmInfo, inputInfo, outputInfo);
    HILOGD("[SaServer][clientId:%d,sessionId:%d]LoadAlgoWrapper finished, retCode is [%d]",
        clientInfo->clientId, clientInfo->sessionId, retCode);
    return retCode;
}

static int InvokeLoadAlgorithm(AiInterface *aiInterface, IpcIo *req, IpcIo *reply)
{
    HILOGI("[SaServer]InvokeLoadAlgorithm start.");
    ClientInfo clientInfo = {0};
    AlgorithmInfo algorithmInfo = {0};
    DataInfo inputInfo = {0};
    int retCode = UnParcelInfo(req, &clientInfo, &algorithmInfo, &inputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelInfo failed, retCode[%d].", retCode);
        return retCode;
    }

    DataInfo outputInfo = {
        .data = NULL,
        .length = 0,
    };
    retCode = aiInterface->LoadAlgorithm(&clientInfo, &algorithmInfo, &inputInfo, &outputInfo);
    IpcIoPushInt32(reply, retCode);
    ParcelDataInfo(reply, &outputInfo, clientInfo.clientUid);

    FreeDataInfo(&outputInfo);
    FreeClientInfo(&clientInfo);
    FreeAlgorithmInfo(&algorithmInfo);
    FreeDataInfo(&inputInfo);
    return retCode;
}

static int InvokeSyncExecute(AiInterface *aiInterface, IpcIo *req, IpcIo *reply)
{
    HILOGI("[SaServer]InvokeSyncExecute start.");
    ClientInfo clientInfo = {0};
    AlgorithmInfo algorithmInfo = {0};
    DataInfo inputInfo = {0};
    int retCode = UnParcelInfo(req, &clientInfo, &algorithmInfo, &inputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelInfo failed, retCode[%d].", retCode);
        return retCode;
    }

    DataInfo outputInfo = {
        .data = NULL,
        .length = 0,
    };
    retCode = aiInterface->SyncExecuteAlgorithm(&clientInfo, &algorithmInfo, &inputInfo, &outputInfo);
    IpcIoPushInt32(reply, retCode);
    ParcelDataInfo(reply, &outputInfo, clientInfo.clientUid);
    FreeDataInfo(&outputInfo);

    // inputInfo is hold by request, and freed when request is destructed in SaServerAdapter::SyncExecute().
    FreeClientInfo(&clientInfo);
    FreeAlgorithmInfo(&algorithmInfo);
    return retCode;
}

static int InvokeAsyncExecute(AiInterface *aiInterface, IpcIo *req, IpcIo *reply)
{
    HILOGI("[SaServer]InvokeAsyncExecute start.");
    ClientInfo clientInfo = {0};
    AlgorithmInfo algorithmInfo = {0};
    DataInfo inputInfo = {0};
    int retCode = UnParcelInfo(req, &clientInfo, &algorithmInfo, &inputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelInfo failed, retCode[%d].", retCode);
        return retCode;
    }

    retCode = aiInterface->AsyncExecuteAlgorithm(&clientInfo, &algorithmInfo, &inputInfo);
    // inputInfo is hold by request, and freed when request is destructed in SaServerAdapter::AsyncExecute().
    FreeClientInfo(&clientInfo);
    FreeAlgorithmInfo(&algorithmInfo);
    IpcIoPushInt32(reply, retCode);
    return retCode;
}

static int InvokeDestroyEngine(AiInterface *aiInterface, IpcIo *req, IpcIo *reply)
{
    HILOGI("[SaServer]InvokeDestroyEngine start.");
    ClientInfo clientInfo = {0};
    int retCode = UnParcelClientInfo(req, &clientInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelClientInfo failed, retCode[%d].", retCode);
        return retCode;
    }

    retCode = aiInterface->DestroyEngine(&clientInfo);
    FreeClientInfo(&clientInfo);
    IpcIoPushInt32(reply, retCode);
    return retCode;
}

static int InvokeSetOption(AiInterface *aiInterface, IpcIo *req, IpcIo *reply)
{
    HILOGI("[SaServer]InvokeSetOption start.");
    ClientInfo clientInfo = {0};
    int retCode = UnParcelClientInfo(req, &clientInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelClientInfo failed, retCode[%d].", retCode);
        return retCode;
    }

    int optionType = IpcIoPopInt32(req);
    DataInfo inputInfo = {0};
    retCode = UnParcelDataInfo(req, &inputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelDataInfo failed, retCode[%d].", retCode);
        FreeClientInfo(&clientInfo);
        FreeDataInfo(&inputInfo);
        return retCode;
    }

    retCode = aiInterface->SetOption(&clientInfo, optionType, &inputInfo);
    FreeClientInfo(&clientInfo);
    FreeDataInfo(&inputInfo);
    IpcIoPushInt32(reply, retCode);
    return retCode;
}

static int InvokeGetOption(AiInterface *aiInterface, IpcIo *req, IpcIo *reply)
{
    HILOGI("[SaServer]InvokeGetOption start.");
    ClientInfo clientInfo = {0};
    int retCode = UnParcelClientInfo(req, &clientInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelClientInfo failed, retCode[%d].", retCode);
        return retCode;
    }

    int optionType = IpcIoPopInt32(req);

    DataInfo inputInfo = {0};
    retCode = UnParcelDataInfo(req, &inputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelDataInfo failed, retCode[%d].", retCode);
        FreeClientInfo(&clientInfo);
        FreeDataInfo(&inputInfo);
        return retCode;
    }

    DataInfo outputInfo = {
        .data = NULL,
        .length = 0,
    };
    retCode = aiInterface->GetOption(&clientInfo, optionType, &inputInfo, &outputInfo);
    IpcIoPushInt32(reply, retCode);
    ParcelDataInfo(reply, &outputInfo, clientInfo.clientUid);

    FreeDataInfo(&outputInfo);
    FreeClientInfo(&clientInfo);
    FreeDataInfo(&inputInfo);
    return retCode;
}

static int InvokeRegisterCallback(AiInterface *aiInterface, IpcIo *req, IpcIo *reply)
{
    HILOGI("[SaServer]InvokeRegisterCallback start.");
    SvcIdentity *sid = IpcIoPopSvc(req);
    if (sid == NULL) {
        HILOGE("[SaServer]sid is null.");
        return RETCODE_NULL_PARAM;
    }
#ifdef __LINUX__
    BinderAcquire(sid->ipcContext, sid->handle);
#endif
    ClientInfo clientInfo = {0};
    int retCode = UnParcelClientInfo(req, &clientInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelClientInfo failed, retCode[%d].", retCode);
        return retCode;
    }
    retCode = RegisterCallbackWrapper(&clientInfo, sid);
    HILOGD("[SaServer][clientId:%d]RegisterCallbackWrapper finished, retCode is [%d].", clientInfo.clientId, retCode);

    FreeClientInfo(&clientInfo);
    IpcIoPushInt32(reply, retCode);
    return retCode;
}

static int InvokeUnregisterCallback(AiInterface *aiInterface, IpcIo *req, IpcIo *reply)
{
    HILOGI("[SaServer]InvokeUnRegisterClientCallback start.");
    ClientInfo clientInfo = {0};
    int retCode = UnParcelClientInfo(req, &clientInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServer]UnParcelClientInfo failed, retCode[%d].", retCode);
        return retCode;
    }
    retCode = aiInterface->UnregisterCallback(&clientInfo);
    FreeClientInfo(&clientInfo);
    IpcIoPushInt32(reply, retCode);
    return retCode;
}

static int UnregisterCallback(const ClientInfo* clientInfo)
{
    if (clientInfo == NULL) {
        HILOGE("[SaServer]Fail to SyncExecuteAlgorithm, because parameter verification failed.");
        return RETCODE_NULL_PARAM;
    }
    int retCode = UnregisterCallbackWrapper(clientInfo);
    HILOGD("[SaServer][clientId:%d]UnregisterCallbackWrapper finished, retCode is [%d].",
        clientInfo->clientId, retCode);
    return retCode;
}

static int InvokeUnloadAlgorithm(AiInterface *aiInterface, IpcIo *req, IpcIo *reply)
{
    HILOGI("[SaServer]InvokeUnloadAlgorithm start.");
    ClientInfo clientInfo = {0};
    AlgorithmInfo algorithmInfo = {0};
    DataInfo inputInfo = {0};
    int retCode = UnParcelInfo(req, &clientInfo, &algorithmInfo, &inputInfo);
    if (retCode != RETCODE_SUCCESS) {
        return retCode;
    }

    retCode = aiInterface->UnloadAlgorithm(&clientInfo, &algorithmInfo, &inputInfo);
    FreeClientInfo(&clientInfo);
    FreeAlgorithmInfo(&algorithmInfo);
    FreeDataInfo(&inputInfo);
    IpcIoPushInt32(reply, retCode);
    return retCode;
}

static int UnloadAlgorithm(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo, const DataInfo *inputInfo)
{
    if (clientInfo == NULL || algoInfo == NULL) {
        HILOGE("[SaServer]Fail to UnloadAlgorithm, because parameter verification failed.");
        return RETCODE_NULL_PARAM;
    }

    int retCode = UnloadAlgoWrapper(clientInfo, algoInfo, inputInfo);
    HILOGD("[SaServer][clientId:%d,sessionId:%d]UnloadAlgoWrapper finished, retCode is [%d].",
        clientInfo->clientId, clientInfo->sessionId, retCode);
    return retCode;
}

static int Invoke(IServerProxy *proxy, int funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    HILOGI("[SaServer]Begin to call Invoke, funcId is [%d].", funcId);
    AiInterface *aiInterface = (AiInterface *)proxy;
    switch (funcId) {
        case ID_INIT_ENGINE: {
            InvokeInitSaEngine(aiInterface, req, reply);
            break;
        }
        case ID_LOAD_ALGORITHM: {
            InvokeLoadAlgorithm(aiInterface, req, reply);
            break;
        }
        case ID_SYNC_EXECUTE_ALGORITHM: {
            InvokeSyncExecute(aiInterface, req, reply);
            break;
        }
        case ID_DESTROY_ENGINE: {
            InvokeDestroyEngine(aiInterface, req, reply);
            break;
        }
        case ID_SET_OPTION: {
            InvokeSetOption(aiInterface, req, reply);
            break;
        }
        case ID_GET_OPTION: {
            InvokeGetOption(aiInterface, req, reply);
            break;
        }
        case ID_REGISTER_CALLBACK: {
            InvokeRegisterCallback(aiInterface, req, reply);
            break;
        }
        case ID_UNREGISTER_CALLBACK: {
            InvokeUnregisterCallback(aiInterface, req, reply);
            break;
        }
        case ID_ASYNC_EXECUTE_ALGORITHM: {
            InvokeAsyncExecute(aiInterface, req, reply);
            break;
        }
        case ID_UNLOAD_ALGORITHM: {
            InvokeUnloadAlgorithm(aiInterface, req, reply);
            break;
        }
        default:{
            break;
        }
    }
    return EC_SUCCESS;
}

static AiEngineService g_aiEngine = {
    .GetName = GetName,
    .Initialize = Initialize,
    .MessageHandle = MessageHandle,
    .GetTaskConfig = GetTaskConfig,
    SERVER_IPROXY_IMPL_BEGIN,
    .Invoke = Invoke,
    .InitEngine = InitEngine,
    .SyncExecuteAlgorithm = SyncExecuteAlgorithm,
    .AsyncExecuteAlgorithm = AsyncExecuteAlgorithm,
    .UnloadAlgorithm = UnloadAlgorithm,
    .DestroyEngine = DestroyEngine,
    .SetOption = SetOption,
    .GetOption = GetOption,
    .UnregisterCallback = UnregisterCallback,
    .LoadAlgorithm = LoadAlgorithm,
    IPROXY_END,
};

static void Init(void)
{
    HILOGI("[SaServer]Init start.");
    SAMGR_GetInstance()->RegisterService((Service *)&g_aiEngine);
    SAMGR_GetInstance()->RegisterDefaultFeatureApi(AI_SERVICE, GET_IUNKNOWN(g_aiEngine));
}
SYSEX_SERVICE_INIT(Init);
