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

#ifndef I_ASYNC_TASK_MANAGER_H
#define I_ASYNC_TASK_MANAGER_H

#include "protocol/data_channel/include/i_request.h"
#include "server_executor/include/i_future.h"
#include "server_executor/include/i_future_listener.h"

namespace OHOS {
namespace AI {
class IAsyncTaskManager {
public:
    virtual ~IAsyncTaskManager() = default;

    /**
     * Algorithmic inference for asynchronous tasks. You need to override this function to
     * implement your own processing logic.
     *
     * @param [in] request Request information of asynchronous task.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int AsyncExecute(IRequest *request) = 0;

    /**
     * Register callback for the transactionId. You need to override this function to
     * implement your own processing logic.
     *
     * @param [in] listener Callback.
     * @param [in] transactionId Transaction ID.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int RegisterListener(IFutureListener *listener, long long transactionId) = 0;

    /**
     * Unregister the callback corresponding to transactionId. You need to override this function to
     * implement your own processing logic.
     *
     * @param [in] transactionId Transaction ID.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int UnRegisterListener(long long transactionId) = 0;
};

IAsyncTaskManager *GetAsyncTaskManager();
} // namespace AI
} // namespace OHOS

#endif // I_ASYNC_TASK_MANAGER_H