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

#include "protocol/data_channel/include/response.h"

#include "protocol/data_channel/include/i_response.h"
#include "protocol/data_channel/include/request.h"
#include "utils/inf_cast_impl.h"

namespace OHOS {
namespace AI {
namespace {
    const int INVALID_ALGO_PLUGIN_TYPE = -1;
}
Response::Response(IRequest *request) : requestId_(0), innerSequenceId_(0), transactionId_(0),
    retCode_(RETCODE_SUCCESS), algoPluginType_(INVALID_ALGO_PLUGIN_TYPE)
{
    result_.data = nullptr;
    result_.length = 0;
    if (request != nullptr) {
        requestId_ = request->GetRequestId();
        innerSequenceId_ = (reinterpret_cast<Request *>(request))->GetInnerSequenceId();
        transactionId_ = request->GetTransactionId();
        algoPluginType_ = request->GetAlgoPluginType();
    }
}

Response::~Response()
{
    if (result_.data != nullptr) {
        free(result_.data);
        result_.data = nullptr;
        result_.length = 0;
    }
}

int Response::GetRequestId() const
{
    return requestId_;
}

long long Response::GetInnerSequenceId() const
{
    return innerSequenceId_;
}

void Response::SetInnerSequenceId(long long seqId)
{
    innerSequenceId_ = seqId;
}

long long Response::GetTransactionId() const
{
    return transactionId_;
}

void Response::SetTransactionId(long long transactionId)
{
    transactionId_ = transactionId;
}

int Response::GetAlgoPluginType() const
{
    return algoPluginType_;
}

void Response::SetAlgoPluginType(int type)
{
    algoPluginType_ = type;
}

int Response::GetRetCode() const
{
    return retCode_;
}

void Response::SetRetCode(int retCode)
{
    retCode_ = retCode;
}

const std::string &Response::GetRetDesc() const
{
    return retDesc_;
}

void Response::SetRetDesc(const std::string &retDesc)
{
    retDesc_ = retDesc;
}

const DataInfo &Response::GetResult() const
{
    return result_;
}

void Response::SetResult(const DataInfo &result)
{
    result_ = result;
}

void Response::Detach()
{
    result_.data = nullptr;
}

DEFINE_IMPL_CLASS_CAST(ResponseCast, IResponse, Response);

IResponse *IResponse::Create(IRequest *request)
{
    return ResponseCast::Create(request);
}

void IResponse::Destroy(IResponse *&response)
{
    ResponseCast::Destroy(response);
}

int IResponse::GetRequestId() const
{
    return ResponseCast::Ref(this).GetRequestId();
}

long long IResponse::GetTransactionId() const
{
    return ResponseCast::Ref(this).GetTransactionId();
}

void IResponse::SetTransactionId(long long transactionId)
{
    ResponseCast::Ref(this).SetTransactionId(transactionId);
}

int IResponse::GetRetCode() const
{
    return ResponseCast::Ref(this).GetRetCode();
}

void IResponse::SetRetCode(int retCode)
{
    ResponseCast::Ref(this).SetRetCode(retCode);
}

const std::string &IResponse::GetRetDesc() const
{
    return ResponseCast::Ref(this).GetRetDesc();
}

void IResponse::SetRetDesc(const std::string &retDesc)
{
    ResponseCast::Ref(this).SetRetDesc(retDesc);
}

const DataInfo &IResponse::GetResult() const
{
    return ResponseCast::Ref(this).GetResult();
}

void IResponse::SetResult(const DataInfo &result)
{
    ResponseCast::Ref(this).SetResult(result);
}

void IResponse::Detach()
{
    ResponseCast::Ref(this).Detach();
}
} // namespace AI
} // namespace OHOS