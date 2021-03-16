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
 * @file kws_retcode.h
 *
 * @brief Defines the return codes for the KWS task.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef KWS_RETCODE_H
#define KWS_RETCODE_H

namespace OHOS {
namespace AI {
/**
 * @brief Enumerates the return codes supported by the KWS SDK.
 *
 * @since 2.2
 * @version 1.0
 */
typedef enum {
    KWS_RETCODE_FAILURE = -1,                  /** Return code for an operation failure */
    KWS_RETCODE_SUCCESS = 0,                   /** Return code for an operation success */
    KWS_RETCODE_INIT_ERROR = 1001,             /** Return code for an initialization error */
    KWS_RETCODE_NULL_PARAM = 1002,             /** Return code for a null parameter error */
    KWS_RETCODE_SERIALIZATION_ERROR = 2001,    /** Return code for a serialization error */
    KWS_RETCODE_UNSERIALIZATION_ERROR = 2002,  /** Return code for a deserialization error */
    KWS_RETCODE_PLUGIN_EXECUTION_ERROR = 3001, /** Return code for a plugin inference error */
    KWS_RETCODE_PLUGIN_SESSION_ERROR = 3002,   /** Return code for a plugin session error */
} KWSRetCode;
} // namespace AI
} // namespace OHOS
#endif // KWS_RETCODE_H
/** @} */