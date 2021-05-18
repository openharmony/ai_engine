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
 * @addtogroup cr_sdk
 * @{
 *
 * @brief Defines the development tool functions, constants, and error codes for card rectification (CR).
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file cr_sdk.h
 *
 * @brief Defines the development tool functions for card rectification.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef CR_SDK_H
#define CR_SDK_H

#include <memory>

#include "cr_callback.h"
#include "cr_constants.h"

namespace OHOS {
namespace AI {
class CrSdk {
public:
    CrSdk() = default;

    ~CrSdk();

    /**
     * @brief Establishes a connection from the development tool to the AI server.
     *
     * @return Returns {@link CR_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link CrRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t Prepare();

    /**
     * @brief Imports images from the development tool to the card rectification model for prediction.
     *
     * @param [in] input Indicates the input image.
     * @param [out] output Indicates the result.
     * @return Returns {@link CR_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link CrRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t RectifyCardSync(const RectifyCardInput &input, RectifyCardOutput &output);

    /**
     * @brief Defines the asynchronous function. No impletementation for now.
     *
     * @param [in] input Indicates the input image.
     * @param [in] callback Indicates the specified callback.
     * @return Returns {@link CR_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link CrRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t RectifyCardAsync(const RectifyCardInput &input, const ICallback &callback);

    /**
     * @brief Destroys the loaded model and related resources.
     *
     * @return Returns {@link CR_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link CrRetCode} otherwise.
     *
     * @since 1.0
     * @version 1.0
     */
    int32_t Release();
};
} // namespace AI
} // namespace OHOS

#endif // CR_SDK_H
/** @} */