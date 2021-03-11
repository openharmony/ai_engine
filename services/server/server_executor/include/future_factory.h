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

#ifndef FUTURE_FACTORY_H
#define FUTURE_FACTORY_H

#include <atomic>
#include <map>
#include <mutex>

#include "plugin/i_plugin_callback.h"
#include "protocol/data_channel/include/request.h"
#include "protocol/data_channel/include/response.h"
#include "server_executor/include/future.h"
#include "server_executor/include/i_future.h"
#include "server_executor/include/i_future_listener.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class FutureFactory {
    FORBID_COPY_AND_ASSIGN(FutureFactory);
    FORBID_CREATE_BY_SELF(FutureFactory);
public:
    static FutureFactory *GetInstance();

    static void ReleaseInstance();

    /**
     * Generate a future instance and add it to process queue.
     *
     * @param [in] request Task request information.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int CreateFuture(IRequest *request);

    /**
     * Release the future corresponding to sequenceId.
     *
     * @param [in] sequenceId The sequence number that uniquely identifies an asynchronous task.
     */
    void Release(long long sequenceId);

    /**
     * Register callback for the transactionId.
     *
     * @param [in] listener Callback.
     * @param [in] transactionId Transaction ID.
     */
    void RegisterListener(IFutureListener *listener, long long transactionId);

    /**
     * Unregister the callback corresponding to transactionId.
     *
     * @param [in] transactionId Transaction ID.
     */
    void UnregisterListener(long long transactionId);

    /**
     * Process the asynchronous task response and pass it to the upper level callback.
     *
     * @param [in] event Plugin event, {@code ON_PLUGIN_SUCCEED} or {@code ON_PLUGIN_FAIL}.
     * @param [in] response Response of asynchronous task processing.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int ProcessResponse(PluginEvent event, IResponse *response);

private:
    void AddFuture(long long sequenceId, Future* future);

    void DeleteFuture(long long sequenceId);

    Future* FetchFuture(Response *response);

    IFutureListener *FindListener(long long transactionId);

    long long FindSequenceId();

private:
    static std::mutex mutex_;
    static FutureFactory *instance_;

private:
    std::mutex innerMutex_;
    using Futures = std::map<long long, Future*>;
    Futures futures_;
    std::atomic<long long> sequenceId_;
    using FutureListeners = std::map<long long, IFutureListener*>;
    FutureListeners listeners_;
};
} // namespace AI
} // namespace OHOS

#endif // FUTURE_FACTORY_H