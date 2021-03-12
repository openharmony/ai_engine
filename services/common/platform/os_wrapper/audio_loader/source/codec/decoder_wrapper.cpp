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

#include "decoder_wrapper.h"

#include "aie_log.h"
#include "aie_retcode_inner.h"
#include "audio_retcode.h"
#include "audio_utils.h"

using namespace OHOS::AI;
using namespace OHOS::AI::AudioUtils;
using namespace OHOS::Media;

namespace OHOS {
namespace AI {
namespace {
    const std::string CODER_NAME = "codec.aac.decoder.wrapper";
    const uint16_t DECODE_TIMEOUT_MS = 100u;
}

DecoderWrapper::DecoderWrapper()
    : codeHandle_(nullptr),
      started_(false)
{
    HILOGD("[DecoderWrapper]DecoderWrapper ctor");
}

DecoderWrapper::~DecoderWrapper()
{
    HILOGD("[DecoderWrapper]DecoderWrapper dtor");
}

int32_t DecoderWrapper::InitCodecAttr(const CoderConfig &input)
{
    if (codeHandle_ != nullptr) {
        HILOGE("[DecoderWrapper]InitCodec failed");
        return AUDIO_INIT_FAILURE;
    }
    Param attr[AUDIO_DECODE_PARAM_NUM];
    int16_t paramSize = AUDIO_DECODE_PARAM_NUM * sizeof(Param);
    int32_t ret = memset_s(attr, paramSize, 0x00, paramSize);
    if (ret != EOK) {
        HILOGE("[DecoderWrapper]Set decodeAttr failed, ret = 0x%.8x", ret);
        return AUDIO_INIT_FAILURE;
    }
    uint32_t index = 0;
    attr[index++] = {.key = KEY_MIMETYPE,   .val = (void *)(&codecMime_),  .size = sizeof(AvCodecMime)};
    attr[index++] = {.key = KEY_BUFFERSIZE, .val = (void *)(&bufSize_),    .size = sizeof(uint32_t)};
    attr[index++] = {.key = KEY_CODEC_TYPE, .val = (void *)(&decodeType_), .size = sizeof(CodecType)};

    HILOGD("[DecoderWrapper]InitCodecAttr before codeHandle_ %lld", (long long)codeHandle_);
    ret = CodecCreate(CODER_NAME.c_str(), attr, index, &codeHandle_);
    if (ret != AUDIO_SUCCESS) {
        HILOGE("[DecoderWrapper]CoderCreate failed 0x%.8x", ret);
        return AUDIO_INIT_FAILURE;
    }
    HILOGD("[DecoderWrapper]InitCodecAttr after codeHandle_ %lld", (long long)codeHandle_);
    return AUDIO_SUCCESS;
}

int32_t DecoderWrapper::Initialize(const CoderConfig &input)
{
    if (!AudioUtils::IsAudioCoderFormatSupported(input.audioFormat)) {
        HILOGE("[DecoderWrapper]Input.audioFormat = 0x%.8x", input.audioFormat);
        return AUDIO_INIT_FAILURE;
    }
    int32_t ret = InitCodecAttr(input);
    if (ret != AUDIO_SUCCESS) {
        HILOGE("[DecoderWrapper]InitAencAttr failed with ret = 0x%.8x", ret);
        return AUDIO_INIT_FAILURE;
    }
    return AUDIO_SUCCESS;
}

int32_t DecoderWrapper::Start()
{
    int32_t ret = AUDIO_SUCCESS;
    if (started_) {
        return ret;
    }
    if (codeHandle_ == nullptr) {
        return AUDIO_NULL_CODER;
    }
    if ((ret = CodecStart(codeHandle_)) != AUDIO_SUCCESS) {
        HILOGE("[DecoderWrapper]CodeHandle start failed 0x%.8x", ret);
        return ret;
    }
    started_ = true;
    return ret;
}

int32_t DecoderWrapper::PushSourceStream(const CoderStream &stream)
{
    if (!started_) {
        HILOGE("[DecoderWrapper]Decoder has not been started");
        return AUDIO_CONVERT_FAILURE;
    }
    InputInfo inputData = {0, nullptr, 0, 0};
    if (codeHandle_ == nullptr) {
        return AUDIO_NULL_CODER;
    }
    int32_t ret = CodecDequeInput(codeHandle_, DECODE_TIMEOUT_MS, &inputData);
    if (ret != AUDIO_SUCCESS) {
        HILOGE("[DecoderWrapper]Fail to deque input, with 0x%.8x", ret);
        return ret;
    }
    CodecBufferInfo inputBuf = {};
    inputBuf.addr = stream.buffer;
    inputBuf.length = stream.size;
    inputData.bufferCnt = AUDIO_CHANNEL_MONO;
    inputData.buffers = &inputBuf;
    inputData.flag = 0;
    return CodecQueueInput(codeHandle_, &inputData, DECODE_TIMEOUT_MS);
}

int32_t DecoderWrapper::PullCodedStream(CoderStream &outStream)
{
    if (!started_) {
        HILOGE("[DecoderWrapper]Codec not started");
        return AUDIO_INVALID_OPERATION;
    }
    if (outStream.buffer == nullptr || outStream.size == 0) {
        HILOGE("[DecoderWrapper]Stream.buffer is nullptr");
        return AUDIO_INVALID_OPERATION;
    }
    if (codeHandle_ == nullptr) {
        return AUDIO_NULL_CODER;
    }
    OutputInfo pcmInfo = {0, nullptr, 0, 0};
    CodecBufferInfo pcmBuf = {};
    pcmInfo.bufferCnt = AUDIO_CHANNEL_MONO;
    pcmInfo.buffers = &pcmBuf;
    int32_t ret = CodecDequeueOutput(codeHandle_, DECODE_TIMEOUT_MS, nullptr, &pcmInfo);
    if (ret != AUDIO_SUCCESS || pcmInfo.buffers[0].addr == nullptr) {
        HILOGE("[DecoderWrapper]Load pcm data failed with ret = 0x%.8x", ret);
        return AUDIO_CONVERT_FAILURE;
    }
    if (pcmInfo.buffers[0].length <= 0) {
        return AUDIO_CONVERT_FAILURE;
    }
    outStream.size = pcmInfo.buffers[0].length;
    errno_t retCopy =
        memcpy_s(outStream.buffer, outStream.size, pcmInfo.buffers[0].addr, pcmInfo.buffers[0].length);
    (void)CodecQueueOutput(codeHandle_, &pcmInfo, DECODE_TIMEOUT_MS, -1);
    if (retCopy != EOK) {
        HILOGE("[DecoderWrapper]Fail to memcpy_s with ret = 0x%.8x", retCopy);
        return AUDIO_INVALID_OPERATION;
    }
    return AUDIO_SUCCESS;
}

int32_t DecoderWrapper::Stop()
{
    if (!started_) {
        HILOGW("[DecoderWrapper]Codec not start");
        return AUDIO_SUCCESS;
    }
    if (codeHandle_ != nullptr) {
        int32_t ret = CodecStop(codeHandle_);
        if (ret != AUDIO_SUCCESS) {
            HILOGE("[DecoderWrapper]CodecHandle stop failed 0x%.8x", ret);
            return AUDIO_INVALID_OPERATION;
        }
        CodecDestroy(codeHandle_);
        codeHandle_ = nullptr;
    }
    return AUDIO_SUCCESS;
}
} // namespace AI
} // namespace OHOS