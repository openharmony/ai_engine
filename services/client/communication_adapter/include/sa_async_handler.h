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

#include "client_executor/include/async_handler.h"

namespace OHOS {
namespace AI {
void SaClientCallback(int sessionId, const DataInfo &result, int resultCode, int requestId);
void SaDeathCallback();

class SaAsyncHandler : public AsyncHandler {
    FORBID_COPY_AND_ASSIGN(SaAsyncHandler);
    FORBID_CREATE_BY_SELF(SaAsyncHandler);

public:
    static SaAsyncHandler *GetInstance();
    static void ReleaseInstance();

    /**
     * Register client callback for algorithm async processing.
     *
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int RegisterAsyncClientCb() override;

    /**
     * Unregister client callback for algorithm async processing.
     *
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int UnRegisterAsyncClientCb() override;

    /**
     * Register dead callback to transfer dead message of aiserver to sdk/third-party application.
     *
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int RegisterServiceDeathCb() override;

    /**
     * Unregister dead callback to transfer dead message of aiserver to sdk/third-party application.
     *
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    int UnRegisterServiceDeathCb() override;

private:
    static std::mutex instance_mutex_;
    static SaAsyncHandler *instance_;
};
} // namespace AI
} // namespace OHOS

#endif // SA_ASYNC_HANDLER_H