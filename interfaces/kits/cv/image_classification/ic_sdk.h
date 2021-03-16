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
 * @addtogroup ic_sdk
 * @{
 *
 * @brief Defines the development tool functions, constants, and error codes for image classification (IC).
 *
 * @since 2.2
 * @version 1.0
 */

/**
 * @file ic_sdk.h
 *
 * @brief Defines the development tool functions for image classification.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef IC_SDK_H
#define IC_SDK_H

#include <memory>

#include "ic_callback.h"

namespace OHOS {
namespace AI {
class IcSdk {
public:
    /**
     * @brief Defines the constructor for the development tool.
     *
     * @since 2.2
     * @version 1.0
     */
    IcSdk() = default;

    /**
     * @brief Defines the destructor for the development tool, release model, and related variables.
     *
     * @since 2.2
     * @version 1.0
     */
    ~IcSdk();

    /**
     * @brief Establishes a connection from the development tool to the AI server.
     *
     * @return Returns {@link IC_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link IcRetCode} otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t Create();

    /**
     * @brief Imports images from the development tool to the image classification model for prediction.
     *
     * @param picInput Indicates the input BGR image.
     * @return Returns {@link IC_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link IcRetCode} otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t SyncExecute(const IcInput &picInput);

    /**
     * @brief Defines the callback function.
     *
     * @param callback Indicates the specified callback.
     * @return Returns {@link IC_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link IcRetCode} otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t SetCallback(std::shared_ptr<IcCallback> callback);

    /**
     * @brief Destroys the loaded model and related resources.
     *
     * @return Returns {@link IC_RETCODE_SUCCESS} if the operation is successful;
     * returns a non-zero error code defined by {@link IcRetCode} otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t Destroy();
};
} // namespace AI
} // namespace OHOS

#endif // IC_SDK_H
/** @} */