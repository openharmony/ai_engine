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
#include "service_dead_cb.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
    const char * const INPUT_CHARACTER_001 = "inputData001";
    const char * const INPUT_CHARACTER_002 = "Data002";
    const char * const CONFIG_DESCRIPTION = "config information";
    const int INTERVAL = 10;
    const long long CLIENT_INFO_VERSION = 1;
    const int CLIENT_ID = -1;
    const int SESSION_ID = -1;
    const int PRIORITY = 345;
    const int TIME_OUT = 456;
    const int EXTEND_LENGTH = 10;
    const long long ALGORITHM_INFO_CLIENT_VERSION = 2;
    const int ALGORITHM_TYPE = 0;
    const long long ALGORITHM_VERSION = 1;
    const int OPERATE_ID = 2;
    const int REQUEST_ID = 3;
    const int ALGORITHM_EXTEND_LENGTH = 0;
    const int CYCLENUMS = 5;
    constexpr uid_t INVALID_UID = 0;
}

class SyncProcessFunctionTest : public testing::Test {
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
        HILOGI("[Test]TestAieClientSyncProcess OnResult resultCode[%d],"\
            "requestId[%d], resultData[%s], resultLength[%d].",
            resultCode, requestId, result.data, result.length);
    }
};

static void TestGetRightInfo(ConfigInfo &configInfo, ClientInfo &clientInfo, AlgorithmInfo &algoInfo)
{
    HILOGI("[Test]Begin TestGetRightInfo");
    const char *str = INPUT_CHARACTER_002;
    char *inputData = const_cast<char*>(str);

    configInfo.description = CONFIG_DESCRIPTION;

    clientInfo.clientVersion = CLIENT_INFO_VERSION;
    clientInfo.clientId = CLIENT_ID;
    clientInfo.sessionId = SESSION_ID;
    clientInfo.serverUid = INVALID_UID,
    clientInfo.clientUid = INVALID_UID,
    clientInfo.extendLen = EXTEND_LENGTH;
    clientInfo.extendMsg = (unsigned char*)inputData;

    algoInfo.clientVersion = ALGORITHM_INFO_CLIENT_VERSION;
    algoInfo.isAsync = false;
    algoInfo.algorithmType = ALGORITHM_TYPE;
    algoInfo.algorithmVersion = ALGORITHM_VERSION;
    algoInfo.isCloud = true;
    algoInfo.operateId = OPERATE_ID;
    algoInfo.requestId = REQUEST_ID;
    algoInfo.extendLen = EXTEND_LENGTH;
    algoInfo.extendMsg = (unsigned char*)inputData;

    HILOGI("[Test]End TestGetRightInfo");
}

/**
 * @tc.name: TestAieClientSyncProcess001
 * @tc.desc: Test sync process function: the parameter of isAsync is false and the inputInfo is not null.
 * @tc.type: FUNC
 * @tc.require: AR000F77NQ
 */
HWTEST_F(SyncProcessFunctionTest, TestAieClientSyncProcess001, TestSize.Level0)
{
    HILOGI("[Test]Begin to testAieClientSyncProcess001");
    const char *str = INPUT_CHARACTER_001;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ConfigInfo configInfo;
    ClientInfo clientInfo;
    AlgorithmInfo algoInfo;

    TestGetRightInfo(configInfo, clientInfo, algoInfo);

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0,
    };
    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, nullptr);
    EXPECT_EQ(prepareRetCode, RETCODE_SUCCESS);

    outputInfo = {
        .data = nullptr,
        .length = 0,
    };
    int processRetCode = AieClientSyncProcess(clientInfo, algoInfo, inputInfo, outputInfo);
    EXPECT_EQ(processRetCode, RETCODE_SUCCESS);
}

/**
 * @tc.name: TestAieClientSyncProcess002
 * @tc.desc: Test sync process function: the parameter of isAsync is false and the inputInfo is null.
 * @tc.type: FUNC
 * @tc.require: AR000F77NQ
 */
