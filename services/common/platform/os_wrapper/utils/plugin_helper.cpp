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

#include "plugin_helper.h"

using namespace OHOS::AI;

#define ARRAY_DATA_DECODE_IMPL(type)                                          \
template<>                                                                    \
int32_t DataDecoder::DecodeOneParameter(Array<type> &val)                     \
{                                                                             \
    if (val.size != 0 || val.data != nullptr) {                               \
        HILOGE("[PluginHelper]Fail to decode with non-empty data");           \
        return RETCODE_FAILURE;                                               \
    }                                                                         \
    if (RecursiveDecode(val.size) != RETCODE_SUCCESS) {                       \
        HILOGE("[PluginHelper]Fail to decode with illegal arraySize");        \
        return RETCODE_FAILURE;                                               \
    }                                                                         \
    AIE_NEW(val.data, type[val.size]);                                        \
    if (val.data == nullptr) {                                                \
        HILOGE("[PluginHelper]Fail to allocate buffer for decoder");          \
        return RETCODE_FAILURE;                                               \
    }                                                                         \
    for (size_t i = 0; i < val.size; ++i) {                                   \
        if (DecodeOneParameter(val.data[i]) != RETCODE_SUCCESS) {             \
            HILOGE("[PluginHelper]Fail to decode arrayData at index %zu", i); \
            AIE_DELETE_ARRAY(val.data);                                       \
            return RETCODE_FAILURE;                                           \
        }                                                                     \
    }                                                                         \
    return RETCODE_SUCCESS;                                                   \
}

#define ARRAY_DATA_ENCODE_IMPL(type)                                          \
template<>                                                                    \
int32_t DataEncoder::EncodeOneParameter(const Array<type> &val)               \
{                                                                             \
    if (val.size == 0 || val.data == nullptr) {                               \
        HILOGE("[PluginHelper]Fail to encode with empty data");               \
        return RETCODE_FAILURE;                                               \
    }                                                                         \
    if (RecursiveEncode(val.size) != RETCODE_SUCCESS) {                       \
        HILOGE("[PluginHelper]Fail to encode with illegal arraySize");        \
        return RETCODE_FAILURE;                                               \
    }                                                                         \
    for (size_t i = 0; i < val.size; ++i) {                                   \
        if (EncodeOneParameter(val.data[i]) != RETCODE_SUCCESS) {             \
            HILOGE("[PluginHelper]Fail to encode arrayData at index %zu", i); \
            return RETCODE_FAILURE;                                           \
        }                                                                     \
    }                                                                         \
    return RETCODE_SUCCESS;                                                   \
}

namespace {
    using Item = std::pair<int32_t, int32_t>;
    using Items = std::vector<Item>;
    const uint8_t PAIR_SIZE = 2;
}

template<>
int32_t DataEncoder::EncodeOneParameter(const Items &outputData)
{
    if (RecursiveEncode(outputData.size() * PAIR_SIZE) != RETCODE_SUCCESS) {
        HILOGE("[PluginHelper]Fail to encode with illegal arraySize");
        return RETCODE_FAILURE;
    }
    for (size_t i = 0; i < outputData.size(); ++i) {
        if (EncodeOneParameter(outputData[i].first) != RETCODE_SUCCESS) {
            HILOGE("[PluginHelper]Fail to encode labels from outputData");
            return RETCODE_FAILURE;
        }
    }
    for (size_t i = 0; i < outputData.size(); ++i) {
        if (EncodeOneParameter(outputData[i].second) != RETCODE_SUCCESS) {
            HILOGE("[PluginHelper]Fail to encode scores from outputData");
            return RETCODE_FAILURE;
        }
    }
    return RETCODE_SUCCESS;
}

ARRAY_DATA_ENCODE_IMPL(uint8_t);

ARRAY_DATA_ENCODE_IMPL(uint16_t);

ARRAY_DATA_ENCODE_IMPL(uint32_t);

ARRAY_DATA_ENCODE_IMPL(int16_t);

ARRAY_DATA_ENCODE_IMPL(int32_t);

ARRAY_DATA_DECODE_IMPL(uint8_t);

ARRAY_DATA_DECODE_IMPL(uint16_t);

ARRAY_DATA_DECODE_IMPL(uint32_t);

ARRAY_DATA_DECODE_IMPL(int16_t);

ARRAY_DATA_DECODE_IMPL(int32_t);