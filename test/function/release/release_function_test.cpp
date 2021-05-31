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

#include "gtest/gtest.h"

#include "client_executor/include/i_aie_client.inl"
#include "protocol/struct_definition/aie_info_define.h"
#include "service_dead_cb.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
    const char * const INPUT_CHARACTER = "inputData";
    const char * const CONFIG_DESCRIPTION = "config information";
    const int INTERVAL = 10;
    const long long CLIENT_INFO_VERSION = 1;
    const int CLIENT_ID = -1;
    const int SESSION_ID = -1;
    const int PRIORITY = 345;
    const int TIME_OUT = 456;
    const int EXTEND_LENGTH = 10;
    const long long ALGORITHM_INFO_CLIENT_VERSION = 2;
    const int ALGORITHM_TYPE_SYNC = 0;
    const int ALGORITHM_TYPE_ASYNC = 1;
    const long long ALGORITHM_VERSION = 1;
    const int OPERATE_ID = 2;
    const int REQUEST_ID = 3;
    const int ALGORITHM_EXTEND_LENGTH = 0;
}

class ReleaseFunctionTest : public testing::Test {
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

class ClientCallback : public IClientCb {
public:
    ClientCallback() = default;
    ~ClientCallback() override = default;
    void OnResult(const DataInfo &result, int resultCode, int requestId) override
    {
        HILOGI("[Test]TestAlgorithmInfo002 OnResult resultCode[%d], requestId[%d], resultData[%s], resultLength[%d].",
            resultCode, requestId, result.data, result.length);
    }
};

/**
 * @tc.name: TestAieClientRelease001
 * @tc.desc: Test release function: the registered callback is empty and test case does not execute
             the method of AieClientSyncProcess.
 * @tc.type: FUNC
 * @tc.require: AR000F77NO
 */
HWTEST_F(ReleaseFunctionTest, TestAieClientRelease001, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientRelease001.");

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    const char *str = INPUT_CHARACTER;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = CLIENT_ID,
        .sessionId = SESSION_ID,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
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
        .extendMsg = (unsigned char*)inputData,
    };

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    ServiceDeadCb deadCb = ServiceDeadCb();
    int resultCodeInit = AieClientInit(configInfo, clientInfo, algoInfo, &deadCb);
    ASSERT_EQ(resultCodeInit, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    IClientCb *callback = nullptr;
    DataInfo outputInfo;
    int resultCodePrepare = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, callback);
    ASSERT_EQ(resultCodePrepare, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    int resultCodeRelease = AieClientRelease(clientInfo, algoInfo, inputInfo);
    ASSERT_EQ(resultCodeRelease, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    AieClientDestroy(clientInfo);
}

/**
 * @tc.name: TestAieClientRelease002
 * @tc.desc: Test release function: the registered callback is not empty and test case does not execute
             the method of AieClientSyncProcess.
 * @tc.type: FUNC
 * @tc.require: AR000F77NO
 */
HWTEST_F(ReleaseFunctionTest, TestAieClientRelease002, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientRelease002.");

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    const char *str = INPUT_CHARACTER;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = CLIENT_ID,
        .sessionId = SESSION_ID,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };

    AlgorithmInfo algoInfo = {
        .clientVersion = ALGORITHM_INFO_CLIENT_VERSION,
        .isAsync = true,
        .algorithmType = ALGORITHM_TYPE_ASYNC,
        .algorithmVersion = ALGORITHM_VERSION,
        .isCloud = true,
        .operateId = OPERATE_ID,
        .requestId = REQUEST_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int resultCodeInit = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(resultCodeInit, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    ClientCallback callback = ClientCallback();
    DataInfo outputInfo;
    int resultCodePrepare = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, &callback);
    ASSERT_EQ(resultCodePrepare, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    int resultCodeRelease = AieClientRelease(clientInfo, algoInfo, inputInfo);
    ASSERT_EQ(resultCodeRelease, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    AieClientDestroy(clientInfo);
}

/**
 * @tc.name: TestAieClientRelease003
 * @tc.desc: Test release function: the registered callback is empty and test case executes
             the method of AieClientSyncProcess.
 * @tc.type: FUNC
 * @tc.require: AR000F77NO
 */
HWTEST_F(ReleaseFunctionTest, TestAieClientRelease003, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientRelease003.");

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    const char *str = INPUT_CHARACTER;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = CLIENT_ID,
        .sessionId = SESSION_ID,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
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
        .extendMsg = (unsigned char*)inputData,
    };

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int resultCodeInit = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(resultCodeInit, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    IClientCb *callback = nullptr;
    DataInfo outputInfo;
    int resultCodePrepare = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, callback);
    ASSERT_EQ(resultCodePrepare, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    int resultCodeSyncProcess = AieClientSyncProcess(clientInfo, algoInfo, inputInfo, outputInfo);
    ASSERT_EQ(resultCodeSyncProcess, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    int resultCodeRelease = AieClientRelease(clientInfo, algoInfo, inputInfo);
    ASSERT_EQ(resultCodeRelease, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    AieClientDestroy(clientInfo);
}

/**
 * @tc.name: TestAieClientRelease004
 * @tc.desc: Test release function: the registered callback is not empty and test case executes
             the method of AieClientSyncProcess.
 * @tc.type: FUNC
 * @tc.require: AR000F77NO
 */
HWTEST_F(ReleaseFunctionTest, TestAieClientRelease004, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientRelease004.");

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    const char *str = INPUT_CHARACTER;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = CLIENT_ID,
        .sessionId = SESSION_ID,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };

    AlgorithmInfo algoInfo = {
        .clientVersion = ALGORITHM_INFO_CLIENT_VERSION,
        .isAsync = true,
        .algorithmType = ALGORITHM_TYPE_ASYNC,
        .algorithmVersion = ALGORITHM_VERSION,
        .isCloud = true,
        .operateId = OPERATE_ID,
        .requestId = REQUEST_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int resultCodeInit = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(resultCodeInit, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    ClientCallback callback = ClientCallback();
    DataInfo outputInfo;
    int resultCodePrepare = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, &callback);
    ASSERT_EQ(resultCodePrepare, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    int resultCodeAsyncProcess = AieClientAsyncProcess(clientInfo, algoInfo, inputInfo);
    ASSERT_EQ(resultCodeAsyncProcess, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    int resultCodeRelease = AieClientRelease(clientInfo, algoInfo, inputInfo);
    ASSERT_EQ(resultCodeRelease, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);

    AieClientDestroy(clientInfo);
}
