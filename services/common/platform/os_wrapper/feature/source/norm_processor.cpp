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

#include "norm_processor.h"

#include <climits>
#include <cstdio>
#include <cstdlib>

#include "aie_log.h"
#include "aie_macros.h"
#include "aie_retcode_inner.h"

using namespace OHOS::AI::Feature;

namespace {
    const char NUMBER_DELIMITER = ' ';
    const float EPSILON = 1e-6;
}

static int32_t LoadInfoFromFile(FILE *&fp, float *&data, size_t length)
{
    size_t count = 0;
    char c = '\0';
    std::string token = "";
    do {
        size_t readLen = fread(&c, sizeof(char), 1, fp);
        if (readLen == 0u) {
            break;
        }
        if (c == NUMBER_DELIMITER) {
            data[count++] = atof(token.c_str());
            token = "";
            continue;
        }
        token += c;
    } while (count < length);
    if (token.empty()) {
        return count;
    }
    if (count >= length) {
        HILOGW("[NormProcessor]Lost value %s", token.c_str());
        count = length;
    } else {
        data[count++] = atof(token.c_str());
    }
    return count;
}

static int32_t ReadFixedLenFloatData(const std::string &filePath, float *&data, size_t length)
{
    char realPath[PATH_MAX + 1] = {0};
    if (realpath(filePath.c_str(), realPath) == nullptr) {
        HILOGE("[NormProcessor]Invalid filePath [%s]", filePath.c_str());
        return RETCODE_FAILURE;
    }
    FILE *fp = fopen(realPath, "rb");
    if (fp == nullptr) {
        HILOGE("[NormProcessor]File [%s] not exists", realPath);
        return RETCODE_FAILURE;
    }
    size_t readLen = LoadInfoFromFile(fp, data, length);
    fclose(fp);
    if (readLen != length) {
        HILOGE("[NormProcessor]The data length is not equal (got %zu, but expected %zu)", readLen, length);
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

static int32_t LoadMeanAndStd(const std::string &meanFilePath, const std::string &stdFilePath,
    float *&mean, float *&std, size_t length)
{
    int32_t retCode = ReadFixedLenFloatData(meanFilePath, mean, length);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[NormProcessor]Fail to load mean from file");
        return RETCODE_FAILURE;
    }
    retCode = ReadFixedLenFloatData(stdFilePath, std, length);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[NormProcessor]Fail to load std from file");
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

NormProcessor::NormProcessor()
    : isInitialized_(false),
      workBuffer_(nullptr),
      mean_(nullptr),
      std_(nullptr),
      converter_(nullptr)
{
    config_ = {};
}

NormProcessor::~NormProcessor()
{
    Release();
}

int32_t NormProcessor::Init(const FeatureProcessorConfig *config)
{
    if (isInitialized_) {
        HILOGE("[NormProcessor]Fail to initialize more than once. Release it, then try again");
        return RETCODE_FAILURE;
    }
    if (config == nullptr) {
        HILOGE("[NormProcessor]Fail with null config pointer");
        return RETCODE_FAILURE;
    }
    config_ = *(static_cast<const NormProcessorConfig *>(config));
    if (config_.inputSize == 0 || config_.numChannels == 0) {
        HILOGE("[NormProcessor]Illegal config, the inputSize or numChannels is 0");
        return RETCODE_FAILURE;
    }
    if (config_.inputSize % config_.numChannels != 0) {
        HILOGE("[NormProcessor]The inputSize cannot be divided by numChannels");
        return RETCODE_FAILURE;
    }
    TypeConverterConfig convertConfig;
    convertConfig.dataType = FLOAT;
    convertConfig.size = config_.inputSize;
    converter_ = std::unique_ptr<TypeConverter>(new (std::nothrow) TypeConverter());
    if (converter_ == nullptr) {
        HILOGE("[NormProcessor]Fail to create typeConverter");
        return RETCODE_FAILURE;
    }
    if (converter_->Init(&convertConfig) != RETCODE_SUCCESS) {
        HILOGE("[NormProcessor]Fail to initialize typeConverter");
        return RETCODE_FAILURE;
    }
    if (config_.numChannels > MAX_SAMPLE_SIZE || config_.inputSize > MAX_SAMPLE_SIZE) {
        HILOGE("[NormProcessor]The required memory size is larger than MAX_SAMPLE_SIZE[%zu]", MAX_SAMPLE_SIZE);
        return RETCODE_FAILURE;
    }
    AIE_NEW(mean_, float[config_.numChannels]);
    AIE_NEW(std_, float[config_.numChannels]);
    AIE_NEW(workBuffer_, float[config_.inputSize]);
    if (mean_ == nullptr || std_ == nullptr || workBuffer_ == nullptr) {
        HILOGE("[NormProcessor]Fail to allocate memory");
        Release();
        return RETCODE_FAILURE;
    }
    int32_t retCode = LoadMeanAndStd(config_.meanFilePath, config_.stdFilePath, mean_, std_, config_.numChannels);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[NormProcessor]Fail to load mean and std");
        Release();
        return RETCODE_FAILURE;
    }
    isInitialized_ = true;
    return RETCODE_SUCCESS;
}

void NormProcessor::Release()
{
    AIE_DELETE_ARRAY(workBuffer_);
    AIE_DELETE_ARRAY(mean_);
    AIE_DELETE_ARRAY(std_);
    converter_ = nullptr;
    isInitialized_ = false;
}

int32_t NormProcessor::Process(const FeatureData &input, FeatureData &output)
{
    if (!isInitialized_) {
        HILOGE("[NormProcessor]Fail to process without successfully init");
        return RETCODE_FAILURE;
    }
    if (output.data != nullptr || output.size != 0) {
        HILOGE("[NormProcessor]Fail with non-empty output");
        return RETCODE_FAILURE;
    }
    if (input.data == nullptr || input.size == 0) {
        HILOGE("[NormProcessor]Fail to process with empty input");
        return RETCODE_FAILURE;
    }
    if (input.dataType == UNKNOWN) {
        HILOGE("[NormProcessor]Fail to process with [UNKNOWN] dataType");
        return RETCODE_FAILURE;
    }
    if (input.size != config_.inputSize) {
        HILOGE("[NormProcessor]Fail with illegal input size");
        return RETCODE_FAILURE;
    }
    if (input.dataType == FLOAT) {
        output = input;
    } else {
        converter_->Process(input, output);
    }
    float *data = reinterpret_cast<float *>(output.data);
    float meanVal = 0.0f;
    float stdVal = 0.0f;
    for (size_t i = 0; i < output.size; ++i) {
        stdVal = std_[i % config_.numChannels];
        meanVal = mean_[i % config_.numChannels];
        workBuffer_[i] = (std::abs(stdVal) < EPSILON) ? 0.0f : ((data[i] - meanVal) / stdVal) * config_.scale;
    }
    output.data = static_cast<void *>(workBuffer_);
    output.dataType = FLOAT;
    return RETCODE_SUCCESS;
}