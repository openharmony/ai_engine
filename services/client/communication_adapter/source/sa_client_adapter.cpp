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

#include "communication_adapter/include/sa_client_adapter.h"

#include "communication_adapter/include/sa_async_handler.h"
#include "communication_adapter/include/sa_client.h"
#include "platform/time/include/time.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
namespace {
    const char * const CONNECT_MANAGER_WORKER_NAME = "ConnectMgrWorker";
    const int CLIENT_RECONNECTION_INTERVAL = 10;
}

ConnectMgrWorker::ConnectMgrWorker(const ConfigInfo &configInfo, ClientInfo &clientInfo)
    : configInfo_(configInfo), clientInfo_(clientInfo)
{
}

const char *ConnectMgrWorker::GetName() const
{
    return CONNECT_MANAGER_WORKER_NAME;
}

bool ConnectMgrWorker::OneAction()
{
    SaClientAdapter *clientAdapter = SaClientAdapter::GetInstance();
    if (clientAdapter == nullptr) {
        HILOGE("[SaClientAdapter][ThreadName:%s, ThreadId:0x%lx]Fail to SaClientAdapter::GetInstance",
            GetName(), GetThreadId());
        return false;
    }
    if (clientAdapter->GetClientId() != INVALID_CLIENT_ID) {
        StepSleepMs(CLIENT_RECONNECTION_INTERVAL);
        return true;
    }

    SaClient *client = SaClient::GetInstance();
    if (client == nullptr) {
        HILOGE("[SaClientAdapter][ThreadName:%s, ThreadId:0x%lx]Fail to SaClient::GetInstance",
            GetName(), GetThreadId());
        return false;
    }

    int retCode = client->Init(configInfo_, clientInfo_);
    if (retCode != RETCODE_SUCCESS || clientInfo_.clientId == INVALID_CLIENT_ID) {
        HILOGE("[SaClientAdapter][ThreadName:%s, ThreadId:0x%lx]Fail to Init to server. errorCode:%d",
            GetName(), GetThreadId(), retCode);
        StepSleepMs(CLIENT_RECONNECTION_INTERVAL);
        return true;
    }
    clientAdapter->SetClientId(clientInfo_.clientId);
    return true;
}

bool ConnectMgrWorker::Initialize()
{
    return true;
}

void ConnectMgrWorker::Uninitialize()
{
    SaClient *client = SaClient::GetInstance();
    if (client == nullptr) {
        HILOGE("[SaClientAdapter][ThreadName:%s, ThreadId:0x%lx]UnInitialize, fail to SaClient::GetInstance.",
            GetName(), GetThreadId());
        return;
    }

    SaClientAdapter *clientAdapter = SaClientAdapter::GetInstance();
    if (clientAdapter == nullptr) {
        HILOGE("[SaClientAdapter][ThreadName:%s, ThreadId:0x%lx]UnInitialize fail to GetInstance",
            GetName(), GetThreadId());
        return;
    }
    int retCode = client->Destroy(clientInfo_);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[SaClientAdapter][ThreadName:%s, ThreadId:0x%lx]UnInitialize, fail to Destroy.",
            GetName(), GetThreadId());
    }
    clientAdapter->SetClientId(INVALID_CLIENT_ID);
}

std::mutex SaClientAdapter::instance_mutex_;
SaClientAdapter *SaClientAdapter::instance_ = nullptr;

SaClientAdapter::SaClientAdapter() = default;

SaClientAdapter::~SaClientAdapter() = default;

SaClientAdapter *SaClientAdapter::GetInstance()
{
    CHK_RET(instance_ != nullptr, instance_);
    std::lock_guard<std::mutex> lock(instance_mutex_);
    CHK_RET(instance_ != nullptr, instance_);
    AIE_NEW(instance_, SaClientAdapter);
    return instance_;
}

void SaClientAdapter::ReleaseInstance()
{
    std::lock_guard<std::mutex> lock(instance_mutex_);
    AIE_DELETE(instance_);
}

int SaClientAdapter::InitAiServer(const ConfigInfo &configInfo, ClientInfo &clientInfo,
    const AlgorithmInfo &algorithmInfo)
{
    HILOGI("[SaClientAdapter]Begin to call InitAiServer.");
    ThreadPool *threadPool = ThreadPool::GetInstance();
    CHK_RET(threadPool == nullptr, RETCODE_OUT_OF_MEMORY);

    connectMgrThread_ = threadPool->Pop();
    CHK_RET(connectMgrThread_ == nullptr, RETCODE_OUT_OF_MEMORY);

    AIE_NEW(connectMgrWorker_, ConnectMgrWorker(configInfo, clientInfo));
    if (connectMgrWorker_ == nullptr) {
        threadPool->Push(connectMgrThread_);
        connectMgrThread_ = nullptr;
        return RETCODE_OUT_OF_MEMORY;
    }

    bool startSuccess = connectMgrThread_->StartThread(connectMgrWorker_);
    if (!startSuccess) {
        HILOGE("[SaClientAdapter]Fail to start thread.");
        threadPool->Push(connectMgrThread_);
        connectMgrThread_ = nullptr;
        AIE_DELETE(connectMgrWorker_);
        return RETCODE_START_THREAD_FAILED;
    }
    return RETCODE_SUCCESS;
}

