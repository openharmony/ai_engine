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

#ifndef SYNC_MSG_HANDLER_H
#define SYNC_MSG_HANDLER_H

#include "platform/queuepool/queue_pool.h"
#include "platform/semaphore/include/simple_event_notifier.h"
#include "plugin/i_plugin.h"
#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "server_executor/include/i_handler.h"
#include "server_executor/include/task.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class SyncMsgHandler : public IHandler {
public:
    SyncMsgHandler(Queue<Task> &queue, IPlugin *pluginAlgorithm);

    ~SyncMsgHandler() override = default;

    /**
     * Deal with sync task.
     *
     * @param [in] task Task info need to be processed.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int Process(const Task &task) override;

    /**
     * Set pluginAlgorithm.
     *
     * @param [in] pluginAlgorithm Algorithm function symbol of dynamic library.
     */
    void SetPluginAlgorithm(IPlugin *pluginAlgorithm) override;

    /**
     * Add request to the end of processing queue.
     *
     * @param [in] request info needed for algorithm.
     * @param [in, out] notifier Semaphore.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int SendRequest(IRequest *request, SimpleEventNotifier<IResponse> &notifier);

    /**
     * Receive response.
     *
     * @param [in] timeOut Delayed time, measured by millisecond.
     * @param [in, out] notifier Semaphore.
     * @param [out] response Response info.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int ReceiveResponse(int timeOut, SimpleEventNotifier<IResponse> &notifier, IResponse *&response);

private:
    Queue<Task> &queue_;
    IPlugin *pluginAlgorithm_;
};
} // namespace AI
} // namespace OHOS

#endif // SYNC_MSG_HANDLER_H