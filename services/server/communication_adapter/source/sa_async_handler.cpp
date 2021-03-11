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

#include "communication_adapter/include/future_listener.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "server_executor/include/i_async_task_manager.h"
#include "utils/aie_guard.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
std::mutex SaAsyncHandler::mutex_;
SaAsyncHandler *SaAsyncHandler::instance_ = nullptr;

SaAsyncHandler *SaAsyncHandler::GetInstance()
{
    CHK_RET(instance_ != nullptr, instance_);

    std::lock_guard<std::mutex> lock(mutex_);
    CHK_RET(instance_ != nullptr, instance_);

    AIE_NEW(instance_, SaAsyncHandler);
    return instance_;
}

SaAsyncHandler::SaAsyncHandler() = default;

SaAsyncHandler::~SaAsyncHandler()
{
    for (auto &iter : clients_) {
        AIE_DELETE(iter.second);
    }
    clients_.clear();
}

void SaAsyncHandler::StopClientListenerHandler(int clientId)
{
    ReadGuard<RwLock> guard(rwLock_);
    auto iter = clients_.find(clientId);
    CHK_RET_NONE(iter == clients_.end());

    iter->second->StopAsyncProcessThread();
}

void SaAsyncHandler::RemoveClientListenerHandler(int clientId)
{
    WriteGuard<RwLock> guard(rwLock_);
    ClientListenerHandlerMap::iterator iter = clients_.find(clientId);
    CHK_RET_NONE(iter == clients_.end());

    AIE_DELETE(iter->second);
    clients_.erase(iter);
}

ClientListenerHandler *SaAsyncHandler::FindClientListenerHandler(int clientId)
{
    ReadGuard<RwLock> guard(rwLock_);
    ClientListenerHandlerMap::iterator iter = clients_.find(clientId);
    CHK_RET(iter == clients_.end(), nullptr);

    return iter->second;
}

ClientListenerHandler *SaAsyncHandler::AddClientListenerHandler(int clientId)
{
    ClientListenerHandler *handler = nullptr;
    AIE_NEW(handler, ClientListenerHandler);
    CHK_RET(handler == nullptr, nullptr);

    WriteGuard<RwLock> guard(rwLock_);
    clients_[clientId] = handler;
    return handler;
}

void SaAsyncHandler::RemoveTransaction(long long transactionId)
{
    WriteGuard<RwLock> guard(rwLock_);
    transactions_.erase(transactionId);
}

bool SaAsyncHandler::IsExistTransaction(long long transactionId)
{
    ReadGuard<RwLock> guard(rwLock_);
    auto iter = transactions_.find(transactionId);
    return (iter != transactions_.end());
}

void SaAsyncHandler::SaveTransaction(long long transactionId)
{
    WriteGuard<RwLock> guard(rwLock_);
    transactions_.insert(transactionId);
}

void SaAsyncHandler::PushAsyncResponse(int clientId, IResponse *response)
{
    ClientListenerHandler *client = FindClientListenerHandler(clientId);
    CHK_RET_NONE(client == nullptr);
    client->AddCallbackRecord(response);
}

int SaAsyncHandler::RegisterAsyncHandler(int clientId)
{
    if (FindClientListenerHandler(clientId) != nullptr) {
        HILOGI("[SaAsyncHandler]The client has already add handler, clientId: %d.", clientId);
        return RETCODE_SUCCESS;
    }

    AddClientListenerHandler(clientId);
    return RETCODE_SUCCESS;
}

int SaAsyncHandler::StartAsyncProcess(int clientId, SaServerAdapter *adapter)
{
    ReadGuard<RwLock> guard(rwLock_);
    ClientListenerHandlerMap::iterator iter = clients_.find(clientId);
    if (iter == clients_.end()) {
        HILOGE("[SaAsyncHandler]The client do not preRegister AsyncHandler, clientId: %d.", clientId);
        return RETCODE_SA_ASYNC_HANDLER_NOT_FOUND;
    }

    if (iter->second == nullptr) {
        HILOGE("[SaAsyncHandler]AsyncHandler is null, clientId: %d.", clientId);
        return RETCODE_SA_ASYNC_HANDLER_NOT_FOUND;
    }
    return iter->second->StartAsyncProcessThread(clientId, adapter);
}

void SaAsyncHandler::StopAsyncProcess(int clientId)
{
    StopClientListenerHandler(clientId);
    RemoveClientListenerHandler(clientId);
}

int SaAsyncHandler::StartAsyncTransaction(long long transactionId, int clientId)
{
    CHK_RET(IsExistTransaction(transactionId), RETCODE_SUCCESS);
    IFutureListener *listener = nullptr;
    AIE_NEW(listener, FutureListener(this, clientId));
    if (listener == nullptr) {
        HILOGE("[SaAsyncHandler]Allocate future listener failed.");
        return RETCODE_OUT_OF_MEMORY;
    }
    IAsyncTaskManager *taskManager = GetAsyncTaskManager();
    if (taskManager == nullptr) {
        HILOGE("[SaAsyncHandler]Failed to get async task manager.");
        AIE_DELETE(listener);
        return RETCODE_OUT_OF_MEMORY;
    }
    taskManager->RegisterListener(listener, transactionId);
    SaveTransaction(transactionId);
    return RETCODE_SUCCESS;
}

void SaAsyncHandler::StopAsyncTransaction(long long transactionId)
{
    CHK_RET_NONE(!IsExistTransaction(transactionId));

    IAsyncTaskManager *taskManager = GetAsyncTaskManager();
    if (taskManager == nullptr) {
        HILOGE("[SaAsyncHandler]Failed to get async task manager, transactionId:0x%llx.", transactionId);
        return;
    }
    taskManager->UnRegisterListener(transactionId);
    RemoveTransaction(transactionId);
}
} // namespace AI
} // namespace OHOS