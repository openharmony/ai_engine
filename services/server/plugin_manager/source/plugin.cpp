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

#include "plugin_manager/include/plugin.h"

#include <dlfcn.h>

#include "securec.h"

#include "platform/dl_operation/include/aie_dl_operation.h"
#include "plugin_manager/include/plugin_label.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/aie_macros.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
class HandleGuard {
public:
    HandleGuard(const std::string &aid, void *handle) : aid_(aid), handle_(handle)
    {
    }

    void Detach()
    {
        handle_ = nullptr;
    }

    ~HandleGuard()
    {
        CHK_RET_NONE(handle_ == nullptr);
        AieDlclose(handle_);
        handle_ = nullptr;
        HILOGI("[Plugin]Succeed to close library, aid=%s.", aid_.c_str());
    }
private:
    std::string aid_;
    void *handle_ = nullptr;
};

Plugin::Plugin(const std::string &aid, long long version)
    : pluginAlgorithm_(nullptr), aid_(aid), version_(version), handle_(nullptr)
{
}

Plugin::~Plugin()
{
    UnloadPluginAlgorithm();
    PluginLabel *pluginLabel = PluginLabel::GetInstance();
    if (pluginLabel != nullptr) {
        pluginLabel->ReleaseInstance();
    }
}

int Plugin::LoadPluginAlgorithm()
{
    PluginLabel *pluginLabel = PluginLabel::GetInstance();
    if (pluginLabel == nullptr) {
        HILOGE("[Plugin]PluginLabel is nullptr.");
        return RETCODE_FAILURE;
    }
    std::string libPath;
    int retCode = pluginLabel->GetLibPath(aid_, version_, libPath);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[Plugin]Failed to get lib path.");
        return retCode;
    }
    void *handle = AieDlopen(libPath.c_str());
    if (handle == nullptr) {
#ifdef DEBUG
        HILOGE("[Plugin]Failed to open lib(%s), ret: %s.", libPath.c_str(), AieDlerror());
#else
        HILOGE("[Plugin]Failed to open lib(%s).", libPath.c_str());
#endif
        return RETCODE_OPEN_SO_FAILED;
    }

    HandleGuard handleGuard(aid_, handle);

    IPLUGIN_INTERFACE fp = (IPLUGIN_INTERFACE)AieDlsym(handle, PLUGIN_INTERFACE_NAME);
    if (fp == nullptr) {
        HILOGE("[Plugin]Failed to get symbol %s in file %s.", PLUGIN_INTERFACE_NAME, libPath.c_str());
        return RETCODE_SO_LACK_SYMBOL;
    }

    IPlugin *pluginAlgorithm = (*fp)();
    if (pluginAlgorithm == nullptr) {
        HILOGE("[Plugin]Get plugin object failed.");
        return RETCODE_NULL_PARAM;
    }

    handleGuard.Detach();
    pluginAlgorithm_ = pluginAlgorithm;
    handle_ = handle;

    return RETCODE_SUCCESS;
}

void Plugin::UnloadPluginAlgorithm()
{
    AIE_DELETE(pluginAlgorithm_);
    if (handle_) {
        AieDlclose(handle_);
        handle_ = nullptr;
    }
    HILOGI("[Plugin]Succeed to unload plugin algorithm, aid=%s.", aid_.c_str());
}

IPlugin *Plugin::GetPluginAlgorithm()
{
    return pluginAlgorithm_;
}

void Plugin::SetPluginAlgorithm(IPlugin *pluginAlgorithm)
{
    pluginAlgorithm_ = pluginAlgorithm;
}

long long Plugin::GetVersion() const
{
    return version_;
}

std::string Plugin::GetAid() const
{
    return aid_;
}
} // namespace AI
} // namespace OHOS