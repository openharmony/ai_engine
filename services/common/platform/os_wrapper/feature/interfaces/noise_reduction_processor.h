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
 * @file noise_reduction_processor.h
 *
 * @brief Defines NoiseReductionProcessor that reduces noise after estimating the noise based on the
 * frequency-domain data.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef AUDIO_PREPROCESS_NOISE_REDUCTION_PROCESSOR_H
#define AUDIO_PREPROCESS_NOISE_REDUCTION_PROCESSOR_H

#include <cstdint>
#include <memory>

#include "feature_processor.h"

namespace OHOS {
namespace AI {
namespace Feature {
/**
 * @brief Specifies the structure for the NoiseReductionProcessor configuration.
 *
 * @since 2.2
 * @version 1.0
 */
struct NoiseReductionConfig : FeatureProcessorConfig {
    /** Indicates whether to execute the energy gain after noise reduction. */
    bool enablePcanGain;
    /** Number of smooth bits. Before signal processing, the value is multiplied by 2 to the power of
     * <b>smoothingBits</b> for signal smoothing. */
    int16_t smoothingBits;
    /** Number of gain bits. The value is multiplied by 2 to the power of <b>gainBits</b> to achieve the gain effect. */
    int16_t gainBits;
    /** Number of signal shifts. This parameter is used to reduce the semaphore difference.
     * The value ranges from <b>0</b> to <b>32</b>. */
    uint16_t correctionBits;
    /** Number of frequency-domain channels for noise reduction. The maximum value is {@link MAX_NUM_CHANNELS}. */
    size_t numChannels;
    /** Smoothing coefficient for even channels. The value ranges from <b>0.0</b> to <b>1.0</b>. */
    float evenSmoothing;
    /** Smoothing coefficient for odd channels. The value ranges from <b>0.0</b> to <b>1.0</b>. */
    float oddSmoothing;
    /** Signal reservation ratio. The value ranges from <b>0.0</b> to <b>1.0</b>.
     * Value <b>1.0</b> indicates that signals are not filtered,
     * and value <b>0.0</b> indicates that signals are all filtered. */
    float minSignalRemaining;
    /** Gain normalization index. The value ranges from <b>0.0</b> to <b>1.0</b>.
     * Value <b>0.0</b> indicates no gain, and value <b>1.0</b> indicates full gain. */
    float strength;
    /** Normalization offset. The value must be greater than <b>0.0</b>. */
    float offset;
};

/**
 * @brief Defines the functions for NoiseReductionProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
class NoiseReductionProcessor : public FeatureProcessor {
public:
    /**
     * @brief Defines the constructor for NoiseReductionProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    NoiseReductionProcessor();

    /**
     * @brief Defines deconstructor for NoiseReductionProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~NoiseReductionProcessor();

    /**
     * @brief Initializes NoiseReductionProcessor.
     *
     * @param config Indicates the pointer to the basic configuration of FeatureProcessor.
     * The caller needs to pass in a pointer address defined by {@link NoiseReductionConfig}
     * and release the pointer after using it.
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
     * The caller must pass in FeatureData of the UINT32 type defined by {@link DataType},
     * besides the address and data length must meet the configuration requirements.
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
    class NoiseReductionImpl;
    std::unique_ptr<NoiseReductionImpl> impl_;
};
} // namespace Feature
} // namespace AI
} // namespace OHOS
#endif // AUDIO_PREPROCESS_NOISE_REDUCTION_PROCESSOR_H
/** @} */