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

#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

#include "gtest/gtest.h"

#include "client_executor/include/i_aie_client.inl"
#include "service_dead_cb.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
    const char * const INPUT_CHARACTER = "inputData";
    const char * const CONFIG_DESCRIPTION = "config information";
    const long long CLIENT_INFO_VERSION = 1;
    const int EXTEND_LENGTH = 10;
    const long long ALGORITHM_INFO_CLIENT_VERSION = 2;
    const int ALGORITHM_TYPE = 66;
    const long long ALGORITHM_VERSION = 2;
    const int OPERATE_ID = 2;
    const int REQUEST_ID = 3;
    const int CHAR_TYPE = 4;
    const char MIN_UPPER_CASE_CHAR = 'A';
    const char MIN_LOWER_CASE_CHAR = 'a';
    const char MIN_NUMERIC_CHAR = '0';
    const char TRAILING_CHAR = '\0';
    const int NUMBER_OF_ALPHABETS = 26;
    const int NUMBER_OF_DIGITS = 10;
    const int CHAR_TYPE_UPPER_CASE = 1;
    const int CHAR_TYPE_LOWER_CASE = 2;
    const int CHAR_TYPE_WHITE_SPACE = 3;
    const char WHITE_SPACE = ' ';
}

class InitFunctionTest : public testing::Test {
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

static void RandStr(const int len, char *str)
{
    srand(time(nullptr));
    int i;
    for (i = 0; i < len - 1; ++i) {
        switch (rand() % CHAR_TYPE) {
            case CHAR_TYPE_UPPER_CASE:
                str[i] = MIN_UPPER_CASE_CHAR + rand() % NUMBER_OF_ALPHABETS;
                break;
            case CHAR_TYPE_LOWER_CASE:
                str[i] = MIN_LOWER_CASE_CHAR + rand() % NUMBER_OF_ALPHABETS;
                break;
            case CHAR_TYPE_WHITE_SPACE:
                str[i] = WHITE_SPACE;
                break;
            default:
                str[i] = MIN_NUMERIC_CHAR + rand() % NUMBER_OF_DIGITS;
                break;
        }
    }

    str[i] = TRAILING_CHAR;
}

/**
 * @tc.name: TestAieClientInitConfigInfo001
 * @tc.desc: Test initial execution of certain algorithm plugin.
 * @tc.type: FUNC
 * @tc.require: AR000F77NM
 */
HWTEST_F(InitFunctionTest, TestAieClientInitConfigInfo001, TestSize.Level1)
{
    HILOGI("[Test]Begin TestAieClientInitConfigInfo001.");
    const char *str = INPUT_CHARACTER;
    char *inputData = const_cast<char*>(str);

    char configDesc[21];
    RandStr(21, configDesc);
    ConfigInfo configInfo {.description = configDesc};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = -1,
        .sessionId = -1,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = EXTEND_LENGTH,
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
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
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int initResult = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initResult, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);
    ASSERT_TRUE(clientInfo.sessionId > 0);

    int destroyResult = AieClientDestroy(clientInfo);
    ASSERT_EQ(destroyResult, RETCODE_SUCCESS);
}

/**
 * @tc.name: TestAieClientInitConfigInfo002
 * @tc.desc: Test initial execution of certain algorithm plugin with over-size config message.
 * @tc.type: FUNC
 * @tc.require: AR000F77NM
 */
HWTEST_F(InitFunctionTest, TestAieClientInitConfigInfo002, TestSize.Level1)
{
    HILOGI("[Test]Begin TestAieClientInitConfigInfo002.");
    const char *str = INPUT_CHARACTER;
    char *inputData = const_cast<char*>(str);

    char configDesc[129];
    RandStr(129, configDesc);
    ConfigInfo configInfo {.description = configDesc};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = -1,
        .sessionId = -1,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = EXTEND_LENGTH,
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
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
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int initResult = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initResult, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);
    ASSERT_TRUE(clientInfo.sessionId > 0);

    int destroyResult = AieClientDestroy(clientInfo);
    ASSERT_EQ(destroyResult, RETCODE_SUCCESS);
}

