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

#include "kws_sdk.h"

#include "aie_log.h"
#include "kws_retcode.h"
#include "kws_sdk_impl.h"

namespace OHOS {
namespace AI {
KWSSdk::KWSSdk() : kwsSdkImpl_(nullptr)
{
}

KWSSdk::~KWSSdk()
{
    if (kwsSdkImpl_ != nullptr) {
        kwsSdkImpl_->Destroy();
        kwsSdkImpl_ = nullptr;
    }
}

int32_t KWSSdk::Create()
{
    if (kwsSdkImpl_ != nullptr) {
        return kwsSdkImpl_->Create();
    }
    kwsSdkImpl_ = std::unique_ptr<KWSSdkImpl>(new (std::nothrow) KWSSdkImpl());
    if (kwsSdkImpl_ == nullptr) {
        HILOGE("[KWSSdk]Fail to allocate memory for kws sdk");
        return KWS_RETCODE_FAILURE;
    }
    return kwsSdkImpl_->Create();
}

int32_t KWSSdk::SetCallback(const std::shared_ptr<KWSCallback> &callback)
{
    if (kwsSdkImpl_ == nullptr) {
        HILOGE("[KWSSdk]The SDK has not been created");
        return KWS_RETCODE_FAILURE;
    }
    return kwsSdkImpl_->SetCallback(callback);
}

int32_t KWSSdk::SyncExecute(const Array<int16_t> &input)
{
    if (kwsSdkImpl_ == nullptr) {
        HILOGE("[KWSSdk]The SDK has not been created");
        return KWS_RETCODE_FAILURE;
    }
    return kwsSdkImpl_->SyncExecute(input);
}

int32_t KWSSdk::Destroy()
{
    if (kwsSdkImpl_ == nullptr) {
        return KWS_RETCODE_SUCCESS;
    }
    return kwsSdkImpl_->Destroy();
}
} // namespace AI
} // namespace OHOS