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

#ifndef AI_RETCODE_H
#define AI_RETCODE_H

typedef enum {
    AI_RETCODE_SUCCESS = 0,
    AI_RETCODE_FAILURE = -1
} AiRetCode;

typedef enum {
    PLUGIN_RETCODE_FAILURE = -1,                  /** Return code for an operation failure */
    PLUGIN_RETCODE_SUCCESS = 0,                   /** Return code for an operation success */
    PLUGIN_RETCODE_INIT_ERROR = 1001,             /** Return code for an initialization error */
    PLUGIN_RETCODE_NULL_PARAM = 1002,             /** Return code for a null parameter error */
    PLUGIN_RETCODE_DUPLICATE_INIT_ERROR = 1004,   /** Return code for a duplicate initialization error */
    PLUGIN_RETCODE_SERIALIZATION_ERROR = 2001,    /** Return code for a serialization error */
    PLUGIN_RETCODE_UNSERIALIZATION_ERROR = 2002,  /** Return code for a deserialization error */
    PLUGIN_RETCODE_PLUGIN_EXECUTION_ERROR = 3001, /** Return code for a plugin execution error */
    PLUGIN_RETCODE_PLUGIN_SESSION_ERROR = 3002,   /** Return code for a plugin session error */
} PluginRetCode;
#endif // AI_RETCODE_H