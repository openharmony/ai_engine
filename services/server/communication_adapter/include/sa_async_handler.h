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

#ifndef SA_ASYNC_HANDLER_H
#define SA_ASYNC_HANDLER_H

#include <map>
#include <mutex>

#include "communication_adapter/include/client_listener_handler.h"
#include "platform/lock/include/rw_lock.h"
#include "server_executor/include/i_future_listener.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class SaAsyncHandler {
    FORBID_COPY_AND_ASSIGN(SaAsyncHandler);
    FORBID_CREATE_BY_SELF(SaAsyncHandler);
public:
    static SaAsyncHandler *GetInstance();

    /**
     * Save the response for the client async handler.
     *
     * @param clientId Client of this handler.
     * @param response The media of asynchronous result.
     */
    void PushAsyncResponse(int clientId, IResponse *response);

    /**
     * Allocate client listener handler for certain client ID.
     *
     * @param clientId Client ID.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int RegisterAsyncHandler(int clientId);

    /**
     * Start client async handler thread to execute algorithm.
     *
     * @param clientId Client ID.
     * @param adapter The adapter of above client.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int StartAsyncProcess(int clientId, SaServerAdapter *adapter);

    /**
     * Stop client async handler thread to execute algorithm.
     *
     * @param clientId The client to be stopped.
     */
    void StopAsyncProcess(int clientId);

    /**
     * Make pair of transaction ID and FutureListener.
     *
     * @param transactionId Transaction ID.
     * @param clientId Client ID.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int StartAsyncTransaction(long long transactionId, int clientId);

    /**
     * Remove transaction ID and its FutureListener.
     *
     * @param transactionId.
     */
    void StopAsyncTransaction(long long transactionId);

private:

    /**
     * Stop the thread of client listener handler by client ID and remove it.
     * {@link StopClientListenerHandler(clientId)}.
     *
     * @param clientId Client ID.
     */
    void StopClientListenerHandler(int clientId);

    /**
     * Remove client listener handler from the map.
     *
     * @param clientId Client ID.
     */
    void RemoveClientListenerHandler(int clientId);
    ClientListenerHandler *FindClientListenerHandler(int clientId);
    ClientListenerHandler *AddClientListenerHandler(int clientId);

    /**
     * Remove transaction ID from a set<transactionId> {@link TransactionIds}.
     *
     * @param transactionId transaction ID.
     */
    void RemoveTransaction(long long transactionId);

    /**
     * Check if transaction ID exists.
     *
     * @param transactionId transaction ID.
     * @return 1-exist, 0-not exist.
     */
    bool IsExistTransaction(long long transactionId);

    /**
     * Save transaction ID in a set<transactionId> {@link TransactionIds}.
     *
     * @param transactionId transaction ID.
     */
    void SaveTransaction(long long transactionId);

private:
    static std::mutex mutex_;
    static SaAsyncHandler *instance_;
    RwLock rwLock_;

    using ClientListenerHandlerMap = std::map<int, ClientListenerHandler*>;
    ClientListenerHandlerMap clients_;

    using TransactionIds = std::set<long long>;
    TransactionIds transactions_;
};
} // namespace AI
} // namespace OHOS

#endif // SA_ASYNC_HANDLER_H