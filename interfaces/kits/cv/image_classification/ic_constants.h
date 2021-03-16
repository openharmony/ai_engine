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
 * @file ic_constants.h
 *
 * @brief Defines the constants and, input and output formats for calling the image classification plugin.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef IC_CONSTANTS_H
#define IC_CONSTANTS_H

#include <cstddef>
#include <cstdint>
#include <vector>

#include "ai_datatype.h"

namespace OHOS {
namespace AI {
/**
 * @brief Defines the version number of the image classification client.
 *
 * @since 2.2
 * @version 1.0
 */
const long long CLIENT_VERSION_IC = 20001003;

/**
 * @brief Defines the version number of the image classification algorithm.
 *
 * @since 2.2
 * @version 1.0
 */
const long long ALGOTYPE_VERSION_IC = 20001001;

/**
 * @brief Defines the value of invalid handles.
 *
 * @since 2.2
 * @version 1.0
 */
const intptr_t INVALID_IC_HANDLE = -1;

/**
 * @brief Defines the input format for image classification.
 *
 * @since 2.2
 * @version 1.0
 */
using IcInput = Array<uint8_t>;

/**
 * @brief Defines the output format for image classification.
 *
 * @since 2.2
 * @version 1.0
 */
using IcOutput = Array<int32_t>;
} // namespace AI
} // namespace OHOS

#endif // IC_CONSTANTS_H
/** @} */