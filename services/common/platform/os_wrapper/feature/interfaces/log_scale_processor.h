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
 * and other related configuration paramters.
 *
 * @since 2.2
 * @version 1.0
 */

/**
 * @file log_scale_processor.h
 *
 * @brief Defines LogScaleProcessor that implements weighting on the natural logarithm obtained
 * from feature calculation.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef AUDIO_PREPROCESS_LOG_SCALE_PROCESSOR_H
#define AUDIO_PREPROCESS_LOG_SCALE_PROCESSOR_H

#include <cstdint>
#include <memory>

#include "feature_processor.h"

namespace OHOS {
namespace AI {
namespace Feature {
/**
 * @brief Specifies the structure for the LogScaleProcessor configuration.
 *
 * @since 2.2
 * @version 1.0
 */
struct LogScaleConfig : FeatureProcessorConfig {
    /** Indicates whether to process input data */
    bool enableLogScale;
    /** Weight. The output logarithm is multiplied by 2 to the power of <b>scaleShift</b>. */
    int16_t scaleShift;
    /** Number of bits moved by the original data's shift operation. */
    int16_t correctionBits;
    /** Number of input data channels. The maximum value is {@link MAX_NUM_CHANNELS}. */
    uint32_t numChannels;
};

/**
 * @brief Defines the functions for LogScaleProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
class LogScaleProcessor : public FeatureProcessor {
public:
    /**
     * @brief Defines the constructor for LogScaleProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    LogScaleProcessor();

    /**
     * @brief Defines the destructor for LogScaleProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~LogScaleProcessor();

    /**
     * @brief Initializes LogScaleProcessor.
     *
     * @param config Indicates the pointer to the basic configuration of FeatureProcessor.
     * The caller needs to pass in a pointer address defined by {@link LogScaleConfig}
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
    class LogScaleImpl;
    std::unique_ptr<LogScaleImpl> impl_;
};
} // namespace Feature
} // namespace AI
} // namespace OHOS
#endif // AUDIO_PREPROCESS_LOG_SCALE_PROCESSOR_H
/** @} */