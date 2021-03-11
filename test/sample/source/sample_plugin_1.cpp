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

#include "sample/include/sample_plugin_1.h"

#include <cstring>

#include "securec.h"

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/log/aie_log.h"

namespace OHOS::AI {
SamplePlugin1::SamplePlugin1() = default;

SamplePlugin1::~SamplePlugin1()
{
    if (optionData != nullptr) {
        free(optionData);
        optionData = nullptr;
        optionLength = 0;
    }
}

const long long SamplePlugin1::GetVersion() const
{
    return ALG_VERSION;
}

const char *SamplePlugin1::GetName() const
{
    return ALG_NAME;
}

const char *SamplePlugin1::GetInferMode() const
{
    return PLUGIN_INFER_MODEL;
}

int SamplePlugin1::SyncProcess(IRequest *request, IResponse *&response)
{
    DataInfo inputInfo = request->GetMsg();
    if (inputInfo.data != nullptr) {
        HILOGE("[SamplePlugin2]inputInfo data is %p.", inputInfo.data);
    }
    response = IResponse::Create(request);
    CHK_RET(response == nullptr, RETCODE_FAILURE);

    const char *str = "sample_plugin_1 SyncProcess outputInfo";
    char *outputData = const_cast<char*>(str);
    int len = strlen(str) + 1;
    DataInfo outputInfo = {
        .data = (unsigned char*)outputData,
        .length = len,
    };
    response->SetResult(outputInfo);
    response->SetRetCode(RETCODE_SUCCESS);
    return RETCODE_SUCCESS;
}

int SamplePlugin1::AsyncProcess(IRequest *request, IPluginCallback *callback)
{
    DataInfo inputInfo = request->GetMsg();
    if (inputInfo.data != nullptr) {
        HILOGE("[SamplePlugin2]inputInfo data is %p.", inputInfo.data);
    }
    IResponse *response = IResponse::Create(request);
    CHK_RET(response == nullptr, RETCODE_FAILURE);

    const char *str = "sample_plugin_1 AsyncProcess outputInfo";
    char *outputData = const_cast<char*>(str);
    int len = strlen(str) + 1;
    DataInfo outputInfo = {
        .data = (unsigned char*)outputData,
        .length = len,
    };
    response->SetResult(outputInfo);
    response->SetRetCode(RETCODE_SUCCESS);
    int retCode = callback->OnEvent(ON_PLUGIN_SUCCEED, response);
    return retCode;
}

int SamplePlugin1::Prepare(long long transactionId, const DataInfo &inputInfo, DataInfo &outputInfo)
{
    const char *str = "sample_plugin_1 Prepare outputInfo";
    char *outputData = const_cast<char*>(str);
    int len = strlen(str) + 1;
    outputInfo = {
        .data = (unsigned char*)outputData,
        .length = len,
    };
    return RETCODE_SUCCESS;
}

int SamplePlugin1::Release(bool isFullUnload, long long transactionId, const DataInfo &inputInfo)
{
    if (optionData != nullptr) {
        free(optionData);
        optionData = nullptr;
        optionLength = 0;
    }
    return RETCODE_SUCCESS;
}

int SamplePlugin1::SetOption(int optionType, const DataInfo &inputInfo)
{
    if (optionData != nullptr) {
        free(optionData);
        optionData = nullptr;
        optionLength = 0;
    }
    if (inputInfo.data == nullptr) {
        return RETCODE_SUCCESS;
    }
    optionLength = inputInfo.length;
    optionData = (unsigned char *)malloc(sizeof(unsigned char) * optionLength);
    if (optionData == nullptr) {
        HILOGE("[SamplePlugin1]Failed to request memory.");
        return RETCODE_OUT_OF_MEMORY;
    }
    errno_t retCode = memcpy_s(optionData, optionLength, inputInfo.data, optionLength);
    if (retCode != EOK) {
        HILOGE("[SamplePlugin1]Failed to memory copy, retCode[%d].", retCode);
        free(optionData);
        optionData = nullptr;
        return RETCODE_MEMORY_COPY_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int SamplePlugin1::GetOption(int optionType, const DataInfo &inputInfo, DataInfo &outputInfo)
{
    if (optionData == nullptr) {
        optionLength = strlen(DEFAULT_OPTION_DATA) + 1;
        optionData = (unsigned char *)malloc(sizeof(unsigned char) * optionLength);
        if (optionData == nullptr) {
            HILOGE("[SamplePlugin1]Failed to request memory.");
            return RETCODE_OUT_OF_MEMORY;
        }
        errno_t retCode = memcpy_s(optionData, optionLength, DEFAULT_OPTION_DATA, optionLength);
        if (retCode != EOK) {
            HILOGE("[SamplePlugin1]Failed to memory copy, retCode[%d].", retCode);
            free(optionData);
            optionData = nullptr;
            return RETCODE_MEMORY_COPY_FAILURE;
        }
    }
    outputInfo = {
        .data = optionData,
        .length = optionLength
    };
    return RETCODE_SUCCESS;
}

PLUGIN_INTERFACE_IMPL(SamplePlugin1);
}
