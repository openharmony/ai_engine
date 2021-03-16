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

#include "kws_sdk_impl.h"

#include "aie_guard.h"
#include "aie_retcode_inner.h"
#include "encdec_facade.h"
#include "i_aie_client.inl"
#include "kws_retcode.h"
#include "mfcc_processor.h"
#include "plugin_helper.h"

using namespace OHOS::AI::Feature;

namespace OHOS {
namespace AI {
namespace {
    const int16_t ONE_SECOND_MS = 1000;
}

static void InitMFCCConfiguration(MFCCConfig &config)
{
    config.dataType = UINT16;
    config.windowSize = (DEFAULT_MFCC_WINDOW_SIZE_MS * DEFAULT_MFCC_SAMPLE_RATE) / ONE_SECOND_MS;
    config.slideSize = (DEFAULT_MFCC_SLIDE_SIZE_MS * DEFAULT_MFCC_SAMPLE_RATE) / ONE_SECOND_MS;
    config.sampleRate = DEFAULT_MFCC_SAMPLE_RATE;
    config.featureSize = DEFAULT_MFCC_FEATURE_SIZE;
    config.numChannels = DEFAULT_MFCC_NUM_CHANNELS;
    config.filterbankLowerBandLimit = DEFAULT_FILTERBANK_LOWER_BAND_LIMIT;
    config.filterbankUpperBandLimit = DEFAULT_FILTERBANK_UPPER_BAND_LIMIT;
    config.noiseSmoothingBits = DEFAULT_NOISE_SMOOTHING_BITS;
    config.noiseEvenSmoothing = DEFAULT_NOISE_EVEN_SMOOTHING;
    config.noiseOddSmoothing = DEFAULT_NOISE_ODD_SMOOTHING;
    config.noiseMinSignalRemaining = DEFAULT_NOISE_MIN_SIGNAL_REMAINING;
    config.enablePcanGain = DEFAULT_ENABLE_PCAN_GAIN;
    config.pcanGainStrength = DEFAULT_PCAN_GAIN_STRENGTH;
    config.pcanGainOffset = DEFAULT_PCAN_GAIN_OFFSET;
    config.pcanGainBits = DEFAULT_PCAN_GAIN_BITS;
    config.enableLogScale = DEFAULT_ENABLE_LOG_SCALE;
    config.logScaleShift = DEFAULT_LOG_SCALE_SHIFT;
}

KWSSdk::KWSSdkImpl::KWSSdkImpl() = default;

KWSSdk::KWSSdkImpl::~KWSSdkImpl()
{
    Destroy();
}

int32_t KWSSdk::KWSSdkImpl::Create()
{
    if (kwsHandle_ != INVALID_KWS_HANDLE) {
        HILOGE("[KWSSdkImpl]Fail to create kws sdk more than once");
        return KWS_RETCODE_INIT_ERROR;
    }
    if (InitComponents() != KWS_RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]Fail to init sdk components");
        return KWS_RETCODE_INIT_ERROR;
    }
    int32_t retCode = AieClientInit(configInfo_, clientInfo_, algorithmInfo_, nullptr);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]AieClientInit failed. Error code[%d]", retCode);
        return KWS_RETCODE_INIT_ERROR;
    }
    if (clientInfo_.clientId == INVALID_CLIENT_ID) {
        HILOGE("[KWSSdkImpl]Fail to allocate client id");
        return KWS_RETCODE_INIT_ERROR;
    }
    DataInfo inputInfo = {0};
    DataInfo outputInfo = {0};
    retCode = AieClientPrepare(clientInfo_, algorithmInfo_, inputInfo, outputInfo, nullptr);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]AieClientPrepare failed. Error code[%d]", retCode);
        return KWS_RETCODE_INIT_ERROR;
    }
    if (outputInfo.data == nullptr || outputInfo.length <= 0) {
        HILOGE("[KWSSdkImpl]The data or length of output info is invalid");
        return KWS_RETCODE_INIT_ERROR;
    }
    MallocPointerGuard<unsigned char> pointerGuard(outputInfo.data);
    retCode = EncdecFacade::ProcessDecode(outputInfo, kwsHandle_);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]Fail to get handle from input info");
        return KWS_RETCODE_UNSERIALIZATION_ERROR;
    }
    return KWS_RETCODE_SUCCESS;
}

int32_t KWSSdk::KWSSdkImpl::SyncExecute(const Array<int16_t> &input)
{
    if (kwsHandle_ == INVALID_KWS_HANDLE) {
        HILOGE("[KWSSdkImpl]The SDK has not been created");
        return KWS_RETCODE_FAILURE;
    }
    if (callback_ == nullptr) {
        HILOGE("[KWSSdkImpl]Fail to execute with nullptr callback");
        return KWS_RETCODE_FAILURE;
    }
    Array<int16_t> pcmInput = {0};
    Array<uint16_t> mfccInput = {0};
    FeatureData pcmFeature = {
        .dataType = INT16,
        .data = nullptr,
        .size = 0
    };
    FeatureData mfccFeature = {
        .dataType = UINT16,
        .data = nullptr,
        .size = 0
    };
    int32_t retCode = pcmIterator_->SetInput(input);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]Fail to set input to pcm iterator");
        return KWS_RETCODE_FAILURE;
    }
    while (pcmIterator_->HasNext()) {
        pcmInput = pcmIterator_->Next();
        pcmFeature.data = pcmInput.data;
        pcmFeature.size = pcmInput.size;
        // Preprocess
        if (mfccProcessor_->Process(pcmFeature, mfccFeature) != RETCODE_SUCCESS) {
            HILOGE("[KWSSdkImpl]Fail to process pcm data");
            return KWS_RETCODE_FAILURE;
        }
        mfccInput.data = static_cast<uint16_t *>(mfccFeature.data);
        mfccInput.size = mfccFeature.size;
        // Execute
        retCode = Execute(mfccInput);
        if (retCode != KWS_RETCODE_SUCCESS) {
            HILOGE("[KWSSdkImpl]Fail to execute synchronously");
            return retCode;
        }
        mfccFeature.data = nullptr;
        mfccFeature.size = 0;
    }
    return KWS_RETCODE_SUCCESS;
}

