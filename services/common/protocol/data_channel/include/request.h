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

#ifndef REQUEST_H
#define REQUEST_H

#include "protocol/data_channel/include/i_request.h"

namespace OHOS {
namespace AI {
class Request {
public:
    Request();
    ~Request();

    /**
     * Get inner sequence Id, which is globally unique.
     * Inner sequence Id indicates the future class, which is used to process asynchronous tasks.
     *
     * @return Inner sequence Id.
     */
    long long GetInnerSequenceId() const;

    /**
     * Set inner sequence Id, which is globally unique.
     *
     * @param [in] seqId Inner sequence Id.
     */
    void SetInnerSequenceId(long long seqId);

    /**
     * Get request Id.
     *
     * @return Request Id.
     */
    int GetRequestId() const;

    /**
     * Set request Id.
     *
     * @param [in] requestId Request Id.
     */
    void SetRequestId(int requestId);

    /**
     * Get request operation Id.
     *
     * @return Request operation Id.
     */
    int GetOperationId() const;

    /**
     * Set request operation Id.
     *
     * @param [in] operationId Operation Id.
     */
    void SetOperationId(int operationId);

    /**
     * Get request transaction Id.
     *
     * @return Request transaction Id.
     */
    long long GetTransactionId() const;

    /**
     * Set request transaction Id.
     *
     * @param [in] transactionId Transaction Id.
     */
    void SetTransactionId(long long transactionId);

    /**
     * Get algorithm plugin type.
     *
     * @return Algorithm plugin type.
     */
    int GetAlgoPluginType() const;

    /**
     * Set algorithm plugin type.
     *
     * @param [in] type Algorithm plugin type.
     */
    void SetAlgoPluginType(int type);

    /**
     * Get the message body carried by this request.
     *
     * @return Message.
     */
    const DataInfo &GetMsg() const;

    /**
     * Get the message body carried by this request.
     *
     * @param [in] msg Message.
     */
    void SetMsg(const DataInfo &msg);

private:
    long long innerSequenceId_;
    int requestId_;
    int operationId_;
    long long transactionId_;
    int algoPluginType_;
    DataInfo msg_;
};
} // namespace AI
} // namespace OHOS

#endif // REQUEST_H