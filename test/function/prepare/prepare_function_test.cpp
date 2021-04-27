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
#include "server_executor/include/server_executor.h"
#include "service_dead_cb.h"
#include "utils/aie_macros.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
    const int REQUEST_ID = 1;
    const int TRANSACTION_ID = 10000001;
    const int PRIORITY = 4;
    const int TIME_OUT = 100;
    const int ALGORITHM_PLUGIN_TYPE = 0;
    const int IVP_PLUGIN = 0;
    const int CHILDMODE_PLUGIN = 1;
    const int OPERATE_ID = 2;
    const long long CLIENT_INFO_VERSION = 1;
    const int SESSION_ID = -1;
    const long long ALGORITHM_INFO_CLIENT_VERSION = 1;
    const int ALGORITHM_SYNC_TYPE = 0;
    const int ALGORITHM_ASYNC_TYPE = 1;
    const long long ALGORITHM_VERSION = 1;
    const char * const CONFIG_DESCRIPTION = "Prepare config information";
    const char * const PREPARE_INPUT_SYNC = "Sync prepare inputData";
    const char * const PREPARE_INPUT_ASYNC = "Async prepare inputData";
}

class ClientCallback : public IClientCb {
public:
    ClientCallback() = default;
    ~ClientCallback() override = default;
    void OnResult(const DataInfo &result, int resultCode, int requestId) override
    {
        HILOGI("[Test]TestAieClientAsyncProcess execute ClientCallbackOnResult."\
            " resultCode[%d], requestId[%d], resultData[%s], resultLength[%d].",
            resultCode, requestId, result.data, result.length);
    }
};

class PrepareFunctionTest : public testing::Test {
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
 * @tc.name: TestAlgorithmInfo001
 * @tc.desc: Test preparing execution of certain synchronous plugin.
 * @tc.type: FUNC
 * @tc.require: AR000F77NN
 */
HWTEST_F(PrepareFunctionTest, TestAlgorithmInfo001, TestSize.Level1)
{
    HILOGI("[Test]TestAlgorithmInfo001.");
    const char *str = PREPARE_INPUT_SYNC;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = SESSION_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };

    AlgorithmInfo algoInfo = {
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

    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    DataInfo outputInfo;
    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, nullptr);
    EXPECT_EQ(prepareRetCode, RETCODE_SUCCESS);
    EXPECT_NE(outputInfo.data, nullptr);
}

/**
 * @tc.name: TestAlgorithmInfo002
 * @tc.desc: Test preparing execution of certain asynchronous plugin.
 * @tc.type: FUNC
 * @tc.require: AR000F77NN
 */
HWTEST_F(PrepareFunctionTest, TestAlgorithmInfo002, TestSize.Level1)
{
    HILOGI("[Test]TestAlgorithmInfo002.");
    const char *str = PREPARE_INPUT_ASYNC;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = SESSION_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };

    AlgorithmInfo algoInfo = {
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

    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0
    };
    ClientCallback callback = ClientCallback();
    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, &callback);
    EXPECT_EQ(prepareRetCode, RETCODE_SUCCESS);
    EXPECT_NE(outputInfo.data, nullptr);
}

/**
 * @tc.name: TestInputInfo001
 * @tc.desc: Test preparing execution of certain synchronous plugin
 *           with 'inputInfo.data = nullptr' and 'outputInfo.data = nullptr'.
 * @tc.type: FUNC
 * @tc.require: AR000F77NN
 */
HWTEST_F(PrepareFunctionTest, TestInputInfo001, TestSize.Level1)
{
    HILOGI("[Test]TestInputInfo001.");
    const char *str = PREPARE_INPUT_ASYNC;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = SESSION_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };

    AlgorithmInfo algoInfo = {
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

    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    DataInfo inputInfo = {
        .data = nullptr,
        .length = 0,
    };

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0
    };
    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, nullptr);
    EXPECT_EQ(prepareRetCode, RETCODE_SUCCESS);
    EXPECT_NE(outputInfo.data, nullptr);
}

/**
 * @tc.name: TestInputInfo002
 * @tc.desc: Test preparing execution of certain asynchronous plugin
 *           with 'inputInfo.data = nullptr' and 'outputInfo.data = nullptr'.
 * @tc.type: FUNC
 * @tc.require: AR000F77NJ
 */
HWTEST_F(PrepareFunctionTest, TestInputInfo002, TestSize.Level1)
{
    HILOGI("[Test]TestInputInfo002.");
    const char *str = PREPARE_INPUT_ASYNC;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = SESSION_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };

    AlgorithmInfo algoInfo = {
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

    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    DataInfo inputInfo = {
        .data = nullptr,
        .length = 0,
    };

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0
    };
    ClientCallback callback = ClientCallback();
    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, &callback);
    EXPECT_EQ(prepareRetCode, RETCODE_SUCCESS);
    EXPECT_NE(outputInfo.data, nullptr);
}

/**
 * @tc.name: TestCallback001
 * @tc.desc: Test preparing execution of certain plugin
 *           with 'isAsync = false' and 'callback != nullptr'.
 * @tc.type: FUNC
 * @tc.require: AR000F77NJ
 */
HWTEST_F(PrepareFunctionTest, TestCallback001, TestSize.Level1)
{
    HILOGI("[Test]TestCallback001.");
    const char *str = PREPARE_INPUT_ASYNC;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = SESSION_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };

    AlgorithmInfo algoInfo = {
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

    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0
    };
    ClientCallback callback = ClientCallback();
    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, &callback);
    EXPECT_EQ(prepareRetCode, RETCODE_SUCCESS);
    EXPECT_NE(outputInfo.data, nullptr);
}

/**
 * @tc.name: TestCallback002
 * @tc.desc: Test preparing execution of certain plugin
 *           with 'isAsync = true' and 'callback = nullptr'.
 * @tc.type: FUNC
 * @tc.require: AR000F77NJ
 */
HWTEST_F(PrepareFunctionTest, TestCallback002, TestSize.Level1)
{
    HILOGI("[Test]TestCallback002.");
    const char *str = PREPARE_INPUT_ASYNC;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = SESSION_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)inputData,
    };

    AlgorithmInfo algoInfo = {
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

    ServiceDeadCb cb = ServiceDeadCb();
    int initRetCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initRetCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    DataInfo inputInfo = {
        .data = (unsigned char*)inputData,
        .length = len,
    };

    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0
    };
    int prepareRetCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, nullptr);
    EXPECT_NE(prepareRetCode, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo.data, nullptr);
}
