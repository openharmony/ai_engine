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

#include "audio_wrapper.h"

#include <cstdio>
#include <unistd.h>

#include "aie_log.h"
#include "audio_retcode.h"
#include "decoder_wrapper.h"
#include "hal_display.h"

namespace OHOS {
namespace AI {
using namespace OHOS::Media;

IMPLEMENT_SINGLE_INSTANCE(AudioWrapper);

AudioWrapper::AudioWrapper()
{
    HalPlayerSysInit();
    HILOGD("[AudioWrapper]ctor");
}

AudioWrapper::~AudioWrapper()
{
    HILOGD("[AudioWrapper]dtor");
}

void AudioWrapper::SetCodecMode(bool needCodec)
{
    std::lock_guard<std::mutex> lock(mutex_);
    needCodec_ = needCodec;
}

int32_t AudioWrapper::Init(ConvertType typeId, intptr_t &handler)
{
    if (handler != INVALID_HANDLER) {
        return AUDIO_INIT_FAILURE;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    if (needCodec_ && !hasInitCodec_) {
        CodecInit();
        hasInitCodec_ = true;
    }
    switch (typeId) {
        case CONVERT_AAC_TO_PCM:
            InitAudioDecoder(handler);
            break;
        case CONVERT_PCM_TO_AAC:
            HILOGE("[AudioWrapper]Don't support convert pcm to aac now");
            return AUDIO_ILLEGAL_TYPE;
        default:
            return AUDIO_ILLEGAL_TYPE;
    }
    return AUDIO_SUCCESS;
}

void AudioWrapper::InitAudioDecoder(intptr_t &handler)
{
    CoderWrapper *converter = nullptr;
    AIE_NEW(converter, DecoderWrapper);
    if (converter == nullptr) {
        HILOGE("[AudioWrapper]InitAudioDecoder failed");
        return;
    }
    handler = (intptr_t)(converter);
    coders_.emplace(handler, converter);
}

void AudioWrapper::Release()
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto iter = coders_.begin(); iter != coders_.end(); ++iter) {
        if (iter->second->Stop() != AUDIO_SUCCESS) {
            HILOGE("[AudioWrapper]Release %lld fail", (long long)iter->first);
        }
        AIE_DELETE(iter->second);
        iter->second = nullptr;
    }
    coders_.clear();
    if (needCodec_ && hasInitCodec_) {
        CodecDeinit();
        hasInitCodec_ = false;
    }
}

int32_t AudioWrapper::Deinit(intptr_t handler)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = coders_.find(handler);
    if (iter == coders_.end()) {
        return AUDIO_NO_KEY;
    }
    int32_t retCode = iter->second->Stop();
    if (retCode != AUDIO_SUCCESS) {
        HILOGE("[AudioWrapper]Release %lld fail", (long long)iter->first);
    }
    AIE_DELETE(iter->second);
    iter->second = nullptr;
    coders_.erase(iter);
    if (coders_.empty() && needCodec_ && hasInitCodec_) {
        CodecDeinit();
        hasInitCodec_ = false;
    }
    return retCode;
}

int32_t AudioWrapper::SetConfig(intptr_t handler, const CoderConfig &config)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = coders_.find(handler);
    if (iter == coders_.end()) {
        return AUDIO_NO_KEY;
    }
    return iter->second->Initialize(config);
}

int32_t AudioWrapper::StartCodec(intptr_t handler)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = coders_.find(handler);
    if (iter == coders_.end()) {
        HILOGE("[AudioWrapper]Convert: AUDIO_NO_KEY");
        return AUDIO_NO_KEY;
    }
    if (iter->second->Start() != AUDIO_SUCCESS) {
        HILOGE("[AudioWrapper]Convert fail to start coder");
        return AUDIO_CONVERT_FAILURE;
    }
    return AUDIO_SUCCESS;
}

int32_t AudioWrapper::Convert(intptr_t handler, const CoderStream &source, CoderStream &converted)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto iter = coders_.find(handler);
    if (iter == coders_.end()) {
        HILOGE("[AudioWrapper]Convert: AUDIO_NO_KEY");
        return AUDIO_NO_KEY;
    }
    int32_t ret = iter->second->PushSourceStream(source);
    if (ret != AUDIO_SUCCESS) {
        HILOGE("[AudioWrapper]Convert: AUDIO_CONVERT_FAILURE");
        return AUDIO_CONVERT_FAILURE;
    }
    return iter->second->PullCodedStream(converted);
}
} // namespace AI
} // namespace OHOS