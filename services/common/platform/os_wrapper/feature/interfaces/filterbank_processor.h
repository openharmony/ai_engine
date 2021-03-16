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
 * @file filterbank_processor.h
 *
 * @brief Defines FilterBankProcessor that calculates FilterBank features based on the 16-bit PCM audio data.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef AUDIO_PREPROCESS_FILTER_BANK_PROCESSOR_H
#define AUDIO_PREPROCESS_FILTER_BANK_PROCESSOR_H

#include <cstdint>
#include <memory>
#include <string>

#include "feature_processor.h"

namespace OHOS {
namespace AI {
namespace Feature {
/**
 * @brief Specifies the structure for the FilterBankProcessor configuration.
 *
 * @since 2.2
 * @version 1.0
 */
struct FilterBankConfig : FeatureProcessorConfig {
    /** Number of frequency channels. The maximum value is {@link MAX_NUM_CHANNELS}. */
    uint32_t numChannels;
    /** Sampling rate for time-domain audio data. */
    uint32_t sampleRate;
    /** Maximum frequency. The value must be greater than <b>0</b> and the actual value depends
     * on the specific frequency requirements. */
    float upperBandLimit;
    /** Minimum frequency. The value must be greater than <b>0</b> and less than <b>upperBandLimit</b>. */
    float lowerBandLimit;
    /** Number of frequency-domain samples for the Fast Fourier Transform (FFT). The maximum value is <b>4096</b>. */
    size_t fftSize;
    /** Number of time-domain sample points for FFT. The maximum value is <b>16000</b>. */
    size_t inputSize;
};

/**
 * @brief Defines the functions for FilterBankProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
class FilterBankProcessor : public FeatureProcessor {
public:
    /**
     * @brief Defines the constructor for FilterBankProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    FilterBankProcessor();

    /**
     * @brief Defines the destructor for FilterBankProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~FilterBankProcessor();

    /**
     * @brief Initializes FilterBankProcessor.
     *
     * @param config Indicates the pointer to the basic configuration of FeatureProcessor.
     * The caller needs to pass in a pointer address defined by {@link FilterBankConfig}
     * and release the pointer after using it.
     * @return Returns {@link RETCODE_SUCCESS} if the operation is successful;
     * returns {@link RETCODE_FAILURE} otherwise.
     *
     * @since 2.2
     * @version 1.0
     */
    int32_t Init(const FeatureProcessorConfig *config) override;

    /**
     * @brief Processes feature data to obtain FilterBank features.
     *
     * @param input Indicates the input data for FeatureProcessor.
     * The caller must pass in FeatureData of the INT16 type defined by {@link DataType},
     * and the address and data length must meet the configuration requirements.
     * @param output Indicates the output data for FeatureProcessor.
     * The caller must pass in FeatureData of the UINT32 type defined by {@link DataType}.
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
    class FilterBankImpl;
    std::unique_ptr<FilterBankImpl> impl_;
};
} // namespace Feature
} // namespace AI
} // namespace OHOS
#endif // AUDIO_PREPROCESS_FILTER_BANK_PROCESSOR_H
/** @} */