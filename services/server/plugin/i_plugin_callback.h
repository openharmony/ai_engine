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

#ifndef I_PLUGIN_CALLBACK_H
#define I_PLUGIN_CALLBACK_H

#include "protocol/retcode_inner/aie_retcode_inner.h"

namespace OHOS {
namespace AI {
enum PluginEvent {
    ON_PLUGIN_SUCCEED,
    ON_PLUGIN_FAIL,
};

class IResponse;

class IPluginCallback {
public:
    virtual ~IPluginCallback() = default;

    /**
     * Called when asynchronous inference is complete.
     *
     * @param [in] event The type of callback event.
     * @param [in] response Results of encapsulated algorithmic inference.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int OnEvent(PluginEvent event, IResponse *response) = 0;
};
} // namespace AI
} // namespace OHOS

#endif // I_PLUGIN_CALLBACK_H