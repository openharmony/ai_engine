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

#ifndef I_ENGINE_MANAGER_H
#define I_ENGINE_MANAGER_H

#include <memory>

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/aie_macros.h"
#include "utils/constants/constants.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
class IEngineManager {
public:
    virtual ~IEngineManager() = default;

    /**
     * Start engine.
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] algoInfo Algorithm information.
     * @param [in] inputInfo Input parameters for starting the engine.
     * @param [out] outputInfo Result information for starting the engine.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int StartEngine(long long transactionId, const AlgorithmInfo &algoInfo, const DataInfo &inputInfo,
        DataInfo &outputInfo) = 0;

    /**
     * Stop engine.
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] inputInfo Input parameters for stopping the engine.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int StopEngine(long long transactionId, const DataInfo &inputInfo) = 0;

    /**
     * Set the configuration parameters of the plugin algorithm.
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] optionType The type of setting option.
     * @param [in] inputInfo Configuration parameter needed to set up the plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int SetOption(long long transactionId, int optionType, const DataInfo &inputInfo) = 0;

    /**
     * Get the configuration parameters of the plugin algorithm.
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] optionType The type of getting option.
     * @param [in] inputInfo Parameter information for getting options.
     * @param [out] outputInfo The configuration parameter information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int GetOption(long long transactionId, int optionType, const DataInfo &inputInfo,
        DataInfo &outputInfo) = 0;
};

IEngineManager *GetEngineManager();

void ReleaseEngineManager();
} // namespace AI
} // namespace OHOS

#endif // I_ENGINE_MANAGER_H