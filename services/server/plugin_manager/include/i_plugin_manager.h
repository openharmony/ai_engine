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

#ifndef I_PLUGIN_MANAGER_H
#define I_PLUGIN_MANAGER_H

#include <memory>
#include <string>

#include "plugin_manager/include/plugin.h"

namespace OHOS {
namespace AI {
class IPluginManager {
public:
    /**
     * Create plugin manager.
     *
     * @return plugin manager
     */
    static IPluginManager *GetPluginManager();

    /**
     * Destroy plugin.
     */
    virtual void Destroy() = 0;

    /**
     * Get plugin.
     *
     * @param [in] aid Algorithm id
     * @param [in] version Algorithm version
     * @param [out] plugin Plugin
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int GetPlugin(const std::string &aid, long long version, std::shared_ptr<Plugin> &plugin) = 0;

    /**
     * Unload plugin.
     *
     * @param [in] aid Algorithm id
     * @param [in] version Algorithm version
     */
    virtual void UnloadPlugin(const std::string &aid, long long version) = 0;

protected:
    virtual ~IPluginManager() = default;
};
} // namespace AI
} // namespace OHOS
#endif // I_PLUGIN_MANAGER_H