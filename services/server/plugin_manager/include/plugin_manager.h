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

#ifndef PLUGIN_MANAGER_H
#define PLUGIN_MANAGER_H

#include <map>
#include <memory>
#include <mutex>

#include "plugin/i_plugin.h"
#include "plugin_manager/include/i_plugin_manager.h"
#include "plugin_manager/include/plugin.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class IPlugin;

struct PluginKey {
    std::string aid;
    long long version;

    PluginKey(const std::string &aid, long long version)
        : aid(aid), version(version)
    {
    }

    inline bool operator< (const PluginKey& another) const
    {
        if (aid < another.aid) {
            return true;
        }
        if (aid == another.aid && version < another.version) {
            return true;
        }

        return false;
    }
};

typedef std::map<PluginKey, std::shared_ptr<Plugin>> PluginMap;

class PluginManager : public IPluginManager {
    FORBID_COPY_AND_ASSIGN(PluginManager);
    FORBID_CREATE_BY_SELF(PluginManager);
public:
    static PluginManager *GetInstance();
    void Destroy() override;
    int GetPlugin(const std::string &aid, long long version, std::shared_ptr<Plugin> &plugin) override;
    void UnloadPlugin(const std::string &aid, long long version) override;

private:
    std::shared_ptr<Plugin> FindPlugin(const PluginKey &pluginKey);
    void AddPlugin(const PluginKey &pluginKey, const std::shared_ptr<Plugin> &pluginValue);
    void DelPlugin(const PluginKey &pluginKey);
    void DelPluginByAID(const std::string &aid);
    int LoadPlugin(const std::string &aid, long long version, std::shared_ptr<Plugin> &plugin);

private:
    static std::mutex instanceLock_;
    static PluginManager *instance_;

private:
    std::mutex mutex4Interface_;
    std::mutex mutex_;
    PluginMap pluginMap_;
};
} // namespace AI
} // namespace OHOS

#endif // PLUGIN_MANAGER_H