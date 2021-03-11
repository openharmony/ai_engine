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

#include "server_executor/include/engine_manager.h"

#include <cstring>

#include "platform/lock/include/rw_lock.h"
#include "plugin/i_plugin.h"
#include "plugin_manager/include/aie_plugin_info.h"
#include "plugin_manager/include/i_plugin_manager.h"
#include "server_executor/include/engine.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
namespace {
    const int PLUGIN_NUM_FOR_UNLOAD = 1;
}
EngineManager::EngineManager() = default;

EngineManager::~EngineManager()
{
    Uninitialize();
}

int EngineManager::Initialize()
{
    return RETCODE_SUCCESS;
}

void EngineManager::Uninitialize()
{
    HILOGI("[EngineManager]Begin to release engine manager.");
    engines_.clear();
    clientEngines_.clear();

    IPluginManager *pluginManager = IPluginManager::GetPluginManager();
    if (pluginManager != nullptr) {
        pluginManager->Destroy();
    }
}

int EngineManager::StartEngine(long long transactionId, const AlgorithmInfo &algoInfo, const DataInfo &inputInfo,
    DataInfo &outputInfo)
{
    std::string aid = GetAlgorithmIdByType(algoInfo.algorithmType);
    if (aid == ALGORITHM_ID_INVALID) {
        HILOGE("[EngineManager]Start engine failed, aid is invalid.");
        return RETCODE_ALGORITHM_ID_INVALID;
    }
    EngineKey engineKey(aid, algoInfo.algorithmVersion);
    std::shared_ptr<Engine> engine = FindEngine(engineKey);
    if (engine == nullptr) {
        int retCode = CreateEngineWithCheck(engineKey, engine);
        if (retCode != RETCODE_SUCCESS) {
            HILOGE("[EngineManager][transactionId:%lld]Create engine failed, retCode=[%d].",
                transactionId, retCode);
            return retCode;
        }
    }
    std::shared_ptr<Plugin> plugin = engine->GetPlugin();
    if (plugin == nullptr) {
        HILOGE("[EngineManager]Plugin is nullptr.");
        return RETCODE_FAILURE;
    }
    int retCode = plugin->GetPluginAlgorithm()->Prepare(transactionId, inputInfo, outputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[EngineManager]Start engine failed, failed to prepare.");
        return retCode;
    }
    engine->AddEngineReference();
    RecordClient(transactionId, engine);
    return RETCODE_SUCCESS;
}

