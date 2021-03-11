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

#ifndef I_FUTURE_H
#define I_FUTURE_H

#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"

namespace OHOS {
namespace AI {
enum FutureStatus {
    FUTURE_OK,
    FUTURE_ERROR,
};

class IFuture {
public:
    virtual ~IFuture() = default;

    /**
     * Get response some time later.
     *
     * @param [in] timeOut Delayed time, measured by millisecond.
     * @return Return response pointer.
     */
    virtual IResponse *GetResponse(int timeOut) const = 0;

    /**
     * Release resources.
     */
    virtual void Release() = 0;

    /**
     * Get future status for async execution response.
     *
     * @return Returns FUTURE_OK if the operation is successful, returns FUTURE_ERROR otherwise.
     */
    virtual FutureStatus Status() const = 0;
};
} // namespace AI
} // namespace OHOS

#endif // I_FUTURE_H