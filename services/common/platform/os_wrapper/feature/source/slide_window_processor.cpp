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

#include "slide_window_processor.h"

#include <memory>

#include "aie_log.h"
#include "aie_macros.h"
#include "aie_retcode_inner.h"
#include "securec.h"

using namespace OHOS::AI::Feature;

namespace {
    const uint8_t ILLEGAL_BUFFER_MULTIPLIER = 0;
}

SlideWindowProcessor::SlideWindowProcessor()
    : isInitialized_(false),
      workBuffer_(nullptr),
      inputFeature_(nullptr),
      inType_(UNKNOWN),
      typeSize_(0),
      startIndex_(0),
      initIndex_(0),
      bufferSize_(0),
      windowSize_(0),
      stepSize_(0) {}

SlideWindowProcessor::~SlideWindowProcessor()
{
    Release();
}

int32_t SlideWindowProcessor::Init(const FeatureProcessorConfig *config)
{
    if (isInitialized_) {
        HILOGE("[SlideWindowProcessor]Fail to init more than once. Release it, then try again");
        return RETCODE_FAILURE;
    }
    if (config == nullptr) {
        HILOGE("[SlideWindowProcessor]Fail to init with null config");
        return RETCODE_FAILURE;
    }
    auto localConfig = *(static_cast<const SlideWindowProcessorConfig *>(config));
    if (localConfig.dataType == UNKNOWN) {
        HILOGE("[SlideWindowProcessor]Fail with unsupported dataType[UNKNOWN]");
        return RETCODE_FAILURE;
    }
    if (localConfig.windowSize < localConfig.stepSize) {
        HILOGE("[SlideWindowProcessor]Illegal configuration. The stepSize cannot be greater than windowSize");
        return RETCODE_FAILURE;
    }
    if (localConfig.bufferMultiplier == ILLEGAL_BUFFER_MULTIPLIER) {
        HILOGE("[SlideWindowProcessor]Illegal configuration. The bufferMultiplier cannot be zero");
        return RETCODE_FAILURE;
    }
    inType_ = localConfig.dataType;
    typeSize_ = CONVERT_DATATYPE_TO_SIZE(inType_);
    windowSize_ = localConfig.windowSize;
    stepSize_ = localConfig.stepSize * typeSize_;
    initIndex_ = (windowSize_ * typeSize_) - stepSize_;
    startIndex_ = initIndex_;
    bufferSize_ = (windowSize_ * localConfig.bufferMultiplier) * typeSize_;
    if (windowSize_ > MAX_SAMPLE_SIZE) {
        HILOGE("[SlideWindowProcessor]The required memory size is larger than MAX_SAMPLE_SIZE[%zu]",
            MAX_SAMPLE_SIZE);
        return RETCODE_FAILURE;
    }
    AIE_NEW(workBuffer_, char[bufferSize_]);
    if (workBuffer_ == nullptr) {
        HILOGE("[SlideWindowProcessor]Fail to allocate memory for workBuffer");
        return RETCODE_FAILURE;
    }
    (void)memset_s(workBuffer_, bufferSize_, 0, bufferSize_);
    inputFeature_ = workBuffer_;
    isInitialized_ = true;
    return RETCODE_SUCCESS;
}

void SlideWindowProcessor::Release()
{
    AIE_DELETE_ARRAY(workBuffer_);
    inputFeature_ = nullptr;
    isInitialized_ = false;
}

int32_t SlideWindowProcessor::Process(const FeatureData &input, FeatureData &output)
{
    if (!isInitialized_) {
        HILOGE("[SlideWindowProcessor]Fail to process without successfully init");
        return RETCODE_FAILURE;
    }
    if (input.dataType != inType_) {
        HILOGE("[SlideWindowProcessor]Fail with unmatched input dataType");
        return RETCODE_FAILURE;
    }
    if (input.data == nullptr || input.size == 0) {
        HILOGE("[SlideWindowProcessor]Fail with NULL input");
        return RETCODE_FAILURE;
    }
    size_t inputBytes = input.size * typeSize_;
    if (inputBytes != stepSize_) {
        HILOGE("[SlideWindowProcessor]Fail with unmatched input dataSize, expected [%zu]", stepSize_ / typeSize_);
        return RETCODE_FAILURE;
    }
    if (output.data != nullptr || output.size != 0) {
        HILOGE("[SlideWindowProcessor]Fail with non-empty output");
        return RETCODE_FAILURE;
    }
    output.dataType = inType_;
    // Update the last window by the input data of new window
    errno_t retCode = memcpy_s(workBuffer_ + startIndex_, bufferSize_ - startIndex_, input.data, inputBytes);
    if (retCode != EOK) {
        HILOGE("[SlideWindowProcessor]Fail to copy input data to workBuffer [%d]", retCode);
        return RETCODE_FAILURE;
    }
    output.data = inputFeature_;
    output.size = windowSize_;
    startIndex_ += stepSize_;
    // Slide to the next position
    inputFeature_ += stepSize_;
    // Post-processing
    if (bufferSize_ - startIndex_ < stepSize_) {
        retCode = memmove_s(workBuffer_, bufferSize_, inputFeature_, initIndex_);
        if (retCode != EOK) {
            HILOGE("[SlideWindowProcessor]Fail with memory move. Error code[%d]", retCode);
            return RETCODE_FAILURE;
        }
        startIndex_ = initIndex_;
        inputFeature_ = workBuffer_;
    }
    return RETCODE_SUCCESS;
}