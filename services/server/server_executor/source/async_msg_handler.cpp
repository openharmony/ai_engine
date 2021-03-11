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

#include "server_executor/include/async_msg_handler.h"

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "server_executor/include/engine_manager.h"
#include "server_executor/include/future_factory.h"
#include "utils/aie_guard.h"
#include "utils/aie_macros.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
AsyncMsgHandler::AsyncMsgHandler(Queue<Task> &g_queue, IPlugin *pluginAlgorithm)
    : queue_(g_queue), pluginAlgorithm_(pluginAlgorithm)
{
}

int AsyncMsgHandler::Process(const Task &task)
{
    if (pluginAlgorithm_ == nullptr) {
        HILOGI("[AsyncMsgHandler]The pluginAlgorithm_ is null.");
        return RETCODE_PLUGIN_LOAD_FAILED;
    }

    IRequest *request = task.request;
    if (request == nullptr) {
        HILOGE("[AsyncMsgHandler]Invalid request param");
        return RETCODE_NULL_PARAM;
    }

    ResGuard<IRequest> guardReq(request);
    return pluginAlgorithm_->AsyncProcess(request, this);
}

int AsyncMsgHandler::OnEvent(PluginEvent event, IResponse *response)
{
    if (response == nullptr) {
        HILOGE("[AsyncMsgHandler]Receive Event[%d], but response is nullptr", event);
        return RETCODE_NULL_PARAM;
    }
    FutureFactory *futureFactory = FutureFactory::GetInstance();
    CHK_RET(futureFactory == nullptr, RETCODE_NULL_PARAM);
    return futureFactory->ProcessResponse(event, response);
}

void AsyncMsgHandler::SetPluginAlgorithm(IPlugin *pluginAlgorithm)
{
    pluginAlgorithm_ = pluginAlgorithm;
}

size_t GetAsyncMsgQueueCapacity()
{
    return MAX_ASYNC_MSG_NUM;
}

int AsyncMsgHandler::SendRequest(IRequest *request)
{
    if (request == nullptr) {
        HILOGE("[AsyncMsgHandler]Invalid request param");
        return RETCODE_NULL_PARAM;
    }

    if (queue_.Count() >= GetAsyncMsgQueueCapacity()) {
        HILOGE("[AsyncMsgHandler]Task queue overload");
        return RETCODE_QUEUE_FULL;
    }

    FutureFactory *futureFactory = FutureFactory::GetInstance();
    CHK_RET(futureFactory == nullptr, RETCODE_NULL_PARAM);

    int retCode = futureFactory->CreateFuture(request);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[AsyncMsgHandler]Create future for async msg failed");
        return retCode;
    }

    Task task(this, request, nullptr);
    return queue_.PushBack(task);
}
} // namespace AI
} // namespace OHOS