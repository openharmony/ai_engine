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
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "service_dead_cb.h"
#include "utils/aie_macros.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
    const int REQUEST_ID = 1;
    const int PRIORITY = 4;
    const int TIME_OUT = 100;
    const int OPERATE_ID = 2;
    const long long CLIENT_INFO_VERSION = 1;
    const long long ALGORITHM_INFO_CLIENT_VERSION = 1;
    const int ALGORITHM_SYNC_TYPE = 0;
    const int ALGORITHM_ASYNC_TYPE = 1;
    const long long ALGORITHM_VERSION = 1;
    const int RAND = 2;
    const char * const CONFIG_DESCRIPTION = "Prepare config information";
    const char * const PREPARE_INPUT_SYNC = "Sync prepare inputData";
    const char * const EXTEND_INFORMATION = "Extended information";
    const char * const SET_OPTION_INPUT = "First set option inputData";
    const char * const SET_OPTION_DATA = "Second set option inputData";
    constexpr uid_t INVALID_UID = 0;
}

class OptionFunctionTest : public testing::Test {
public:
    // SetUpTestCase:The preset action of the test suite is executed before the first TestCase
    static void SetUpTestCase()
    {
        srand(time(nullptr));
    };

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
        HILOGI("[Test]TestSetOption/GetOption OnResult resultCode[%d], requestId[%d], resultData[%s].",
            resultCode, requestId, result.data);
    }
};

static void GetConfigInfo(ConfigInfo &configInfo)
{
    configInfo = {.description = CONFIG_DESCRIPTION};
}

static void GetClientInfo(ClientInfo &clientInfo)
{
    const char *str = EXTEND_INFORMATION;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = INVALID_SESSION_ID,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };
}

static void GetSyncAlgorithmInfo(AlgorithmInfo &algoInfo)
{
    const char *str = EXTEND_INFORMATION;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    algoInfo = {
        .clientVersion = ALGORITHM_INFO_CLIENT_VERSION,
        .isAsync = false,
        .algorithmType = ALGORITHM_SYNC_TYPE,
        .algorithmVersion = ALGORITHM_VERSION,
        .isCloud = true,
        .operateId = OPERATE_ID,
        .requestId = REQUEST_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };
}

static void GetAsyncAlgorithmInfo(AlgorithmInfo &algoInfo)
{
    const char *str = EXTEND_INFORMATION;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    algoInfo = {
        .clientVersion = ALGORITHM_INFO_CLIENT_VERSION,
        .isAsync = true,
        .algorithmType = ALGORITHM_ASYNC_TYPE,
        .algorithmVersion = ALGORITHM_VERSION,
        .isCloud = true,
        .operateId = OPERATE_ID,
        .requestId = REQUEST_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };
}

/**
 * @tc.name: TestOption001
 * @tc.desc: Test get/set option function: after loading the plugin,
             the test case sets the parameter value and gets the parameter value successfully.
 * @tc.type: FUNC
 * @tc.require: AR000F77NP
 */
HWTEST_F(OptionFunctionTest, TestOption001, TestSize.Level0)
{
    HILOGI("[Test]TestOption001.");
    ConfigInfo configInfo;
    GetConfigInfo(configInfo);

    ClientInfo clientInfo;
    GetClientInfo(clientInfo);

    // Randomly generate synchronous or asynchronous algorithm information.
    AlgorithmInfo algoInfo;
    if (rand() % RAND == 0) {
        HILOGI("[Test]The current algorithm information is asynchronous.");
        GetAsyncAlgorithmInfo(algoInfo);
    } else {
        HILOGI("[Test]The current algorithm information is synchronous.");
        GetSyncAlgorithmInfo(algoInfo);
    }
    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    const char *str = PREPARE_INPUT_SYNC;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0
    };

    ClientCallback *callback = nullptr;
    if (algoInfo.isAsync) {
        AIE_NEW(callback, ClientCallback());
    }
    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, callback);
    EXPECT_EQ(prepareRetCode, RETCODE_SUCCESS);
    EXPECT_NE(outputInfo.data, nullptr);
    EXPECT_TRUE(outputInfo.length > 0);

    str = SET_OPTION_INPUT;
    inputData = const_cast<char*>(str);
    len = strlen(str) + 1;

    inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };
    int optionType = 0;
    int setOptionRetCode = AieClientSetOption(clientInfo, optionType, inputInfo);
    EXPECT_EQ(setOptionRetCode, RETCODE_SUCCESS);

    outputInfo = {
        .data = nullptr,
        .length = 0
    };
    int getRetCode = AieClientGetOption(clientInfo, optionType, inputInfo, outputInfo);
    EXPECT_EQ(getRetCode, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo.length, inputInfo.length);
    AIE_DELETE(callback);
}

/**
 * @tc.name: TestOption002
 * @tc.desc: Test get/set option function: after loading the plugin,
             the test case sets the parameter value and gets the parameter value successfully for many times.
 * @tc.type: FUNC
 * @tc.require: AR000F77NP
 */
