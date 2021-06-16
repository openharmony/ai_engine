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

#include "platform/dl_operation/include/aie_dl_operation.h"
#include "utils/log/aie_log.h"

using namespace testing::ext;

typedef int(*FUNC_ADD)(int, int);
namespace {
    const int AIE_NUM1 = 1;
    const int AIE_NUM2 = 2;
    const int AIE_RESULT = 3;
    const char * const TEST_SO_PATH = "/usr/lib/libdlOperationSo.so";
}

class DlOperationTest : public testing::Test {
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
 * @tc.name: TestDlOption001
 * @tc.desc: Test dl-option function, including AieDlopen, AieDlsym and AieDlclose.
 * @tc.type: FUNC
 * @tc.require: AR000F77MR
 */
HWTEST_F(DlOperationTest, TestDlOption001, TestSize.Level1)
{
    void *handle = AieDlopen(TEST_SO_PATH);
    HILOGD("[Test]Begin to excute handle.");
    ASSERT_NE(handle, nullptr);
    FUNC_ADD addFunc = (FUNC_ADD)AieDlsym(handle, "AddFunc");
    ASSERT_NE(addFunc, nullptr);
    int result = addFunc(AIE_NUM1, AIE_NUM2);
    ASSERT_EQ(result, AIE_RESULT);
    AieDlclose(handle);
}