/**
 * @tc.name: TestAieClientInitConfigInfo003
 * @tc.desc: Test initial execution of certain algorithm plugin with much shorter(length = 1) config message.
 * @tc.type: FUNC
 * @tc.require: AR000F77NM
 */
HWTEST_F(InitFunctionTest, TestAieClientInitConfigInfo003, TestSize.Level1)
{
    HILOGI("[Test]Begin TestAieClientInitConfigInfo003.");
    const char *str = INPUT_CHARACTER;
    char *inputData = const_cast<char*>(str);

    char configDesc[1];
    RandStr(1, configDesc);
    ConfigInfo configInfo {.description = configDesc};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = -1,
        .sessionId = -1,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = EXTEND_LENGTH,
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
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
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int initResult = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initResult, RETCODE_SUCCESS);
    ASSERT_TRUE(clientInfo.clientId > 0);
    ASSERT_TRUE(clientInfo.sessionId > 0);

    int destroyResult = AieClientDestroy(clientInfo);
    ASSERT_EQ(destroyResult, RETCODE_SUCCESS);
}

/**
 * @tc.name: TestAieClientInitClientInfo001
 * @tc.desc: Test initial execution of certain algorithm plugin with 'clientId' being -1.
 * @tc.type: FUNC
 * @tc.require: AR000F77NM
 */
HWTEST_F(InitFunctionTest, TestAieClientInitClientInfo001, TestSize.Level1)
{
    HILOGI("[Test]Begin TestAieClientInitClientInfo001.");
    const char *str = INPUT_CHARACTER;
    char *inputData = const_cast<char*>(str);

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = -1,
        .sessionId = -1,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = EXTEND_LENGTH,
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
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
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int initResult = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initResult, RETCODE_SUCCESS);

    int destroyResult = AieClientDestroy(clientInfo);
    ASSERT_EQ(destroyResult, RETCODE_SUCCESS);
}

/**
 * @tc.name: TestAieClientInitAlgoInfo001
 * @tc.desc: Test initial execution of certain algorithm plugin with client
 *           with 'clientId' being -1 and 'isAsync' being true.
 * @tc.type: FUNC
 * @tc.require: AR000F77NM
 */
HWTEST_F(InitFunctionTest, TestAieClientInitAlgoInfo001, TestSize.Level1)
{
    HILOGI("[Test]Begin TestAieClientInitAlgoInfo001.");
    const char *str = INPUT_CHARACTER;
    char *inputData = const_cast<char*>(str);

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = -1,
        .sessionId = -1,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = EXTEND_LENGTH,
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
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
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int initResult = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initResult, RETCODE_SUCCESS);

    int destroyResult = AieClientDestroy(clientInfo);
    ASSERT_EQ(destroyResult, RETCODE_SUCCESS);
}

/**
 * @tc.name: TestAieClientInitAlgoInfo002
 * @tc.desc: Test initial execution of certain algorithm plugin with client
 *           with 'clientId' being -1 and 'isAsync' being false.
 * @tc.type: FUNC
 * @tc.require: AR000F77NM
 */
HWTEST_F(InitFunctionTest, TestAieClientInitAlgoInfo002, TestSize.Level1)
{
    HILOGI("[Test]Begin TestAieClientInitAlgoInfo002.");
    const char *str = INPUT_CHARACTER;
    char *inputData = const_cast<char*>(str);

    ConfigInfo configInfo {.description = CONFIG_DESCRIPTION};

    ClientInfo clientInfo = {
        .clientVersion = CLIENT_INFO_VERSION,
        .clientId = -1,
        .sessionId = -1,
        .serverUid = INVALID_UID,
        .clientUid = INVALID_UID,
        .extendLen = EXTEND_LENGTH,
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
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
        .extendMsg = reinterpret_cast<unsigned char*>(inputData),
    };

    ServiceDeadCb cb = ServiceDeadCb();
    int initResult = AieClientInit(configInfo, clientInfo, algoInfo, &cb);
    ASSERT_EQ(initResult, RETCODE_SUCCESS);

    int destroyResult = AieClientDestroy(clientInfo);
    ASSERT_EQ(destroyResult, RETCODE_SUCCESS);
}
