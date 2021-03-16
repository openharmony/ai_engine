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

#ifndef KWS_SDK_IMPL_H
#define KWS_SDK_IMPL_H

#include <vector>

#include "ai_datatype.h"
#include "aie_algorithm_type.h"
#include "aie_info_define.h"
#include "constants.h"
#include "feature_processor.h"
#include "kws_callback.h"
#include "kws_constants.h"
#include "kws_sdk.h"
#include "pcm_iterator.h"

namespace OHOS {
namespace AI {
class KWSSdk::KWSSdkImpl {
public:
    KWSSdkImpl();
    virtual ~KWSSdkImpl();
    int32_t Create();
    int32_t SyncExecute(const Array<int16_t> &input);
    int32_t Destroy();
    int32_t SetCallback(std::shared_ptr<KWSCallback> callback);

private:
    int32_t InitComponents();
    int32_t Execute(const Array<uint16_t> &input);

private:
    std::shared_ptr<KWSCallback> callback_ = nullptr;
    std::unique_ptr<PCMIterator> pcmIterator_ = nullptr;
    std::unique_ptr<Feature::FeatureProcessor> mfccProcessor_ = nullptr;
    ConfigInfo configInfo_ {.description = "kws config description"};
    intptr_t kwsHandle_ = INVALID_KWS_HANDLE;
    ClientInfo clientInfo_ {
        .clientVersion = CLIENT_VERSION_KWS,
        .clientId = INVALID_CLIENT_ID,
        .sessionId = INVALID_SESSION_ID,
        .extendLen = EXT_MSG_LEN_DEFAULT,
        .extendMsg = nullptr
    };
    AlgorithmInfo algorithmInfo_ {
        .clientVersion = CLIENT_VERSION_KWS,
        .isAsync = false,
        .algorithmType = ALGORITHM_TYPE_KWS,
        .algorithmVersion = ALGOTYPE_VERSION_KWS,
        .isCloud = false,
        .operateId = STARTING_OPERATE_ID,
        .requestId = STARTING_REQ_ID,
        .extendLen = EXT_MSG_LEN_DEFAULT,
        .extendMsg = nullptr
    };
};
} // namespace AI
} // namespace OHOS
#endif // KWS_SDK_IMPL_H