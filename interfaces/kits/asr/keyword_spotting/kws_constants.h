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
 * @addtogroup kws_sdk
 * @{
 *
 * @brief Defines the basic functions, constants, and error codes for the wakeup keyword spotting (KWS) SDK.
 *
 * @since 2.2
 * @version 1.0
 */

/**
 * @file kws_constants.h
 *
 * @brief Defines the default configuration items for the KWS task.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef KWS_CONSTANTS_H
#define KWS_CONSTANTS_H

#include <string>

namespace OHOS {
namespace AI {
/**
 * @brief Defines the invalid handle for the KWS task.
 *
 * @since 2.2
 * @version 1.0
 */
const intptr_t INVALID_KWS_HANDLE = -1;

/**
 * @brief Defines the client version for the KWS task.
 *
 * @since 2.2
 * @version 1.0
 */
const long long CLIENT_VERSION_KWS = 20001001;

/**
 * @brief Defines the algorithm version for the KWS task.
 *
 * @since 2.2
 * @version 1.0
 */
const long long ALGOTYPE_VERSION_KWS = 20001002;

/**
 * @brief Defines the default size of the output window for SlideWindowProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const size_t DEFAULT_SLIDE_WINDOW_SIZE = 4000;

/**
 * @brief Defines the default step for SlideWindowProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const size_t DEFAULT_SLIDE_STEP_SIZE = 400;

/**
 * @brief Defines the default mean file path for NormProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const std::string DEFAULT_NORM_MEAN_FILE_PATH = "/storage/data/kws_mean.txt";

/**
 * @brief Defines the default standard deviation file path for NormProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const std::string DEFAULT_NORM_STD_FILE_PATH = "/storage/data/kws_std.txt";

/**
 * @brief Defines the default input size for NormProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const size_t DEFAULT_NORM_INPUT_SIZE = DEFAULT_SLIDE_STEP_SIZE;

/**
 * @brief Defines the default number of channels for NormProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const size_t DEFAULT_NORM_NUM_CHANNELS = 40;

/**
 * @brief Defines the default scaling coefficient for NormProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const float DEFAULT_NORM_SCALE = 4096.0f;

/**
 * @brief Defines the default step (in ms) of the audio sampling window for MFCCProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const int32_t DEFAULT_MFCC_WINDOW_SIZE_MS = 30;

/**
 * @brief Defines the default interval (in ms) between audio sampling windows for MFCCProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const int32_t DEFAULT_MFCC_SLIDE_SIZE_MS = 20;

/**
 * @brief Defines the default sampling rate for MFCCProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const uint32_t DEFAULT_MFCC_SAMPLE_RATE = 16000;

/**
 * @brief Defines the default number of frequency-domain channels for MFCCProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const uint32_t DEFAULT_MFCC_NUM_CHANNELS = 40;

/**
 * @brief Defines the default number of features for MFCCProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const uint32_t DEFAULT_MFCC_FEATURE_SIZE = DEFAULT_NORM_INPUT_SIZE;

/**
 * @brief Defines the default logarithmic weight for MFCCProcessor.
 *
 * @since 2.2
 * @version 1.0
 */
const int16_t DEFAULT_LOG_SCALE_SHIFT = 6;

/**
 * @brief Defines the default number of smooth bits for noise reduction.
 *
 * @since 2.2
 * @version 1.0
 */
const int16_t DEFAULT_NOISE_SMOOTHING_BITS = 10;

/**
 * @brief Defines the default number of gain bits.
 *
 * @since 2.2
 * @version 1.0
 */
const int16_t DEFAULT_PCAN_GAIN_BITS = 21;

/**
 * @brief Defines the default gain normalization index.
 *
 * @since 2.2
 * @version 1.0
 */
const float DEFAULT_PCAN_GAIN_STRENGTH = 0.95f;

/**
 * @brief Defines the default denominator offset for gain normalization.
 *
 * @since 2.2
 * @version 1.0
 */
const float DEFAULT_PCAN_GAIN_OFFSET = 80.0f;

/**
 * @brief Defines the default minimum frequency threshold of the filter.
 *
 * @since 2.2
 * @version 1.0
 */
const float DEFAULT_FILTERBANK_LOWER_BAND_LIMIT = 125.0f;

/**
 * @brief Defines the default maximum frequency threshold of the filter.
 *
 * @since 2.2
 * @version 1.0
 */
const float DEFAULT_FILTERBANK_UPPER_BAND_LIMIT = 7500.0f;

/**
 * @brief Defines the smoothing coefficient for even channels of noise estimation.
 *
 * @since 2.2
 * @version 1.0
 */
const float DEFAULT_NOISE_EVEN_SMOOTHING = 0.025f;

/**
 * @brief Defines the smoothing coefficient for odd channels of noise estimation.
 *
 * @since 2.2
 * @version 1.0
 */
const float DEFAULT_NOISE_ODD_SMOOTHING = 0.06f;

/**
 * @brief Defines the signal reservation ratio for noise estimation.
 *
 * @since 2.2
 * @version 1.0
 */
const float DEFAULT_NOISE_MIN_SIGNAL_REMAINING = 0.05f;

/**
 * @brief Specifies whether energy gain is executed for MFCCProcessor by default.
 *
 * @since 2.2
 * @version 1.0
 */
const bool DEFAULT_ENABLE_PCAN_GAIN = true;

/**
 * @brief Specifies whether weighted logarithm is executed for MFCCProcessor by default.
 *
 * @since 2.2
 * @version 1.0
 */
const bool DEFAULT_ENABLE_LOG_SCALE = true;
} // namespace AI
} // namespace OHOS
#endif // KWS_CONSTANTS_H
/** @} */