HWTEST_F(OptionFunctionTest, TestOption002, TestSize.Level0)
{
    HILOGI("[Test]TestOption002.");
    ConfigInfo configInfo;
    GetConfigInfo(configInfo);

    ClientInfo clientInfo;
    GetClientInfo(clientInfo);

    // Randomly generate synchronous or asynchronous algorithm information.
    AlgorithmInfo algoInfo;
    if (rand() % RAND == 0) {
        HILOGI("[Test]The current algorithm information is asynchronous.");
        GetAsyncAlgorithmInfo(algoInfo);
    } else {
        HILOGI("[Test]The current algorithm information is synchronous.");
        GetSyncAlgorithmInfo(algoInfo);
    }
    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    const char *str = PREPARE_INPUT_SYNC;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;
    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0
    };

    ClientCallback *callback = nullptr;
    if (algoInfo.isAsync) {
        AIE_NEW(callback, ClientCallback());
    }

    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, callback);
    EXPECT_EQ(prepareRetCode, RETCODE_SUCCESS);
    EXPECT_NE(outputInfo.data, nullptr);
    EXPECT_TRUE(outputInfo.length > 0);

    str = SET_OPTION_INPUT;
    inputData = const_cast<char*>(str);
    len = strlen(str) + 1;
    inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };
    int optionType = 0;
    int setOptionRetCode = AieClientSetOption(clientInfo, optionType, inputInfo);
    EXPECT_EQ(setOptionRetCode, RETCODE_SUCCESS);

    int getOptionRetCode = AieClientGetOption(clientInfo, optionType, inputInfo, outputInfo);
    EXPECT_EQ(getOptionRetCode, RETCODE_SUCCESS);

    str = SET_OPTION_DATA;
    inputData = const_cast<char*>(str);
    len = strlen(str) + 1;
    inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    int setRetCode = AieClientSetOption(clientInfo, optionType, inputInfo);
    EXPECT_EQ(setRetCode, RETCODE_SUCCESS);

    int getRetCode = AieClientGetOption(clientInfo, optionType, inputInfo, outputInfo);
    EXPECT_EQ(getRetCode, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo.length, inputInfo.length);
    AIE_DELETE(callback);
}

/**
 * @tc.name: TestOption003
 * @tc.desc: Test get/set option function: the test case sets the parameter value and gets the parameter value
             which does not load the plugin.
 * @tc.type: FUNC
 * @tc.require: AR000F77NP
 */
HWTEST_F(OptionFunctionTest, TestOption003, TestSize.Level0)
{
    HILOGI("[Test]TestOption003.");
    ConfigInfo configInfo;
    GetConfigInfo(configInfo);

    ClientInfo clientInfo;
    GetClientInfo(clientInfo);

    // Randomly generate synchronous or asynchronous algorithm information.
    AlgorithmInfo algoInfo;
    if (rand() % RAND == 0) {
        HILOGI("[Test]The current algorithm information is asynchronous.");
        GetAsyncAlgorithmInfo(algoInfo);
    } else {
        HILOGI("[Test]The current algorithm information is synchronous.");
        GetSyncAlgorithmInfo(algoInfo);
    }
    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    const char *str = SET_OPTION_INPUT;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };
    int optionType = 0;
    int setOptionRetCode = AieClientSetOption(clientInfo, optionType, inputInfo);
    EXPECT_NE(setOptionRetCode, RETCODE_SUCCESS);

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0
    };
    int getOptionRetCode = AieClientGetOption(clientInfo, optionType, inputInfo, outputInfo);
    EXPECT_NE(getOptionRetCode, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo.data, nullptr);
    EXPECT_EQ(outputInfo.length, 0);
}

/**
 * @tc.name: TestOption004
 * @tc.desc: Test get/set option function: the test case gets the parameter value
             which does not set the parameter value and load the plugin.
 * @tc.type: FUNC
 * @tc.require: AR000F77NP
 */
HWTEST_F(OptionFunctionTest, TestOption004, TestSize.Level0)
{
    HILOGI("[Test]TestOption004.");

    ConfigInfo configInfo;
    GetConfigInfo(configInfo);

    ClientInfo clientInfo;
    GetClientInfo(clientInfo);

    // Randomly generate synchronous or asynchronous algorithm information.
    AlgorithmInfo algoInfo;
    if (rand() % RAND == 0) {
        HILOGI("[Test]The current algorithm information is asynchronous.");
        GetAsyncAlgorithmInfo(algoInfo);
    } else {
        HILOGI("[Test]The current algorithm information is synchronous.");
        GetSyncAlgorithmInfo(algoInfo);
    }
    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    int optionType = 0;

    const char *str = SET_OPTION_DATA;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0
    };
    int getOptionRetCode = AieClientGetOption(clientInfo, optionType, inputInfo, outputInfo);
    EXPECT_NE(getOptionRetCode, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo.data, nullptr);
    EXPECT_EQ(outputInfo.length, 0);
}
