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

#ifndef AUDIO_WRAPPER_H
#define AUDIO_WRAPPER_H

#include <cstdint>
#include <memory>
#include <mutex>
#include <unordered_map>

#include "coder_wrapper.h"
#include "single_instance.h"

namespace OHOS {
namespace AI {
class AudioWrapper {
DECLARE_SINGLE_INSTANCE(AudioWrapper);

public:
    /* Initializes audio wrapper to convert AAC to PCM. */
    int32_t Init(ConvertType typeId, intptr_t &handler);

    /* Sets coder mode, indicate whether to CodecInit. */
    void SetCodecMode(bool needCodec);

    /* Sets configuration of specific coder. */
    int32_t SetConfig(intptr_t handler, const CoderConfig &config);

    /* Starts specific coder. */
    int32_t StartCodec(intptr_t handler);

    /* Convert CoderStream from source to converted. */
    int32_t Convert(intptr_t handler, const CoderStream &source, CoderStream &converted);

    /* Deinits specific coder. */
    int32_t Deinit(intptr_t handler);

    /* Clears all coders. */
    void Release();

private:
    void InitAudioDecoder(intptr_t &handler);

private:
    std::mutex mutex_;
    std::unordered_map<intptr_t, CoderWrapper *> coders_;
    bool hasInitCodec_ = false;
    bool needCodec_ = true;
};
} // namespace AI
} // namespace OHOS
#endif // AUDIO_WRAPPER_H