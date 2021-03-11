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

#ifndef ENGINE_MANAGER_H
#define ENGINE_MANAGER_H

#include <cstddef>
#include <map>

#include "platform/lock/include/rw_lock.h"
#include "server_executor/include/engine.h"

namespace OHOS {
namespace AI {
// Synchronization message default size
const size_t MAX_SYNC_MSG_NUM = 64;

// Asynchronous message default size
const size_t MAX_ASYNC_MSG_NUM = 64;

struct EngineKey {
    std::string aid;
    long long version;

    EngineKey(const std::string &aid, long long version) : aid(aid), version(version)
    {
    }

    inline bool operator < (const EngineKey &another) const
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

class EngineManager {
public:
    EngineManager();
    ~EngineManager();

    /**
     * Initialize the engine manager.
     *
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int Initialize();

    /**
     * Start engine.
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] algoInfo Algorithm information.
     * @param [in] inputInfo Data information needed to start engine.
     * @param [out] outputInfo The returned data information after starting the algorithm plugin.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int StartEngine(long long transactionId, const AlgorithmInfo &algoInfo, const DataInfo &inputInfo,
        DataInfo &outputInfo);

    /**
     * Stop engine.
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] inputInfo Data information needed to stop engine.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int StopEngine(long long transactionId, const DataInfo &inputInfo);

    /**
     * Find the engine based on transaction ID.
     *
     * @param [in] transactionId Transaction ID.
     * @return Engine corresponding to transaction ID.
     */
    std::shared_ptr<Engine> FindEngine(long long transactionId);

    /**
     * Set the configuration parameters of the plugin algorithm.
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] optionType The type of setting option.
     * @param [in] inputInfo Configuration parameter needed to set up the plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int SetOption(long long transactionId, int optionType, const DataInfo &inputInfo);

    /**
     * Get the configuration parameters of the plugin algorithm.
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] optionType The type of getting option.
     * @param [in] inputInfo Parameter information for getting options.
     * @param [out] outputInfo The configuration parameter information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int GetOption(long long transactionId, int optionType, const DataInfo &inputInfo,
        DataInfo &outputInfo);

private:
    void Uninitialize();
    void RecordClient(long long transactionId, const std::shared_ptr<Engine> &engine);
    void UnRecordClient(long long transactionId);
    int CreateEngine(const EngineKey &engineKey, std::shared_ptr<Engine> &engine);
    int CreateEngineWithCheck(const EngineKey &engineKey, std::shared_ptr<Engine> &engine);
    std::shared_ptr<Engine> FindEngine(const EngineKey &engineKey);
    void DelEngine(const EngineKey &engineKey);

private:
    RwLock rwLock_;
    using Engines = std::map<EngineKey, std::shared_ptr<Engine>>;
    Engines engines_;
    using ClientEngines = std::map<long long, std::shared_ptr<Engine>>;
    ClientEngines clientEngines_;
};
} // namespace AI
} // namespace OHOS

#endif // ENGINE_MANAGER_H