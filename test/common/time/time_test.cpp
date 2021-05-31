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

#include "gtest/gtest.h"

#include "platform/time/include/time.h"
#include "platform/time/include/time_elapser.h"
#include "utils/log/aie_log.h"

using namespace testing::ext;
using namespace OHOS::AI;

namespace {
    const int SLEEP_TIME = 2000;
    const int S_TO_MS = 1000; // milliseconds to seconds
}

class TimeTest : public testing::Test {
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
 * @tc.name: TimeTest001
 * @tc.desc: Test StepSleepMs function.
 * @tc.type: FUNC
 * @tc.require: AR000F77OO
 */
HWTEST_F(TimeTest, TimeTest001, TestSize.Level1)
{
    time_t firstTime = GetCurTimeSec();
    HILOGD("[Test]First time is %lld", firstTime);
    StepSleepMs(SLEEP_TIME);
    time_t curTime = GetCurTimeSec();
    HILOGD("[Test]Second time is %lld", curTime);
    ASSERT_EQ((curTime - firstTime)*S_TO_MS, SLEEP_TIME);
}
