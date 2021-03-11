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

#ifndef I_HANDLER_H
#define I_HANDLER_H

#include "plugin/i_plugin.h"
#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"
#include "server_executor/include/task.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class IHandler {
public:
    virtual ~IHandler() = default;

    /**
     * Interface to process task, override by sync and async message handler.
     *
     * @param [in] task Task info need to be processed.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int Process(const Task &task) = 0;

    /**
     * Set plugin algorithm, override by sync and async message handler.
     *
     * @param [in] pluginAlgorithm Algorithm handler.
     */
    virtual void SetPluginAlgorithm(IPlugin *pluginAlgorithm) = 0;
};
} // namespace AI
} // namespace OHOS

#endif // IHANDLER_H