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

#include "ic_sdk_impl.h"

#include <algorithm>

#include "aie_guard.h"
#include "aie_retcode_inner.h"
#include "encdec_facade.h"
#include "i_aie_client.inl"
#include "ic_retcode.h"
#include "plugin_helper.h"

using namespace OHOS::AI;
namespace {
    const uint32_t MAX_IPC_BUFFER_SIZE = 25600;
}

IMPLEMENT_SINGLE_INSTANCE(IcSdkImpl);

IcSdkImpl::IcSdkImpl() = default;

IcSdkImpl::~IcSdkImpl() = default;

int32_t IcSdkImpl::Create()
{
    HILOGI("[IcSdkImpl]Start");
    if (icHandle_ != INVALID_IC_HANDLE) {
        HILOGE("[IcSdkImpl]Do not create again");
        return IC_RETCODE_INIT_ERROR;
    }
    int32_t retCode = AieClientInit(configInfo_, clientInfo_, algorithmInfo_, nullptr);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_INIT_ERROR))
                               : HILOGD("[IcSdkImpl]No callback");
        HILOGE("[IcSdkImpl]AieClientInit failed. Error code[%d]", retCode);
        return IC_RETCODE_INIT_ERROR;
    }
    if (clientInfo_.clientId == INVALID_CLIENT_ID) {
        (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_INIT_ERROR))
                               : HILOGD("[IcSdkImpl]No callback");
        HILOGE("[IcSdkImpl]Fail to allocate client id");
        return IC_RETCODE_INIT_ERROR;
    }
    DataInfo inputInfo = {.data = nullptr, .length = 0};
    DataInfo outputInfo = {.data = nullptr, .length = 0};
    retCode = AieClientPrepare(clientInfo_, algorithmInfo_, inputInfo, outputInfo, nullptr);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_INIT_ERROR))
                               : HILOGD("[IcSdkImpl]No callback");
        HILOGE("[IcSdkImpl]AieClientPrepare failed. Error code[%d]", retCode);
        return IC_RETCODE_INIT_ERROR;
    }
    if (outputInfo.data == nullptr || outputInfo.length <= 0) {
        (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_INIT_ERROR))
                               : HILOGD("[IcSdkImpl]No callback");
        HILOGE("[IcSdkImpl]The data or length of output info is invalid");
        return IC_RETCODE_INIT_ERROR;
    }
    MallocPointerGuard<unsigned char> pointerGuard(outputInfo.data);
    retCode = EncdecFacade::ProcessDecode(outputInfo, icHandle_);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_UNSERIALIZATION_ERROR))
                               : HILOGD("[IcSdkImpl]No callback");
        HILOGE("[IcSdkImpl]Failed to UnSerializeHandle");
        return IC_RETCODE_UNSERIALIZATION_ERROR;
    }
    return IC_RETCODE_SUCCESS;
}

int32_t IcSdkImpl::OnSyncExecute(const IcInput &inputData, DataInfo &outputInfo)
{
    if (inputData.data == nullptr || inputData.size == 0) {
        HILOGE("[IcSdkImpl]Empty input");
        return IC_RETCODE_NULL_PARAM;
    }
    DataInfo inputInfo = {
        .data = nullptr,
        .length = 0
    };
    IcInput tmpImage = {
        .data = nullptr,
        .size = MAX_IPC_BUFFER_SIZE
    };
    uint32_t offset = 0;
    int32_t retCode = RETCODE_SUCCESS;
    while (offset < inputData.size) {
        tmpImage.data = &inputData.data[offset];
        tmpImage.size = std::min(inputData.size - offset, MAX_IPC_BUFFER_SIZE);
        inputInfo.data = nullptr;
        inputInfo.length = 0;
        outputInfo.data = nullptr;
        outputInfo.length = 0;
        retCode = EncdecFacade::ProcessEncode(inputInfo, icHandle_, offset, tmpImage);
        if (retCode != RETCODE_SUCCESS) {
            (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_SERIALIZATION_ERROR))
                                   : HILOGD("[IcSdkImpl]No callback");
            HILOGE("[IcSdkImpl]Failed to UnSerializeHandle");
            return IC_RETCODE_SERIALIZATION_ERROR;
        }
        retCode = AieClientSyncProcess(clientInfo_, algorithmInfo_, inputInfo, outputInfo);
        if (retCode != RETCODE_SUCCESS) {
            (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_FAILURE))
                                   : HILOGD("[IcSdkImpl]No callback");
            HILOGE("[IcSdkImpl]SyncExecute AieClientSyncProcess failed");
            return IC_RETCODE_FAILURE;
        }
        offset += tmpImage.size;
    }
    return IC_RETCODE_SUCCESS;
}

