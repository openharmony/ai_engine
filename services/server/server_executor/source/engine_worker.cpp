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

#include "server_executor/include/engine_worker.h"

#include "platform/time/include/time.h"
#include "server_executor/include/i_handler.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
namespace {
const char * const ENGINE_WORKER_NAME = "EngineWorker";
}

EngineWorker::EngineWorker(Queue<Task> &queue) : queue_(queue)
{
}

static void ClearQueue(Queue<Task> &queue)
{
    size_t size = queue.Count();
    for (size_t i = 0; i < size; ++i) {
        Task task;
        int retCode = queue.PopFront(task);
        if (retCode != RETCODE_SUCCESS) {
            break;
        }
        IRequest::Destroy(task.request);
    }
}

const char *EngineWorker::GetName() const
{
    return ENGINE_WORKER_NAME;
}

bool EngineWorker::OneAction()
{
    if (queue_.IsEmpty()) {
        StepSleepMs(1);
        CHK_RET(queue_.IsEmpty(), true);
    }

    Task task;
    int retCode = queue_.PopFront(task);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[EngineWorker]Fetch task from queue failed. error code is [%d].", retCode);
        return true;
    }

    if (task.handler == nullptr) {
        HILOGE("[EngineWorker]The handler is null.");
        return true;
    }

    retCode = task.handler->Process(task);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[EngineWorker]Failed to process task.");
    }
    return true;
}

void EngineWorker::Uninitialize()
{
    ClearQueue(queue_);
}
} // namespace AI
} // namespace OHOS