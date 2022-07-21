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

#include "client_executor/include/client_factory.h"

#include <thread>
#include <unistd.h>

#include "client_executor/include/i_client_cb.h"
#include "communication_adapter/include/sa_async_handler.h"
#include "platform/time/include/time.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
std::mutex ClientFactory::sessionIdMutex_;
namespace {
    const int MAXIMUM_NUMBER_OF_SESSION = 100;
    const int CHECK_CONNECTION_INTERVAL = 10;
    const int CHECK_CONNECTION_TIMES = 100;
}

ClientFactory::ClientFactory() : clientId_(INVALID_CLIENT_ID), sessionId_(AIE_SESSION_ID_BEGIN)
{
}

ClientFactory::~ClientFactory() = default;

int ClientFactory::ClientInit(const ConfigInfo &configInfo, ClientInfo &clientInfo,
    const AlgorithmInfo &algorithmInfo, IServiceDeadCb *cb)
{
    HILOGI("[ClientFactory]Begin to call ClientInit.");
    if (clientInfo.sessionId != INVALID_SESSION_ID) {
        HILOGW("[ClientFactory]Session id is valid, does not need to do Init.");
        // Valid sessionId tells it has been already INIT successfully before
        return RETCODE_SUCCESS;
    }

    clientInfo.sessionId = GenerateSessionId();
    bool isFirstSession = AddSessionInfo(clientInfo, algorithmInfo);
    int retCode = RETCODE_SUCCESS;
    if (isFirstSession) {
        retCode = InitAiServer(configInfo, clientInfo, algorithmInfo);
        if (retCode != RETCODE_SUCCESS) {
            HILOGE("[ClientFactory][clientId:%d,sessionId:%d]Fail to connect and init server, retCode[%d].",
                clientId_, clientInfo.sessionId, retCode);
            return retCode;
        }
    }
    retCode = WaitConnection();
    CHK_RET(retCode != RETCODE_SUCCESS, retCode);
    clientInfo.clientId = clientId_;
    clientInfo.serverUid = serverUid_;
    clientInfo.clientUid = getuid();

    return RegisterDeadCb(clientInfo.sessionId, cb);
}

int ClientFactory::ClientPrepare(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo, IClientCb *cb)
{
    HILOGI("[ClientFactory]Begin to call ClientPrepare.");
    if (clientInfo.sessionId == INVALID_SESSION_ID) {
        HILOGI("[ClientFactory][clientId:%d, sessionId:%d]SessionId is invalid,"\
            "please call aie clientInit firstly.",
            clientId_, clientInfo.sessionId);
        return RETCODE_SERVER_NOT_INIT;
    }
    int retCode = RETCODE_SUCCESS;
    if (algorithmInfo.isAsync) {
        if (cb == nullptr) {
            HILOGE("[ClientFactory][clientId:%d,sessionId:%d]It is Async Algorithm, but the input callback is null.",
                clientId_, clientInfo.sessionId);
            return RETCODE_NULL_PARAM;
        }
        retCode = RegisterCb(clientInfo.sessionId, cb);
        if (retCode != RETCODE_SUCCESS) {
            HILOGE("[ClientFactory][clientId:%d,sessionId:%d]Failed to register callback, result code[%d].",
                clientId_, clientInfo.sessionId, retCode);
            return retCode;
        }
    }

    retCode = LoadAlgorithm(clientInfo, algorithmInfo, inputInfo, outputInfo);
    HILOGD("[ClientFactory][clientId:%d,sessionId:%d]End to call load algorithm, result code[%d]",
        clientId_, clientInfo.sessionId, retCode);
    if (retCode != RETCODE_SUCCESS) {
        (void)UnRegisterCb(clientInfo.sessionId); // revert what's already inited, ignore return value.
    }
    return retCode;
}

int ClientFactory::ClientAsyncProcess(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo)
{
    HILOGI("[ClientFactory]Begin to call ClientAsyncProcess.");
    if (clientInfo.sessionId == INVALID_SESSION_ID) {
        HILOGE("[ClientFactory][clientId:%d, sessionId:%d] sessionId is invalid, please call Init firstly.",
            clientId_, clientInfo.sessionId);
        return RETCODE_SERVER_NOT_INIT;
    }
    if (!algorithmInfo.isAsync) {
        HILOGE("[ClientFactory]algorithm is synchronous, but async process is called.");
        return RETCODE_INVALID_PARAM;
    }

    int retCode = AsyncExecute(clientInfo, algorithmInfo, inputInfo);
    HILOGD("[ClientFactory][clientId:%d,sessionId:%d]End to call AsyncExecute, result code[%d]",
        clientId_, clientInfo.sessionId, retCode);
    return retCode;
}

