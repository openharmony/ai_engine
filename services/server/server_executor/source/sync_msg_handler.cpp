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

#include "server_executor/include/sync_msg_handler.h"

#include "platform/queuepool/queue_pool.h"
#include "platform/semaphore/include/simple_event_notifier.h"
#include "plugin/i_plugin.h"
#include "server_executor/include/engine_manager.h"
#include "utils/aie_guard.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
SyncMsgHandler::SyncMsgHandler(Queue<Task> &queue, IPlugin *pluginAlgorithm)
    : queue_(queue), pluginAlgorithm_(pluginAlgorithm)
{
}

int SyncMsgHandler::Process(const Task &task)
{
    CHK_RET(pluginAlgorithm_ == nullptr, RETCODE_PLUGIN_LOAD_FAILED);

    IRequest *request = task.request;
    if (request == nullptr) {
        HILOGE("[SyncMsgHandler]Invalid request param");
        return RETCODE_NULL_PARAM;
    }

    IResponse *response = nullptr;
    int processRetCode = pluginAlgorithm_->SyncProcess(request, response);

    if (response == nullptr) {
        response = IResponse::Create(task.request);
        CHK_RET(response == nullptr, RETCODE_OUT_OF_MEMORY);
    }

    if (processRetCode != RETCODE_SUCCESS) {
        response->SetRetCode(RETCODE_ALGORITHM_PROCESS_ERROR);
    } else {
        response->SetRetCode(RETCODE_SUCCESS);
    }

    if (task.notifier != nullptr) {
        (task.notifier)->AddToBack(response);
    } else {
        IResponse::Destroy(response);
    }

    return processRetCode;
}

void SyncMsgHandler::SetPluginAlgorithm(IPlugin *pluginAlgorithm)
{
    pluginAlgorithm_ = pluginAlgorithm;
}

size_t GetSynMsgQueueCapacity()
{
    return MAX_SYNC_MSG_NUM;
}

int SyncMsgHandler::SendRequest(IRequest *request, SimpleEventNotifier<IResponse> &notifier)
{
    if (request == nullptr) {
        HILOGE("[SyncMsgHandler]Invalid request param");
        return RETCODE_NULL_PARAM;
    }

    if (queue_.Count() >= GetSynMsgQueueCapacity()) {
        HILOGE("[SyncMsgHandler]Queue overload");
        return RETCODE_QUEUE_FULL;
    }

    Task task(this, request, &notifier);
    int retCode = queue_.PushBack(task);
    if (retCode != RETCODE_SUCCESS) {
        HILOGI("[SyncMsgHandler]Push sync msg result is %d.", retCode);
    }
    return retCode;
}

int SyncMsgHandler::ReceiveResponse(int timeOut, SimpleEventNotifier<IResponse> &notifier,
    IResponse *&response)
{
    bool notifierCode = notifier.GetFromFront(timeOut, response);
    if (!notifierCode) {
        HILOGI("[SyncMsgHandler]Sync ReceiveResponse timeout.");
        return RETCODE_SYNC_MSG_TIMEOUT;
    }
    return RETCODE_SUCCESS;
}
} // namespace AI
} // namespace OHOS