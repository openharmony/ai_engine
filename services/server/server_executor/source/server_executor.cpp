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

#include "server_executor/include/server_executor.h"

#include "protocol/data_channel/include/i_request.h"
#include "server_executor/include/future_factory.h"
#include "server_executor/include/i_future.h"

namespace OHOS {
namespace AI {
std::mutex ServerExecutor::mutex_;
ServerExecutor *ServerExecutor::instance_ = nullptr;

ServerExecutor *ServerExecutor::GetInstance()
{
    CHK_RET(instance_ != nullptr, instance_);

    std::lock_guard<std::mutex> lock(mutex_);
    CHK_RET(instance_ != nullptr, instance_);

    ServerExecutor *tempInstance = nullptr;
    AIE_NEW(tempInstance, ServerExecutor);
    CHK_RET(tempInstance == nullptr, nullptr);

    if (tempInstance->Initialize() != RETCODE_SUCCESS) {
        AIE_DELETE(tempInstance);
        return nullptr;
    }
    instance_ = tempInstance;
    return instance_;
}

void ServerExecutor::ReleaseInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    AIE_DELETE(instance_);
}

ServerExecutor::ServerExecutor() : engineMgr_(nullptr)
{
}

ServerExecutor::~ServerExecutor()
{
    Uninitialize();
}

int ServerExecutor::Initialize()
{
    AIE_NEW(engineMgr_, EngineManager);
    if (engineMgr_ == nullptr) {
        HILOGE("[ServerExecutor]Failed to new engine manager");
        return RETCODE_OUT_OF_MEMORY;
    }
    int retCode = engineMgr_->Initialize();
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[ServerExecutor]Failed to initialize engine manager");
        AIE_DELETE(engineMgr_);
    }
    return retCode;
}

void ServerExecutor::Uninitialize()
{
    AIE_DELETE(engineMgr_);
    FutureFactory::ReleaseInstance();
}

int ServerExecutor::StartEngine(long long transactionId, const AlgorithmInfo &algoInfo, const DataInfo &inputInfo,
    DataInfo &outputInfo)
{
    CHK_RET(engineMgr_ == nullptr, RETCODE_ENGINE_MANAGER_NOT_INIT);

    HILOGI("[ServerExecutor]Begin to start engine for algorithm, algorithmType:%d.", algoInfo.algorithmType);
    return engineMgr_->StartEngine(transactionId, algoInfo, inputInfo, outputInfo);
}

int ServerExecutor::StopEngine(long long transactionId, const DataInfo &inputInfo)
{
    CHK_RET(engineMgr_ == nullptr, RETCODE_ENGINE_MANAGER_NOT_INIT);

    HILOGI("[ServerExecutor]Begin to stop engine for algorithm.");
    return engineMgr_->StopEngine(transactionId, inputInfo);
}

int ServerExecutor::SetOption(long long transactionId, int optionType, const DataInfo &inputInfo)
{
    CHK_RET(engineMgr_ == nullptr, RETCODE_ENGINE_MANAGER_NOT_INIT);

    HILOGI("[ServerExecutor]Begin to set option for algorithm.");
    return engineMgr_->SetOption(transactionId, optionType, inputInfo);
}

int ServerExecutor::GetOption(long long transactionId, int optionType, const DataInfo &inputInfo,
    DataInfo &outputInfo)
{
    CHK_RET(engineMgr_ == nullptr, RETCODE_ENGINE_MANAGER_NOT_INIT);

    HILOGI("[ServerExecutor]Begin to get option for algorithm.");
    return engineMgr_->GetOption(transactionId, optionType, inputInfo, outputInfo);
}

int ServerExecutor::SyncExecute(IRequest *request, IResponse *&response)
{
    CHK_RET(engineMgr_ == nullptr, RETCODE_ENGINE_MANAGER_NOT_INIT);
    CHK_RET(request == nullptr, RETCODE_NULL_PARAM);
    HILOGI("[ServerExecutor]Begin to call SyncExecute, algoType:%d.", request->GetAlgoPluginType());

    std::shared_ptr<Engine> engine = engineMgr_->FindEngine(request->GetTransactionId());
    CHK_RET(engine == nullptr, RETCODE_ENGINE_NOT_EXIST);

    return engine->SyncExecute(request, response);
}

int ServerExecutor::AsyncExecute(IRequest *request)
{
    if (engineMgr_ == nullptr) {
        HILOGI("[ServerExecutor]The engineMgr_ is null.");
        return RETCODE_ENGINE_MANAGER_NOT_INIT;
    }

    if (request == nullptr) {
        HILOGI("[ServerExecutor]The request is null.");
        return RETCODE_NULL_PARAM;
    }

    std::shared_ptr<Engine> engine = engineMgr_->FindEngine(request->GetTransactionId());
    CHK_RET(engine == nullptr, RETCODE_ENGINE_NOT_EXIST);

    int retCode = engine->AsyncExecute(request);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[ServerExecutor]Engine AsyncExecute failed, retCode is %d.", retCode);
    }
    return retCode;
}

int ServerExecutor::RegisterListener(IFutureListener *listener, long long transactionId)
{
    FutureFactory *futureFactory = FutureFactory::GetInstance();
    CHK_RET(futureFactory == nullptr, RETCODE_NULL_PARAM);
    futureFactory->RegisterListener(listener, transactionId);
    return RETCODE_SUCCESS;
}

int ServerExecutor::UnRegisterListener(long long transactionId)
{
    FutureFactory *futureFactory = FutureFactory::GetInstance();
    CHK_RET(futureFactory == nullptr, RETCODE_NULL_PARAM);
    futureFactory->UnregisterListener(transactionId);
    return RETCODE_SUCCESS;
}

ISyncTaskManager *GetSyncTaskManager()
{
    return ServerExecutor::GetInstance();
}

IAsyncTaskManager *GetAsyncTaskManager()
{
    return ServerExecutor::GetInstance();
}

IEngineManager *GetEngineManager()
{
    return ServerExecutor::GetInstance();
}

void ReleaseEngineManager()
{
    return ServerExecutor::ReleaseInstance();
}
} // namespace AI
} // namespace OHOS
