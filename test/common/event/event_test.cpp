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

#include "platform/event/include/i_event.h"

#include <iostream>
#include <unistd.h>

#include "gtest/gtest.h"

#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace OHOS {
namespace AI {
const int EVENT_WAIT_TIME_MS = 1000;
class IEvent;
std::shared_ptr<IEvent> g_event = IEvent::MakeShared();
} // namespace AI
} // namespace OHOS

class EventTest : public testing::Test {
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
 * @tc.name: EventTest001
 * @tc.desc: Test the basic usage process of event function.
 * @tc.type: FUNC
 * @tc.require: AR000F77MR
 */
HWTEST_F(EventTest, EventTest001, TestSize.Level1)
{
    ASSERT_TRUE(g_event->Signal());
    ASSERT_TRUE(g_event->Wait(EVENT_WAIT_TIME_MS));
    ASSERT_TRUE(g_event->Reset());
    ASSERT_FALSE(g_event->IsSet());
}
