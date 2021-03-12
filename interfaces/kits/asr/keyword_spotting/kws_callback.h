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

/**
 * @addtogroup kws_sdk
 * @{
 *
 * @brief Defines the basic functions, constants, and error codes for the wakeup keyword spotting (KWS) SDK.
 *
 * @since 2.2
 * @version 1.0
 */

/**
 * @file kws_callback.h
 *
 * @brief Defines the KWS callback for inference results and result codes.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef KWS_CALLBACK_H
#define KWS_CALLBACK_H

#include "ai_datatype.h"

namespace OHOS {
namespace AI {
class KWSCallback {
public:
    /**
     * @brief Defines the destructor for the KWS callback.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~KWSCallback() = default;

    /**
     * @brief Defines the KWS callback for inference errors.
     *
     * @param errorCode Indicates the error code defined by {@link KWSRetCode} for the KWS task.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual void OnError(int32_t errorCode) = 0;

    /**
     * @brief Defines the KWS callback for inference results.
     *
     * @param result Indicates the result array defined by {@link Array} for the KWS task.
     * The element type is {@link int32_t}.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual void OnResult(const Array<int32_t> &result) = 0;
};
} // namespace AI
} // namespace OHOS
#endif // KWS_CALLBACK_H
/** @} */