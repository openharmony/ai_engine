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

#ifndef ENGINE_WORKER_H
#define ENGINE_WORKER_H

#include "protocol/retcode_inner/aie_retcode_inner.h"

#include "platform/queuepool/queue.h"
#include "platform/queuepool/queue_pool.h"
#include "platform/threadpool/include/thread.h"
#include "platform/threadpool/include/thread_pool.h"
#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"
#include "server_executor/include/task.h"

namespace OHOS {
namespace AI {
class EngineWorker : public IWorker {
public:
    explicit EngineWorker(Queue<Task> &queue);
    ~EngineWorker() override = default;

    /**
     * Get worker name, and cannot return null.
     *
     * @return Worker name.
     */
    const char *GetName() const override;

    /**
     * Thread will call OneAction loop, until the thread is stopped or return false.
     *
     * @return true thread is running, false thread is stop.
     */
    bool OneAction() override;

    /**
     * The method is called when thread stop, it is still run in thread.
     */
    void Uninitialize() override;

private:
    Queue<Task> &queue_;
};
} // namespace AI
} // namespace OHOS

#endif // ENGINE_WORKER_H