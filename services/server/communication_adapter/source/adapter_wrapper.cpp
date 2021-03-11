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

#include "communication_adapter/include/adapter_wrapper.h"

#include <cstring>
#include <map>

#include "communication_adapter/include/sa_async_handler.h"
#include "communication_adapter/include/sa_server_adapter.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/aie_macros.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
namespace {
constexpr int STARTING_CLIENT_ID = 1;
constexpr int MAX_NUM_CLIENTS = 1024;
using ServerAdapters = std::map<int, SaServerAdapter*>;
ServerAdapters g_saServerAdapters;
std::atomic<int> g_clientIdAtomic(0);
std::mutex g_serverAdapterMutex;
std::mutex g_connectMutex;

int FindValidClientId()
{
    if (g_saServerAdapters.size() > MAX_NUM_CLIENTS) {
        HILOGE("[AdapterWrapper]Num of valid clients reaches max.");
        return INVALID_CLIENT_ID;
    }

    do {
        ++g_clientIdAtomic;
        if (g_clientIdAtomic < STARTING_CLIENT_ID) {
            HILOGI("[AdapterWrapper]Client id reaches max, reset to starting value [%d].", STARTING_CLIENT_ID);
            g_clientIdAtomic = STARTING_CLIENT_ID;
        }
    } while (g_saServerAdapters.find(g_clientIdAtomic) != g_saServerAdapters.end());

    return g_clientIdAtomic;
}

int AllocateClientAdapter()
{
    SaServerAdapter *adapter = nullptr;
    AIE_NEW(adapter, SaServerAdapter(g_clientIdAtomic));
    if (adapter == nullptr) {
        HILOGE("[AdapterWrapper]Failed to new adapter.");
        return INVALID_CLIENT_ID;
    }

    int clientId = adapter->GetAdapterId();

    std::lock_guard<std::mutex> guard(g_serverAdapterMutex);
    g_saServerAdapters[clientId] = adapter;
    return clientId;
}
}

class AdapterWrapper {
public:
    explicit AdapterWrapper(SaServerAdapter *adapter) : adapter_(adapter)
    {
        if (adapter_) {
            adapter_->IncRef();
        }
    }

    ~AdapterWrapper()
    {
        if (adapter_) {
            adapter_->DecRef();
            adapter_ = nullptr;
        }
    }

private:
    SaServerAdapter *adapter_ = nullptr;
};

SaServerAdapter* FindAdapter(const int clientId)
{
    std::lock_guard<std::mutex> guard(g_serverAdapterMutex);
    ServerAdapters::iterator iter = g_saServerAdapters.find(clientId);
    if (iter != g_saServerAdapters.end()) {
        return iter->second;
    }
    return nullptr;
}

int GenerateClient()
{
    HILOGI("[AdapterWrapper]Begin to call GenerateClient.");
    std::lock_guard<std::mutex> guard(g_connectMutex);

    if (FindValidClientId() == INVALID_CLIENT_ID) {
        return INVALID_CLIENT_ID;
    }

    return AllocateClientAdapter();
}

int SyncExecAlgoWrapper(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo, const DataInfo *inputInfo,
    DataInfo *outputInfo)
{
    HILOGI("[AdapterWrapper]Begin to call SyncExecAlgoWrapper.");
    if (clientInfo == nullptr || algoInfo == nullptr) {
        HILOGE("[AdapterWrapper]The clientInfo or algoInfo is nullptr");
        return RETCODE_NULL_PARAM;
    }

    if (algoInfo->isAsync) {
        HILOGW("[AdapterWrapper]SyncExecute but the algoInfo is AsyncExecute");
        return RETCODE_WRONG_INFER_MODE;
    }

    SaServerAdapter *adapter = FindAdapter(clientInfo->clientId);
    if (adapter == nullptr) {
        HILOGE("[AdapterWrapper]No adapter found for client[%d].", clientInfo->clientId);
        return RETCODE_NO_CLIENT_FOUND;
    }

    AdapterWrapper adapterGuard(adapter);
    return adapter->SyncExecute(*clientInfo, *algoInfo, *inputInfo, *outputInfo);
}

