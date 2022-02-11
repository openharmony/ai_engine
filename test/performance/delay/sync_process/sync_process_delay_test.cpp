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
#ifdef __LINUX__
#   include <unistd.h>
#   include <fcntl.c>
#endif

#include "gtest/gtest.h"

#include "client_executor/include/i_aie_client.inl"
#include "platform/time/include/time.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "service_dead_cb.h"
#include "utils/aie_macros.h"
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
    const int CHILD_MODE_PLUGIN = 1;
    const int OPERATE_ID = 2;
    const long long CLIENT_INFO_VERSION = 1;
    const int SESSION_ID = -1;
    const long long ALGORITHM_INFO_CLIENT_VERSION = 1;
    const int ALGORITHM_SYNC_TYPE = 0;
    const int ALGORITHM_ASYNC_TYPE = 1;
    const long long ALGORITHM_VERSION = 1;
    const int EXECUTE_TIMES = 100;
    const int COUNT = 20;
    const int CHAR_TYPE = 4;
    const int DESCRIPTION_LENGTH = 129;
    const int ALPHABET_LENGTH = 26;
    const int DIGIT = 10;
    const int UPPER_POSITION = 1;
    const int LOWER_POSITION = 2;
    const int SPACE_POSITION = 3;
    const char MIN_UPPER_CASE_CHAR = 'A';
    const char MIN_LOWER_CASE_CHAR = 'a';
    const char MIN_NUMERIC_CHAR = '0';
    const char TRAILING_CHAR = '\0';
    const char * const CONFIG_DESCRIPTION = "Prepare config information";
    const char * const PREPARE_INPUT_SYNC = "Sync prepare inputData";
    const char * const PREPARE_INPUT_ASYNC = "Async prepare inputData";
    const int EXCEPTED_SYNC_PROCESS_TIME = 30;
}

class SyncProcessTimeTest : public testing::Test {
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

static int Random(void)
{
#ifndef __LINUX__
    return -1;
#else
#ifndef O_RDONLY
#define O_RDONLY 0u
#endif
    int r = -1;
    int fd = open("/dev/random", O_RDONLY);
    fd = open("/dev/random", O_RDONLY);
    if (fd > 0) {
        read(fd, &r, sizeof(int));
    }
    close(fd);

    return r;
#endif
}

static void RandStr(const int len, char *str)
{
    int i;
    for (i = 0; i < len - 1; ++i) {
        switch (Random() % CHAR_TYPE) {
            case UPPER_POSITION:
                str[i] = MIN_UPPER_CASE_CHAR + Random() % ALPHABET_LENGTH;
                break;
            case LOWER_POSITION:
                str[i] = MIN_LOWER_CASE_CHAR + Random() % ALPHABET_LENGTH;
                break;
            case SPACE_POSITION:
                str[i] = ' ';
                break;
            default:
                str[i] = MIN_NUMERIC_CHAR + Random() % DIGIT;
                break;
        }
    }

    str[i] = TRAILING_CHAR;
}

/**
 * @tc.name: TestSyncTime001
 * @tc.desc: Test Time Consumption of Sync Process Interface in AI System.
 * @tc.type: PERF
 * @tc.require: AR000F77MI
 */
HWTEST_F(SyncProcessTimeTest, TestSyncTime001, TestSize.Level0)
{
    HILOGI("[Test]SyncProcessTimeTest001.");
    long long processTotalTime = 0;

    const char *str = PREPARE_INPUT_SYNC;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    for (int i = 0; i < COUNT; ++i) {
        HILOGI("[Test]Test delay times:[%d]", i);
        char config[DESCRIPTION_LENGTH];
        RandStr(DESCRIPTION_LENGTH, config);
        ConfigInfo configInfo {.description = config};
        ClientInfo clientInfo = {
            .clientVersion = CLIENT_INFO_VERSION,
            .clientId = INVALID_CLIENT_ID,
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

        ServiceDeadCb *cb = nullptr;
        AIE_NEW(cb, ServiceDeadCb());
        ASSERT_NE(cb, nullptr);
        int resultCode = AieClientInit(configInfo, clientInfo, algoInfo, cb);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        DataInfo inputInfo = {
            .data = (unsigned char*)inputData,
            .length = len,
        };
        DataInfo outputInfo;
        resultCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, nullptr);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        int optionType = 0;
        resultCode = AieClientSetOption(clientInfo, optionType, inputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        outputInfo = {
            .data = nullptr,
            .length = 0
        };
        resultCode = AieClientGetOption(clientInfo, optionType, inputInfo, outputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        std::time_t processStartTime = GetCurTimeMillSec();
        resultCode = AieClientSyncProcess(clientInfo, algoInfo, inputInfo, outputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);
        std::time_t processEndTime = GetCurTimeMillSec();
        processTotalTime += processEndTime - processStartTime;

        resultCode = AieClientRelease(clientInfo, algoInfo, inputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);

        resultCode = AieClientDestroy(clientInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);
        AIE_DELETE(cb);
    }
    std::time_t duration = processTotalTime / EXECUTE_TIMES;
    HILOGI("[Test][CheckTimeSyncProcess][%lld]", duration);
    ASSERT_TRUE((duration > 0) && (duration <= EXCEPTED_SYNC_PROCESS_TIME));
}