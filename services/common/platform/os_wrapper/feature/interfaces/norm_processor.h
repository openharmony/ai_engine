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
 * @file norm_processor.h
 *
 * @brief Defines NormProcessor that normalizes and quantizes features based on the inference model.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef PREPROCESS_NORM_PROCESSOR_H
#define PREPROCESS_NORM_PROCESSOR_H

#include <cstdint>
#include <memory>
#include <string>

#include "feature_processor.h"
#include "type_converter.h"

namespace OHOS {
namespace AI {
namespace Feature {
/**
 * @brief Specifies the structure for the NormProcessor configuration.
 *
 * @since 2.2
 * @version 1.0
 */
struct NormProcessorConfig : FeatureProcessorConfig {
    /** Local address for the mean file of NormProcessor */
    std::string meanFilePath;
    /** Local address for the standard deviation file of NormProcessor */
    std::string stdFilePath;
    /** Number of channels for normalized parameters. The value must be greater than <b>0</b>
     * and can be exactly divided by <b>inputSize</b>. */
    size_t numChannels;
    /** Sample size for the input data.
     * The value must be greater than <b>0</b> but less than or equal to {@link MAX_SAMPLE_SIZE}. */
    size_t inputSize;
    /** Scaling coefficient. */
    float scale = 1.0f;
};

/**
 * @brief Defines the functions for NormProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
class NormProcessor : public FeatureProcessor {
public:
    /**
     * @brief Defines the constructor for NormProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    NormProcessor();

    /**
     * @brief Defines the destructor for NormProcessor.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~NormProcessor();

    /**
     * @brief Initializes NormProcessor.
     *
     * @param config Indicates the pointer to the basic configuration of FeatureProcessor.
     * The caller needs to pass in a pointer address defined by {@link NormProcessorConfig} and
     * release the pointer after using it.
     * @return Returns {@link RETCODE_SUCCESS} if the operation is successfull;
     * returns {@link RETCODE_FAILURE} otherwise
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
     * However the address and data length must meet the configuration requirements.
     * @param output Indicates the output data for FeatureProcessor.
     * The caller must pass in FeatureData of the FLOAT type defined by {@link DataType}.
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
    float *workBuffer_;
    float *mean_;
    float *std_;
    NormProcessorConfig config_;
    std::unique_ptr<TypeConverter> converter_;
};
} // namespace Feature
} // namespace AI
} // namespace OHOS
#endif // PREPROCESS_NORM_PROCESSOR_H
/** @} */