int ClientFactory::ClientDestroy(ClientInfo &clientInfo)
{
    HILOGI("[ClientFactory]Begin to call ClientDestroy.");
    if (clientInfo.sessionId == INVALID_SESSION_ID) {
        HILOGW("[ClientFactory][clientId:%d, sessionId:%d] sessionId is invalid value, not need destroy.",
            clientId_, clientInfo.sessionId);
        // invalid session id tells it has been already destroyed before, it is still successfully destroy
        return RETCODE_FAILURE;
    }
    int retCode = EraseSessionInfo(clientInfo.sessionId);
    if (retCode != RETCODE_SUCCESS) {
        return retCode;
    }
    bool isLastSession = sessionInfos_.empty();
    if (isLastSession) {
        retCode = CloseAiServer();
        if (retCode != RETCODE_SUCCESS) {
            HILOGE("[ClientFactory][clientId:%d, sessionId:%d]Fail to dis-connect and unInit server, result code[%d].",
                clientId_, clientInfo.sessionId, retCode);
            return retCode;
        }
        if (clientId_ != INVALID_CLIENT_ID) {
            HILOGE("[ClientFactory][clientId:%d, sessionId:%d]Fail to reset client id.",
                clientId_, clientInfo.sessionId);
            return RETCODE_FAILURE;
        }
    }
    retCode = UnRegisterDeadCb(clientInfo.sessionId);
    clientInfo.sessionId = INVALID_SESSION_ID;
    clientInfo.clientId = INVALID_CLIENT_ID;
    HILOGD("[ClientFactory][clientId:%d, sessionId:%d]End to call ClientDestroy, result code[%d].",
        clientId_, clientInfo.sessionId, retCode);
    return retCode;
}

int ClientFactory::ClientSetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo)
{
    HILOGI("[ClientFactory]Begin to call ClientSetOption.");
    return SetOption(clientInfo, optionType, inputInfo);
}

int ClientFactory::ClientGetOption(const ClientInfo &clientInfo, int optionType,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    HILOGI("[ClientFactory]Begin to call ClientGetOption.");
    return GetOption(clientInfo, optionType, inputInfo, outputInfo);
}

int ClientFactory::ClientSyncProcess(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    HILOGI("[ClientFactory]Begin to call ClientSyncProcess.");
    if (clientInfo.sessionId == INVALID_SESSION_ID) {
        HILOGE("[ClientFactory]SessionId is invalid, please call Init firstly.");
        return RETCODE_SERVER_NOT_INIT;
    }
    if (algorithmInfo.isAsync) {
        HILOGE("[ClientFactory]algorithm is asynchronous, but sync process is called.");
        return RETCODE_INVALID_PARAM;
    }
    return SyncExecute(clientInfo, algorithmInfo, inputInfo, outputInfo);
}

int ClientFactory::ClientRelease(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo)
{
    HILOGI("[ClientFactory]Begin to call ClientRelease.");
    if (clientInfo.sessionId == INVALID_SESSION_ID) {
        HILOGI("[ClientFactory][clientId:%d, sessionId:%d] sessionId is invalid value, not need release.",
            clientId_, clientInfo.sessionId);
        return RETCODE_SUCCESS;
    }

    if (algorithmInfo.isAsync) {
        int unRegisterCbRetCode = UnRegisterCb(clientInfo.sessionId);
        if (unRegisterCbRetCode != RETCODE_SUCCESS) {
            HILOGE("[ClientFactory][clientId:%d,sessionId:%d]Fail to unRegister callback, error code[%d]",
                clientId_, clientInfo.sessionId, unRegisterCbRetCode);
            return unRegisterCbRetCode;
        }
    }
    int retCode = UnLoadAlgorithm(clientInfo, algorithmInfo, inputInfo);
    HILOGD("[ClientFactory][clientId:%d,sessionId:%d]End to call unLoad algorithm, result code[%d]", clientId_,
        clientInfo.sessionId, retCode);
    return retCode;
}

void ClientFactory::SetClientId(int clientId)
{
    clientId_ = clientId;
}

int ClientFactory::GetClientId() const
{
    return clientId_;
}

void ClientFactory::SetServerUid(const uid_t clientId)
{
    serverUid_ = clientId;
}

uid_t ClientFactory::GetServerUid() const
{
    return serverUid_;
}

int ClientFactory::GenerateSessionId()
{
    std::lock_guard<std::mutex> lock(sessionIdMutex_);
    if (sessionInfos_.size() > MAXIMUM_NUMBER_OF_SESSION) {
        HILOGE("[ClientFactory]Session id exceed maximum numbers.");
        return INVALID_SESSION_ID;
    }
    do {
        ++sessionId_;
        if (sessionId_ < AIE_SESSION_ID_BEGIN) {
            // session id may come to the maximum value and turn to negative value
            HILOGI("[ClientFactory]SessionId reaches max int value, now reset it to init value(%d).",
                AIE_SESSION_ID_BEGIN);
            sessionId_ = AIE_SESSION_ID_BEGIN;
        }
    } while (sessionInfos_.find(sessionId_) != sessionInfos_.end());
    return sessionId_;
}

