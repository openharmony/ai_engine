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

#ifndef ASYNC_MSG_HANDLER_H
#define ASYNC_MSG_HANDLER_H

#include "platform/queuepool/queue.h"
#include "plugin/i_plugin.h"
#include "plugin/i_plugin_callback.h"
#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"
#include "server_executor/include/future.h"
#include "server_executor/include/i_handler.h"
#include "server_executor/include/task.h"

namespace OHOS {
namespace AI {
class AsyncMsgHandler : public IHandler, public IPluginCallback {
public:
    AsyncMsgHandler(Queue<Task> &queue, IPlugin *pluginAlgorithm);
    ~AsyncMsgHandler() override = default;

    /**
     * Asynchronous task processing.
     *
     * @param [in] task Asynchronous task.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int Process(const Task &task) override;

    /**
     * Called when asynchronous task processing is completed.
     *
     * @param [in] event The event type of the asynchronous task processing result.
     * @param [in] response Response of asynchronous task processing.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int OnEvent(PluginEvent event, IResponse *response) override;

    /**
     * Set plugin algorithm for {@code AsyncMsgHandler}.
     *
     * @param [in] pluginAlgorithm Function symbol corresponding to plugin algorithm.
     */
    void SetPluginAlgorithm(IPlugin *pluginAlgorithm) override;

    /**
     * Encapsulates the request as a task and puts it in the task queue.
     *
     * @param [in] request Request information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int SendRequest(IRequest *request);

private:
    Queue<Task> &queue_;
    IPlugin *pluginAlgorithm_;
};
} // namespace AI
} // namespace OHOS

#endif // ASYNC_MSG_HANDLER_H