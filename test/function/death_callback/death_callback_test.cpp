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

#include <cstring>
#include <unistd.h>

#include "client_executor/include/i_aie_client.inl"
#include "client_executor/include/i_client_cb.h"
#include "communication_adapter/include/sa_client.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "service_dead_cb.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;

namespace {
    unsigned char g_inputCharacter[] = "inputData";
    const char * const CONFIG_DESCRIPTION = "config information";
    const int INVALID_CLIENT_ID = -1;
    const int INTERVAL = 10;
    const long long CLIENT_INFO_VERSION = 1;
    const int CLIENT_ID = 1;
    const int INVALID_SESSION_ID = -1;
    const int SESSION_ID = 1;
    const int PRIORITY = 345;
    const int TIME_OUT = 456;
    const int EXTEND_LENGTH = 10;
    const long long ALGORITHM_INFO_CLIENT_VERSION = 2;
    const int ALGORITHM_TYPE = 66;
    const long long ALGORITHM_VERSION = 2;
    const int OPERATE_ID = 2;
    const int REQUEST_ID = 3;
    const int ALGORITHM_EXTEND_LENGTH = 0;
    constexpr uid_t INVALID_UID = 0;
}

int main()
{
    unsigned char *inputData = g_inputCharacter;

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};
    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = -1,
        .sessionId = -1,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = EXTEND_LENGTH,
        .extendMsg = inputData,
    };

    AlgorithmInfo algoInfo = {
        .clientVersion = ALGORITHM_INFO_CLIENT_VERSION,
        .isAsync = true,
        .algorithmType = ALGORITHM_TYPE,
        .algorithmVersion = ALGORITHM_VERSION,
        .isCloud = true,
        .operateId = OPERATE_ID,
        .requestId = REQUEST_ID,
        .extendLen = EXTEND_LENGTH,
        .extendMsg = inputData,
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int resultCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    HILOGI("[Test]TestAieClientInit001 result is [%d][clientID:%d][sessionId:%d].",
        resultCode, clientInfo.clientId, clientInfo.sessionId);

    clientInfo.sessionId = -1;
    ServiceDeadCb callBack = ServiceDeadCb();
    resultCode = AieClientInit(configInfo, clientInfo, algoInfo, &callBack);
    HILOGI("[Test]TestAieClientInit001 result is [%d][clientID:%d][sessionId:%d].",
        resultCode, clientInfo.clientId, clientInfo.sessionId);
    sleep(INTERVAL);
}
