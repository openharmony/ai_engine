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

#include <future>
#include <unistd.h>

#include "gtest/gtest.h"
#include "securec.h"

#include "client_executor/include/i_aie_client.inl"
#include "service_dead_cb.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
const char * const CONFIG_DESCRIPTION = "config information";
constexpr int ALGORITHM_TYPE_SYNC = 0; // identity for sync algorithm, determines ALGORITHM_TYPE_ID_LIST[algorithmType]
constexpr int ALGORITHM_TYPE_ASYNC = 1; // identity for async algorithm, determines ALGORITHM_TYPE_ID_LIST[algorithmType]
constexpr long long ALGORITHM_VERSION = 1;
constexpr uint32_t LONG_MEMORY_LENGTH = 2U * 1024U * 1024U; // 2 MB long data
constexpr int RAND = 2;
constexpr char DUMP_CONTENT = 'm'; // randomly chosen to stuff inputInfo.

void FreeDataInfo(DataInfo *dataInfo)
{
    if (dataInfo != nullptr && dataInfo->data != nullptr) {
        free(dataInfo->data);
        dataInfo->data = nullptr;
        dataInfo->length = 0;
    }
}

void FreeAlgorithmInfo(AlgorithmInfo *algorithmInfo)
{
    if (algorithmInfo != nullptr && algorithmInfo->extendMsg != nullptr) {
        free(algorithmInfo->extendMsg);
        algorithmInfo->extendMsg = nullptr;
        algorithmInfo->extendLen = 0;
    }
}

void FreeClientInfo(ClientInfo *clientInfo)
{
    if (clientInfo != nullptr && clientInfo->extendMsg != nullptr) {
        free(clientInfo->extendMsg);
        clientInfo->extendMsg = nullptr;
        clientInfo->extendLen = 0;
    }
}

class ClientCallback : public IClientCb {
public:
    ClientCallback()
    {
        AIE_NEW(promiseResult_, std::promise<DataInfo>);
    }

    ~ClientCallback() override
    {
        AIE_DELETE(promiseResult_);
    }

    void OnResult(const DataInfo &result, int resultCode, int requestId) override
    {
        HILOGI("[Test]ShareMemoryFunctionTest OnResult retCode[%d], requestId[%d], resultLength[%d].",
            resultCode, requestId, result.length);
        DataInfo outputInfo {};
        if (resultCode == RETCODE_SUCCESS && result.data != nullptr && result.length > 0) {
            outputInfo.length = result.length;
            outputInfo.data = reinterpret_cast<unsigned char*>(malloc(outputInfo.length));
            if (outputInfo.data == nullptr) {
                HILOGE("[OnResult]malloc failed.");
                return;
            }
            if (memcpy_s(outputInfo.data, outputInfo.length, result.data, result.length) != EOK) {
                HILOGE("[OnResult]memcpy_s failed.");
                FreeDataInfo(&outputInfo);
                return;
            }
        }
        promiseResult_->set_value(outputInfo);
    }

    DataInfo GetResult() const
    {
        return promiseResult_->get_future().get();
    }

private:
    std::promise<DataInfo> *promiseResult_ {nullptr};
};
} // anonymous namespace

class ShareMemoryFunctionTest : public testing::Test {
public:
    // SetUpTestCase:The preset action of the test suite is executed before the first TestCase
    static void SetUpTestCase() {};

    // TearDownTestCase:The test suite cleanup action is executed after the last TestCase
    static void TearDownTestCase() {};

    // SetUp:Execute before each test case
    void SetUp() override
    {
        clientInfo_ = {
            .clientVersion = 0,
            .clientId = INVALID_CLIENT_ID,
            .sessionId = INVALID_SESSION_ID,
            .serverUid = 0,
            .clientUid = 0,
            .extendLen = 0,
            .extendMsg = nullptr,
        };

        auto extMsg = reinterpret_cast<unsigned char*>(malloc(LONG_MEMORY_LENGTH));
        ASSERT_NE(extMsg, nullptr);
        ASSERT_EQ(
            memset_s(extMsg, LONG_MEMORY_LENGTH, DUMP_CONTENT, LONG_MEMORY_LENGTH), EOK);
        algorithmInfo_ = {
            .clientVersion = 0,
            .isAsync = false,
            .algorithmType = ALGORITHM_TYPE_SYNC,
            .algorithmVersion = ALGORITHM_VERSION,
            .isCloud = false,
            .operateId = 0,
            .requestId = 0,
            .extendLen = LONG_MEMORY_LENGTH,
            .extendMsg = extMsg,
        };

        extMsg = reinterpret_cast<unsigned char*>(malloc(LONG_MEMORY_LENGTH));
        ASSERT_NE(extMsg, nullptr);
        ASSERT_EQ(
            memset_s(extMsg, LONG_MEMORY_LENGTH, DUMP_CONTENT, LONG_MEMORY_LENGTH), EOK);
        inputInfo_ = {
            .data = extMsg,
            .length = LONG_MEMORY_LENGTH,
        };

        outputInfo_ = {
            .data = nullptr,
            .length = 0,
        };
    }

