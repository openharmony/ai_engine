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

#include "audio_utils.h"

#include "aie_log.h"

namespace OHOS {
namespace AI {
namespace AudioUtils {
bool IsAudioCoderFormatSupported(AudioCodecFormat format)
{
    if ((format < AAC_LC) || (format > AAC_ELD)) {
        HILOGE("[AudioUtils]Invalid format = %d", format);
        return false;
    }
    return true;
}

Profile GetProfileFromAudioCoderFormat(AudioCodecFormat format)
{
    switch (format) {
        case AAC_LC:
            return AAC_LC_PROFILE;
        case AAC_HE_V1:
            return AAC_HE_V1_PROFILE;
        case AAC_HE_V2:
            return AAC_HE_V2_PROFILE;
        case AAC_LD:
            return AAC_LD_PROFILE;
        case AAC_ELD:
            return AAC_ELD_PROFILE;
        default:
            HILOGW("[AudioUtils]Invalid format = 0x%.8x, replace with default", format);
            return AAC_LC_PROFILE;
    }
}

AudioSampleRate ConvertSampleRateForCoder(uint32_t sampleRate)
{
    switch (sampleRate) {
        case AUD_SAMPLE_RATE_8000:
        case AUD_SAMPLE_RATE_11025:
        case AUD_SAMPLE_RATE_12000:
        case AUD_SAMPLE_RATE_16000:
        case AUD_SAMPLE_RATE_22050:
        case AUD_SAMPLE_RATE_24000:
        case AUD_SAMPLE_RATE_32000:
        case AUD_SAMPLE_RATE_44100:
        case AUD_SAMPLE_RATE_48000:
        case AUD_SAMPLE_RATE_64000:
        case AUD_SAMPLE_RATE_96000:
            return static_cast<AudioSampleRate>(sampleRate);
        default:
            HILOGW("[AudioUtils]Invalid sampleRate = %u, replace with default", sampleRate);
            return AUD_SAMPLE_RATE_48000;
    }
}

AudioSoundMode ConvertSoundModeForCoder(uint32_t channelCount)
{
    switch (channelCount) {
        case AUDIO_CHANNEL_MONO:
            return AUD_SOUND_MODE_MONO;
        case AUDIO_CHANNEL_STEREO:
            return AUD_SOUND_MODE_STEREO;
        default:
            HILOGW("[AudioUtils]Invalid soundMode: %u, replace with default", channelCount);
            return AUD_SOUND_MODE_MONO;
    }
}
} // namespace AudioUtils
} // namespace AI
} // namespace OHOS