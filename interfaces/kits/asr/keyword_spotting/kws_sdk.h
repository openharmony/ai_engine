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
 * @file kws_sdk.h
 *
 * @brief Defines the basic functions for the KWS SDK.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef KWS_SDK_H
#define KWS_SDK_H

#include <memory>

#include "ai_datatype.h"
#include "kws_callback.h"

namespace OHOS {
namespace AI {
class KWSSdk {
public:
    /**
     * @brief Defines the constructor for the KWS SDK.
     *
     * @since 2.2
     * @version 1.0
     */
    KWSSdk();

    /**
     * @brief Defines the destructor for the KWS SDK.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~KWSSdk();

    /**
     * @brief Creates a KWS SDK instance.
     *
     * @return Returns {@link KWS_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link KWSRetCode} otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t Create();

    /**
     * @brief Synchronously executes the KWS task.
     *
     * @param input Indicates the input array defined by {@link Array} for the KWS task. The element type is int16_t.
     * @return Returns {@link KWS_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link KWSRetCode} otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t SyncExecute(const Array<int16_t> &input);

    /**
     * @brief Sets the callback for the KWS task.
     *
     * @param callback Indicates the callback defined by {@link KWSCallback} for implementing the post-processing logic.
     * @return Returns {@link KWS_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link KWSRetCode} otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t SetCallback(const std::shared_ptr<KWSCallback> &callback);

    /**
     * @brief Destroys the KWS SDK instance to release the session engaged with the plugin.
     *
     * @return Returns {@link KWS_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link KWSRetCode} otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t Destroy();
private:
    class KWSSdkImpl;
    std::unique_ptr<KWSSdkImpl> kwsSdkImpl_;
};
} // namespace AI
} // namespace OHOS
#endif // KWS_SDK_H
/** @} */