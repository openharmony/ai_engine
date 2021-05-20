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

#ifndef I_REQUEST_H
#define I_REQUEST_H

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class IRequest {
    FORBID_COPY_AND_ASSIGN(IRequest);
    FORBID_CREATE_BY_SELF(IRequest);
public:
    /**
     * Plugins prohibit the use of new, so add a create method here.
     *
     * @return A request object.
     */
    static IRequest *Create();

    /**
     * Destroy request method.
     *
     * @param [in,out] request Destroyed request object.
     */
    static void Destroy(IRequest *&request);

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
     * Operation Id indicates which plugin algorithm function to operate.
     *
     * @return Request operation Id.
     */
    int GetOperationId() const;

    /**
     * Set request operation Id.
     *
     * @param [in] OperationId Operation Id.
     */
    void SetOperationId(int OperationId);

    /**
     * Get client uid.
     *
     * @return Client uid.
     */
    uid_t GetClientUid() const;

    /**
     * Set client uid.
     *
     * @param [in] Client uid.
     */
    void SetClientUid(const uid_t clientUid);

    /**
     * Get request transaction Id.
     * The transaction Id indicates which client the task is from.
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
};
} // namespace AI
} // namespace OHOS

#endif // I_REQUEST_H