    // TearDown:Execute after each test case
    void TearDown() override
    {
        FreeClientInfo(&clientInfo_);
        FreeAlgorithmInfo(&algorithmInfo_);
        FreeDataInfo(&inputInfo_);
        FreeDataInfo(&outputInfo_);
    };

public:
    ClientInfo clientInfo_ {};
    AlgorithmInfo algorithmInfo_ {};
    DataInfo inputInfo_ {};
    DataInfo outputInfo_ {};
    ConfigInfo configInfo_ {.description = CONFIG_DESCRIPTION};
    ServiceDeadCb cb_ = ServiceDeadCb();
};

/**
 * @tc.name: TestAieClientShareMemoryInitDestroy
 * @tc.desc: Test the execution of Init/Destroy function with share memory.
 * @tc.type: FUNC
 * @tc.require: AR000F77NL
 */
HWTEST_F(ShareMemoryFunctionTest, TestAieClientShareMemoryInitDestroy, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientShareMemoryInitDestroy.");

    int resultCodeInit = AieClientInit(configInfo_, clientInfo_, algorithmInfo_, &cb_);
    ASSERT_EQ(resultCodeInit, RETCODE_SUCCESS);
    EXPECT_NE(clientInfo_.clientId, INVALID_CLIENT_ID);

    int resultCodeDestroy = AieClientDestroy(clientInfo_);
    EXPECT_EQ(resultCodeDestroy, RETCODE_SUCCESS);
    EXPECT_EQ(clientInfo_.clientId, INVALID_CLIENT_ID);
}

/**
 * @tc.name: TestAieClientShareMemoryPrepareRelease
 * @tc.desc: Test the execution of prepare/release function with share memory.
 * @tc.type: FUNC
 * @tc.require: AR000F77NL
 */
HWTEST_F(ShareMemoryFunctionTest, TestAieClientShareMemoryPrepareRelease, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientShareMemoryPrepareRelease.");

    int resultCodeInit = AieClientInit(configInfo_, clientInfo_, algorithmInfo_, &cb_);
    ASSERT_EQ(resultCodeInit, RETCODE_SUCCESS);
    EXPECT_NE(clientInfo_.clientId, INVALID_CLIENT_ID);

    int resultCodePrepare = AieClientPrepare(clientInfo_, algorithmInfo_, inputInfo_, outputInfo_, nullptr);
    EXPECT_EQ(resultCodePrepare, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo_.length, inputInfo_.length);
    EXPECT_EQ(memcmp(outputInfo_.data, inputInfo_.data, inputInfo_.length), 0);

    DataInfo dummy {};
    int resultCodeRelease = AieClientRelease(clientInfo_, algorithmInfo_, dummy);
    EXPECT_EQ(resultCodeRelease, RETCODE_SUCCESS);

    int resultCodeDestroy = AieClientDestroy(clientInfo_);
    EXPECT_EQ(resultCodeDestroy, RETCODE_SUCCESS);
    EXPECT_EQ(clientInfo_.clientId, INVALID_CLIENT_ID);
}

/**
 * @tc.name: TestAieClientShareMemorySyncProcess
 * @tc.desc: Test the execution of SyncProcess function with share memory.
 * @tc.type: FUNC
 * @tc.require: AR000F77NL
 */
HWTEST_F(ShareMemoryFunctionTest, TestAieClientShareMemorySyncProcess, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientShareMemorySyncProcess.");

    int resultCodeInit = AieClientInit(configInfo_, clientInfo_, algorithmInfo_, &cb_);
    ASSERT_EQ(resultCodeInit, RETCODE_SUCCESS);
    EXPECT_NE(clientInfo_.clientId, INVALID_CLIENT_ID);

    int resultCodePrepare = AieClientPrepare(clientInfo_, algorithmInfo_, inputInfo_, outputInfo_, nullptr);
    EXPECT_EQ(resultCodePrepare, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo_.length, inputInfo_.length);
    EXPECT_EQ(memcmp(outputInfo_.data, inputInfo_.data, inputInfo_.length), 0);
    FreeDataInfo(&outputInfo_);

    int resultCodeSyncProcess = AieClientSyncProcess(clientInfo_, algorithmInfo_, inputInfo_, outputInfo_);
    EXPECT_EQ(resultCodeSyncProcess, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo_.length, inputInfo_.length);
    EXPECT_EQ(memcmp(outputInfo_.data, inputInfo_.data, inputInfo_.length), 0);

    DataInfo dummy {};
    int resultCodeRelease = AieClientRelease(clientInfo_, algorithmInfo_, dummy);
    EXPECT_EQ(resultCodeRelease, RETCODE_SUCCESS);

    int resultCodeDestroy = AieClientDestroy(clientInfo_);
    EXPECT_EQ(resultCodeDestroy, RETCODE_SUCCESS);
    EXPECT_EQ(clientInfo_.clientId, INVALID_CLIENT_ID);
}

