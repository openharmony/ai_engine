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

#include "type_converter.h"

#include <algorithm>
#include <memory>
#include <numeric>
#include <vector>

#include "aie_log.h"
#include "aie_macros.h"
#include "aie_retcode_inner.h"

using namespace OHOS::AI::Feature;

namespace {
#define TYPE_CONVERT_ITEMS(input, output, indices)                                \
    do {                                                                          \
        std::for_each(indices.begin(), indices.end(), [input, output](size_t i) { \
            output[i] = input[i];                                                 \
        });                                                                       \
    } while (0)
}

template<typename InType>
static int32_t DoCast(const InType *inputAddr, size_t inputSize, FeatureData &buffer)
{
    std::vector<size_t> indices(inputSize);
    std::iota(indices.begin(), indices.end(), 0);
    switch (buffer.dataType) {
        case UINT8: {
            auto outputAddr = static_cast<uint8_t *>(buffer.data);
            TYPE_CONVERT_ITEMS(inputAddr, outputAddr, indices);
            break;
        }
        case INT8: {
            auto outputAddr = static_cast<int8_t *>(buffer.data);
            TYPE_CONVERT_ITEMS(inputAddr, outputAddr, indices);
            break;
        }
        case UINT16: {
            auto outputAddr = static_cast<uint16_t *>(buffer.data);
            TYPE_CONVERT_ITEMS(inputAddr, outputAddr, indices);
            break;
        }
        case INT16: {
            auto outputAddr = static_cast<int16_t *>(buffer.data);
            TYPE_CONVERT_ITEMS(inputAddr, outputAddr, indices);
            break;
        }
        case UINT32: {
            auto outputAddr = static_cast<uint32_t *>(buffer.data);
            TYPE_CONVERT_ITEMS(inputAddr, outputAddr, indices);
            break;
        }
        case INT32: {
            auto outputAddr = static_cast<int32_t *>(buffer.data);
            TYPE_CONVERT_ITEMS(inputAddr, outputAddr, indices);
            break;
        }
        case FLOAT: {
            auto outputAddr = static_cast<float *>(buffer.data);
            TYPE_CONVERT_ITEMS(inputAddr, outputAddr, indices);
            break;
        }
        default:
            return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

TypeConverter::TypeConverter(): isInitialized_(false)
{
    workBuffer_ = {
        .dataType = UNKNOWN,
        .data = nullptr,
        .size = 0,
    };
}

TypeConverter::~TypeConverter()
{
    Release();
}

int32_t TypeConverter::Init(const FeatureProcessorConfig *config)
{
    if (isInitialized_) {
        HILOGE("[TypeConverter]Fail to initialize more than once. Release it, then try again");
        return RETCODE_FAILURE;
    }
    if (config == nullptr) {
        HILOGE("[TypeConverter]Fail with null config pointer");
        return RETCODE_FAILURE;
    }
    auto localConfig = *(static_cast<const TypeConverterConfig *>(config));
    if (localConfig.dataType == UNKNOWN) {
        HILOGE("[TypeConverter]The configured dataType[UNKNOWN] is unsupported");
        return RETCODE_FAILURE;
    }
    workBuffer_.dataType = localConfig.dataType;
    workBuffer_.size = localConfig.size;
    uint8_t typeSize = CONVERT_DATATYPE_TO_SIZE(workBuffer_.dataType);
    size_t bufferSize = workBuffer_.size * typeSize;
    if (bufferSize > MAX_SAMPLE_SIZE) {
        HILOGE("[TypeConverter]The required memory size is larger than MAX_SAMPLE_SIZE[%zu]",
            MAX_SAMPLE_SIZE);
        return RETCODE_FAILURE;
    }
    AIE_NEW(workBuffer_.data, uint8_t[bufferSize]);
    if (workBuffer_.data == nullptr) {
        HILOGE("[TypeConverter]Fail to allocate memory for workBuffer");
        return RETCODE_FAILURE;
    }
    isInitialized_ = true;
    return RETCODE_SUCCESS;
}

void TypeConverter::Release()
{
    if (isInitialized_) {
        auto bufferAddr = static_cast<uint8_t *>(workBuffer_.data);
        AIE_DELETE_ARRAY(bufferAddr);
        isInitialized_ = false;
    }
}

int32_t TypeConverter::Process(const FeatureData &input, FeatureData &output)
{
    if (output.data != nullptr || output.size != 0) {
        HILOGE("[TypeConverter]Fail with non-empty output");
        return RETCODE_FAILURE;
    }
    if (input.data == nullptr || input.size == 0) {
        HILOGE("[TypeConverter]Fail to process with nullptr input");
        return RETCODE_FAILURE;
    }
    if (input.size != workBuffer_.size) {
        HILOGE("[TypeConverter]The input size[%zu] is not equal to the output size[%zu]",
            input.size, workBuffer_.size);
        return RETCODE_FAILURE;
    }
    if (InnerProcess(input) != RETCODE_SUCCESS) {
        HILOGE("[TypeConverter]Fail to convert dataType from input to output");
        return RETCODE_FAILURE;
    }
    output.dataType = workBuffer_.dataType;
    output.data = workBuffer_.data;
    output.size = workBuffer_.size;
    return RETCODE_SUCCESS;
}

int32_t TypeConverter::InnerProcess(const FeatureData &input)
{
    switch (input.dataType) {
        case UINT8:
            return DoCast<uint8_t>(static_cast<uint8_t *>(input.data), input.size, workBuffer_);
        case INT8:
            return DoCast<int8_t>(static_cast<int8_t *>(input.data), input.size, workBuffer_);
        case UINT16:
            return DoCast<uint16_t>(static_cast<uint16_t *>(input.data), input.size, workBuffer_);
        case INT16:
            return DoCast<int16_t>(static_cast<int16_t *>(input.data), input.size, workBuffer_);
        case UINT32:
            return DoCast<uint32_t>(static_cast<uint32_t *>(input.data), input.size, workBuffer_);
        case INT32:
            return DoCast<int32_t>(static_cast<int32_t *>(input.data), input.size, workBuffer_);
        case FLOAT:
            return DoCast<float>(static_cast<float *>(input.data), input.size, workBuffer_);
        default:
            HILOGE("[TypeConverter]Fail with unknown input type");
            return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}