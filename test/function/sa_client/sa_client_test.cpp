/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include "gtest/gtest.h"

#include <unistd.h>
#include "client_executor/include/i_aie_client.inl"
#include "service_dead_cb.h"
#include "utils/log/aie_log.h"
#include "communication_adapter/source/sa_client.cpp"
#include "protocol/struct_definition/aie_info_define.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
    const char * const INPUT_CHARACTER = "inputData";
    const char * const CONFIG_DESCRIPTION = "config information";
    const long long CLIENT_INFO_VERSION = 1;
    const int CLIENT_ID = -1;
    const int SESSION_ID = -1;
    const long long ALGORITHM_INFO_CLIENT_VERSION = 2;
    const int ALGORITHM_TYPE_SYNC = 0;
    const int ALGORITHM_TYPE_ASYNC = 1;
    const long long ALGORITHM_VERSION = 1;
    const int OPERATE_ID = 2;
    const int REQUEST_ID = 3;
}

class SaClientTest : public testing::Test {
public:
    // SetUpTestCase:The preset action of the test suite is executed before the first TestCase
    static void SetUpTestCase() {};

    // TearDownTestCase:The test suite cleanup action is executed after the last TestCase
    static void TearDownTestCase() {};

    // SetUp:Execute before each test case
    void SetUp() {};

    // TearDown:Execute after each test case
    void TearDown() {};
};

/**
 * @tc.name: TestSaClient001
 * @tc.desc: Test Call SA proxy, to connect the server to get the client ID.
 * @tc.type: FUNC
 * @tc.require: AR000F77NK
 */
static HWTEST_F(SaClientTest, TestSaClient001, TestSize.Level0)
{
    HILOGI("[Test]TestSaClient001.");

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};
    const char *inputData = INPUT_CHARACTER;
    int len = strlen(inputData) + 1;

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = CLIENT_ID,
        .sessionId = SESSION_ID,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = len,
        .extendMsg = reinterpret_cast<unsigned char*>(const_cast<char *>(inputData)),
      };
    AlgorithmInfo algoInfo = {
        .clientVersion = ALGORITHM_INFO_CLIENT_VERSION,
        .isAsync = false,
        .algorithmType = ALGORITHM_TYPE_SYNC,
        .algorithmVersion = ALGORITHM_VERSION,
        .isCloud = true,
        .operateId = OPERATE_ID,
        .requestId = REQUEST_ID,
        .extendLen = len,
        .extendMsg = reinterpret_cast<unsigned char*>(const_cast<char *>(inputData)),
    };

    ServiceDeadCb deadCb = ServiceDeadCb();
    int resultCodeInit = AieClientInit(configInfo, clientInfo, algoInfo, &deadCb);
    ASSERT_EQ(resultCodeInit, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);
 
    SaClient *client = SaClient::GetInstance();
    if (client == nullptr) {
        HILOGE("[SaClientAdapter] Fail to SaClient::GetInstance");
        return;
    }

    int retCode = client->Init(configInfo, clientInfo);
    if (retCode != RETCODE_SUCCESS || clientInfo.clientId == INVALID_CLIENT_ID) {
        HILOGE("[SaClientAdapter] Fail to Init to server. errorCode:%d", retCode);
        return;
    }
    ASSERT_EQ(retCode, RETCODE_SUCCESS);
}
