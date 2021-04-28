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

#ifndef CLIENT_FACTORY_H
#define CLIENT_FACTORY_H

#include <atomic>
#include <map>
#include <mutex>

#include "client_executor/include/i_client_cb.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/constants/constants.h"

namespace OHOS {
namespace AI {
const int AIE_SESSION_ID_BEGIN = 0;
constexpr uid_t INVALID_UID = 0;

class ClientFactory {
public:
    ClientFactory();
    virtual ~ClientFactory();

    /**
     * Connect the server to get the client ID and initialize the client.
     *
     * @param [in] configInfo Engine configuration information.
     * @param [out] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] cb Service dead callback, Called when the service is dead.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int ClientInit(const ConfigInfo &configInfo, ClientInfo &clientInfo,
        const AlgorithmInfo &algorithmInfo, IServiceDeadCb *cb);

    /**
     * Load algorithm plugin and model based on algorithm information and client information.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to load algorithm plugin.
     * @param [out] outputInfo The returned data information after loading the algorithm plugin.
     * @param [in] cb Callback function, asynchronous inference needs to be passed in,
     *                synchronous inference is not required.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int ClientPrepare(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo, DataInfo &outputInfo, IClientCb *cb);

    /**
     * Set the configuration parameters of the engine or plugin.
     *
     * @param [in] clientInfo Client information.
     * @param [in] optionType The type of setting option.
     * @param [in] inputInfo Configuration parameter needed to set up the engine or plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int ClientSetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo);

    /**
     * Get the configuration parameters of the engine or plugin.
     *
     * @param [in] clientInfo Client information.
     * @param [in] optionType The type of getting option.
     * @param [in] inputInfo Parameter information for getting options.
     * @param [out] outputInfo The configuration parameter information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int ClientGetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo,
        DataInfo &outputInfo);

    /**
     * Disconnect the client from the server, release and destroy the client's resources.
     *
     * @param [in] clientInfo Client information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int ClientDestroy(ClientInfo &clientInfo);

    /**
     * Algorithmic inference interface for asynchronous tasks.
     *
     * The inference result is notified to the client through callback function passed in by
     * {@link ClientPrepare(ClientInfo, AlgorithmInfo,DataInfo, DataInfo, IClientCb)}.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to asynchronous execution algorithm.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int ClientAsyncProcess(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo);

    /**
     * Algorithmic inference interface for synchronous tasks.
     *
     * @param [in] clientInfo Client information.
     * @param [in] AlgorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to synchronous execution algorithm.
     * @param [out] outputInfo Algorithm inference results.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int ClientSyncProcess(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo, DataInfo &outputInfo);

    /**
     * Unload model and plugin.
     *
     * After the algorithm is executed successfully, the method needs to be called when exiting.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to unload the plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int ClientRelease(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo);

    void SetClientId(int clientId);
    int GetClientId() const;
    void SetServerUid(const uid_t clientId);
    uid_t GetServerUid() const;
    int GetSessionInfo(int sessionId, int &algorithmType);
    void ResetClient();

private:
    int GenerateSessionId();
    bool AddSessionInfo(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo);
    int EraseSessionInfo(int sessionId);
    int RegisterCb(int sessionId, IClientCb *cb);
    int UnRegisterCb(const int sessionId);
    int RegisterDeadCb(int sessionId, IServiceDeadCb *cb);
    int UnRegisterDeadCb(const int sessionId);
    int WaitConnection();

    virtual int InitAiServer(const ConfigInfo &configInfo, ClientInfo &clientInfo,
        const AlgorithmInfo &algorithmInfo) = 0;
    virtual int CloseAiServer() = 0;
    virtual int LoadAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo, DataInfo &outputInfo) = 0;
    virtual int AsyncExecute(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo) = 0;
    virtual int SyncExecute(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo, DataInfo &outputInfo) = 0;
    virtual int SetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo) = 0;
    virtual int GetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo,
        DataInfo &outputInfo) = 0;
    virtual int UnLoadAlgorithm(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
        const DataInfo &inputInfo) = 0;

private:
    static std::mutex sessionIdMutex_;

    int clientId_ {INVALID_CLIENT_ID};
    uid_t serverUid_ {INVALID_UID};
    std::atomic<int> sessionId_ {AIE_SESSION_ID_BEGIN};

    // map <sessionId, algorithmType>
    using SessionInfos = std::map<int, int>;
    SessionInfos sessionInfos_;
};

ClientFactory *GetClient();
} // namespace AI
} // namespace OHOS

#endif // CLIENT_FACTORY_H
