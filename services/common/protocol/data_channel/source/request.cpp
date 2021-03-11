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

#include "protocol/data_channel/include/request.h"

#include "protocol/struct_definition/aie_info_define.h"
#include "utils/inf_cast_impl.h"

namespace OHOS {
namespace AI {
Request::Request()
    : innerSequenceId_(0),
      requestId_(0),
      operationId_(0),
      transactionId_(0),
      algoPluginType_(0)
{
    msg_.data = nullptr;
    msg_.length = 0;
}

Request::~Request()
{
    if (msg_.data != nullptr) {
        free(msg_.data);
        msg_.data = nullptr;
        msg_.length = 0;
    }
}

long long Request::GetInnerSequenceId() const
{
    return innerSequenceId_;
}

void Request::SetInnerSequenceId(long long seqId)
{
    innerSequenceId_ = seqId;
}

int Request::GetRequestId() const
{
    return requestId_;
}

void Request::SetRequestId(int requestId)
{
    requestId_ = requestId;
}

int Request::GetOperationId() const
{
    return operationId_;
}

void Request::SetOperationId(int operationId)
{
    operationId_ = operationId;
}

long long Request::GetTransactionId() const
{
    return transactionId_;
}

void Request::SetTransactionId(long long transactionId)
{
    transactionId_ = transactionId;
}

int Request::GetAlgoPluginType() const
{
    return algoPluginType_;
}

void Request::SetAlgoPluginType(int type)
{
    algoPluginType_ = type;
}

const DataInfo &Request::GetMsg() const
{
    return msg_;
}

void Request::SetMsg(const DataInfo &msg)
{
    msg_ = msg;
}

DEFINE_IMPL_CLASS_CAST(RequestCast, IRequest, Request);

IRequest *IRequest::Create()
{
    return RequestCast::Create();
}

void IRequest::Destroy(IRequest *&request)
{
    RequestCast::Destroy(request);
}

int IRequest::GetRequestId() const
{
    return RequestCast::Ref(this).GetRequestId();
}

void IRequest::SetRequestId(int requestId)
{
    RequestCast::Ref(this).SetRequestId(requestId);
}

int IRequest::GetOperationId() const
{
    return RequestCast::Ref(this).GetOperationId();
}

void IRequest::SetOperationId(int operationId)
{
    RequestCast::Ref(this).SetOperationId(operationId);
}

long long IRequest::GetTransactionId() const
{
    return RequestCast::Ref(this).GetTransactionId();
}

void IRequest::SetTransactionId(long long transactionId)
{
    RequestCast::Ref(this).SetTransactionId(transactionId);
}

int IRequest::GetAlgoPluginType() const
{
    return RequestCast::Ref(this).GetAlgoPluginType();
}

void IRequest::SetAlgoPluginType(int type)
{
    RequestCast::Ref(this).SetAlgoPluginType(type);
}

const DataInfo &IRequest::GetMsg() const
{
    return RequestCast::Ref(this).GetMsg();
}

void IRequest::SetMsg(const DataInfo &msg)
{
    RequestCast::Ref(this).SetMsg(msg);
}
} // namespace AI
} // namespace OHOS