int32_t IcSdkImpl::SyncExecute(const IcInput &inputData)
{
    HILOGI("[IcSdkImpl]Start");
    DataInfo outputInfo = {.data = nullptr, .length = 0};
    int32_t retCode = OnSyncExecute(inputData, outputInfo);
    if (outputInfo.data == nullptr || outputInfo.length <= 0 || retCode != IC_RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_FAILURE))
                               : HILOGD("[IcSdkImpl]No callback");
        HILOGE("[IcSdkImpl]SyncExecute failed");
        return retCode;
    }
    IcOutput icResult = {.data = nullptr, .size = 0};
    intptr_t receivedHandle = INVALID_IC_HANDLE;
    MallocPointerGuard<unsigned char> pointerGuard(outputInfo.data);
    retCode = EncdecFacade::ProcessDecode(outputInfo, receivedHandle, icResult);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_UNSERIALIZATION_ERROR))
                               : HILOGD("[IcSdkImpl]No callback");
        HILOGE("[IcSdkImpl]Failed to UnSerializeHandle");
        return IC_RETCODE_UNSERIALIZATION_ERROR;
    }
    if (icHandle_ != receivedHandle) {
        (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_FAILURE))
                               : HILOGD("[IcSdkImpl]No callback");
        HILOGE("[IcSdkImpl]The handle[%lld] of output data is not equal to the current handle[%lld]",
            (long long)receivedHandle, (long long)icHandle_);
        return IC_RETCODE_FAILURE;
    }
    (callback_ != nullptr) ? (callback_->OnResult(icResult))
                           : HILOGD("[IcSdkImpl]No callback");
    return IC_RETCODE_SUCCESS;
}

int32_t IcSdkImpl::SetCallback(std::shared_ptr<IcCallback> callback)
{
    if (callback == nullptr) {
        return IC_RETCODE_FAILURE;
    }
    callback_ = callback;
    return IC_RETCODE_SUCCESS;
}

int32_t IcSdkImpl::Destroy()
{
    HILOGI("[IcSdkImpl]Destroy");
    if (icHandle_ == INVALID_IC_HANDLE) {
        return IC_RETCODE_SUCCESS;
    }
    DataInfo inputInfo = {.data = nullptr, .length = 0};
    int32_t retCode = EncdecFacade::ProcessEncode(inputInfo, icHandle_);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_SERIALIZATION_ERROR))
                               : HILOGD("[IcSdkImpl]No callback");
        HILOGE("[IcSdkImpl]Failed to SerializeHandle");
        return IC_RETCODE_SERIALIZATION_ERROR;
    }
    retCode = AieClientRelease(clientInfo_, algorithmInfo_, inputInfo);
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_FAILURE))
                               : HILOGD("[IcSdkImpl]No callback");
        HILOGE("[IcSdkImpl]AieClientRelease failed. Error code[%d]", retCode);
        return IC_RETCODE_FAILURE;
    }
    retCode = AieClientDestroy(clientInfo_);
    icHandle_ = INVALID_IC_HANDLE;
    if (retCode != RETCODE_SUCCESS) {
        (callback_ != nullptr) ? (callback_->OnError(IC_RETCODE_FAILURE))
                               : HILOGD("[IcSdkImpl]No callback");
        HILOGE("[IcSdkImpl]AieClientDestroy failed. Error code[%d]", retCode);
        return IC_RETCODE_FAILURE;
    }
    return IC_RETCODE_SUCCESS;
}