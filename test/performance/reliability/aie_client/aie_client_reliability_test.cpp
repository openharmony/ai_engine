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
#include <ctime>
#include <unistd.h>

#include "gtest/gtest.h"

#include "client_executor/include/i_aie_client.inl"
#include "kits/ai_retcode.h"
#include "platform/time/include/time.h"
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
    const int ALGORITHM_SYNC_TYPE = 0;
    const int ALGORITHM_ASYNC_TYPE = 1;
    const long long ALGORITHM_VERSION = 1;
    const int OPERATE_ID = 2;
    const int REQUEST_ID = 3;
    const int ALGORITHM_EXTEND_LENGTH = 0;
    const time_t TIME_TEST = 20;
}

class AieClientReliabilityTest : public testing::Test {
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
static void FreeDataInfo(DataInfo &dataInfo)
{
    if (dataInfo.data == nullptr || dataInfo.length <= 0) {
        return;
    }
    free(dataInfo.data);
    dataInfo.data = nullptr;
    dataInfo.length = 0;
}

/**
 * @tc.name: AieClientSyncReliabilityTest001
 * @tc.desc: Test AieClient sync reliability
 * @tc.type: RELI
 * @tc.require: AR000F77MI
 */
HWTEST_F(AieClientReliabilityTest, AieClientSyncReliabilityTest001, TestSize.Level0)
{
    HILOGI("[Test]TestSyncAieClientReliability.");

    time_t asyncStart = GetCurTimeSec();
    while (true) {
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
            .algorithmType = ALGORITHM_SYNC_TYPE,
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

        ServiceDeadCb cb;
        int resultCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        DataInfo outputInfo;

        resultCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, nullptr);
        FreeDataInfo(outputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        resultCode = AieClientSyncProcess(clientInfo, algoInfo, inputInfo, outputInfo);
        FreeDataInfo(outputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        resultCode = AieClientRelease(clientInfo, algoInfo, inputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        resultCode = AieClientDestroy(clientInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        time_t currentTime = GetCurTimeSec();
        if ((currentTime - asyncStart) >= TIME_TEST) {
            break;
        }
    }
}

/**
 * @tc.name: AieClientAsyncReliabilityTest001
 * @tc.desc: Test AieClient async reliability
 * @tc.type: RELI
 * @tc.require: AR000F77MI
 */
HWTEST_F(AieClientReliabilityTest, AieClientAsyncReliabilityTest001, TestSize.Level0)
{
    HILOGI("[Test]TestAsyncAieClientReliability.");

    time_t asyncStart = GetCurTimeSec();
    while (true) {
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
            .algorithmType = ALGORITHM_ASYNC_TYPE,
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

        ServiceDeadCb cb;
        int resultCode = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        ClientCallback callback;

        DataInfo outputInfo;
        resultCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, &callback);
        FreeDataInfo(outputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        resultCode = AieClientAsyncProcess(clientInfo, algoInfo, inputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);
        StepSleepMs(500);

        resultCode = AieClientRelease(clientInfo, algoInfo, inputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        resultCode = AieClientDestroy(clientInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        time_t currentTime = GetCurTimeSec();
        if ((currentTime - asyncStart) >= TIME_TEST) {
            break;
        }
    }
}
