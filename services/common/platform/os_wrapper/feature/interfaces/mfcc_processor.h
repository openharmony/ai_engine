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

/**
 * @addtogroup feature_processor
 * @{
 *
 * @brief Defines the basic functions for FeatureProcessor, including the supported data types
 * and other related configuration parameters.
 *
 * @since 2.2
 * @version 1.0
 */

/**
 * @file mfcc_processor.h
 *
 * @brief Defines MFCCProcessor that calculates the Mel-frequency cepstral coefficient (MFCC) based
 * on the 16-bit PCM audio data.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef AUDIO_PREPROCESS_MFCC_PROCESSOR_H
#define AUDIO_PREPROCESS_MFCC_PROCESSOR_H

#include <cstdint>
#include <memory>

#include "feature_processor.h"

namespace OHOS {
namespace AI {
namespace Feature {
/**
 * @brief Specifies the structure for the MFCCProcessor configuration.
 *
 * @since 2.2
 * @version 1.0
 */
struct MFCCConfig : FeatureProcessorConfig {
    /** Indicates whether to execute the energy gain */
    bool enablePcanGain;
    /** Indicates whether to execute the weighted logarithm */
    bool enableLogScale;
    /** Number of smooth bits for noise reduction.
     * The value is multiplied by 2 to the power of <b>noiseSmoothingBits</b> for signal smoothing. */
    int16_t noiseSmoothingBits;
    /** Number of gain bits.
     * The value is multiplied by 2 to the power of <b>pcanGainBits</b> to achieve the gain effect. */
    int16_t pcanGainBits;
    /** Logarithmic weight. The output logarithm is multiplied by 2 to the power of <b>logScaleShift</b>. */
    int16_t logScaleShift;
    /** Size of the audio sampling window. The maximum value is <b>16000</b>. */
    uint32_t windowSize;
    /** Interval between audio sliding windows. The maximum value is <b>16000</b>. */
    uint32_t slideSize;
    /** Sampling rate of audio data. */
    uint32_t sampleRate;
    /** Number of MFCC features. The maximum value is {@link MAX_SAMPLE_SIZE}. */
    uint32_t featureSize;
    /** Number of frequency-domain channels. The maximum value is {@link MAX_NUM_CHANNELS}. */
    uint32_t numChannels;
    /** Maximum frequency threshold of the filter. The value must be greater than <b>0</b> and the actual value depends
    * on the specific frequency requirements. */
    float filterbankLowerBandLimit;
    /** Minimum frequency threshold of the filter.
     * The value must be greater than <b>0</b> and less than <b>filterbankLowerBandLimit</b>. */
    float filterbankUpperBandLimit;
    /** Smoothing coefficient for even channels of noise estimation. The value ranges from <b>0.0</b> to <b>1.0</b>. */
    float noiseEvenSmoothing;
    /** Smoothing coefficient for odd channels of noise estimation. The value ranges from <b>0.0</b> to <b>1.0</b>. */
    float noiseOddSmoothing;
    /** Signal reservation ratio for noise estimation. The value ranges from <b>0.0</b> to <b>1.0</b>.
     * Value <b>1.0</b> indicates the signals are not filtered, and value <b>0.0</b> indicates signals are
     * all filtered. */
    float noiseMinSignalRemaining;
    /** Gain normalization index. The value ranges from <b>0.0</b> to <b>1.0</b>.
     * Value <b>0.0</b> indicates no gain, and value <b>1.0</b> indicates full gain. */
    float pcanGainStrength;
    /** Denominator offset for gain normalization to prevent the base number from being <b>0</b>.
     * The value must be greater than <b>0</b>. */
    float pcanGainOffset;
};

/**
 * @brief Defines the sliding window configuration for MFCCProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
struct MFCCLocalConfig {
    /** Number of samples for each sliding. The value is equal to that defined by {@link MFCCConfig.slideSize}. */
    uint32_t slideSampleNum;
    /** Number of samples for each sampling. The value is equal to that defined by {@link MFCCConfig.windowSize}. */
    uint32_t windowSampleNum;
    /** Number of frequency-domain channels. The value is equal to that defined by {@link MFCCConfig.numChannels}. */
    uint32_t numChannels;
    /** Number of MFCC features corresponding to the samples.
     * The value is equal to that defined by {@link MFCCConfig.featureSize}. */
    uint32_t featureSize;
};

/**
 * @brief Defines the functions for MFCCProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
class MFCCProcessor : public FeatureProcessor {
public:
    /**
     * @brief Defines the constructor for MFCCProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    MFCCProcessor();

    /**
     * @brief Defines the destructor for MFCCProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~MFCCProcessor();

    /**
     * @brief Initializes MFCCProcessor.
     *
     * @param config Indicates the pointer to the basic configuration of FeatureProcessor.
     * The caller needs to pass in a pointer address defined by {@link MFCCConfig} and
     * release the pointer after using it.
     * @return Returns {@link RETCODE_SUCCESS} if the operation is successful;
     * returns {@link RETCODE_FAILURE} otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t Init(const FeatureProcessorConfig *config) override;

    /**
     * @brief Performs feature processing.
     *
     * @param input Indicates the input data for FeatureProcessor.
     * The caller must pass in FeatureData of the INT16 type defined by {@link DataType},
     * besides the address and data length must meet the configuration requirements.
     * @param output Indicates the output data for FeatureProcessor.
     * The caller must pass in FeatureData of the UINT16 type defined by {@link DataType}.
     * If and only if its address is empty and the data length is <b>0</b>,
     * data will be filled by the FeatureProcessor.
     * @return Returns {@link RETCODE_SUCCESS} if the operation is successful;
     * returns {@link RETCODE_FAILURE} otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t Process(const FeatureData &input, FeatureData &output) override;

    /**
     * @brief Releases resources.
     *
     * @since 2.2
     * @version 1.0
     */
    void Release() override;

private:
    class MFCCImpl;
    std::unique_ptr<MFCCImpl> impl_;
};
} // namespace Feature
} // namespace AI
} // namespace OHOS
#endif // AUDIO_PREPROCESS_MFCC_PROCESSOR_H
/** @} */