bool ClientFactory::AddSessionInfo(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo)
{
    std::lock_guard<std::mutex> lock(sessionIdMutex_);
    bool isFirstSession = sessionInfos_.empty();
    sessionInfos_.insert(std::make_pair(clientInfo.sessionId, algorithmInfo.algorithmType));
    return isFirstSession;
}

int ClientFactory::EraseSessionInfo(int sessionId)
{
    std::lock_guard<std::mutex> lock(sessionIdMutex_);
    if (!sessionInfos_.erase(sessionId)) {
        HILOGW("[ClientFactory][sessionId:%d]session id does not exist.", sessionId);
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int ClientFactory::GetSessionInfo(int sessionId, int &algorithmType)
{
    std::lock_guard<std::mutex> lock(sessionIdMutex_);
    SessionInfos::iterator iter = sessionInfos_.find(sessionId);
    CHK_RET(iter == sessionInfos_.end(), RETCODE_EMPTY_MAP);
    algorithmType = iter->second;
    return RETCODE_SUCCESS;
}

int ClientFactory::RegisterCb(int sessionId, IClientCb *cb)
{
    HILOGI("[ClientFactory]Begin to call RegisterCb.");
    SaAsyncHandler *saAsyncHandler = SaAsyncHandler::GetInstance();
    CHK_RET(saAsyncHandler == nullptr, RETCODE_NULL_PARAM);
    int retCode = saAsyncHandler->RegisterCb(sessionId, cb);
    CHK_RET(retCode != RETCODE_SUCCESS, retCode);
    retCode = saAsyncHandler->RegisterAsyncClientCb();
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[ClientFactory]Fail to register callback, retCode[%d].", retCode);
        (void)saAsyncHandler->UnRegisterCb(sessionId); // revert what's already inited, ignore return value.
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int ClientFactory::UnRegisterCb(const int sessionId)
{
    HILOGI("[ClientFactory]Begin to call UnRegisterCb.");
    SaAsyncHandler *saAsyncHandler = SaAsyncHandler::GetInstance();
    CHK_RET(saAsyncHandler == nullptr, RETCODE_NULL_PARAM);
    int serviceCallbackRetCode = saAsyncHandler->UnRegisterAsyncClientCb();
    int sdkCallbackRetcode = saAsyncHandler->UnRegisterCb(sessionId);
    if (serviceCallbackRetCode != RETCODE_SUCCESS || sdkCallbackRetcode != RETCODE_SUCCESS) {
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int ClientFactory::RegisterDeadCb(int sessionId, IServiceDeadCb *cb)
{
    HILOGI("[ClientFactory]Begin to call RegisterDeadCb.");
    SaAsyncHandler *saAsyncHandler = SaAsyncHandler::GetInstance();
    CHK_RET(saAsyncHandler == nullptr, RETCODE_NULL_PARAM);
    int retCode = saAsyncHandler->RegisterDeadCb(sessionId, cb);
    CHK_RET(retCode != RETCODE_SUCCESS, retCode);
    retCode = saAsyncHandler->RegisterServiceDeathCb();
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[ClientFactory]Fail to register death callback, retCode[%d].", retCode);
        (void)saAsyncHandler->UnRegisterCb(sessionId); // revert what's already inited, ignore return value.
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int ClientFactory::UnRegisterDeadCb(const int sessionId)
{
    HILOGI("[ClientFactory]Begin to call UnRegisterDeadCb.");
    SaAsyncHandler *saAsyncHandler = SaAsyncHandler::GetInstance();
    CHK_RET(saAsyncHandler == nullptr, RETCODE_NULL_PARAM);
    int serviceCallbackRetCode = saAsyncHandler->UnRegisterServiceDeathCb();
    int sdkCallbackRetcode = saAsyncHandler->UnRegisterDeadCb(sessionId);
    if (serviceCallbackRetCode != RETCODE_SUCCESS || sdkCallbackRetcode != RETCODE_SUCCESS) {
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int ClientFactory::WaitConnection()
{
    // wait for 100 ms
    for (int i = 0; i < CHECK_CONNECTION_TIMES; ++i) {
        if (clientId_ != INVALID_CLIENT_ID) {
            HILOGI("[ClientFactory][clientId:%d]status is connected.", clientId_);
            return RETCODE_SUCCESS;
        }
        StepSleepMs(CHECK_CONNECTION_INTERVAL);
    }

    HILOGE("[ClientFactory][clientId:%d]The connection has not been created.", clientId_);
    return RETCODE_SA_SERVICE_EXCEPTION;
}

void ClientFactory::ResetClient()
{
    clientId_ = INVALID_CLIENT_ID;
    sessionId_ = INVALID_SESSION_ID;
    serverUid_ = INVALID_UID;
    clientId_ = INVALID_UID;
}
} // namespace AI
} // namespace OHOS