int AsyncExecAlgoWrapper(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo, const DataInfo *inputInfo)
{
    HILOGI("[AdapterWrapper]Begin to call AsyncExecAlgoWrapper.");
    if (clientInfo == nullptr || algoInfo == nullptr) {
        HILOGE("[AdapterWrapper]The clientInfo or algoInfo is nullptr.");
        return RETCODE_NULL_PARAM;
    }

    if (!algoInfo->isAsync) {
        HILOGW("[AdapterWrapper]AsyncExecute but the algoInfo is SyncExecute.");
        return RETCODE_WRONG_INFER_MODE;
    }

    SaServerAdapter *adapter = FindAdapter(clientInfo->clientId);
    if (adapter == nullptr) {
        HILOGE("[AdapterWrapper]No adapter found for client[%d].", clientInfo->clientId);
        return RETCODE_NO_CLIENT_FOUND;
    }
    AdapterWrapper adapterGuard(adapter);

    return adapter->AsyncExecute(*clientInfo, *algoInfo, *inputInfo);
}

int LoadAlgoWrapper(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo, const DataInfo *inputInfo,
    DataInfo *outputInfo)
{
    HILOGI("[AdapterWrapper]Begin to call LoadAlgoWrapper.");
    if (clientInfo == nullptr || algoInfo == nullptr) {
        HILOGE("[AdapterWrapper]The clientInfo or algoInfo is null");
        return RETCODE_NULL_PARAM;
    }

    SaServerAdapter *adapter = FindAdapter(clientInfo->clientId);
    if (adapter == nullptr) {
        HILOGE("[AdapterWrapper]No adapter found for client[%d].", clientInfo->clientId);
        return RETCODE_NO_CLIENT_FOUND;
    }

    AdapterWrapper adapterGuard(adapter);
    long long transactionId = adapter->GetTransactionId(clientInfo->sessionId);
    int retCode = adapter->LoadAlgorithm(transactionId, *algoInfo, *inputInfo, *outputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[AdapterWrapper][transactionId:%lld]Failed to load algorithm, retCode[%d], aid[%d].",
            transactionId, retCode, algoInfo->algorithmType);
        return retCode;
    }

    if (algoInfo->isAsync) {
        SaAsyncHandler *saAsyncHandler = SaAsyncHandler::GetInstance();
        CHK_RET(saAsyncHandler == nullptr, RETCODE_NULL_PARAM);
        retCode = saAsyncHandler->StartAsyncTransaction(transactionId, clientInfo->clientId);
        HILOGI("[AdapterWrapper]StartAsyncTransaction retCode is [%d].", retCode);
    }
    return retCode;
}