int32_t KWSSdk::KWSSdkImpl::Execute(const Array<uint16_t> &input)
{
    intptr_t receivedHandle = 0;
    Array<int32_t> kwsResult = {0};
    DataInfo inputInfo = {0};
    DataInfo outputInfo = {0};
    int32_t retCode = EncdecFacade::ProcessEncode(inputInfo, kwsHandle_, input);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]Fail to serialize input data");
        callback_->OnError(KWS_RETCODE_SERIALIZATION_ERROR);
        return KWS_RETCODE_SERIALIZATION_ERROR;
    }
    retCode = AieClientSyncProcess(clientInfo_, algorithmInfo_, inputInfo, outputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]AieClientSyncProcess failed. Error code[%d]", retCode);
        callback_->OnError(KWS_RETCODE_PLUGIN_EXECUTION_ERROR);
        return KWS_RETCODE_PLUGIN_EXECUTION_ERROR;
    }
    if (outputInfo.data == nullptr || outputInfo.length <= 0) {
        HILOGE("[KWSSdkImpl]The data or length of output info is invalid. Error code[%d]", retCode);
        callback_->OnError(KWS_RETCODE_NULL_PARAM);
        return KWS_RETCODE_NULL_PARAM;
    }
    MallocPointerGuard<unsigned char> pointerGuard(outputInfo.data);
    retCode = EncdecFacade::ProcessDecode(outputInfo, receivedHandle, kwsResult);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]UnSerializeOutputData failed. Error code[%d]", retCode);
        callback_->OnError(KWS_RETCODE_UNSERIALIZATION_ERROR);
        return KWS_RETCODE_UNSERIALIZATION_ERROR;
    }
    if (kwsHandle_ != receivedHandle) {
        HILOGE("[KWSSdkImpl]The handle[%lld] of output data is not equal to the current handle[%lld]",
            static_cast<long long>(receivedHandle), static_cast<long long>(kwsHandle_));
        callback_->OnError(KWS_RETCODE_PLUGIN_SESSION_ERROR);
        return KWS_RETCODE_PLUGIN_SESSION_ERROR;
    }
    callback_->OnResult(kwsResult);
    return KWS_RETCODE_SUCCESS;
}

int32_t KWSSdk::KWSSdkImpl::SetCallback(std::shared_ptr<KWSCallback> callback)
{
    if (callback == nullptr) {
        return KWS_RETCODE_NULL_PARAM;
    }
    callback_ = callback;
    return KWS_RETCODE_SUCCESS;
}

int32_t KWSSdk::KWSSdkImpl::InitComponents()
{
    // Create MFCC Processor
    MFCCConfig mfccConfig;
    InitMFCCConfiguration(mfccConfig);
    mfccProcessor_ = std::unique_ptr<FeatureProcessor>(new (std::nothrow) MFCCProcessor());
    if (mfccProcessor_ == nullptr) {
        HILOGE("[KWSSdkImpl]Fail to allocate memory for MFCCProcessor");
        return KWS_RETCODE_FAILURE;
    }
    if (mfccProcessor_->Init(&mfccConfig) != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]Fail to init MFCCProcessor");
        return KWS_RETCODE_FAILURE;
    }
    // Create PCM Iterator
    pcmIterator_ = std::unique_ptr<PCMIterator>(new (std::nothrow) PCMIterator());
    if (pcmIterator_ == nullptr) {
        HILOGE("[KWSSdkImpl]Fail to allocate memory for PCMIterator");
        return KWS_RETCODE_FAILURE;
    }
    size_t stepSize = (DEFAULT_SLIDE_STEP_SIZE / DEFAULT_MFCC_NUM_CHANNELS) * mfccConfig.slideSize;
    size_t windowSize = stepSize + mfccConfig.windowSize - mfccConfig.slideSize;
    if (pcmIterator_->Init(stepSize, windowSize) != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]Fail to init PCMIterator");
        return KWS_RETCODE_FAILURE;
    }
    return KWS_RETCODE_SUCCESS;
}

int32_t KWSSdk::KWSSdkImpl::Destroy()
{
    if (kwsHandle_ == INVALID_KWS_HANDLE) {
        return KWS_RETCODE_SUCCESS;
    }
    DataInfo inputInfo = {0};
    int32_t retCode = EncdecFacade::ProcessEncode(inputInfo, kwsHandle_);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]SerializeHandle failed. Error code[%d]", retCode);
        return KWS_RETCODE_SERIALIZATION_ERROR;
    }
    retCode = AieClientRelease(clientInfo_, algorithmInfo_, inputInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]AieClientRelease failed. Error code[%d]", retCode);
        return KWS_RETCODE_FAILURE;
    }
    retCode = AieClientDestroy(clientInfo_);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSSdkImpl]AieClientDestroy failed. Error code[%d]", retCode);
        return KWS_RETCODE_FAILURE;
    }
    mfccProcessor_ = nullptr;
    pcmIterator_ = nullptr;
    callback_ = nullptr;
    kwsHandle_ = INVALID_KWS_HANDLE;
    return KWS_RETCODE_SUCCESS;
}
} // namespace AI
} // namespace OHOS