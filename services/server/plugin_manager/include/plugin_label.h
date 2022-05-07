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

#ifndef PLUGIN_LABEL_H
#define PLUGIN_LABEL_H

#include <mutex>
#include <string>

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class PluginLabel {
    FORBID_COPY_AND_ASSIGN(PluginLabel);
    FORBID_CREATE_BY_SELF(PluginLabel);
public:
    /**
     * Get plugin label which combined algorithm id and algorithm version.
     *
     * @return plugin label
     */
    static PluginLabel *GetInstance();

    /**
     * Release the singleton instance.
     */
    static void ReleaseInstance();

    /**
     * Get algorithm path
     *
     * @param [in] aid Algorithm id
     * @param [in] version Algorithm version
     * @param [out] libPath Algorithm path
     * @return Algorithm path
     */
    int GetLibPath(const std::string &aid, long long &version, std::string &libPath);

private:
    static std::mutex instanceLock_;
    static PluginLabel *instance_;
};
} // namespace AI
} // namespace OHOS

#endif // PLUGIN_LABEL_H
