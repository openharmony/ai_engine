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

#ifndef AIE_RETCODE_INNER_H
#define AIE_RETCODE_INNER_H

typedef enum {
    // General error code
    RETCODE_SUCCESS = 0,
    RETCODE_FAILURE = -1,
    // Common error code(1001~1099)
    RETCODE_OUT_OF_MEMORY = 1001,
    RETCODE_NULL_PARAM = 1002,
    RETCODE_ALGORITHM_ID_INVALID = 1003,
    RETCODE_MEMORY_COPY_FAILURE = 1004,
    RETCODE_SERVER_NOT_INIT = 1005,

    // SA error code(1100~1199)
    RETCODE_SA_SERVICE_EXCEPTION = 1100,
    RETCODE_SA_CLIENT_EXCEPTION = 1101,
    RETCODE_SA_ASYNC_HANDLER_NOT_FOUND = 1102,

    // Server error code(1200~1299)
    RETCODE_SO_LACK_SYMBOL = 1200,
    RETCODE_PLUGIN_LOAD_FAILED = 1201,
    RETCODE_OPEN_SO_FAILED = 1202,
    RETCODE_NO_LISTENER_FOUND = 1203,
    RETCODE_ENGINE_NOT_EXIST = 1204,
    RETCODE_ENGINE_MANAGER_NOT_INIT = 1205,
    RETCODE_NO_CLIENT_FOUND = 1206,
    RETCODE_WRONG_INFER_MODE = 1207,
    RETCODE_ASYNC_CB_STARTED = 1208,

    // Client error code(1300~1399)
    RETCODE_SYNC_MSG_TIMEOUT = 1300,
    RETCODE_ASYNC_MSG_TIMEOUT = 1301,

    // Thread error code(1400~1499)
    RETCODE_START_THREAD_FAILED = 1400,

    // Queue error code(1500~1599)
    RETCODE_QUEUE_FULL = 1500,
    RETCODE_QUEUE_EMPTY = 1501,
    RETCODE_QUEUE_NODE_INVALID = 1502,

    // Map error code(1600~1699)
    RETCODE_EMPTY_MAP = 1600,

    // Algorithm error code(1700~1799)
    RETCODE_ALGORITHM_PROCESS_ERROR = 1700,
} AieInnerCode;

#endif // AIE_RETCODE_INNER_H