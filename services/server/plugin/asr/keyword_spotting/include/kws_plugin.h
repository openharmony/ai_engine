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

#ifndef KEYWORD_SPOTTING_PLUGIN_H
#define KEYWORD_SPOTTING_PLUGIN_H

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <vector>
#include <string>

#include "ai_datatype.h"
#include "engine_adapter.h"
#include "feature_processor.h"
#include "keyword_spotting/kws_constants.h"
#include "plugin_helper.h"
#include "plugin/i_plugin.h"

namespace OHOS {
namespace AI {
struct KWSWorkplace {
    PluginConfig config;
    std::shared_ptr<Feature::FeatureProcessor> normProcessor;
    std::shared_ptr<Feature::FeatureProcessor> typeConverter;
    std::shared_ptr<Feature::FeatureProcessor> slideProcessor;
};

class KWSPlugin : public IPlugin {
public:
    KWSPlugin();
    ~KWSPlugin();

    const long long GetVersion() const override;
    const char *GetName() const override;
    const char *GetInferMode() const override;

    int32_t Prepare(long long transactionId, const DataInfo &inputInfo, DataInfo &outputInfo) override;
    int32_t SetOption(int optionType, const DataInfo &inputInfo) override;
    int32_t GetOption(int optionType, const DataInfo &inputInfo, DataInfo &outputInfo) override;
    int32_t SyncProcess(IRequest *request, IResponse *&response) override;
    int32_t AsyncProcess(IRequest *request, IPluginCallback *callback) override;
    int32_t Release(bool isFullUnload, long long transactionId, const DataInfo &inputInfo) override;

private:
    int32_t InitComponents(KWSWorkplace &workplace);
    int32_t GetNormedFeatures(const Array<uint16_t> &input, Array<int32_t> &output, const KWSWorkplace &worker);
    int32_t BuildConfig(intptr_t handle, PluginConfig &config);
    int32_t MakeInference(intptr_t handle, Array<int32_t> &input, PluginConfig &config, DataInfo &outputInfo);
    void FreeHandle(intptr_t handle);
    void ReleaseAllHandles();

private:
    std::shared_ptr<EngineAdapter> adapter_;
    std::mutex mutex_;
    std::map<intptr_t, KWSWorkplace> handles_;
};
}  // namespace AI
}  // namespace OHOS
#endif  // KEYWORD_SPOTTING_PLUGIN_H