HWTEST_F(SyncProcessFunctionTest, TestAieClientSyncProcess002, TestSize.Level0)
{
    HILOGI("[Test]Begin testAieClientSyncProcess002");
    const char *str = INPUT_CHARACTER_002;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = CLIENT_ID,
        .sessionId = SESSION_ID,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = EXTEND_LENGTH,
        .extendMsg = (unsigned char*)inputData,
    };

    AlgorithmInfo algoInfo = {
        .clientVersion = ALGORITHM_INFO_CLIENT_VERSION,
        .isAsync = false,
        .algorithmType = ALGORITHM_TYPE,
        .algorithmVersion = ALGORITHM_VERSION,
        .isCloud = true,
        .operateId = OPERATE_ID,
        .requestId = REQUEST_ID,
        .extendLen = EXTEND_LENGTH,
        .extendMsg = (unsigned char*)inputData,
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0,
    };

    DataInfo prepareInputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, prepareInputInfo, outputInfo, nullptr);
    EXPECT_EQ(prepareRetCode, RETCODE_SUCCESS);

    outputInfo = {
        .data = nullptr,
        .length = 0,
    };

    DataInfo processInputInfo = {
        .data = nullptr,
        .length = 0,
    };

    int processRetCode = AieClientSyncProcess(clientInfo, algoInfo, processInputInfo, outputInfo);
    EXPECT_EQ(processRetCode, RETCODE_SUCCESS);
}

/**
 * @tc.name: TestAieClientSyncProcess003
 * @tc.desc: Test sync process function: the parameter of isAsync is true.
 * @tc.type: FUNC
 * @tc.require: AR000F77NQ
 */
HWTEST_F(SyncProcessFunctionTest, TestAieClientSyncProcess003, TestSize.Level0)
{
    HILOGI("[Test]Begin testAieClientSyncProcess003");
    const char *str = INPUT_CHARACTER_002;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = CLIENT_ID,
        .sessionId = SESSION_ID,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = EXTEND_LENGTH,
        .extendMsg = (unsigned char*)inputData,
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
        .extendMsg = (unsigned char*)inputData,
    };

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0,
    };
    ClientCallback callback = ClientCallback();
    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, &callback);
    EXPECT_EQ(prepareRetCode, RETCODE_SUCCESS);

    outputInfo = {
        .data = nullptr,
        .length = 0,
    };

    int processRetCode = AieClientSyncProcess(clientInfo, algoInfo, inputInfo, outputInfo);
    EXPECT_NE(processRetCode, RETCODE_SUCCESS);
}

/**
 * @tc.name: TestAieClientSyncProcess004
 * @tc.desc: Test sync process function: the parameter of isAsync is false and the test case executes
             the method of AieClientSyncProcess five times.
 * @tc.type: FUNC
 * @tc.require: AR000F77NQ
 */
HWTEST_F(SyncProcessFunctionTest, TestAieClientSyncProcess004, TestSize.Level0)
{
    HILOGI("[Test]Begin testAieClientSyncProcess004");

    const char *str = INPUT_CHARACTER_001;
    char *inputData = const_cast<char *>(str);
    int len = strlen(str) + 1;

    ConfigInfo configInfo;
    ClientInfo clientInfo;
    AlgorithmInfo algoInfo;

    TestGetRightInfo(configInfo, clientInfo, algoInfo);

    DataInfo inputInfo = {
        .data = (unsigned char *)inputData,
        .length = len,
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0,
    };

    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, nullptr);
    EXPECT_EQ(prepareRetCode, RETCODE_SUCCESS);

    for (int i = 0; i < CYCLENUMS; ++i) {
        HILOGI("[Test]CycleNum is [%d]", i);
        int processRetCode = AieClientSyncProcess(clientInfo, algoInfo, inputInfo, outputInfo);
        EXPECT_EQ(processRetCode, RETCODE_SUCCESS);
    }
}
