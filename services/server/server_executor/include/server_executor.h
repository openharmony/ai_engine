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

#ifndef SERVER_EXECUTOR_H
#define SERVER_EXECUTOR_H

#include <mutex>

#include "platform/queuepool/queue.h"
#include "platform/queuepool/queue_pool.h"
#include "platform/threadpool/include/thread.h"
#include "platform/threadpool/include/thread_pool.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "server_executor/include/engine_manager.h"
#include "server_executor/include/i_async_task_manager.h"
#include "server_executor/include/i_engine_manager.h"
#include "server_executor/include/i_future_listener.h"
#include "server_executor/include/i_sync_task_manager.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class ServerExecutor : public ISyncTaskManager, public IAsyncTaskManager, public IEngineManager {
    FORBID_COPY_AND_ASSIGN(ServerExecutor);
    FORBID_CREATE_BY_SELF(ServerExecutor);
public:
    /**
     * Use singleton pattern.
     *
     * @return Pointer to the singleton.
     */
    static ServerExecutor *GetInstance();

    /**
     * Destroy the singleton.
     */
    static void ReleaseInstance();

    /**
     * Process sync execute request.
     *
     * @param [in] request Algorithm input.
     * @param [out] response Algorithm output.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int SyncExecute(IRequest *request, IResponse *&response) override;

    /**
     * process async execute request
     *
     * @param [in] request Algorithm input.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int AsyncExecute(IRequest *request) override;

    /**
     * Register listener by Transaction ID.
     *
     * @param [in] listener Listener for async execution.
     * @param [in] transactionId Transaction ID.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int RegisterListener(IFutureListener *listener, long long transactionId) override;

    /**
     * Unregister listener by Transaction ID.
     *
     * @param [in] transactionId Transaction ID.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int UnRegisterListener(long long transactionId) override;

    /**
     * Start engine
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] algoInfo Algorithm info.
     * @param [in] inputInfo Parameter info for starting engine.
     * @param [out] outputInfo Parameter info for starting engine.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int StartEngine(long long transactionId, const AlgorithmInfo &algoInfo, const DataInfo &inputInfo,
        DataInfo &outputInfo) override;

    /**
     * Stop engine
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] inputInfo Parameter information for stopping engine.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int StopEngine(long long transactionId, const DataInfo &inputInfo) override;

    /**
     * Set the configuration parameters of the plugin.
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] optionType The type of setting option.
     * @param [in] inputInfo Parameter information for setting options.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int SetOption(long long transactionId, int optionType, const DataInfo &inputInfo) override;

    /**
     * Get the configuration parameters of the plugin.
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] optionType The type of getting option.
     * @param [in] inputInfo Parameter information for getting options.
     * @param [out] outputInfo The configuration parameter information.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int GetOption(long long transactionId, int optionType, const DataInfo &inputInfo,
        DataInfo &outputInfo) override;

private:
    int Initialize();
    void Uninitialize();

private:
    static std::mutex mutex_;
    static ServerExecutor *instance_;

private:
    EngineManager *engineMgr_;
};
} // namespace AI
} // namespace OHOS

#endif // SERVER_EXECUTOR_H