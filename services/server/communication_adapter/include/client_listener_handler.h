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

#ifndef CLIENT_LISTENER_HANDLER_H
#define CLIENT_LISTENER_HANDLER_H

#include <list>
#include <mutex>

#include "communication_adapter/include/sa_server_adapter.h"
#include "platform/event/include/i_event.h"
#include "platform/threadpool/include/thread_pool.h"
#include "protocol/data_channel/include/i_response.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"

namespace OHOS {
namespace AI {
const char * const ASYNC_PROCESS_WORKER = "AsyncProcessWorker";
const int EVENT_WAIT_TIME_MS = 1000;

class ClientListenerHandler;

/**
 * Thread class for listening async process result
 */
class AsyncProcessWorker : public IWorker {
public:
    AsyncProcessWorker(ClientListenerHandler *handler, int clientId, SaServerAdapter *adapter);
    ~AsyncProcessWorker() override = default;
    const char *GetName() const override;
    bool OneAction() override;
    bool Initialize() override;
    void Uninitialize() override;

private:
    void IpcIoResponse(IResponse *response, IpcIo &io, char *data, int length);

private:
    ClientListenerHandler *handler_;
    int clientId_;
    SaServerAdapter *adapter_;
};

class ClientListenerHandler {
public:
    ClientListenerHandler();
    ~ClientListenerHandler();
    IResponse *FetchCallbackRecord();

    /**
     * Add response to record callback.
     *
     * @param [in] response Certain object to record callback.
     */
    void AddCallbackRecord(IResponse *response);

    /**
     * Start the thread to listen the result of async process.
     *
     * @param [in] clientId Client identity.
     * @param [in] adapter Client adapter.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int StartAsyncProcessThread(int clientId, SaServerAdapter *adapter);

    /**
     * Stop the thread to listen the result of async process.
     */
    void StopAsyncProcessThread();

private:
    std::mutex mutex_;
    std::shared_ptr<IEvent> event_;
    using ResponseList = std::list<IResponse*>;
    ResponseList responses_;
    std::shared_ptr<Thread> asyncProcessThread_ = nullptr;
    AsyncProcessWorker *asyncProcessWorker_ = nullptr;
};
} // namespace AI
} // namespace OHOS

#endif // CLIENT_LISTENER_HANDLER_H