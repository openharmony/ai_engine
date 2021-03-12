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

#ifndef IC_PLUGIN_H
#define IC_PLUGIN_H

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "ai_datatype.h"
#include "engine_adapter.h"
#include "plugin_helper.h"
#include "plugin/i_plugin.h"

namespace OHOS {
namespace AI {
struct ICPluginConfig : PluginConfig {
    size_t maxOutputSize;
};

class ICPlugin : public IPlugin {
public:
    ICPlugin();
    virtual ~ICPlugin();

    const long long GetVersion() const override;
    const char* GetName() const override;
    const char* GetInferMode() const override;
    int32_t Prepare(long long transactionId, const DataInfo &inputInfo, DataInfo &outputInfo) override;
    int32_t SyncProcess(IRequest *request, IResponse *&response) override;
    int32_t AsyncProcess(IRequest *request, IPluginCallback *callback) override;
    int32_t Release(bool isFullUnload, long long transactionId, const DataInfo &inputInfo) override;
    int32_t SetOption(int32_t optionType, const DataInfo &inputInfo) override;
    int32_t GetOption(int32_t optionType, const DataInfo &inputInfo, DataInfo &outputInfo) override;

private:
    int32_t BuildConfig(intptr_t handle, ICPluginConfig &config);
    int32_t DoProcess(intptr_t handle, const ICPluginConfig &config, IRequest *request, IResponse *&response);
    int32_t MakeInference(intptr_t handle, const ICPluginConfig &config, DataInfo &outputInfo);
    void ReleaseAllHandles();

private:
    std::mutex mutex_;
    std::shared_ptr<EngineAdapter> adapter_;
    std::map<intptr_t, ICPluginConfig> handles_;
};
} // namespace AI
} // namespace OHOS
#endif // IC_PLUGIN_H