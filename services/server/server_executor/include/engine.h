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

#ifndef ENGINE_H
#define ENGINE_H

#include <memory>

#include "platform/queuepool/queue.h"
#include "plugin_manager/include/i_plugin_manager.h"
#include "plugin_manager/include/plugin.h"
#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"
#include "server_executor/include/engine_worker.h"
#include "server_executor/include/future.h"
#include "server_executor/include/i_handler.h"

namespace OHOS {
namespace AI {
const int SYNC_MSG_TIMEOUT = -1;
const char * const PLUGIN_SYNC_INFER = "SYNC";

class Engine {
public:
    Engine(std::shared_ptr<Plugin> &plugin, std::shared_ptr<Thread> &thread, std::shared_ptr<Queue<Task>> &queue);
    ~Engine();

    /**
     * Initialize the engine, which is called when the engine is created.
     *
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int Initialize();

    std::shared_ptr<Plugin> GetPlugin() const;

    /**
     * Get the number of references for the engine.
     *
     * @return Number of engine references.
     */
    int GetEngineReference() const;

    /**
     * Engine reference count increased by 1, which is called when the engine starts.
     */
    void AddEngineReference();

    /**
     * The engine reference count is reduced by 1, which is called when the engine stops.
     */
    void DelEngineReference();

    /**
     * Algorithmic execution interface for synchronous tasks.
     *
     * @param [in] request Request information of synchronous task.
     * @param [out] response Response of synchronous task.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int SyncExecute(IRequest *request, IResponse *&response);

    /**
     * Algorithmic execution interface for asynchronous tasks.
     *
     * @param [in] request Request information of asynchronous task.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int AsyncExecute(IRequest *request);

private:
    void Uninitialize();

private:
    std::atomic<int> refCount_;
    std::shared_ptr<Plugin> plugin_;
    std::shared_ptr<Thread> thread_;
    std::shared_ptr<Queue<Task>> queue_;
    IHandler *msgHandler_;
    EngineWorker worker_;
};
} // namespace AI
} // namespace OHOS

#endif // ENGINE_H