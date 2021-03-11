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

#ifndef SA_CLIENT_H
#define SA_CLIENT_H

#include <chrono>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <vector>

#include "iproxy_client.h"

#include "platform/semaphore/include/simple_event_notifier.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
typedef void(*CallbackHandle)(int sessionId, const DataInfo &result, int resultCode, int requestId);
typedef void(*DeathCallbackHandle)();

class SaClient {
    FORBID_COPY_AND_ASSIGN(SaClient);
    FORBID_CREATE_BY_SELF(SaClient);

public:
    static SaClient *GetInstance();
    static void ReleaseInstance();

    /**
     * Register SA callback for async processing; Transfer async result to sdk after receiving it
     *
     * @param [in] Callback Handle to deal with the async result.
     */
    void RegisterSaClientCb(CallbackHandle result);

    /**
     * Unregister SA callback for async processing; Transfer async result to sdk after receiving it
     */
    void UnRegisterSaClientCb();

    /**
     * Save dead callback to transfer dead message of aiserver to sdk/third-party application
     *
     * @param [in] Callback Handle to deal with the AI SA server dead message.
     */
    void RegisterSaDeathCb(DeathCallbackHandle deathCb);

    /**
     * Delete dead callback to transfer dead message of aiserver to sdk/third-party application
     */
    void UnRegisterSaDeathCb();
    CallbackHandle GetSaClientResultCb();
    DeathCallbackHandle GetSaDeathResultCb();

    /**
     * Call SA proxy, to connect the server to get the client ID
     * and register server dead callback handle to SA manager.
     *
     * @param [in] configInfo Engine configuration information.
     * @param [in] clientInfo Client information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int Init(const ConfigInfo &configInfo, ClientInfo &clientInfo);

    /**
     * Call SA proxy, to load algorithm plugin and model based on algorithm information and client information.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to load algorithm plugin.
     * @param [out] outputInfo The returned data information after loading the algorithm plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int LoadAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo, DataInfo &outputInfo);

    /**
     * Call SA proxy, to unload algorithm plugin and model based on algorithm information and client information.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to load algorithm plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int UnloadAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo);

    /**
     * Call SA proxy, to execute algorithm inference synchronously.
     *
     * @param [in] clientInfo Client information.
     * @param [in] AlgorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to synchronous execution algorithm.
     * @param [out] outputInfo Algorithm inference results.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int SyncExecuteAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo, DataInfo &outputInfo);

    /**
     * Call SA proxy, to execute algorithm inference asynchronously.
     *
     * The inference result is notified to the client through callback function passed in by
     * {@link RegisterSaClientCb(CallbackHandle result)}.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to asynchronous execution algorithm.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int AsyncExecuteAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo);

    /**
     * Call SA proxy, to disconnect the client from the server, release and destroy information of the client.
     *
     * @param [in] clientInfo Client information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int Destroy(const ClientInfo &clientInfo);

    /**
     * Call SA proxy, to set the configuration parameters of the engine or plugin.
     *
     * @param [in] clientInfo Client information.
     * @param [in] optionType The type of setting option.
     * @param [in] inputInfo Configuration parameter needed to set up the engine or plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int SetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo);

    /**
     * Call SA proxy, to get the configuration parameters of the engine or plugin.
     *
     * @param [in] clientInfo Client information.
     * @param [in] optionType The type of getting option.
     * @param [in] inputInfo Parameter information for getting options.
     * @param [out] outputInfo The configuration parameter information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int GetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo, DataInfo &outputInfo);

    /**
     * Call SA proxy, to register listener for async processing.
     *
     * @param [in] clientInfo Client information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int RegisterCallback(const ClientInfo &clientInfo);

    /**
     * Call SA proxy, to unregister listener for async processing.
     *
     * @param [in] clientInfo Client information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int UnregisterCallback(const ClientInfo &clientInfo);

private:
    static std::mutex instance_mutex_;
    static SaClient *instance_;
    CallbackHandle ResultCb_ {nullptr};
    DeathCallbackHandle deathCb_ {nullptr};

    uint32_t deadId_ {0};
    IClientProxy *proxy_ = nullptr;
    SvcIdentity svc_ {};
};
} // namespace AI
} // namespace OHOS

#endif // SA_CLIENT_H