int UnloadAlgoWrapper(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo, const DataInfo *inputInfo)
{
    HILOGI("[AdapterWrapper]Begin to call UnloadAlgoWrapper.");
    SaServerAdapter *adapter = FindAdapter(clientInfo->clientId);
    if (adapter == nullptr) {
        HILOGE("[AdapterWrapper]No adapter found for client[%d].", clientInfo->clientId);
        return RETCODE_NO_CLIENT_FOUND;
    }

    AdapterWrapper adapterGuard(adapter);
    long long transactionId = adapter->GetTransactionId(clientInfo->sessionId);
    if (algoInfo == nullptr) {
        HILOGE("[AdapterWrapper]AlgoInfo is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    if (algoInfo->isAsync) {
        SaAsyncHandler *saAsyncHandler = SaAsyncHandler::GetInstance();
        if (saAsyncHandler != nullptr) {
            saAsyncHandler->StopAsyncTransaction(transactionId);
        }
    }

    return adapter->UnloadAlgorithm(transactionId, *inputInfo);
}

int RemoveAdapterWrapper(const ClientInfo *clientInfo)
{
    HILOGI("[AdapterWrapper]Begin to call RemoveAdapterWrapper.");
    std::lock_guard<std::mutex> guard(g_serverAdapterMutex);
    ServerAdapters::iterator iter = g_saServerAdapters.find(clientInfo->clientId);
    if (iter == g_saServerAdapters.end()) {
        HILOGE("[AdapterWrapper]Failed to find serverAdapter for client[%d].", clientInfo->clientId);
        return RETCODE_FAILURE;
    }

    AIE_DELETE(iter->second);
    g_saServerAdapters.erase(iter);
    return RETCODE_SUCCESS;
}

int SetOptionWrapper(const ClientInfo *clientInfo, int optionType, const DataInfo *inputInfo)
{
    HILOGI("[AdapterWrapper]Begin to call SetOptionWrapper.");
    SaServerAdapter *adapter = FindAdapter(clientInfo->clientId);
    if (adapter == nullptr) {
        HILOGE("[AdapterWrapper]No adapter found for client[%d].", clientInfo->clientId);
        return RETCODE_NO_CLIENT_FOUND;
    }

    AdapterWrapper adapterGuard(adapter);
    long long transactionId = adapter->GetTransactionId(clientInfo->sessionId);
    return adapter->SetOption(transactionId, optionType, *inputInfo);
}

int GetOptionWrapper(const ClientInfo *clientInfo, int optionType, const DataInfo *inputInfo, DataInfo *outputInfo)
{
    HILOGI("[AdapterWrapper]Begin to call GetOptionWrapper.");
    if (clientInfo == nullptr) {
        HILOGE("[AdapterWrapper]ClientInfo is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    SaServerAdapter *adapter = FindAdapter(clientInfo->clientId);
    if (adapter == nullptr) {
        HILOGE("[AdapterWrapper]No adapter found for client[%d].", clientInfo->clientId);
        return RETCODE_NO_CLIENT_FOUND;
    }

    AdapterWrapper adapterGuard(adapter);
    long long transactionId = adapter->GetTransactionId(clientInfo->sessionId);
    return adapter->GetOption(transactionId, optionType, *inputInfo, *outputInfo);
}

int RegisterCallbackWrapper(const ClientInfo *clientInfo, const SvcIdentity *sid)
{
    HILOGI("[AdapterWrapper]Begin to call RegisterCallbackWrapper.");
    SaServerAdapter *adapter = FindAdapter(clientInfo->clientId);
    if (adapter == nullptr) {
        HILOGE("[AdapterWrapper]No adapter found for client[%d].", clientInfo->clientId);
        return RETCODE_NO_CLIENT_FOUND;
    }
    AdapterWrapper adapterGuard(adapter);

    adapter->SaveEngineListener(sid);

    SaAsyncHandler *saAsyncHandler = SaAsyncHandler::GetInstance();
    CHK_RET(saAsyncHandler == nullptr, RETCODE_NULL_PARAM);
    int retCode = saAsyncHandler->RegisterAsyncHandler(clientInfo->clientId);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[AdapterWrapper]Client[%d] session[%d] RegisterAsyncHandler result is [%d].", clientInfo->clientId,
            clientInfo->sessionId, retCode);
        return retCode;
    }

    return saAsyncHandler->StartAsyncProcess(clientInfo->clientId, adapter);
}

int UnregisterCallbackWrapper(const ClientInfo *clientInfo)
{
    HILOGI("[AdapterWrapper]Begin to call UnregisterCallbackWrapper.");
    SaServerAdapter *adapter = FindAdapter(clientInfo->clientId);
    if (adapter == nullptr) {
        HILOGE("[AdapterWrapper]No adapter found for client[%d].", clientInfo->clientId);
        return RETCODE_NO_CLIENT_FOUND;
    }

    AdapterWrapper adapterGuard(adapter);
    SaAsyncHandler *saAsyncHandler = SaAsyncHandler::GetInstance();
    CHK_RET(saAsyncHandler == nullptr, RETCODE_NULL_PARAM);
    saAsyncHandler->StopAsyncProcess(clientInfo->clientId);
    adapter->ClearEngineListener();
    return RETCODE_SUCCESS;
}