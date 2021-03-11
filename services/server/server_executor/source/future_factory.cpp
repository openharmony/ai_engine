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

#include "server_executor/include/future_factory.h"

#include "platform/time/include/time.h"
#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "server_executor/include/future.h"
#include "server_executor/include/i_future_listener.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
namespace {
const int MAX_NUM_FUTURES = 1024;
const int INVALID_SEQUENCE_ID = -1;
const int MIN_SEQUENCE_ID = 1;
}

std::mutex FutureFactory::mutex_;
FutureFactory *FutureFactory::instance_ = nullptr;

FutureFactory *FutureFactory::GetInstance()
{
    CHK_RET(instance_ != nullptr, instance_);

    std::lock_guard<std::mutex> lock(mutex_);
    CHK_RET(instance_ != nullptr, instance_);

    AIE_NEW(instance_, FutureFactory);

    return instance_;
}

void FutureFactory::ReleaseInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    AIE_DELETE(instance_);
}

FutureFactory::FutureFactory() : sequenceId_(0)
{
}

FutureFactory::~FutureFactory()
{
    HILOGI("[FutureFactory]Begin to release FutureFactory.");
    for (auto &iter : listeners_) {
        AIE_DELETE(iter.second);
    }
    listeners_.clear();
    for (auto &iter : futures_) {
        AIE_DELETE(iter.second);
    }
    futures_.clear();
}

long long FutureFactory::FindSequenceId()
{
    std::lock_guard<std::mutex> lock(innerMutex_);

    if (futures_.size() > MAX_NUM_FUTURES) {
        HILOGE("[FutureFactory]Num of valid futures reaches max.");
        return INVALID_SEQUENCE_ID;
    }

    do {
        ++sequenceId_;
        if (sequenceId_ < MIN_SEQUENCE_ID) {
            HILOGI("[FutureFactory]The sequenceId_ is smaller than MIN_SEQUENCE_ID.");
            sequenceId_ = MIN_SEQUENCE_ID;
        }
    } while (futures_.find(sequenceId_) != futures_.end());

    return sequenceId_;
}

int FutureFactory::CreateFuture(IRequest *request)
{
    long long sequenceId = FindSequenceId();
    if (sequenceId == INVALID_SEQUENCE_ID) {
        HILOGE("[FutureFactory]Invalid sequence id generated.");
        return RETCODE_NULL_PARAM;
    }

    if (request == nullptr) {
        HILOGE("[FutureFactory]Param request is nullptr.");
        return RETCODE_NULL_PARAM;
    }
    Request *req = reinterpret_cast<Request*>(request);
    req->SetInnerSequenceId(sequenceId);

    Future *future = nullptr;
    AIE_NEW(future, Future(request, sequenceId, request->GetTransactionId()));
    CHK_RET(future == nullptr, RETCODE_OUT_OF_MEMORY);

    AddFuture(sequenceId, future);
    return RETCODE_SUCCESS;
}

void FutureFactory::AddFuture(long long sequenceId, Future* future)
{
    std::lock_guard<std::mutex> lock(innerMutex_);

    futures_[sequenceId] = future;
}

void FutureFactory::Release(long long sequenceId)
{
    DeleteFuture(sequenceId);
}

void FutureFactory::DeleteFuture(long long sequenceId)
{
    std::lock_guard<std::mutex> lock(innerMutex_);
    auto iter = futures_.find(sequenceId);
    if (iter != futures_.end()) {
        delete iter->second;
        iter->second = nullptr;
        futures_.erase(sequenceId);
    }
}

void FutureFactory::RegisterListener(IFutureListener *listener, long long transactionId)
{
    std::lock_guard<std::mutex> lock(innerMutex_);
    listeners_[transactionId] = listener;
}

void FutureFactory::UnregisterListener(long long transactionId)
{
    std::lock_guard<std::mutex> lock(innerMutex_);
    auto iter = listeners_.find(transactionId);
    if (iter != listeners_.end()) {
        delete iter->second;
        iter->second = nullptr;
        listeners_.erase(transactionId);
    }
}

int FutureFactory::ProcessResponse(PluginEvent event, IResponse *response)
{
    CHK_RET(response == nullptr, RETCODE_NULL_PARAM);
    HILOGI("[FutureFactory]Begin to Process Response.");
    Response *res = reinterpret_cast<Response *>(response);
    Future *future = FetchFuture(res);
    if (!future) {
        HILOGE("[FutureFactory][transactionId:%lld]No matched future found, seqId=%lld.",
            res->GetTransactionId(), res->GetInnerSequenceId());
        return RETCODE_NULL_PARAM;
    }

    FutureStatus status = Future::ConvertPluginStatus(event);
    future->SetResponse(status, response);

    IFutureListener *listener = FindListener(response->GetTransactionId());
    if (listener == nullptr) {
        HILOGE("[FutureFactory][transactionId:%lld]No matched listener found.", response->GetTransactionId());
        return RETCODE_NO_LISTENER_FOUND;
    }

    listener->OnReply(future);
    future->DetachResponse();
    DeleteFuture(future->GetSequenceId());

    return RETCODE_SUCCESS;
}

Future *FutureFactory::FetchFuture(Response *response)
{
    long long sequenceId = response->GetInnerSequenceId();
    std::lock_guard<std::mutex> lock(innerMutex_);

    auto findProc = futures_.find(sequenceId);
    CHK_RET(findProc == futures_.end(), nullptr);

    return findProc->second;
}

IFutureListener *FutureFactory::FindListener(long long transactionId)
{
    std::lock_guard<std::mutex> lock(innerMutex_);

    auto findProc = listeners_.find(transactionId);
    CHK_RET(findProc == listeners_.end(), nullptr);

    return findProc->second;
}
} // namespace AI
} // namespace OHOS