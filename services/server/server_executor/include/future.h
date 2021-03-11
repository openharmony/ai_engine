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

#ifndef FUTURE_H
#define FUTURE_H

#include <memory>
#include <ctime>

#include "platform/semaphore/include/i_semaphore.h"
#include "plugin/i_plugin_callback.h"
#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"
#include "server_executor/include/i_future.h"

namespace OHOS {
namespace AI {
class FutureFactory;

class Future : public IFuture {
    friend class FutureFactory;
public:
    Future(IRequest *request, long long sequenceId, long long transactionId);

    ~Future() override;

    IResponse *GetResponse(int timeOut) const override;

    /**
     * Release the future.
     */
    void Release() override;

    /**
     * Get future status.
     *
     * @return Future status.
     */
    FutureStatus Status() const override;

    /**
     * Convert the event type to future status, {@code FUTURE_OK} or {@code FUTURE_ERROR}.
     *
     * @param [in] event Plugin event, {@code ON_PLUGIN_SUCCEED} or {@code ON_PLUGIN_FAIL}.
     * @return Future status.
     */
    static FutureStatus ConvertPluginStatus(PluginEvent event);

private:
    void DetachResponse();

    void SetResponse(FutureStatus status, IResponse *response);

    long long GetTransactionId() const;

    long long GetSequenceId() const;

    long long GetCreateTime() const;

    IRequest *GetRequest() const;

private:
    long long sequenceId_;
    long long transactionId_;
    time_t createTime_;
    IRequest *request_;
    IResponse *response_;
    FutureStatus status_;

    std::shared_ptr<ISemaphore> semaphore_;
};
} // namespace AI
} // namespace OHOS

#endif // FUTURE_H