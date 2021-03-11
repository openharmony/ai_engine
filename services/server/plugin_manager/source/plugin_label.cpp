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

#include "plugin_manager/include/plugin_label.h"

#include "plugin_manager/include/aie_plugin_info.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
namespace {
const char * const CONFIG_INI_FILE_PATH = "/etc/ai_engine_plugin.ini";
const std::string PLUS = "+";
const std::string DELIMITER = ":";
}

std::mutex PluginLabel::instanceLock_;
PluginLabel *PluginLabel::instance_ = nullptr;

PluginLabel *PluginLabel::GetInstance()
{
    CHK_RET(instance_ != nullptr, instance_);

    std::lock_guard<std::mutex> lock(instanceLock_);
    CHK_RET(instance_ != nullptr, instance_);

    PluginLabel *temp = nullptr;
    AIE_NEW(temp, PluginLabel);
    CHK_RET(temp == nullptr, nullptr);

    instance_ = temp;
    return instance_;
}

void PluginLabel::ReleaseInstance()
{
    std::lock_guard<std::mutex> lock(instanceLock_);
    AIE_DELETE(instance_);
}

PluginLabel::PluginLabel() = default;

PluginLabel::~PluginLabel()
{
    if (pluginIniConfig_ == nullptr) {
        return;
    }
    try {
        dictionary_del(pluginIniConfig_);
    } catch (...) {
    }
    pluginIniConfig_ = nullptr;
}

int PluginLabel::GetLibPath(const std::string &aid, long long &version, std::string &libPath)
{
    if (pluginIniConfig_ == nullptr) {
        pluginIniConfig_ = iniparser_load(CONFIG_INI_FILE_PATH);
    }
    // The label combined algorithm ID and algorithm version
    std::string label = aid + PLUS + std::to_string(version);
    auto queryKey = label + DELIMITER + ALGORITHM_INFO_TABLE_FIELD_NAME_FULLPATH;
    libPath = iniparser_getstring(pluginIniConfig_, queryKey.c_str(), "");
    if (libPath.empty()) {
        HILOGE("[PluginLabel]Query lib path failed.");
        return RETCODE_FAILURE;
    }
    HILOGI("[PluginLabel]Succeed to get lib path of %s.", aid.c_str());
    return RETCODE_SUCCESS;
}
} // namespace AI
} // namespace OHOS