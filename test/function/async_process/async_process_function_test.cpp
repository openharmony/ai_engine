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
#include "communication_adapter/include/sa_async_handler.h"
#include "communication_adapter/include/sa_client.h"
#include "platform/time/include/time.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "server_executor/include/server_executor.h"
#include "service_dead_cb.h"
#include "utils/aie_macros.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
    const char * const INPUT_DATA = "inputData";
    const char * const EXTEND_MSG = "extendMsg";
    const char * const CONFIG_DESCRIPTION = "config information";

    const int REQUEST_ID = 1;
    const int PRIORITY = 4;
    const int TIME_OUT = 100;
    const int OPERATE_ID = 2;
    const long long CLIENT_INFO_VERSION = 1;
    const int ALGORITHM_ASYNC_TYPE = 1;
    const long long ALGORITHM_VERSION = 1;
    const int DATA_INFO_LEN = 6;
    const int WAIT_CALLBACK_TIME_MS = 2000;
}

class AsyncProcessFunctionTest : public testing::Test {
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
        HILOGI("[Test]TestAieClientAsyncProcess execute ClientCallbackOnResult."\
            " resultCode[%d], requestId[%d], resultData[%s], resultLength[%d].",
            resultCode, requestId, result.data, result.length);
    }
};

static void PreBuildInfo(ConfigInfo &configInfo, ClientInfo &clientInfo, AlgorithmInfo &algoInfo,
    bool isAsync, int requestId)
{
    const char *str = EXTEND_MSG;
    char *extendMsg = const_cast<char*>(str);
    int len = strlen(str) + 1;

    configInfo.description = CONFIG_DESCRIPTION;

    clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = INVALID_SESSION_ID,
        .extendLen = len,
        .extendMsg = (unsigned char*)extendMsg,
    };

    algoInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .isAsync = isAsync,
        .algorithmType = ALGORITHM_ASYNC_TYPE,
        .algorithmVersion = ALGORITHM_VERSION,
        .isCloud = true,
        .operateId = OPERATE_ID,
        .requestId = requestId,
        .extendLen = len,
        .extendMsg = (unsigned char*)extendMsg,
    };
}

/**
 * @tc.name: TestAieClientAsyncProcess001
 * @tc.desc: Test asynchronous execution of certain algorithm plugin.
 * @tc.type: FUNC
 * @tc.require: AR000F77NK
 */
HWTEST_F(AsyncProcessFunctionTest, TestAieClientAsyncProcess001, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientAsyncProcess001 starts.");

    ConfigInfo configInfo;
    ClientInfo clientInfo;
    AlgorithmInfo algoInfo;
    PreBuildInfo(configInfo, clientInfo, algoInfo, true, REQUEST_ID);

    ServiceDeadCb cb = ServiceDeadCb();
    int returnInitCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(returnInitCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    DataInfo inputInfo;
    const char *str = INPUT_DATA;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;
    inputInfo.data = (unsigned char *)inputData;
    inputInfo.length = len;

    ClientCallback callback = ClientCallback();
    DataInfo outputInfo;
    int returnPrepareCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, &callback);
    EXPECT_EQ(returnPrepareCode, RETCODE_SUCCESS);
    EXPECT_NE(outputInfo.data, nullptr);
    EXPECT_TRUE(outputInfo.length > 0);

    int returnProcessCode = AieClientAsyncProcess(clientInfo, algoInfo, inputInfo);
    EXPECT_EQ(returnProcessCode, RETCODE_SUCCESS);

    StepSleepMs(WAIT_CALLBACK_TIME_MS);

    int returnReleaseCode = AieClientRelease(clientInfo, algoInfo, inputInfo);
    EXPECT_EQ(returnReleaseCode, RETCODE_SUCCESS);

    int returnDestroyCode = AieClientDestroy(clientInfo);
    EXPECT_EQ(returnDestroyCode, RETCODE_SUCCESS);
}

/**
 * @tc.name: TestAieClientAsyncProcess002
 * @tc.desc: Test asynchronous execution of certain algorithm plugin with 'inputInfo.data' being 'nullptr'.
 * @tc.type: FUNC
 * @tc.require: AR000F77NK
 */
HWTEST_F(AsyncProcessFunctionTest, TestAieClientAsyncProcess002, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientAsyncProcess002 starts.");

    ConfigInfo configInfo;
    ClientInfo clientInfo;
    AlgorithmInfo algoInfo;
    PreBuildInfo(configInfo, clientInfo, algoInfo, true, REQUEST_ID);

    ServiceDeadCb cb = ServiceDeadCb();
    int returnInitCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(returnInitCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    DataInfo inputInfo;
    const char *str = INPUT_DATA;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;
    inputInfo.data = (unsigned char *)inputData;
    inputInfo.length = len;

    ClientCallback callback = ClientCallback();
    DataInfo outputInfo;
    int returnPrepareCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, &callback);
    EXPECT_EQ(returnPrepareCode, RETCODE_SUCCESS);

    inputInfo.data = nullptr;
    inputInfo.length = 0;

    int returnProcessCode = AieClientAsyncProcess(clientInfo, algoInfo, inputInfo);
    EXPECT_EQ(returnProcessCode, RETCODE_SUCCESS);

    StepSleepMs(WAIT_CALLBACK_TIME_MS);
    int returnReleaseCode = AieClientRelease(clientInfo, algoInfo, inputInfo);
    EXPECT_EQ(returnReleaseCode, RETCODE_SUCCESS);

    int returnDestroyCode = AieClientDestroy(clientInfo);
    EXPECT_EQ(returnDestroyCode, RETCODE_SUCCESS);
}

/**
 * @tc.name: TestAieClientAsyncProcess003
 * @tc.desc: Test asynchronous execution of certain algorithm plugin with 'isAsync' being false.
 * @tc.type: FUNC
 * @tc.require: AR000F77NK
 */
HWTEST_F(AsyncProcessFunctionTest, TestAieClientAsyncProcess003, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientAsyncProcess003 starts.");

    ConfigInfo configInfo;
    ClientInfo clientInfo;
    AlgorithmInfo algoInfo;
    PreBuildInfo(configInfo, clientInfo, algoInfo, false, REQUEST_ID);

    ServiceDeadCb cb = ServiceDeadCb();
    int returnInitCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(returnInitCode, RETCODE_SUCCESS);
    EXPECT_TRUE(clientInfo.clientId > 0);

    DataInfo inputInfo;
    const char *str = INPUT_DATA;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;
    inputInfo.data = (unsigned char *)inputData;
    inputInfo.length = len;

    ClientCallback callback = ClientCallback();
    DataInfo outputInfo;
    int returnPrepareCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, &callback);
    EXPECT_EQ(returnPrepareCode, RETCODE_SUCCESS);
    EXPECT_NE(outputInfo.data, nullptr);
    EXPECT_TRUE(outputInfo.length > 0);

    int returnProcessCode = AieClientAsyncProcess(clientInfo, algoInfo, inputInfo);
    EXPECT_NE(returnProcessCode, RETCODE_SUCCESS);

    StepSleepMs(WAIT_CALLBACK_TIME_MS);

    int returnReleaseCode = AieClientRelease(clientInfo, algoInfo, inputInfo);
    EXPECT_EQ(returnReleaseCode, RETCODE_SUCCESS);

    int returnDestroyCode = AieClientDestroy(clientInfo);
    EXPECT_EQ(returnDestroyCode, RETCODE_SUCCESS);
}
