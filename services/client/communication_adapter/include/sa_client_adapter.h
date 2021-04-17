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

#ifndef SA_CLIENT_ADAPTER_H
#define SA_CLIENT_ADAPTER_H

#include <mutex>

#include "client_executor/include/client_factory.h"
#include "platform/threadpool/include/thread_pool.h"

namespace OHOS {
namespace AI {
/**
 * SA client thread class
 */
class ConnectMgrWorker : public IWorker {
public:
    ConnectMgrWorker(const ConfigInfo &configInfo, ClientInfo &clientInfo);
    ~ConnectMgrWorker() override = default;

    const char *GetName() const override;
    bool OneAction() override;
    bool Initialize() override;
    void Uninitialize() override;

private:
    ConfigInfo configInfo_;
    ClientInfo clientInfo_;
};

class SaClientAdapter : public ClientFactory {
    FORBID_COPY_AND_ASSIGN(SaClientAdapter);
    FORBID_CREATE_BY_SELF(SaClientAdapter);

public:
    static SaClientAdapter *GetInstance();

    static void ReleaseInstance();

private:
    /**
     * Start a thread {@link StartConnectMgrThread(ConfigInfo, ClientInfo, AlgorithmInfo)}
     * to Init client and connect to AI SA server.
     *
     * @param [in] configInfo Engine configuration information.
     * @param [out] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int InitAiServer(const ConfigInfo &configInfo, ClientInfo &clientInfo,
        const AlgorithmInfo &algorithmInfo) override;

    /**
     * Stop a thread {@link StopConnectMgrThread()} to destroy client, and disconnect from AI SA server.
     *
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int CloseAiServer() override;

    /**
     * Call SA client, to load algorithm plugin and model based on algorithm information and client information.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to load algorithm plugin.
     * @param [out] outputInfo The returned data information after loading the algorithm plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int LoadAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo, DataInfo &outputInfo) override;

    /**
     * Call SA client, to unload model and plugin.
     *
     * After the algorithm is executed successfully, the method needs to be called when exiting.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to unload the plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int UnLoadAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo) override;

    /**
     * Call SA client, to execute algorithm inference synchronously.
     *
     * @param [in] clientInfo Client information.
     * @param [in] AlgorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to synchronous execution algorithm.
     * @param [out] outputInfo Algorithm inference results.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int SyncExecute(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo, DataInfo &outputInfo) override;

    /**
     * Call SA client, to execute algorithm inference asynchronously.
     *
     * The inference result is notified to the client through callback function passed in by the callback
     * saved by SaAsyncHandler
     *
     * @param [in] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to asynchronous execution algorithm.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int AsyncExecute(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo) override;

    /**
     * Call SA client, to set the configuration parameters of the engine or plugin.
     *
     * @param [in] clientInfo Client information.
     * @param [in] optionType The type of setting option.
     * @param [in] inputInfo Configuration parameter needed to set up the engine or plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int SetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo) override;

    /**
     * Call SA client, to get the configuration parameters of the engine or plugin.
     *
     * @param [in] clientInfo Client information.
     * @param [in] optionType The type of getting option.
     * @param [in] inputInfo Parameter information for getting options.
     * @param [out] outputInfo The configuration parameter information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int GetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo,
        DataInfo &outputInfo) override;

private:
    static std::mutex instance_mutex_;
    static SaClientAdapter *instance_;

    std::shared_ptr<Thread> connectMgrThread_ = nullptr;
    ConnectMgrWorker *connectMgrWorker_ {nullptr};
};
} // namespace AI
} // namespace OHOS

#endif // SA_CLIENT_ADAPTER_H
