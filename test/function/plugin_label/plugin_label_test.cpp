/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "plugin_manager/include/aie_plugin_info.h"
#include "plugin_manager/include/plugin_label.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
    const std::string AID_DEMO_PLUGIN_SYNC = "cv_card_rectification";
    long long AID_DEMO_PLUGIN_VERSION = 20001001;
}

class PluginLabelTest : public testing::Test {
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

static void TestPluginLabel(const std::string &aid, long long &version, std::string &libPath)
{
    PluginLabel *pluginLabel = PluginLabel::GetInstance();
    ASSERT_NE(pluginLabel, nullptr) << "pluginLabel test failed.";

    int retCode = pluginLabel->GetLibPath(aid, version, libPath);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[Test]Failed to get lib path. return retCode:%d;", retCode);
        return;
    }
    HILOGE("[Test]SUCCESS return retCode:%d;", retCode);
}

/**
 * @tc.name: TestPluginLabel001
 * @tc.desc: Test Get algorithm path.
 * @tc.type: FUNC
 * @tc.require: AR000F77ON
 */
static HWTEST_F(PluginLabelTest, TestPluginLabel001, TestSize.Level0)
{
    HILOGI("[Test]TestPluginLabel001.");
    std::string libPath;
    TestPluginLabel(AID_DEMO_PLUGIN_SYNC, AID_DEMO_PLUGIN_VERSION, libPath);
}