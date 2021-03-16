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

#ifndef IC_SDK_IMPL_H
#define IC_SDK_IMPL_H

#include <vector>

#include "aie_algorithm_type.h"
#include "aie_info_define.h"
#include "constants.h"
#include "ic_constants.h"
#include "ic_sdk.h"
#include "single_instance.h"

namespace OHOS {
namespace AI {
class IcSdkImpl {
DECLARE_SINGLE_INSTANCE(IcSdkImpl);
public:
    int32_t Create();
    int32_t SetCallback(std::shared_ptr<IcCallback> callback);
    int32_t SyncExecute(const IcInput &picInput);
    int32_t Destroy();
private:
    std::shared_ptr<IcCallback> callback_ = nullptr;
    intptr_t icHandle_ = INVALID_IC_HANDLE;
    int32_t OnSyncExecute(const IcInput &picInput, DataInfo &outputInfo);
    ConfigInfo configInfo_ {
        .description = "Ic config description"
    };
    ClientInfo clientInfo_ {
        .clientVersion = CLIENT_VERSION_IC,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = INVALID_SESSION_ID,
        .extendLen = EXT_MSG_LEN_DEFAULT,
        .extendMsg = nullptr
    };
    AlgorithmInfo algorithmInfo_ {
        .clientVersion = CLIENT_VERSION_IC,
        .isAsync = false,
        .algorithmType = ALGORITHM_TYPE_IC,
        .algorithmVersion = ALGOTYPE_VERSION_IC,
        .isCloud = false,
        .operateId = STARTING_OPERATE_ID,
        .requestId = STARTING_REQ_ID,
        .extendLen = EXT_MSG_LEN_DEFAULT,
        .extendMsg = nullptr
    };
};
} // namespace AI
} // namespace OHOS
#endif // IC_SDK_IMPL_H