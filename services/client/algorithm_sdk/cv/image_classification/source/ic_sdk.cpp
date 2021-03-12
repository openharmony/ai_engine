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

#include "ic_sdk.h"

#include "ic_sdk_impl.h"

using namespace OHOS::AI;

IcSdk::~IcSdk()
{
    IcSdkImpl::GetInstance().Destroy();
}

int32_t IcSdk::Create()
{
    return IcSdkImpl::GetInstance().Create();
}

int32_t IcSdk::SetCallback(std::shared_ptr<IcCallback> callback)
{
    return IcSdkImpl::GetInstance().SetCallback(callback);
}

int32_t IcSdk::SyncExecute(const IcInput &inputData)
{
    return IcSdkImpl::GetInstance().SyncExecute(inputData);
}

int32_t IcSdk::Destroy()
{
    return IcSdkImpl::GetInstance().Destroy();
}