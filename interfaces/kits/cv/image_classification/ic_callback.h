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
 * @file ic_callback.h
 *
 * @brief Defines the callback for processing errors and prediction results of image classification.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef IC_CALLBACK_H
#define IC_CALLBACK_H

#include "ic_constants.h"
#include "ic_retcode.h"

namespace OHOS {
namespace AI {
class IcCallback {
public:
    /**
     * @brief Defines the destructor for the callback.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~IcCallback() = default;

    /**
     * @brief Defines the callback for processing errors.
     *
     * @param errorCode Indicates the error code defined by {@link IcRetCode} for the image classification task.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual void OnError(const IcRetCode errorCode) = 0;

    /**
     * @brief Defines the callback for prediction results.
     *
     * @param result Indicates the prediction result.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual void OnResult(const IcOutput &result) = 0;
};
} // namespace AI
} // namespace OHOS

#endif // IC_CALLBACK_H
/** @} */