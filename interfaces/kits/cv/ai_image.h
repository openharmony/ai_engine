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

#ifndef AI_IMAGE_H
#define AI_IMAGE_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace OHOS {
namespace AI {
typedef enum {
    YUV_CHANNEL_Y = 0,
    YUV_CHANNEL_U = 1,
    YUV_CHANNEL_V = 2,
    YUV_CHANNEL_COUNT = 3
} YuvChannels; // for AI_IMAGE_TYPE_YUV422SP format

typedef enum {
    // not supported for now.
    AI_IMAGE_BINARY = 0,

    // only 1 channel
    // memory length = width * height
    // no stride
    AI_IMAGE_GRAY = 1,

    // not supported for now.
    AI_IMAGE_TYPE_BGR = 2,

    // not supported for now.
    AI_IMAGE_TYPE_YUV420SP = 3,

    // 3 channels memory length: Y:1 / U:0.5 / V0.5
    // where width * height = Y
    // format: YYUV for every 2 pixels
    // where Y must be an even number.
    AI_IMAGE_TYPE_YUV422SP = 4
} AiImageType;

typedef struct {
    AiImageType type;
    unsigned char *phyAddr[YUV_CHANNEL_COUNT];
    unsigned int stride[YUV_CHANNEL_COUNT];
    unsigned int width;
    unsigned int height;
    unsigned long long pts;
} AiImage;

typedef struct {
    int topLeftX;
    int topLeftY;
    int topRightX;
    int topRightY;
    int bottomRightX;
    int bottomRightY;
    int bottomLeftX;
    int bottomLeftY;
} FourVertex;
} // namespace AI
} // namespace OHOS

#endif // AI_IMAGE_H
