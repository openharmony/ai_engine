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

#ifndef PLUGIN_HELPER_H
#define PLUGIN_HELPER_H

#include <cstdint>
#include <utility>
#include <vector>

#include "ai_datatype.h"
#include "aie_info_define.h"
#include "aie_macros.h"
#include "data_decoder.h"
#include "data_encoder.h"

namespace OHOS {
namespace AI {
/**
 *
 * @brief Defines the basic config parameters for <b>Plugin</b>s.
 *
 *
 * @since 1.0
 * @version 1.0
 */
struct PluginConfig {
    // Indicates the size of inputData
    size_t inputSize;
    // Indicates the size of outputData
    size_t outputSize;
    // Indicates the start address of input
    uintptr_t inputAddr;
    // Indicates the start address of output
    uintptr_t outputAddr;
};

template<>
int32_t DataEncoder::EncodeOneParameter(const std::vector<std::pair<int32_t, int32_t>> &outputData);

template<>
int32_t DataDecoder::DecodeOneParameter(Array<uint8_t> &val);

template<>
int32_t DataDecoder::DecodeOneParameter(Array<uint16_t> &val);

template<>
int32_t DataDecoder::DecodeOneParameter(Array<uint32_t> &val);

template<>
int32_t DataDecoder::DecodeOneParameter(Array<int16_t> &val);

template<>
int32_t DataDecoder::DecodeOneParameter(Array<int32_t> &val);

template<>
int32_t DataEncoder::EncodeOneParameter(const Array<uint8_t> &val);

template<>
int32_t DataEncoder::EncodeOneParameter(const Array<uint16_t> &val);

template<>
int32_t DataEncoder::EncodeOneParameter(const Array<uint32_t> &val);

template<>
int32_t DataEncoder::EncodeOneParameter(const Array<int16_t> &val);

template<>
int32_t DataEncoder::EncodeOneParameter(const Array<int32_t> &val);
}
}
#endif // PLUGIN_HELPER_H