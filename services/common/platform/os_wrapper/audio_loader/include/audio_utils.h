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

#ifndef AUDIO_UTILS_H
#define AUDIO_UTILS_H

#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <iomanip>

#include "codec_interface.h"
#include "format.h"
#include "media_errors.h"
#include "media_info.h"

namespace OHOS {
namespace AI {
namespace AudioUtils {
constexpr uint8_t AUDIO_CHANNEL_MONO = 1;
constexpr uint8_t AUDIO_CHANNEL_STEREO = 2;

/* Indicates whether to support specific format. */
bool IsAudioCoderFormatSupported(AudioCodecFormat format);
/* Parses profile from AudioCodecFormat. */
Profile GetProfileFromAudioCoderFormat(AudioCodecFormat format);
/* Converts sample rate to AudioSampleRate format. */
AudioSampleRate ConvertSampleRateForCoder(uint32_t sampleRate);
/* Converts channel count to AudioSoundMode format. */
AudioSoundMode ConvertSoundModeForCoder(uint32_t channelCount);
} // namespace AudioUtils
} // namespace AI
} // namespace OHOS
#endif // AUDIO_UTILS_H