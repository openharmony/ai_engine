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

#ifndef ENGINE_ADAPTER_H
#define ENGINE_ADAPTER_H

#include <cstdint>

namespace OHOS {
namespace AI {
// Status for Engine
enum EngineStatus {
    IDLE = 1000,
    PREPARING,
    READY,
    RUNNING,
};

class EngineAdapter {
public:
    virtual ~EngineAdapter() = default;

    /** Initializes the algorithm and get the algorithm execution handle */
    virtual int32_t Init(const char *modelPath, intptr_t &handle) = 0;

    /** De-Initializes all the algorithms.  */
    virtual int32_t Deinit() = 0;

    /** Makes the model based on the given handle Inference once. */
    virtual int32_t Invoke(intptr_t handle) = 0;

    /** Gets the inputBuffer and inputSize after the handle related model is initialized. */
    virtual int32_t GetInputAddr(intptr_t handle, uint16_t nodeId,
                                 uintptr_t &inputBuffer, size_t &inputSize) = 0;

    /** Gets the outputBuffer and outputSize after the handle related model is initialized. */
    virtual int32_t GetOutputAddr(intptr_t handle, uint16_t nodeId,
                                  uintptr_t &outputBuffer, size_t &outputSize) = 0;

    /** Release the algorithm based on the given handle. */
    virtual int32_t ReleaseHandle(intptr_t handle) = 0;
};
}  // namespace AI
}  // namespace OHOS
#endif // ENGINE_ADAPTER_H