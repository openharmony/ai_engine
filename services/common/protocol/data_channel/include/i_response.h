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

#ifndef I_RESPONSE_H
#define I_RESPONSE_H

#include <string>

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class IRequest;

class IResponse {
    FORBID_COPY_AND_ASSIGN(IResponse);
    FORBID_CREATE_BY_SELF(IResponse);
public:
    /**
     * Plugins prohibit the use of new, so add a create method here.
     *
     * @return A response object.
     */
    static IResponse *Create(IRequest *request);

    /**
     * Destroy response method.
     *
     * @param [in,out] response Destroyed response object.
     */
    static void Destroy(IResponse *&response);

    /**
     * Get request Id.
     *
     * @return Request Id.
     */
    int GetRequestId() const;

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
     * @param [in] result Response result.
     */
    void SetResult(const DataInfo &result);

    /**
     * detach DataInfo data ptr
     */
    void Detach();
};
} // namespace AI
} // namespace OHOS

#endif // I_RESPONSE_H