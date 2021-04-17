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

#ifndef RESPONSE_H
#define RESPONSE_H

#include <string>

#include "protocol/data_channel/include/i_request.h"

namespace OHOS {
namespace AI {
class Response {
public:
    explicit Response(IRequest *request);
    ~Response();

    /**
     * Get request Id.
     *
     * @return Request Id.
     */
    int GetRequestId() const;

    /**
     * Get inner sequence Id, which is globally unique.
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
     * Get response return code.
     *
     * @return Response return code.
     */
    int GetRetCode() const;

    /**
     * Set response return code.
     *
     * @param [in] retCode Response return code.
     */
    void SetRetCode(int retCode);

    /**
     * Get client uid.
     *
     * @return Client uid.
     */
    uid_t GetClientUid() const;

    /**
     * Set client uid.
     *
     * @param [in] clientUid Client uid.
     */
    void SetClientUid(const uid_t clientUid);

    /**
     * Get response return description.
     *
     * @return Response return description.
     */
    const std::string &GetRetDesc() const;

    /**
     * Set response return description.
     *
     * @param [in] retDesc Response return description.
     */
    void SetRetDesc(const std::string &retDesc);

    /**
     * Get response result.
     *
     * @return Response result.
     */
    const DataInfo &GetResult() const;

    /**
     * Set response result.
     *
     * @param [in] resLen Message length.
     */
    void SetResult(const DataInfo &result);

    /**
     * detach DataInfo data ptr
     */
    void Detach();

private:
    int requestId_;
    long long innerSequenceId_;
    long long transactionId_;
    int retCode_;
    uid_t clientUid_ = 0;
    std::string retDesc_;
    int algoPluginType_;
    DataInfo result_;
};
} // namespace AI
} // namespace OHOS

#endif // RESPONSE_H
