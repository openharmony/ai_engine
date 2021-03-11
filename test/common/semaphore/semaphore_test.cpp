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

#include <unistd.h>

#include "gtest/gtest.h"

#include "platform/semaphore/include/i_semaphore.h"
#include "platform/semaphore/include/simple_event_notifier.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace OHOS {
namespace AI {
const int WAIT_TIME_MS = 1000;
const int INTERVAL_TIME_S = 2;
const int INT_VALUE = -1;
const int TIME_OUT = 20;
const int CONST_VALUE = 123;
class ISemaphore;
class VectorSimpleEventNotifier;
} // namespace AI
} // namespace OHOS

class SemaphoreTest : public testing::Test {
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
 * @tc.name: SemaphoreTest001
 * @tc.desc: Test semaphore wait function.
 * @tc.type: FUNC
 * @tc.require: AR000F77TL
 */
HWTEST_F(SemaphoreTest, SemaphoreTest001, TestSize.Level0)
{
    HILOGD("[Test]Begin to main. test ISemaphore.");
    std::shared_ptr<ISemaphore> semaphoreExample = ISemaphore::MakeShared(0);
    semaphoreExample->Signal();
    sleep(INTERVAL_TIME_S);
    bool ret = semaphoreExample->Wait(WAIT_TIME_MS);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: SimpleNotifierTest001
 * @tc.desc: Test simpleNotifier AddToBack and GetFromFront functions.
 * @tc.type: FUNC
 * @tc.require: AR000F77TL
 */
HWTEST_F(SemaphoreTest, SimpleNotifierTest001, TestSize.Level0)
{
    HILOGD("[Test]Test simple notifier.");
    SimpleEventNotifier<int> simpleNotifier = SimpleEventNotifier<int>(nullptr);
    int* itemIn = nullptr;
    AIE_NEW(itemIn, int(CONST_VALUE));
    int *itemOut = nullptr;
    simpleNotifier.AddToBack(itemIn);
    sleep(INTERVAL_TIME_S);
    int ret = simpleNotifier.GetFromFront(TIME_OUT, itemOut);
    EXPECT_EQ(*itemOut, CONST_VALUE);
    EXPECT_TRUE(ret);
    AIE_DELETE(itemIn);
}