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
 * @file slide_window_processor.h
 *
 * @brief Defines SlideWindowProcessor that implements sliding window for streaming data.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef PREPROCESS_SLIDE_WINDOW_PROCESSOR_H
#define PREPROCESS_SLIDE_WINDOW_PROCESSOR_H

#include <cstdint>

#include "feature_processor.h"

namespace OHOS {
namespace AI {
namespace Feature {
/**
 * @brief Specifies the structure for the SlideWindowProcessor configuration.
 *
 * @since 2.2
 * @version 1.0
 */
struct SlideWindowProcessorConfig : FeatureProcessorConfig {
    /** Buffer multiplier for SlideWindowProcessor acceleration.
     * The value must be greater than <b>0</b>. The default is <b>4</b>. */
    uint8_t bufferMultiplier = 4;
    /** Input step size. Ensure that the maximum value is not greater than <b>windowSize</b>. */
    size_t stepSize;
    /** Size of the output window. Ensure that the maximum value is not greater than {@link MAX_SAMPLE_SIZE}. */
    size_t windowSize;
};

/**
 * @brief Defines the functions for SlideWindowProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
class SlideWindowProcessor : public FeatureProcessor {
public:
    /**
     * @brief Defines the constructor for SlideWindowProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    SlideWindowProcessor();

    /**
     * @brief Defines the destructor for SlideWindowProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~SlideWindowProcessor();

    /**
     * @brief Initializes SlideWindowProcessor.
     *
     * @param config Indicates the pointer to the basic configuration of FeatureProcessor.
     * The caller needs to pass in a pointer address defined by {@link SlideWindowProcessorConfig} and
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
     * The caller can input FeatureData of any data type defined by {@link DataType} except UNKNOWN.
     * However, the address and data length must meet the configuration requirements of {@link Init}.
     * @param output Indicates the output data for FeatureProcessor.
     * The caller must pass in FeatureData that is consistent with the input data defined by {@link DataType}.
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
    bool isInitialized_;
    char *workBuffer_;
    char *inputFeature_;
    DataType inType_;
    uint8_t typeSize_;
    uint32_t startIndex_;
    uint32_t initIndex_;
    uint32_t bufferSize_;
    size_t windowSize_;
    size_t stepSize_;
};
} // namespace Feature
} // namespace AI
} // namespace OHOS
#endif // PREPROCESS_SLIDE_WINDOW_PROCESSOR_H
/** @} */