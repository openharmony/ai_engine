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

#include "pcm_iterator.h"

#include <memory>

#include "aie_log.h"
#include "aie_macros.h"
#include "aie_retcode_inner.h"
#include "securec.h"

namespace OHOS {
namespace AI {
namespace {
const int32_t MAX_CACHE_SIZE = 16384; // cache data type is int16_t, so the max size is 32KB
}

PCMIterator::PCMIterator()
    : stepSize_(1),
      windowSize_(1),
      numCopy_(0),
      numRealCopy_(0),
      nextCachePos_(0),
      maxCacheSize_(0),
      isInitialized_(false),
      hasNext_(false)
{
}

PCMIterator::~PCMIterator()
{
    Release();
}

int32_t PCMIterator::Init(size_t stepSize, size_t windowSize)
{
    if (isInitialized_) {
        HILOGE("[PCMIterator]Fail to initialize more than once");
        return RETCODE_FAILURE;
    }
    if (stepSize == 0 || windowSize == 0) {
        HILOGE("[PCMIterator]StepSize and WindowSize should be greater than zero");
        return RETCODE_FAILURE;
    }
    if (windowSize < stepSize) {
        HILOGE("[PCMIterator]StepSize can not be greater than WindowSize");
        return RETCODE_FAILURE;
    }
    stepSize_ = stepSize;
    windowSize_ = windowSize;
    size_t maxNumRemain = windowSize_ - 1;
    size_t maxNumSlide = (maxNumRemain - 1) / stepSize_;
    maxCacheSize_ = maxNumSlide * stepSize_ + windowSize_;
    if (maxCacheSize_ > MAX_CACHE_SIZE) {
        HILOGE("[PCMIterator]The required memory size is larger than MAX_CACHE_SIZE[%zu]", MAX_CACHE_SIZE);
        return RETCODE_FAILURE;
    }
    AIE_NEW(pcmCache_.data, int16_t[maxCacheSize_]);
    if (pcmCache_.data == nullptr) {
        HILOGE("[PCMIterator]Fail to allocate memory for pcm cache");
        return RETCODE_OUT_OF_MEMORY;
    }
    isInitialized_ = true;
    return RETCODE_SUCCESS;
}

void PCMIterator::Release()
{
    if (isInitialized_) {
        AIE_DELETE_ARRAY(pcmCache_.data);
        Reset();
        isInitialized_ = false;
    }
}

int32_t PCMIterator::SetInput(const Array<int16_t> &input)
{
    if (!isInitialized_) {
        HILOGE("[PCMIterator]The iterator has not been initialized");
        return RETCODE_FAILURE;
    }
    if (HasNext()) {
        HILOGE("[PCMIterator]Reset the iterator, then set input again");
        return RETCODE_FAILURE;
    }
    if (input.data == nullptr || input.size == 0) {
        HILOGE("[PCMIterator]The input data is nullptr or its size is zero");
        return RETCODE_NULL_PARAM;
    }
    return Prepare(input);
}

int32_t PCMIterator::Prepare(const Array<int16_t> &input)
{
    // copy some data from the input to cache
    numRealCopy_ = 0;
    if (numCopy_ > 0) {
        numRealCopy_ = (numCopy_ > input.size) ? input.size : numCopy_;
        size_t realCacheSize = nextCachePos_ + pcmCache_.size;
        errno_t retCode = memcpy_s(pcmCache_.data + realCacheSize, (maxCacheSize_ - realCacheSize) * sizeof(int16_t),
            input.data, numRealCopy_ * sizeof(int16_t));
        if (retCode != EOK) {
            HILOGE("[PCMIterator]Fail to copy data from input to pcm cache");
            return RETCODE_MEMORY_COPY_FAILURE;
        }
        pcmCache_.size += numRealCopy_;
        numCopy_ -= numRealCopy_;
        if (numCopy_ == 0) {
            int32_t diff = numRealCopy_ - windowSize_ + stepSize_;
            if (diff > 0) {
                pcmData_.data = input.data + diff;
                pcmData_.size = input.size - diff;
            } else {
                pcmData_ = input;
            }
        } else {
            pcmData_ = {0};
        }
    } else {
        pcmData_ = input;
    }
    // update hasNext_
    if (windowSize_ <= pcmCache_.size || windowSize_ <= pcmData_.size) {
        hasNext_ = true;
        return RETCODE_SUCCESS;
    }
    // if pcm cache is empty and input size is larger than zero but less than window size, then move data to cache
    if (pcmCache_.size == 0 && pcmData_.size > 0 && pcmData_.size < windowSize_) {
        return MoveDataToCache(pcmData_);
    }
    return RETCODE_SUCCESS;
}

void PCMIterator::Reset()
{
    numCopy_ = 0;
    numRealCopy_ = 0;
    nextCachePos_ = 0;
    pcmCache_ = {0};
    pcmData_ = {0};
    hasNext_ = false;
}

bool PCMIterator::HasNext()
{
    // copy data from pcmCache_ or pcmData_ to pcmCache_
    if (nextCachePos_ > 0 && numCopy_ == 0 && pcmCache_.size > 0 && pcmCache_.size < windowSize_) {
        Array<int16_t> data = {
            .data = pcmCache_.data + nextCachePos_,
            .size = pcmCache_.size
        };
        MoveDataToCache(data);
        Prepare(pcmData_);
    } else {
        if (pcmCache_.size == 0 && pcmData_.size > 0 && pcmData_.size < windowSize_) {
            MoveDataToCache(pcmData_);
            pcmData_.size = 0;
        }
    }
    return hasNext_;
}

Array<int16_t> PCMIterator::Next()
{
    Array<int16_t> output = {0};
    if (!HasNext()) {
        return output;
    }
    if (windowSize_ <= pcmCache_.size) {
        output.data = pcmCache_.data + nextCachePos_;
        output.size = windowSize_;
        nextCachePos_ += stepSize_;
        pcmCache_.size -= stepSize_;
        if (numCopy_ == 0 && pcmCache_.size < windowSize_) {
            int32_t diff = windowSize_ - stepSize_ - numRealCopy_;
            pcmCache_.size = (diff > 0) ? diff : 0;
        }
    } else {
        if (windowSize_ <= pcmData_.size) {
            output.data = pcmData_.data;
            output.size = windowSize_;
            pcmData_.data += stepSize_;
            pcmData_.size -= stepSize_;
        }
    }
    if (pcmCache_.size < windowSize_ && pcmData_.size < windowSize_) {
        hasNext_ = false;
    }
    return output;
}

int32_t PCMIterator::MoveDataToCache(const Array<int16_t> &input)
{
    errno_t retCode = memcpy_s(pcmCache_.data, maxCacheSize_ * sizeof(int16_t),
                               input.data, input.size * sizeof(int16_t));
    if (retCode != EOK) {
        HILOGE("[PCMIterator]Fail to move data to cache");
        return RETCODE_MEMORY_COPY_FAILURE;
    }
    pcmCache_.size = input.size;
    nextCachePos_ = 0;
    size_t numSlide = (pcmCache_.size - 1) / stepSize_;
    numCopy_ = numSlide * stepSize_ + windowSize_ - pcmCache_.size; // data to be copied
    return RETCODE_SUCCESS;
}
} // namespace AI
} // namespace OHOS