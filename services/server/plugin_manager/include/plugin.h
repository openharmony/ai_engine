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

#ifndef PLUGIN_H
#define PLUGIN_H

#include <string>

#include "plugin/i_plugin.h"

namespace OHOS {
namespace AI {
class Plugin {
public:
    /**
     * Initialize plugin
     *
     * @param [in] aid Algorithm id
     * @param [in] version Algorithm version
     */
    Plugin(const std::string &aid, long long version);

    /**
     * Destructor of plugin
     */
    virtual ~Plugin();

    /**
     * Get algorithm of plugin
     *
     * @return Algorithm handler
     */
    IPlugin *GetPluginAlgorithm();

    /**
     * Set algorithm of plugin
     *
     * @param [in] pluginAlgorithm Algorithm handler
     */
    void SetPluginAlgorithm(IPlugin *pluginAlgorithm);

    /**
     * Load algorithm
     *
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int LoadPluginAlgorithm();

    /**
     * Unload algorithm
     */
    void UnloadPluginAlgorithm();

    /**
     * Get algorithm version
     *
     * @return Algorithm version
     */
    long long GetVersion() const;

    /**
     * Get algorithm id
     *
     * @return Algorithm id
     */
    std::string GetAid() const;

private:
    IPlugin *pluginAlgorithm_ {nullptr};
    std::string aid_ {""};
    long long version_ {0};
    void *handle_ {nullptr};
};
} // namespace AI
} // namespace OHOS

#endif // PLUGIN_H