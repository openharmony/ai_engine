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

#ifndef DECODER_WRAPPER_H
#define DECODER_WRAPPER_H

#include "coder_wrapper.h"

namespace OHOS {
namespace AI {
class DecoderWrapper : public CoderWrapper {
public:
    DecoderWrapper();
    ~DecoderWrapper();

    /* Initializes decoder */
    int32_t Initialize(const CoderConfig &input) override;

    /* Starts decoder */
    int32_t Start() override;

    /* Stops decoder */
    int32_t Stop() override;

    /* Pushes source CoderStream into coder for decoding */
    int32_t PushSourceStream(const CoderStream &stream) override;

    /* Pulls decoded result CoderStream from coder */
    int32_t PullCodedStream(CoderStream &stream) override;

protected:
    int32_t InitCodecAttr(const CoderConfig &input) override;

private:
    CODEC_HANDLETYPE codeHandle_;
    CodecType decodeType_ = CODEC_AAC_TO_PCM;
    AvCodecMime codecMime_ = MEDIA_MIMETYPE_AUDIO_AAC;
    uint32_t bufSize_ = AUDIO_AUDIO_POINT_NUM;
    bool started_ = false;
};
} // namespace AI
} // namespace OHOS
#endif // DECODER_WRAPPER_H