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

#ifndef PCM_ITERATOR_H
#define PCM_ITERATOR_H

#include "ai_datatype.h"

namespace OHOS {
namespace AI {
class PCMIterator {
public:
    PCMIterator();
    virtual ~PCMIterator();
    int32_t Init(size_t stepSize, size_t windowSize);
    void Release();
    int32_t SetInput(const Array<int16_t> &pcmData);
    void Reset();
    bool HasNext();
    Array<int16_t> Next();

private:
    int32_t MoveDataToCache(const Array<int16_t> &input);
    int32_t Prepare(const Array<int16_t> &input);
    size_t stepSize_;
    size_t windowSize_;
    size_t numCopy_;
    size_t numRealCopy_;
    size_t nextCachePos_;
    int32_t maxCacheSize_;
    bool isInitialized_;
    bool hasNext_;
    Array<int16_t> pcmCache_ = {0};
    Array<int16_t> pcmData_ = {0};
};
} // namespace AI
} // namespace OHOS
#endif // PCM_ITERATOR_H