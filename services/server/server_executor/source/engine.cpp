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

#include "server_executor/include/engine.h"

#include <cstring>

#include "platform/time/include/time.h"
#include "plugin_manager/include/aie_plugin_info.h"
#include "server_executor/include/async_msg_handler.h"
#include "server_executor/include/sync_msg_handler.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
Engine::Engine(std::shared_ptr<Plugin> &plugin, std::shared_ptr<Thread> &thread,
    std::shared_ptr<Queue<Task>> &queue)
    : refCount_(0),
      plugin_(plugin),
      thread_(thread),
      queue_(queue),
      msgHandler_(nullptr),
      worker_(*queue)
{
}

Engine::~Engine()
{
    Uninitialize();
}

static bool IsSyncMode(const std::shared_ptr<Plugin> &plugin)
{
    const char *inferMode = plugin->GetPluginAlgorithm()->GetInferMode();
    if (inferMode == nullptr) {
        return false;
    }
    return (strcmp(PLUGIN_SYNC_INFER, inferMode) == 0);
}

std::shared_ptr<Plugin> Engine::GetPlugin() const
{
    return plugin_;
}

int Engine::GetEngineReference() const
{
    return refCount_;
}

void Engine::AddEngineReference()
{
    ++refCount_;
}

void Engine::DelEngineReference()
{
    refCount_--;
}

int Engine::Initialize()
{
    if (plugin_->GetPluginAlgorithm() == nullptr) {
        return RETCODE_NULL_PARAM;
    }
    if (IsSyncMode(plugin_)) {
        AIE_NEW(msgHandler_, SyncMsgHandler(*queue_, plugin_->GetPluginAlgorithm()));
    } else {
        AIE_NEW(msgHandler_, AsyncMsgHandler(*queue_, plugin_->GetPluginAlgorithm()));
    }
    if (msgHandler_ == nullptr) {
        HILOGE("[Engine]Allocate massage handler failed, algoType is [%lld], algoName is [%s].",
            plugin_->GetVersion(), plugin_->GetAid().c_str());
        return RETCODE_OUT_OF_MEMORY;
    }

    bool isStarted = thread_->StartThread(&worker_);
    if (!isStarted) {
        AIE_DELETE(msgHandler_);
        HILOGE("[Engine]Engine(aid is [%s], version is [%lld]) start thread failed.",
            plugin_->GetAid().c_str(), plugin_->GetVersion());
        return RETCODE_START_THREAD_FAILED;
    }
    return RETCODE_SUCCESS;
}

void Engine::Uninitialize()
{
    if (thread_) {
        thread_->StopThread();
        ThreadPool *threadPool = ThreadPool::GetInstance();
        CHK_RET_NONE(threadPool == nullptr);
        threadPool->Push(thread_);
        thread_ = nullptr;
    }

    if (queue_ != nullptr) {
        QueuePool<Task> *queuePool = QueuePool<Task>::GetInstance();
        if (queuePool == nullptr) {
            HILOGE("[Engine]Allocate queue pool failed");
            return;
        }
        queuePool->Push(queue_);
    }

    AIE_DELETE(msgHandler_);
}

int Engine::SyncExecute(IRequest *request, IResponse *&response)
{
    if (plugin_ == nullptr) {
        HILOGE("[Engine]The plugin_ is null.");
        return RETCODE_PLUGIN_LOAD_FAILED;
    }

    SyncMsgHandler *handler = reinterpret_cast<SyncMsgHandler *>(msgHandler_);
    if (handler == nullptr) {
        HILOGE("[Engine]MsgHandler is null, synchronous execution is not supported.");
        return RETCODE_NULL_PARAM;
    }

    if (request == nullptr) {
        HILOGE("[Engine]The request is null.");
        return RETCODE_NULL_PARAM;
    }

    SimpleEventNotifier<IResponse> notifier(IResponse::Destroy);
    int sendRequestRet = handler->SendRequest(request, notifier);
    if (sendRequestRet != RETCODE_SUCCESS) {
        HILOGE("[Engine][transactionId:%lld]Send sync request failed, retCode is [%d].",
            request->GetTransactionId(), sendRequestRet);
        return sendRequestRet;
    }

    int recvResponseRet = handler->ReceiveResponse(SYNC_MSG_TIMEOUT, notifier, response);
    if (recvResponseRet != RETCODE_SUCCESS) {
        HILOGE("[Engine]Receive response failed, retCode is [%d].", recvResponseRet);
        return recvResponseRet;
    }
    return response->GetRetCode();
}

int Engine::AsyncExecute(IRequest *request)
{
    if (plugin_ == nullptr) {
        HILOGE("[Engine]The plugin_ is null.");
        return RETCODE_PLUGIN_LOAD_FAILED;
    }

    AsyncMsgHandler *handler = reinterpret_cast<AsyncMsgHandler *>(msgHandler_);
    if (handler == nullptr) {
        HILOGE("[Engine]MsgHandler is null, asynchronous execution is not supported.");
        return RETCODE_NULL_PARAM;
    }

    return handler->SendRequest(request);
}
} // namespace AI
} // namespace OHOS