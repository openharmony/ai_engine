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

#ifndef SA_SERVER_ADAPTER_H
#define SA_SERVER_ADAPTER_H

#include <atomic>
#include <mutex>
#include <set>

#include "liteipc.h"

#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"

namespace OHOS {
namespace AI {
const unsigned long long TRANS_ID_MASK = 0x00000000FFFFFFFF;

class SaServerAdapter {
public:
    explicit SaServerAdapter(int adapterId);
    ~SaServerAdapter();

    /**
     * Save listener to call client async process.
     *
     * @param [in] sid Client async callback SVC handle identity
     */
    void SaveEngineListener(SvcIdentity *svcIdentity);

    /**
     * Delete the listener.
     */
    void ClearEngineListener();

    /**
     * Get listener to call client async process.
     *
     * @return Client async callback SVC handle.
     */
    SvcIdentity *GetEngineListener() const;

    /**
     * Initialize async task manager to execute algorithm inference asynchronously.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to asynchronous execution algorithm.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int AsyncExecute(const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo, const DataInfo &inputInfo);

    /**
     * Get session ID, according to transaction ID.
     *
     * @param transactionId client ID + session ID
     * @return session ID
     */
    int GetSessionId(long long transactionId) const;
    int GetAdapterId() const;
    void IncRef();
    void DecRef();
    int GetRefCount() const;

    /**
     * Get transaction ID, according to session ID.
     *
     * @param sessionId session ID
     * @return transaction ID
     */
    long long GetTransactionId(int sessionId) const;

    /**
     * Load algorithm plugin and model based on algorithm information and client information.
     *
     * @param [in] transactionId Client ID + Session ID.
     * @param [in] algorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to load algorithm plugin.
     * @param [out] outputInfo The returned data information after loading the algorithm plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int LoadAlgorithm(long long transactionId, const AlgorithmInfo &algoInfo, const DataInfo &inputInfo,
        DataInfo &outputInfo);

    /**
     * Set the configuration parameters of the engine or plugin.
     *
     * @param [in] clientInfo Client information.
     * @param [in] optionType The type of setting option.
     * @param [in] inputInfo Configuration parameter needed to set up the engine or plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int SetOption(long long transactionId, int optionType, const DataInfo &dataInfo);

    /**
     * Get the configuration parameters of the engine or plugin.
     *
     * @param [in] clientInfo Client information.
     * @param [in] optionType The type of getting option.
     * @param [in] inputInfo Parameter information for getting options.
     * @param [out] outputInfo The configuration parameter information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int GetOption(long long transactionId, int optionType, const DataInfo &dataInfo, DataInfo &outputInfo);

    /**
     * Unload algorithm plugin and model based on transaction ID and client input information.
     *
     * @param [in] transactionId Client ID + Session ID.
     * @param [in] inputInfo Data information needed to load algorithm plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int UnloadAlgorithm(long long transactionId, const DataInfo &inputInfo);

    /**
     * Execute algorithm inference synchronously.
     *
     * @param [in] clientInfo Client information.
     * @param [in] AlgorithmInfo Algorithm information.
     * @param [in] inputInfo Data information needed to synchronous execution algorithm.
     * @param [out] outputInfo Algorithm inference results.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int SyncExecute(const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo, const DataInfo &inputInfo,
        DataInfo &outputInfo);

private:
    void Uninitialize();
    void SaveTransaction(long long transactionId);
    void RemoveTransaction(long long transactionId);
    void ConvertToRequest(const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo, const DataInfo &inputInfo,
        IRequest *&request);

private:
    int adapterId_;
    std::atomic<int> refCount_;
    std::mutex mutex_;
    SvcIdentity *svcIdentity_ = nullptr;
    using TransactionIds = std::set<long long>;
    TransactionIds transactionIds_;
};
} // namespace AI
} // namespace OHOS

#endif // SA_SERVER_ADAPTER_H
