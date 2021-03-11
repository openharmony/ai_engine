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

#include "server_executor/include/future.h"

#include "platform/semaphore/include/i_semaphore.h"
#include "platform/time/include/time.h"
#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "server_executor/include/future_factory.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
Future::Future(IRequest *request, long long sequenceId, long long transactionId)
    : sequenceId_(sequenceId),
      transactionId_(transactionId),
      request_(request),
      response_(nullptr),
      status_(FUTURE_OK)
{
    createTime_ = GetCurTimeSec();
    semaphore_ = ISemaphore::MakeShared(0);
}

Future::~Future()
{
    request_ = nullptr;

    if (response_ != nullptr) {
        IResponse::Destroy(response_);
    }
}

IResponse *Future::GetResponse(int timeOut) const
{
    semaphore_->Wait(timeOut);
    return response_;
}

void Future::Release()
{
    FutureFactory *futureFactory = FutureFactory::GetInstance();
    CHK_RET_NONE(futureFactory == nullptr);
    futureFactory->Release(sequenceId_);
}

IRequest *Future::GetRequest() const
{
    return request_;
}

FutureStatus Future::Status() const
{
    return status_;
}

FutureStatus Future::ConvertPluginStatus(PluginEvent event)
{
    if (event == ON_PLUGIN_SUCCEED) {
        return FUTURE_OK;
    }
    return FUTURE_ERROR;
}

void Future::DetachResponse()
{
    response_ = nullptr;
}

void Future::SetResponse(FutureStatus status, IResponse *response)
{
    if (response_ != nullptr) {
        IResponse::Destroy(response_);
    }

    status_ = status;
    response_ = response;
    semaphore_->Signal();
}

long long Future::GetTransactionId() const
{
    return transactionId_;
}

long long Future::GetSequenceId() const
{
    return sequenceId_;
}

long long Future::GetCreateTime() const
{
    return createTime_;
}
} // namespace AI
} // namespace OHOS