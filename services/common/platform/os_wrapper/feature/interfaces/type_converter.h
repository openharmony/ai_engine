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
 * @file type_converter.h
 *
 * @brief Defines TypeConverter with specified input and output types.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef PREPROCESS_TYPE_CONVERTER_H
#define PREPROCESS_TYPE_CONVERTER_H

#include <cstdint>

#include "feature_processor.h"

namespace OHOS {
namespace AI {
namespace Feature {
/**
 * @brief Specifies the structure for the TypeConverter configuration.
 *
 * @since 2.2
 * @version 1.0
 */
struct TypeConverterConfig : FeatureProcessorConfig {
    /** Number of data records supported by a single conversion. The maximum is {@link MAX_SAMPLE_SIZE}. */
    size_t size;
    TypeConverterConfig() = default;
    TypeConverterConfig(DataType dt, size_t sz)
    {
        dataType = dt;
        size = sz;
    }
};

/**
 * @brief Defines the functions for TypeConverter.
 *
 * @since 2.2
 * @version 1.0
 */
class TypeConverter : public FeatureProcessor {
public:
    /**
     * @brief Defines the constructor for TypeConverter.
     *
     * @since 2.2
     * @version 1.0
     */
    TypeConverter();

    /**
     * @brief Defines the destructor for TypeConverter.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~TypeConverter();

    /**
     * @brief Initializes TypeConverter.
     *
     * @param config Indicates the pointer to the basic configuration of FeatureProcessor.
     * The caller needs to pass in a pointer address defined by {@link TypeConverterConfig} and
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
     * The caller can input FeatureData of any data type defind by {@link DataType} except UNKNOWN.
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
    int32_t InnerProcess(const FeatureData &input);

private:
    bool isInitialized_;
    FeatureData workBuffer_;
};
} // namespace Feature
} // namespace AI
} // namespace OHOS
#endif // PREPROCESS_TYPE_CONVERTER_H
/** @} */