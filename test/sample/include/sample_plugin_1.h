/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef SAMPLE_PLUGIN_1_H
#define SAMPLE_PLUGIN_1_H

#include "plugin/i_plugin.h"

namespace OHOS {
namespace AI {
class SamplePlugin1 : public IPlugin {
public:
    SamplePlugin1();

    ~SamplePlugin1() override;

    const long long GetVersion() const override;

    const char *GetName() const override;

    const char *GetInferMode() const override;

    int SyncProcess(IRequest *request, IResponse *&response) override;

    int AsyncProcess(IRequest *request, IPluginCallback *callback) override;

    int Prepare(long long transactionId, const DataInfo &inputInfo, DataInfo &outputInfo) override;

    int Release(bool isFullUnload, long long transactionId, const DataInfo &inputInfo) override;

    int SetOption(int optionType, const DataInfo &inputInfo) override;

    int GetOption(int optionType, const DataInfo &inputInfo, DataInfo &outputInfo) override;

private:
    DataInfo optionData_ {};
};
}
}

#endif // SAMPLE_PLUGIN_1_H