/**
 * @tc.name: TestAieClientShareMemoryAsyncProcess
 * @tc.desc: Test the execution of AsyncProcess function with share memory.
 * @tc.type: FUNC
 * @tc.require: AR000F77NL
 */
HWTEST_F(ShareMemoryFunctionTest, TestAieClientShareMemoryAsyncProcess, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientShareMemoryAsyncProcess.");
    algorithmInfo_.isAsync = true;
    algorithmInfo_.algorithmType = ALGORITHM_TYPE_ASYNC;

    int resultCodeInit = AieClientInit(configInfo_, clientInfo_, algorithmInfo_, &cb_);
    ASSERT_EQ(resultCodeInit, RETCODE_SUCCESS);
    EXPECT_NE(clientInfo_.clientId, INVALID_CLIENT_ID);

    ClientCallback callback = ClientCallback();
    int resultCodePrepare = AieClientPrepare(clientInfo_, algorithmInfo_, inputInfo_, outputInfo_, &callback);
    EXPECT_EQ(resultCodePrepare, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo_.length, inputInfo_.length);
    EXPECT_EQ(memcmp(outputInfo_.data, inputInfo_.data, inputInfo_.length), 0);
    FreeDataInfo(&outputInfo_);

    auto asyncFuture = std::async(AieClientAsyncProcess, clientInfo_, algorithmInfo_, inputInfo_);
    EXPECT_EQ(asyncFuture.get(), RETCODE_SUCCESS);
    outputInfo_ = callback.GetResult();
    EXPECT_EQ(outputInfo_.length, inputInfo_.length);
    EXPECT_EQ(memcmp(outputInfo_.data, inputInfo_.data, inputInfo_.length), 0);

    DataInfo dummy {};
    int resultCodeRelease = AieClientRelease(clientInfo_, algorithmInfo_, dummy);
    EXPECT_EQ(resultCodeRelease, RETCODE_SUCCESS);

    int resultCodeDestroy = AieClientDestroy(clientInfo_);
    EXPECT_EQ(resultCodeDestroy, RETCODE_SUCCESS);
    EXPECT_EQ(clientInfo_.clientId, INVALID_CLIENT_ID);
}

/**
 * @tc.name: TestAieClientShareMemorySetGetOption
 * @tc.desc: Test the execution of SetOption/GetOption function with share memory.
 * @tc.type: FUNC
 * @tc.require: AR000F77NL
 */
HWTEST_F(ShareMemoryFunctionTest, TestAieClientShareMemorySetGetOption, TestSize.Level0)
{
    HILOGI("[Test]TestAieClientShareMemorySetGetOption.");

    int resultCodeInit = AieClientInit(configInfo_, clientInfo_, algorithmInfo_, &cb_);
    ASSERT_EQ(resultCodeInit, RETCODE_SUCCESS);
    EXPECT_NE(clientInfo_.clientId, INVALID_CLIENT_ID);

    int resultCodePrepare = AieClientPrepare(clientInfo_, algorithmInfo_, inputInfo_, outputInfo_, nullptr);
    EXPECT_EQ(resultCodePrepare, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo_.length, inputInfo_.length);
    EXPECT_EQ(memcmp(outputInfo_.data, inputInfo_.data, inputInfo_.length), 0);

    int dummyOptionType = 0;
    int resultCodeSetOption = AieClientSetOption(clientInfo_, dummyOptionType, inputInfo_);
    EXPECT_EQ(resultCodeSetOption, RETCODE_SUCCESS);

    DataInfo dummy {};
    int resultCodeGetOption = AieClientGetOption(clientInfo_, dummyOptionType, dummy, outputInfo_);
    EXPECT_EQ(resultCodeGetOption, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo_.length, inputInfo_.length);
    EXPECT_EQ(memcmp(outputInfo_.data, inputInfo_.data, inputInfo_.length), 0);

    resultCodeSetOption = AieClientSetOption(clientInfo_, dummyOptionType, inputInfo_);
    EXPECT_EQ(resultCodeSetOption, RETCODE_SUCCESS);

    resultCodeGetOption = AieClientGetOption(clientInfo_, dummyOptionType, dummy, outputInfo_);
    EXPECT_EQ(resultCodeGetOption, RETCODE_SUCCESS);
    EXPECT_EQ(outputInfo_.length, inputInfo_.length);
    EXPECT_EQ(memcmp(outputInfo_.data, inputInfo_.data, inputInfo_.length), 0);

    int resultCodeRelease = AieClientRelease(clientInfo_, algorithmInfo_, dummy);
    EXPECT_EQ(resultCodeRelease, RETCODE_SUCCESS);

    int resultCodeDestroy = AieClientDestroy(clientInfo_);
    EXPECT_EQ(resultCodeDestroy, RETCODE_SUCCESS);
    EXPECT_EQ(clientInfo_.clientId, INVALID_CLIENT_ID);
}
