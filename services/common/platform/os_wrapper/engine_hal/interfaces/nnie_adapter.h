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

#ifndef NNIE_ADAPTER_H
#define NNIE_ADAPTER_H

#include <cstdint>
#include <cstdlib>

#include "engine_adapter.h"

namespace OHOS {
namespace AI {
class NNIEAdapter : public EngineAdapter {
public:
    NNIEAdapter();
    virtual ~NNIEAdapter();

    /* Initializes the algorithm and get the algorithm execution handle. */
    int32_t Init(const char *modelPath, intptr_t &handle) override;

    /* De-Initializes all the algorithms. */
    int32_t Deinit() override;

    /* Makes the model based on the given handle Inference once. */
    int32_t Invoke(intptr_t handle) override;

    /* Gets the inputBuffer and inputSize after the handle related model is initialized. */
    int32_t GetInputAddr(intptr_t handle, uint16_t nodeId, uintptr_t &inputBuffer, size_t &inputSize) override;

    /* Gets the outputBuffer and outputSize after the handle related model is initialized. */
    int32_t GetOutputAddr(intptr_t handle, uint16_t nodeId, uintptr_t &outputBuffer, size_t &outputSize) override;

    /* Releases the algorithm based on the given handle. */
    int32_t ReleaseHandle(intptr_t handle) override;
};
} // namespace AI
} // namespace OHOS
#endif // NNIE_ADAPTER_H