int SaClientAdapter::CloseAiServer()
{
    HILOGI("[SaClientAdapter]Begin to call CloseAiServer.");
    if (connectMgrThread_ == nullptr) {
        HILOGW("[SaClientAdapter]Server has already been stopped, there is no need to close.");
        return RETCODE_FAILURE;
    }
    connectMgrThread_->StopThread();
    ThreadPool *threadPool = ThreadPool::GetInstance();
    CHK_RET(threadPool == nullptr, RETCODE_OUT_OF_MEMORY);
    threadPool->Push(connectMgrThread_);
    AIE_DELETE(connectMgrWorker_);
    connectMgrThread_ = nullptr;
    return RETCODE_SUCCESS;
}

int SaClientAdapter::LoadAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    HILOGI("[SaClientAdapter]Begin to call LoadAlgorithm.");
    SaClient *saClient = SaClient::GetInstance();
    CHK_RET(saClient == nullptr, RETCODE_NULL_PARAM);
    SaAsyncHandler *saAsyncHandler = SaAsyncHandler::GetInstance();
    CHK_RET(saAsyncHandler == nullptr, RETCODE_OUT_OF_MEMORY);
    if (algorithmInfo.isAsync && saAsyncHandler->GetAsyncCbSize() == 1) {
        int retCode = saClient->RegisterCallback(clientInfo);
        if (retCode != RETCODE_SUCCESS) {
            HILOGE("[SaClientAdapter]Fail to RegisterServerCb. errCode:%d", retCode);
            return RETCODE_FAILURE;
        }
    }
    return saClient->LoadAlgorithm(clientInfo, algorithmInfo, inputInfo, outputInfo);
}

int SaClientAdapter::UnLoadAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo)
{
    HILOGI("[SaClientAdapter]Begin to call UnLoadAlgorithm.");
    SaClient *client = SaClient::GetInstance();
    CHK_RET(client == nullptr, RETCODE_NULL_PARAM);
    SaAsyncHandler *saAsyncHandler = SaAsyncHandler::GetInstance();
    CHK_RET(saAsyncHandler == nullptr, RETCODE_NULL_PARAM);
    if (algorithmInfo.isAsync && saAsyncHandler->IsCallbackEmpty()) {
        int retCode = client->UnregisterCallback(clientInfo);
        if (retCode != RETCODE_SUCCESS) {
            HILOGE("[SaClientAdapter]Fail to RegisterServerCb. errCode:%d", retCode);
            return RETCODE_FAILURE;
        }
    }

    return client->UnloadAlgorithm(clientInfo, algorithmInfo, inputInfo);
}

int SaClientAdapter::SyncExecute(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    HILOGI("[SaClientAdapter]Begin to call SyncExecute.");
    SaClient *client = SaClient::GetInstance();
    CHK_RET(client == nullptr, RETCODE_NULL_PARAM);

    return client->SyncExecuteAlgorithm(clientInfo, algorithmInfo, inputInfo, outputInfo);
}

int SaClientAdapter::AsyncExecute(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo)
{
    HILOGI("[SaClientAdapter]Begin to call AsyncExecute.");
    SaClient *saClient = SaClient::GetInstance();
    CHK_RET(saClient == nullptr, RETCODE_NULL_PARAM);
    return saClient->AsyncExecuteAlgorithm(clientInfo, algorithmInfo, inputInfo);
}

int SaClientAdapter::SetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo)
{
    HILOGI("[SaClientAdapter]Begin to call SetOption.");
    SaClient *saClient = SaClient::GetInstance();
    CHK_RET(saClient == nullptr, RETCODE_NULL_PARAM);
    return saClient->SetOption(clientInfo, optionType, inputInfo);
}

int SaClientAdapter::GetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo,
    DataInfo &outputInfo)
{
    HILOGI("[SaClientAdapter]Begin to call GetOption.");
    SaClient *saClient = SaClient::GetInstance();
    CHK_RET(saClient == nullptr, RETCODE_NULL_PARAM);
    return saClient->GetOption(clientInfo, optionType, inputInfo, outputInfo);
}

ClientFactory *GetClient()
{
    return SaClientAdapter::GetInstance();
}
} // namespace AI
} // namespace OHOS