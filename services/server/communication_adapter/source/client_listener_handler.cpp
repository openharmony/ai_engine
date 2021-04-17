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

#include "communication_adapter/include/client_listener_handler.h"

#include "liteipc_adapter.h"

#include "platform/os_wrapper/ipc/include/aie_ipc.h"
#include "protocol/ipc_interface/ai_service.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/aie_guard.h"
#include "utils/aie_macros.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
namespace {
const char * const ASYNC_PROCESS_WORKER = "AsyncProcessWorker";
const int EVENT_WAIT_TIME_MS = 1000;
} // anonymous namespace

AsyncProcessWorker::AsyncProcessWorker(ClientListenerHandler *handler, int clientId, SaServerAdapter *adapter)
    : handler_(handler), clientId_(clientId), adapter_(adapter)
{
}

const char *AsyncProcessWorker::GetName() const
{
    return ASYNC_PROCESS_WORKER;
}

void AsyncProcessWorker::IpcIoResponse(IResponse *response, IpcIo &io, char *data, int length)
{
    if (response == nullptr) {
        HILOGE("[ClientListenerHandler]Input param response is nullptr.");
        return;
    }
    IpcIoInit(&io, data, length, IPC_OBJECT_COUNTS);

    int retCode = response->GetRetCode();
    IpcIoPushInt32(&io, retCode);

    int requestId = response->GetRequestId();
    IpcIoPushInt32(&io, requestId);

    long long transactionId = response->GetTransactionId();
    int sessionId = adapter_->GetSessionId(transactionId);
    IpcIoPushInt32(&io, sessionId);

    DataInfo result = response->GetResult();
    ParcelDataInfo(&io, &result);
}

bool AsyncProcessWorker::OneAction()
{
    IResponse *response = handler_->FetchCallbackRecord();
    CHK_RET(response == nullptr, true);
    ResGuard<IResponse> guard(response);

    IpcIo io;
    char tmpData[IPC_IO_DATA_MAX];
    IpcIoResponse(response, io, tmpData, IPC_IO_DATA_MAX);
    response->Detach();

    SvcIdentity *svcIdentity = adapter_->GetEngineListener();
    if (svcIdentity == nullptr) {
        HILOGE("[ClientListenerHandler]Fail to get engine listener, clientId: %d.", clientId_);
        return true;
    }

    IpcIo reply;
    int32_t retCode = Transact(nullptr, *svcIdentity, ON_ASYNC_PROCESS_CODE, &io, &reply, LITEIPC_FLAG_ONEWAY, nullptr);
    if (retCode != LITEIPC_OK) {
        HILOGI("[ClientListenerHandler]End to deal response, ret is %d, clientId: %d.", retCode, clientId_);
    }
    return retCode == LITEIPC_OK;
}

bool AsyncProcessWorker::Initialize()
{
    return true;
}

void AsyncProcessWorker::Uninitialize()
{
}

ClientListenerHandler::ClientListenerHandler()
{
    event_ = IEvent::MakeShared();
}

ClientListenerHandler::~ClientListenerHandler()
{
    std::lock_guard<std::mutex> guard(mutex_);
    for (auto &response : responses_) {
        IResponse::Destroy(response);
    }
}

IResponse *ClientListenerHandler::FetchCallbackRecord()
{
    IResponse *response = nullptr;
    {
        std::lock_guard<std::mutex> guard(mutex_);
        if (!responses_.empty()) {
            response = responses_.front();
            responses_.pop_front();
            if (responses_.empty()) {
                event_->Reset();
            }
            return response;
        }
    }

    // Active the thread every 1s. complete blocking will prevent from joining thread.
    if ((event_->Wait(EVENT_WAIT_TIME_MS)) && (!responses_.empty())) {
        std::lock_guard<std::mutex> guard(mutex_);
        response = responses_.front();
        responses_.pop_front();
        if (responses_.empty()) { // if it's empty now, block thread.
            event_->Reset();
        }
    }
    return response;
}

void ClientListenerHandler::AddCallbackRecord(IResponse *response)
{
    std::lock_guard<std::mutex> guard(mutex_);
    bool empty = responses_.empty();
    responses_.push_back(response);
    CHK_RET_NONE(!empty);

    // If it was empty and new response is coming, stop blocking.
    event_->Signal();
}

int ClientListenerHandler::StartAsyncProcessThread(int clientId, SaServerAdapter *adapter)
{
    ThreadPool *threadPool = ThreadPool::GetInstance();
    CHK_RET(threadPool == nullptr, RETCODE_OUT_OF_MEMORY);

    CHK_RET(asyncProcessThread_ != nullptr, RETCODE_ASYNC_CB_STARTED);
    CHK_RET(asyncProcessWorker_ != nullptr, RETCODE_ASYNC_CB_STARTED);

    asyncProcessThread_ = threadPool->Pop();
    CHK_RET(asyncProcessThread_ == nullptr, RETCODE_OUT_OF_MEMORY);

    AIE_NEW(asyncProcessWorker_, AsyncProcessWorker(this, clientId, adapter));
    if (asyncProcessWorker_ == nullptr) {
        threadPool->Push(asyncProcessThread_);
        asyncProcessThread_ = nullptr;
        return RETCODE_OUT_OF_MEMORY;
    }

    bool startSuccess = asyncProcessThread_->StartThread(asyncProcessWorker_);
    if (!startSuccess) {
        threadPool->Push(asyncProcessThread_);
        asyncProcessThread_ = nullptr;
        AIE_DELETE(asyncProcessWorker_);
        return RETCODE_START_THREAD_FAILED;
    }

    HILOGI("[ClientListenerHandler]Async process thread run to succeed.");
    return RETCODE_SUCCESS;
}

void ClientListenerHandler::StopAsyncProcessThread()
{
    if (asyncProcessThread_ != nullptr) {
        asyncProcessThread_->StopThread();
        ThreadPool *threadPool = ThreadPool::GetInstance();
        CHK_RET_NONE(threadPool == nullptr);
        threadPool->Push(asyncProcessThread_);
        asyncProcessThread_ = nullptr;
    }
    AIE_DELETE(asyncProcessWorker_);
}
} // namespace AI
} // namespace OHOS
