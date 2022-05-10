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

#include "communication_adapter/include/sa_server_adapter.h"

#include "ipc_skeleton.h"
#include "securec.h"

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "server_executor/include/i_async_task_manager.h"
#include "server_executor/include/i_engine_manager.h"
#include "server_executor/include/i_sync_task_manager.h"
#include "utils/aie_guard.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
namespace {
const unsigned int ADAPT_ID_BIT = 32U;
const int INPUT_LENGTH_NULL = 0;
}

SaServerAdapter::SaServerAdapter(int adapterId) : adapterId_(adapterId), refCount_(0)
{
}

SaServerAdapter::~SaServerAdapter()
{
    Uninitialize();
}

int SaServerAdapter::GetSessionId(long long transactionId) const
{
    return static_cast<int>(static_cast<unsigned long long>(transactionId) & TRANS_ID_MASK);
}

int SaServerAdapter::GetAdapterId() const
{
    return adapterId_;
}

void SaServerAdapter::IncRef()
{
    ++refCount_;
}

void SaServerAdapter::DecRef()
{
    refCount_--;
}

int SaServerAdapter::GetRefCount() const
{
    return refCount_;
}

void SaServerAdapter::Uninitialize()
{
    std::lock_guard<std::mutex> guard(mutex_);
    IEngineManager* engineManager = GetEngineManager();
    if (engineManager == nullptr) {
        HILOGE("[SaServerAdapter]Failed to get engine manager.");
        return;
    }

    DataInfo inputInfo = {
        .data = nullptr,
        .length = INPUT_LENGTH_NULL,
    };

    for (auto &item: transactionIds_) {
        engineManager->StopEngine(item, inputInfo);
    }
    transactionIds_.clear();
}

void SaServerAdapter::SaveEngineListener(SvcIdentity *svcIdentity)
{
    svcIdentity_ = *svcIdentity;
}

void SaServerAdapter::ClearEngineListener()
{
    ReleaseSvc(svcIdentity_);
}

SvcIdentity *SaServerAdapter::GetEngineListener()
{
    return &svcIdentity_;
}

long long SaServerAdapter::GetTransactionId(int sessionId) const
{
    return static_cast<long long>(static_cast<unsigned long long>(adapterId_) << ADAPT_ID_BIT) + sessionId;
}

int SaServerAdapter::AsyncExecute(const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo)
{
    IRequest *request = nullptr;
    ConvertToRequest(clientInfo, algoInfo, inputInfo, request);
    ResGuard<IRequest> guardReq(request);

    IAsyncTaskManager *asyncTaskManager = GetAsyncTaskManager();
    if (asyncTaskManager == nullptr) {
        HILOGE("[SaServerAdapter]Get async task manager fail, ret: %d, clientId: %d, sessionId: %d, algoType: %d.",
            RETCODE_OUT_OF_MEMORY, clientInfo.clientId, clientInfo.sessionId, algoInfo.algorithmType);
        return RETCODE_OUT_OF_MEMORY;
    }

    int ret = asyncTaskManager->AsyncExecute(request);
    if (ret != RETCODE_SUCCESS) {
        HILOGE("[SaServerAdapter]Fail to get async execute request, ret is %d.", ret);
        return ret;
    }

    guardReq.Detach();
    return RETCODE_SUCCESS;
}

void SaServerAdapter::ConvertToRequest(const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo, IRequest *&request)
{
    request = IRequest::Create();
    if (request == nullptr) {
        HILOGE("[SaServerAdapter]Fail to create request.");
        return;
    }
    request->SetRequestId(algoInfo.requestId);
    request->SetOperationId(algoInfo.operateId);
    request->SetTransactionId(GetTransactionId(clientInfo.sessionId));
    request->SetAlgoPluginType(algoInfo.algorithmType);
    request->SetMsg(inputInfo);
    request->SetClientUid(clientInfo.clientUid);
}

int SaServerAdapter::LoadAlgorithm(long long transactionId, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    IEngineManager *engineManager = GetEngineManager();
    if (engineManager == nullptr) {
        HILOGE("[SaServerAdapter][transactionId:%lld]Failed to get engine manager.", transactionId);
        return RETCODE_OUT_OF_MEMORY;
    }
    int retCode = engineManager->StartEngine(transactionId, algoInfo, inputInfo, outputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServerAdapter][transactionId:%lld]Failed to load algorithm.", transactionId);
        return retCode;
    }

    SaveTransaction(transactionId);
    return RETCODE_SUCCESS;
}

int SaServerAdapter::SetOption(long long transactionId, int optionType, const DataInfo &dataInfo)
{
    IEngineManager *engineManager = GetEngineManager();
    if (engineManager == nullptr) {
        HILOGE("[SaServerAdapter][transactionId:0x%llx]Failed to get engine manager.", transactionId);
        return RETCODE_OUT_OF_MEMORY;
    }

    return engineManager->SetOption(transactionId, optionType, dataInfo);
}

int SaServerAdapter::GetOption(long long transactionId, int optionType, const DataInfo &dataInfo,
    DataInfo &outputInfo)
{
    IEngineManager *engineManager = GetEngineManager();
    if (engineManager == nullptr) {
        HILOGE("[SaServerAdapter][transactionId:0x%llx]Failed to get engine manager.", transactionId);
        return RETCODE_OUT_OF_MEMORY;
    }

    return engineManager->GetOption(transactionId, optionType, dataInfo, outputInfo);
}

void SaServerAdapter::SaveTransaction(long long transactionId)
{
    std::lock_guard<std::mutex> guard(mutex_);
    transactionIds_.insert(transactionId);
}

int SaServerAdapter::UnloadAlgorithm(long long transactionId, const DataInfo &inputInfo)
{
    IEngineManager* engineInstance = GetEngineManager();
    if (engineInstance == nullptr) {
        HILOGE("[SaServerAdapter][transactionId:%lld]Failed to get engine manager.", transactionId);
        return RETCODE_OUT_OF_MEMORY;
    }
    int retCode = engineInstance->StopEngine(transactionId, inputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaServerAdapter][transactionId:%lld]Failed to unload algorithm.", transactionId);
        return retCode;
    }
    RemoveTransaction(transactionId);
    return RETCODE_SUCCESS;
}

void SaServerAdapter::RemoveTransaction(long long transactionId)
{
    std::lock_guard<std::mutex> guard(mutex_);
    transactionIds_.erase(transactionId);
}

int SaServerAdapter::SyncExecute(const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    IRequest *request = nullptr;
    outputInfo.data = nullptr;
    outputInfo.length = 0;
    ConvertToRequest(clientInfo, algoInfo, inputInfo, request);
    if (request == nullptr) {
        HILOGE("[SaServer]Fail to ConvertToRequest.");
        return RETCODE_OUT_OF_MEMORY;
    }
    ResGuard<IRequest> guardReq(request);
    ISyncTaskManager *taskMgr = GetSyncTaskManager();
    if (taskMgr == nullptr) {
        HILOGE("[SaServerAdapter]Get task manager failed, ret is %d", RETCODE_OUT_OF_MEMORY);
        return RETCODE_OUT_OF_MEMORY;
    }
    IResponse *response = nullptr;
    int retCode = taskMgr->SyncExecute(request, response);
    ResGuard<IResponse> guardRes(response);
    if ((retCode != RETCODE_SUCCESS) || (response == nullptr)) {
        HILOGE("[SaServerAdapter]Execute request failed, ret is %d", retCode);
        return retCode;
    }
    outputInfo = response->GetResult();
    response->Detach();

    return RETCODE_SUCCESS;
}
} // namespace AI
} // namespace OHOS