int EngineManager::StopEngine(long long transactionId, const DataInfo &inputInfo)
{
    std::shared_ptr<Engine> engine = FindEngine(transactionId);
    if (engine == nullptr) {
        HILOGE("[EngineManager]No corresponding engine was found.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }

    std::shared_ptr<Plugin> plugin = engine->GetPlugin();
    if (plugin == nullptr) {
        HILOGE("[EngineManager]The plugin is null.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }

    // only one engine remains, fully unload this plugin
    bool isFullUnload = engine->GetEngineReference() == PLUGIN_NUM_FOR_UNLOAD;
    int retCode = plugin->GetPluginAlgorithm()->Release(isFullUnload, transactionId, inputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[EngineManager]Failed to release plugin.");
        return retCode;
    }

    engine->DelEngineReference();
    if (engine->GetEngineReference() == 0) {
        HILOGI("[EngineManager]The current engine is not in use.");
        IPluginManager *pluginManager = IPluginManager::GetPluginManager();
        if (pluginManager == nullptr) {
            HILOGE("[EngineManager]The pluginManager is null.");
            return RETCODE_NULL_PARAM;
        }

        const std::string aid = plugin->GetAid();
        long long version = plugin->GetVersion();
        pluginManager->UnloadPlugin(aid, version);
        EngineKey engineKey(aid, version);
        DelEngine(engineKey);
    }
    UnRecordClient(transactionId);
    return RETCODE_SUCCESS;
}

int EngineManager::SetOption(long long transactionId, int optionType, const DataInfo &inputInfo)
{
    std::shared_ptr<Engine> engine = FindEngine(transactionId);
    if (engine == nullptr) {
        HILOGE("[EngineManager]No corresponding engine was found.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }
    std::shared_ptr<Plugin> plugin = engine->GetPlugin();
    if (plugin == nullptr) {
        HILOGE("[EngineManager]The plugin is null.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }
    return plugin->GetPluginAlgorithm()->SetOption(optionType, inputInfo);
}

int EngineManager::GetOption(long long transactionId, int optionType, const DataInfo &inputInfo,
    DataInfo &outputInfo)
{
    std::shared_ptr<Engine> engine = FindEngine(transactionId);
    if (engine == nullptr) {
        HILOGE("[EngineManager]No corresponding engine was found.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }
    std::shared_ptr<Plugin> plugin = engine->GetPlugin();
    if (plugin == nullptr) {
        HILOGE("[EngineManager]The plugin is null.");
        return RETCODE_SA_SERVICE_EXCEPTION;
    }
    return plugin->GetPluginAlgorithm()->GetOption(optionType, inputInfo, outputInfo);
}

std::shared_ptr<Engine> EngineManager::FindEngine(long long transactionId)
{
    ReadGuard<RwLock> guard(rwLock_);
    auto iter = clientEngines_.find(transactionId);
    if (iter == clientEngines_.end()) {
        HILOGE("[EngineManager]No corresponding engine was found.");
        return nullptr;
    }
    return iter->second;
}

void EngineManager::RecordClient(long long transactionId, const std::shared_ptr<Engine> &engine)
{
    WriteGuard<RwLock> guard(rwLock_);
    clientEngines_[transactionId] = engine;
}

void EngineManager::UnRecordClient(long long transactionId)
{
    WriteGuard<RwLock> guard(rwLock_);
    clientEngines_.erase(transactionId);
}

int EngineManager::CreateEngine(const EngineKey &engineKey, std::shared_ptr<Engine> &engine)
{
    IPluginManager *pluginManager = IPluginManager::GetPluginManager();
    if (pluginManager == nullptr) {
        HILOGE("[EngineManager]The pluginManager is null.");
        return RETCODE_NULL_PARAM;
    }

    std::shared_ptr<Plugin> plugin = nullptr;
    int retCode = pluginManager->GetPlugin(engineKey.aid, engineKey.version, plugin);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[EngineManager]The plugin(aid=%s, version=%lld) is not existed.",
            engineKey.aid.c_str(), engineKey.version);
        return retCode;
    }

    ThreadPool *threadPool = ThreadPool::GetInstance();
    CHK_RET(threadPool == nullptr, RETCODE_OUT_OF_MEMORY);
    std::shared_ptr<Thread> thread = threadPool->Pop();
    if (thread == nullptr) {
        HILOGE("[EngineManager]Failed to get thread.");
        return RETCODE_OUT_OF_MEMORY;
    }

    QueuePool<Task> *queuePool = QueuePool<Task>::GetInstance(MAX_SYNC_MSG_NUM);
    CHK_RET(queuePool == nullptr, RETCODE_OUT_OF_MEMORY);
    std::shared_ptr<Queue<Task>> queue = queuePool->Pop();
    if (queue == nullptr) {
        HILOGE("[EngineManager]Failed to get queue.");
        threadPool->Push(thread);
        return RETCODE_OUT_OF_MEMORY;
    }

    Engine *newEngine = nullptr;
    AIE_NEW(newEngine, Engine(plugin, thread, queue));
    if (newEngine == nullptr) {
        HILOGE("[EngineManager]Failed to create engine.");
        queuePool->Push(queue);
        threadPool->Push(thread);
        return RETCODE_ENGINE_NOT_EXIST;
    }

    engine.reset(newEngine);
    retCode = engine->Initialize();
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[EngineManager]Initialize engine failed.");
        queuePool->Push(queue);
        threadPool->Push(thread);
        return retCode;
    }
    return RETCODE_SUCCESS;
}

int EngineManager::CreateEngineWithCheck(const EngineKey &engineKey, std::shared_ptr<Engine> &engine)
{
    HILOGI("[EngineManager]Begin to create corresponding engine.");
    WriteGuard<RwLock> guard(rwLock_);
    Engines::iterator iter = engines_.find(engineKey);
    if (iter != engines_.end()) {
        HILOGI("[EngineManager]Success to find engine.");
        engine = iter->second;
        return RETCODE_SUCCESS;
    }

    int retCode = CreateEngine(engineKey, engine);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[EngineManager]Failed to create engine.");
        return retCode;
    }

    engines_[engineKey] = engine;
    return RETCODE_SUCCESS;
}

std::shared_ptr<Engine> EngineManager::FindEngine(const EngineKey &engineKey)
{
    ReadGuard<RwLock> guard(rwLock_);
    Engines::iterator iter = engines_.find(engineKey);
    if (iter == engines_.end()) {
        HILOGI("[EngineManager]No corresponding engine was found.");
        return nullptr;
    }
    return iter->second;
}

void EngineManager::DelEngine(const EngineKey &engineKey)
{
    WriteGuard<RwLock> guard(rwLock_);
    engines_.erase(engineKey);
}
} // namespace AI
} // namespace OHOS