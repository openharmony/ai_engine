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

namespace OHOS {
namespace AI {
namespace {
constexpr long long ALG_VERSION = 1;
const char *ALG_NAME = "SAMPLE_PLUGIN_1";
const char * const PLUGIN_INFER_MODEL = "SYNC";
const char * const DEFAULT_PROCESS_STRING = "sample_plugin_1 SyncProcess default data";

void FreeDataInfo(DataInfo *dataInfo)
{
    if (dataInfo != nullptr && dataInfo->data != nullptr) {
        free(dataInfo->data);
        dataInfo->data = nullptr;
        dataInfo->length = 0;
    }
}

int ReturnDataCopyOrDefaultData(const DataInfo &inputInfo, DataInfo &outputInfo)
{
    errno_t retCode;
    DataInfo sourceData {};
    if (inputInfo.length <= 0 || inputInfo.data == nullptr) {
        sourceData.data = reinterpret_cast<unsigned char*>(const_cast<char*>(DEFAULT_PROCESS_STRING));
        sourceData.length = strlen(DEFAULT_PROCESS_STRING) + 1;
    } else {
        sourceData = inputInfo;
    }

    outputInfo.length = sourceData.length;
    outputInfo.data = reinterpret_cast<unsigned char*>(malloc(sourceData.length));
    if (outputInfo.data == nullptr) {
        HILOGE("[SamplePlugin1]malloc failed.");
        return RETCODE_FAILURE;
    }
    retCode = memcpy_s(outputInfo.data, outputInfo.length, sourceData.data, sourceData.length);
    if (retCode != EOK) {
        HILOGE("[SamplePlugin1]memcpy_s failed[%d].", retCode);
        FreeDataInfo(&outputInfo);
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}
} // anonymous namespace

SamplePlugin1::SamplePlugin1() = default;

SamplePlugin1::~SamplePlugin1()
{
    FreeDataInfo(&optionData_);
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
    if (inputInfo.data != nullptr && inputInfo.length <= 0) {
        HILOGE("[SamplePlugin1]inputInfo data is invalid.");
        return RETCODE_FAILURE;
    }

    response = IResponse::Create(request);
    CHK_RET(response == nullptr, RETCODE_FAILURE);

    DataInfo outputInfo {};
    int retCode = ReturnDataCopyOrDefaultData(inputInfo, outputInfo);

    response->SetResult(outputInfo);
    response->SetRetCode(retCode);
    return retCode;
}

int SamplePlugin1::AsyncProcess(IRequest *request, IPluginCallback *callback)
{
    HILOGE("[SamplePlugin1]Sync plugin, can't run AsyncProcess.");
    return RETCODE_FAILURE;
}

int SamplePlugin1::Prepare(long long transactionId, const DataInfo &inputInfo, DataInfo &outputInfo)
{
    return ReturnDataCopyOrDefaultData(inputInfo, outputInfo);
}

int SamplePlugin1::Release(bool isFullUnload, long long transactionId, const DataInfo &inputInfo)
{
    FreeDataInfo(&optionData_);
    return RETCODE_SUCCESS;
}

int SamplePlugin1::SetOption(int optionType, const DataInfo &inputInfo)
{
    FreeDataInfo(&optionData_);

    if (inputInfo.data == nullptr) {
        return RETCODE_SUCCESS;
    }

    return ReturnDataCopyOrDefaultData(inputInfo, optionData_);
}

int SamplePlugin1::GetOption(int optionType, const DataInfo &inputInfo, DataInfo &outputInfo)
{
    return ReturnDataCopyOrDefaultData(optionData_, outputInfo);
}

PLUGIN_INTERFACE_IMPL(SamplePlugin1);
}
}
