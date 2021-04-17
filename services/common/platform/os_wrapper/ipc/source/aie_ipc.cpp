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

#include "platform/os_wrapper/ipc/include/aie_ipc.h"

#include <cstdlib>

#include "securec.h"

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/log/aie_log.h"

void ParcelDataInfo(IpcIo *request, const DataInfo *dataInfo)
{
    if (dataInfo == nullptr) {
        HILOGE("[AieIpc]The dataInfo is invalid.");
        return;
    }
    if (dataInfo->data == nullptr || dataInfo->length < 0) {
        return;
    }
    if (request == nullptr) {
        HILOGE("[AieIpc]The request is nullptr");
        return;
    }
    // parcel data length first.
    IpcIoPushInt32(request, dataInfo->length);
    // parcel the data only if the data length > 0
    if (dataInfo->length > 0) {
        IpcIoPushFlatObj(request, dataInfo->data, static_cast<uint32_t>(dataInfo->length));
    }
}

int UnParcelDataInfo(IpcIo *request, DataInfo *dataInfo)
{
    if (request == nullptr) {
        HILOGE("[AieIpc]The request is nullptr.");
        return RETCODE_FAILURE;
    }
    if (dataInfo == nullptr) {
        HILOGE("[AieIpc]The dataInfo is nullptr.");
        return RETCODE_FAILURE;
    }

    dataInfo->length = IpcIoPopInt32(request);
    if (dataInfo->length < 0) {
        HILOGE("[AieIpc]The dataInfo length is invalid.");
        return RETCODE_FAILURE;
    }
    if (dataInfo->length == 0) { // no following buffer to unparcel.
        return RETCODE_SUCCESS;
    }

    uint32_t dataBufSize = 0;
    void *dataBuf = IpcIoPopFlatObj(request, &dataBufSize);
    if (dataBuf == nullptr || static_cast<int32_t>(dataBufSize) != dataInfo->length) {
        HILOGE("[AieIpc]The UnParcel dataBuf is invalid.");
        return RETCODE_NULL_PARAM;
    }

    dataInfo->data = reinterpret_cast<unsigned char *>(malloc(sizeof(unsigned char) * dataBufSize));
    if (dataInfo->data == nullptr) {
        HILOGE("[AieIpc]Failed to malloc memory.");
        return RETCODE_OUT_OF_MEMORY;
    }
    errno_t retCode = memcpy_s(dataInfo->data, dataInfo->length, dataBuf, dataBufSize);
    if (retCode != EOK) {
        HILOGE("[AieIpc]Failed to memory copy, retCode[%d].", retCode);
        free(dataInfo->data);
        dataInfo->data = nullptr;
        dataInfo->length = 0;
        return RETCODE_MEMORY_COPY_FAILURE;
    }
    return RETCODE_SUCCESS;
}

void FreeDataInfo(DataInfo *dataInfo)
{
    if (dataInfo != nullptr && dataInfo->data != nullptr) {
        free(dataInfo->data);
        dataInfo->data = nullptr;
        dataInfo->length = 0;
    }
}
