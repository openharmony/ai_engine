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
 * @brief Defines the development tool functions, constants and error codes for card rectification (CR).
 *
 * @since 1.0
 * @version 1.0
 */

/**
 * @file cr_constants.h
 *
 * @brief Defines the constants and, input and output formats for calling the card rectification plugin.
 *
 * @since 1.0
 * @version 1.0
 */

#ifndef CR_CONSTANTS_H
#define CR_CONSTANTS_H

#include "cv/ai_image.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace OHOS {
namespace AI {
/**
 * @brief Defines the algorithm version for the card rectification task.
 *
 * it must match related_sessions in services/common/protocol/plugin_config/plugin_config_ini/cv_card_rectification.ini
 *
 * @since 1.0
 * @version 1.0
 */
const long long ALGORITHM_TYPE_VERSION_CR = 20001001;

typedef struct {
    AiImage image; // original image
    std::vector<std::vector<std::string>> templateTextZoom; // zero or more card text area templates
} RectifyCardInput;

typedef struct {
    AiImage image; // output image of card area
    FourVertex fourVertex; // coordinates of card in original image
    int classIndex; // index of templateTextZoom, indicating card type
    int pageIndex; // index of respondent cards
    float confidence; // confidence
} RectifyCardOutput;
} // namespace AI
} // namespace OHOS

#endif // CR_CONSTANTS_H
/** @} */