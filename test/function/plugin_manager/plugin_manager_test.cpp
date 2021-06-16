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

#include "plugin_manager/include/aie_plugin_info.h"
#include "plugin_manager/include/i_plugin_manager.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
    const std::string AID_DEMO_PLUGIN_SYNC = "sample_plugin_1";
    const std::string AID_DEMO_PLUGIN_ASYNC = "sample_plugin_2";
    const std::string AID_PLUGIN_INVALID = "invalid_plugin";
    const int ALGORITHM_VERSION_VALID = 1;
}

class PluginManagerTest : public testing::Test {
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

static void TestPluginManager(std::string aid, bool isExceptedLoadPluginSuccess)
{
    IPluginManager *pluginManager = IPluginManager::GetPluginManager();
    ASSERT_NE(pluginManager, nullptr) << "GetPluginManager test failed.";

    long long version = ALGORITHM_VERSION_VALID;
    std::shared_ptr<Plugin> plugin = nullptr;
    pluginManager->GetPlugin(aid, version, plugin);
    ASSERT_EQ(isExceptedLoadPluginSuccess, plugin != nullptr) << "pluginManager->GetPlugin test failed.";

    if (plugin != nullptr) {
        const char *name = plugin->GetPluginAlgorithm()->GetName();
        HILOGD("[Test]The plugin name [%s].", name);
        ASSERT_EQ(isExceptedLoadPluginSuccess, name != nullptr) << "Get plugin name failed.";
    }

    pluginManager->UnloadPlugin(aid, version);

    pluginManager->Destroy();
}

static void TestPluginManagerUnloadPlugin(std::string aid)
{
    long long version = ALGORITHM_VERSION_VALID;
    IPluginManager *pluginManager2 = IPluginManager::GetPluginManager();
    ASSERT_NE(pluginManager2, nullptr) << "GetPluginManager test failed.";

    ASSERT_NO_THROW(pluginManager2->UnloadPlugin(aid, version)) << "UnloadPlugin test failed.";
}

/**
 * @tc.name: testPluginManager001
 * @tc.desc: Test the process of loading and unloading synchronous plugin.
 * @tc.type: FUNC
 * @tc.require: AR000F77ON
 */
HWTEST_F(PluginManagerTest, testPluginManager001, TestSize.Level0)
{
    HILOGI("[Test]testPluginManager001.");
    TestPluginManager(AID_DEMO_PLUGIN_SYNC, true);
}

/**
 * @tc.name: testPluginManager002
 * @tc.desc: Test the process of loading and unloading asynchronous plugin.
 * @tc.type: FUNC
 * @tc.require: AR000F77ON
 */
HWTEST_F(PluginManagerTest, testPluginManager002, TestSize.Level0)
{
    HILOGI("[Test]testPluginManager002.");
    TestPluginManager(AID_DEMO_PLUGIN_ASYNC, true);
}

/**
 * @tc.name: testPluginManager003
 * @tc.desc: Test loading plugin whose plugin id is invalid.
 * @tc.type: FUNC
 * @tc.require: AR000F77ON
 */
HWTEST_F(PluginManagerTest, testPluginManager003, TestSize.Level0)
{
    HILOGI("[Test]testPluginManager003.");
    TestPluginManager(AID_PLUGIN_INVALID, false);
}

/**
 * @tc.name: testPluginManager004
 * @tc.desc: Test the process of unloading synchronous plugin without loading the plugin.
 * @tc.type: FUNC
 * @tc.require: AR000F77ON
 */
HWTEST_F(PluginManagerTest, testPluginManager004, TestSize.Level0)
{
    HILOGI("[Test]testPluginManager004.");
    TestPluginManagerUnloadPlugin(AID_DEMO_PLUGIN_SYNC);
}

/**
 * @tc.name: testPluginManager005
 * @tc.desc: Test the process of unloading asynchronous plugin without loading the plugin.
 * @tc.type: FUNC
 * @tc.require: AR000F77ON
 */
HWTEST_F(PluginManagerTest, testPluginManager005, TestSize.Level0)
{
    HILOGI("[Test]testPluginManager005.");
    TestPluginManagerUnloadPlugin(AID_DEMO_PLUGIN_ASYNC);
}

/**
 * @tc.name: testPluginManager006
 * @tc.desc: Test unloading plugin whose plugin id is invalid.
 * @tc.type: FUNC
 * @tc.require: AR000F77ON
 */
HWTEST_F(PluginManagerTest, testPluginManager006, TestSize.Level0)
{
    HILOGI("[Test]testPluginManager006.");
    TestPluginManagerUnloadPlugin(AID_PLUGIN_INVALID);
}
