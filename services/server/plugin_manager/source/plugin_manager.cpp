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

#include "plugin_manager/include/plugin_manager.h"

#include "plugin_manager/include/plugin.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/aie_macros.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
int PluginManager::GetPlugin(const std::string &aid, long long version, std::shared_ptr<Plugin> &plugin)
{
    HILOGI("[PluginManager]Get plugin for server, aid=%s, version=%lld.", aid.c_str(), version);
    PluginKey pluginKey(aid, version);
    plugin = FindPlugin(pluginKey);
    if (plugin) {
        return RETCODE_SUCCESS;
    }

    std::lock_guard<std::mutex> lock(mutex4Interface_);
    plugin = FindPlugin(pluginKey);
    if (plugin) {
        return RETCODE_SUCCESS;
    }

    return LoadPlugin(aid, version, plugin);
}

std::shared_ptr<Plugin> PluginManager::FindPlugin(const PluginKey &pluginKey)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto iter = pluginMap_.find(pluginKey);
    CHK_RET(pluginMap_.end() == iter, nullptr);

    return iter->second;
}

void PluginManager::AddPlugin(const PluginKey &pluginKey, const std::shared_ptr<Plugin> &pluginValue)
{
    std::lock_guard<std::mutex> lock(mutex_);
    pluginMap_[pluginKey] = pluginValue;
}

void PluginManager::DelPlugin(const PluginKey &pluginKey)
{
    std::lock_guard<std::mutex> lock(mutex_);
    pluginMap_.erase(pluginKey);
}

void PluginManager::DelPluginByAID(const std::string &aid)
{
    std::lock_guard<std::mutex> lock(mutex_);
    PluginMap::iterator iter = pluginMap_.begin();
    while (iter != pluginMap_.end()) {
        if (aid == iter->first.aid) {
            HILOGI("[PluginManager]Try to delete plugin. aid=%s, version=%lld.", iter->first.aid.c_str(),
                iter->first.version);
            iter = pluginMap_.erase(iter);
            continue;
        }
        ++iter;
    }
}

int PluginManager::LoadPlugin(const std::string &aid, long long version, std::shared_ptr<Plugin> &plugin)
{
    auto pluginPtr = std::make_shared<Plugin>(Plugin(aid, version));
    if (pluginPtr == nullptr) {
        HILOGE("[PluginManager]The plugin is null.");
        return RETCODE_OUT_OF_MEMORY;
    }
    int ret = pluginPtr->LoadPluginAlgorithm();
    if (ret != RETCODE_SUCCESS) {
        HILOGE("[PluginManager]Failed to load plugin(aid=%s, version=%lld): ret=%d.", aid.c_str(), version, ret);
        return ret;
    }
    PluginKey pluginKey(aid, version);
    AddPlugin(pluginKey, pluginPtr);
    plugin = pluginPtr;
    HILOGI("[PluginManager]End to load plugin: aid=%s, version=%lld.", aid.c_str(), version);
    return RETCODE_SUCCESS;
}

void PluginManager::UnloadPlugin(const std::string &aid, long long version)
{
    PluginKey pluginKey(aid, version);
    std::shared_ptr<Plugin> plugin = FindPlugin(pluginKey);
    if (plugin) {
        DelPlugin(pluginKey);
        HILOGI("[PluginManager]End to unload plugin, aid=%s, version=%lld, successfully.", pluginKey.aid.c_str(),
            pluginKey.version);
        return;
    }
    HILOGI("[PluginManager]End to unload plugin, aid=%s, version=%lld, plugin is not exists, no need to unload.",
        pluginKey.aid.c_str(), pluginKey.version);
}

std::mutex PluginManager::instanceLock_;
PluginManager *PluginManager::instance_ = nullptr;

PluginManager *PluginManager::GetInstance()
{
    CHK_RET(instance_ != nullptr, instance_);

    std::lock_guard<std::mutex> lock(instanceLock_);
    CHK_RET(instance_ != nullptr, instance_);

    AIE_NEW(instance_, PluginManager);
    return instance_;
}

void PluginManager::Destroy()
{
    HILOGI("[PluginManager]Begin to Destroy plugin");
    pluginMap_.clear();
    AIE_DELETE(instance_);
}

PluginManager::PluginManager() = default;

PluginManager::~PluginManager() = default;

IPluginManager *IPluginManager::GetPluginManager()
{
    return reinterpret_cast<IPluginManager*>(PluginManager::GetInstance());
}
} // namespace AI
} // namespace OHOS