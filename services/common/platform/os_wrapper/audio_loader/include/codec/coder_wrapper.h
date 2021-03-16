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

#ifndef CODER_WRAPPER_H
#define CODER_WRAPPER_H

#include <cstdint>
#include <memory>

#include "codec_interface.h"
#include "format.h"
#include "media_errors.h"
#include "media_info.h"
#include "securec.h"

namespace OHOS {
namespace AI {
const int16_t AUDIO_ENCODE_PARAM_NUM = 8;
const int16_t AUDIO_DECODE_PARAM_NUM = 8;
const int16_t AUDIO_FRAME_NUM_IN_BUF = 30;
const int16_t AUDIO_AUDIO_POINT_NUM = 1024;
const int16_t AUDIO_READ_STREAM_TIME_OUT_MS = 1000;
const intptr_t INVALID_HANDLER = -1;

const CodecType CODEC_PCM_TO_AAC = AUDIO_ENCODER;
const CodecType CODEC_AAC_TO_PCM = AUDIO_DECODER;

enum ConvertType {
    CONVERT_AAC_TO_PCM = 1000,
    CONVERT_PCM_TO_AAC
};

struct CoderConfig {
    uint32_t bitRate;
    uint32_t sampleRate;
    uint32_t channelCount;
    AudioCodecFormat audioFormat;
    AudioBitWidth bitWidth = BIT_WIDTH_16;
};

struct CoderStream {
    uint8_t *buffer;
    uint32_t size;
};

class CoderWrapper {
public:
    CoderWrapper() = default;
    virtual ~CoderWrapper() = default;

    /** Initializes coder */
    virtual int32_t Initialize(const CoderConfig &input) = 0;

    /** Starts coder */
    virtual int32_t Start() = 0;

    /** Pushes source CoderStream into coder for coding */
    virtual int32_t PushSourceStream(const CoderStream &stream) = 0;

    /** Pulls coded result CoderStream from coder */
    virtual int32_t PullCodedStream(CoderStream &stream) = 0;

    /** Stops coder */
    virtual int32_t Stop() = 0;

protected:
    virtual int32_t InitCodecAttr(const CoderConfig &input) = 0;
};
} // namespace AI
} // namespace OHOS
#endif // CODER_WRAPPER_H