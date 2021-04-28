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
#include <sys/shm.h>

#include "securec.h"

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/aie_guard.h"
#include "utils/log/aie_log.h"

namespace {
constexpr int IPC_MAX_TRANS_CAPACITY = 200; // memory beyond this limit will use shared memory
constexpr int SHM_KEY_START = 200000; // chosen randomly
constexpr int SHM_KEY_END   = 300000; // chosen randomly
constexpr unsigned int SHM_READ_WRITE_PERMISSIONS = 0777U;

void ReleaseShmId(const int shmId)
{
    if (shmId == -1) {
        return;
    }
    if (shmctl(shmId, IPC_RMID, nullptr) == -1) {
        HILOGE("[AieIpc]shmctl IPC_RMID failed: %d.", errno);
        return;
    }
}

/**
 * Use shared memory to push large memory.
 *
 * @param [in] request Ipc handle.
 * @param [in] dataInfo Data need to transfer.
 * @param receiverUid receiver's uid.
 */
void IpcIoPushSharedMemory(IpcIo *request, const DataInfo *dataInfo, const uid_t receiverUid)
{
    // internal call, no need to check null.
    static int shmKey = SHM_KEY_START;
    int shmId;
    while ((shmId = shmget(shmKey, dataInfo->length, SHM_READ_WRITE_PERMISSIONS | IPC_CREAT | IPC_EXCL)) < 0) {
        if (errno == EEXIST) {
            ++shmKey;
            if (shmKey >= SHM_KEY_END) {
                shmKey = SHM_KEY_START;
            }
            continue;
        }
        HILOGE("[AieIpc]shmget failed: %d.", errno);
        return;
    }
    HILOGI("[AieIpc]shmget succeed, shmKey = %d, shmId = %d.", shmKey, shmId);

    char *shared = reinterpret_cast<char *>(shmat(shmId, nullptr, 0));
    if (shared == reinterpret_cast<char *>(-1)) {
        ReleaseShmId(shmId);
        HILOGE("[AieIpc]shmat failed: %d.", errno);
        return;
    }

    int retCode;
    if ((retCode = memcpy_s(shared, dataInfo->length, dataInfo->data, dataInfo->length)) != EOK) {
        shmdt(shared);
        ReleaseShmId(shmId);
        HILOGE("[AieIpc]memcpy_s failed: %d.", retCode);
        return;
    }

    if (shmdt(shared) == -1) {
        ReleaseShmId(shmId);
        HILOGE("[AieIpc]shmdt failed: %d.", errno);
        return;
    }

    struct shmid_ds shmidDs {};
    if (shmctl(shmId, IPC_STAT, &shmidDs) == -1) {
        HILOGE("[AieIpc]shmctl IPC_STAT failed: %d.", errno);
        ReleaseShmId(shmId);
    }

    shmidDs.shm_perm.uid = receiverUid; // give receiver the privilege to release shared memory.
    if (shmctl(shmId, IPC_SET, &shmidDs) == -1) {
        ReleaseShmId(shmId);
        HILOGE("[AieIpc]shmctl IPC_SET failed: %d.", errno);
        return;
    }

    IpcIoPushInt32(request, shmId);
    IpcIoPushInt32(request, dataInfo->length);
}

/**
 * Use shared memory to pop large memory.
 *
 * @param [in] request Ipc handle.
 * @param [out] dataInfo Data received.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int IpcIoPopSharedMemory(IpcIo *request, DataInfo *dataInfo)
{
    // internal call, no need to check null.
    int shmId = IpcIoPopInt32(request);
    dataInfo->length = IpcIoPopInt32(request); // make sure all data are popped out.

    if (shmId == -1) {
        HILOGE("[AieIpc]shmId is invalid: %d.", shmId);
        return RETCODE_FAILURE;
    }
    if (dataInfo->length <= 0) {
        HILOGE("[AieIpc]dataInfo->length is invalid: %d.", dataInfo->length);
        ReleaseShmId(shmId);
        return RETCODE_FAILURE;
    }

    char *shared = reinterpret_cast<char *>(shmat(shmId, nullptr, 0));
    if (shared == reinterpret_cast<char *>(-1)) {
        HILOGE("[AieIpc]shmat failed %d.", errno);
        ReleaseShmId(shmId);
        return RETCODE_FAILURE;
    }

    if (shared == nullptr) {
        HILOGE("[AieIpc]shared data is nullptr.");
        ReleaseShmId(shmId);
        return RETCODE_NULL_PARAM;
    }

    dataInfo->data = reinterpret_cast<unsigned char *>(malloc(dataInfo->length));
    if (dataInfo->data == nullptr) {
        shmdt(shared);
        ReleaseShmId(shmId);
        HILOGE("[AieIpc]Failed to malloc memory.");
        return RETCODE_OUT_OF_MEMORY;
    }
    OHOS::AI::MallocPointerGuard<unsigned char> dataInfoGuard(dataInfo->data);

    errno_t retCode = memcpy_s(dataInfo->data, dataInfo->length, shared, dataInfo->length);
    if (retCode != EOK) {
        shmdt(shared);
        ReleaseShmId(shmId);
        HILOGE("[AieIpc]Failed to memory copy, retCode[%d].", retCode);
        return RETCODE_MEMORY_COPY_FAILURE;
    }

    if (shmdt(shared) == -1) {
        ReleaseShmId(shmId);
        HILOGE("[AieIpc]shmdt failed: %d.", errno);
        return RETCODE_FAILURE;
    }

    ReleaseShmId(shmId);

    dataInfoGuard.Detach();
    return RETCODE_SUCCESS;
}

/**
 * Use ipc to pop memory.
 *
 * @param [in] request Ipc handle.
 * @param [out] dataInfo Data received.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int IpcIoPopMemory(IpcIo *request, DataInfo *dataInfo)
{
    // internal call, no need to check null.
    uint32_t dataBufSize = 0;
    void *dataBuf = IpcIoPopFlatObj(request, &dataBufSize);
    if (dataBuf == nullptr) {
        HILOGE("[AieIpc]The UnParcel dataBuf is invalid.");
        return RETCODE_NULL_PARAM;
    }
    if (static_cast<int>(dataBufSize) != dataInfo->length) {
        HILOGE("[AieIpc]The UnParcel dataBufSize[%ud] doesn't match dataInfo->length[%d].",
            dataBufSize, dataInfo->length);
        return RETCODE_NULL_PARAM;
    }

    dataInfo->data = reinterpret_cast<unsigned char *>(malloc(dataBufSize));
    if (dataInfo->data == nullptr) {
        HILOGE("[AieIpc]Failed to malloc memory.");
        return RETCODE_OUT_OF_MEMORY;
    }
    errno_t retCode = memcpy_s(dataInfo->data, dataInfo->length, dataBuf, dataBufSize);
    if (retCode != EOK) {
        HILOGE("[AieIpc]Failed to memory copy, retCode[%d].", retCode);
        FreeDataInfo(dataInfo);
        return RETCODE_MEMORY_COPY_FAILURE;
    }
    return RETCODE_SUCCESS;
}
} // anonymous namespace

void ParcelDataInfo(IpcIo *request, const DataInfo *dataInfo, const uid_t receiverUid)
{
    if (dataInfo == nullptr) {
        HILOGE("[AieIpc]The dataInfo is invalid.");
        return;
    }
    if (request == nullptr) {
        HILOGE("[AieIpc]The request is nullptr.");
        return;
    }
    if (dataInfo->data != nullptr && dataInfo->length <= 0) { // invalid datainfo
        HILOGE("[AieIpc]dataInfo->data != nullptr, dataInfo->length <= 0.");
        return;
    }
    if (dataInfo->data == nullptr && dataInfo->length != 0) {  // invalid datainfo
        HILOGE("[AieIpc]dataInfo->data == nullptr, dataInfo->length != 0.");
        return;
    }

    // parcel data length first.
    IpcIoPushInt32(request, dataInfo->length);
    if (dataInfo->data == nullptr && dataInfo->length == 0) { // empty datainfo, no need to parcel, save length(0) only.
        return;
    }
    // parcel the data only if the data length > 0
    if (dataInfo->length < IPC_MAX_TRANS_CAPACITY) {
        IpcIoPushFlatObj(request, dataInfo->data, static_cast<uint32_t>(dataInfo->length));
    } else {
        IpcIoPushSharedMemory(request, dataInfo, receiverUid);
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
        dataInfo->data = nullptr;
        return RETCODE_SUCCESS;
    }

    if (dataInfo->length < IPC_MAX_TRANS_CAPACITY) {
        return IpcIoPopMemory(request, dataInfo);
    } else {
        return IpcIoPopSharedMemory(request, dataInfo);
    }
}

void FreeDataInfo(DataInfo *dataInfo)
{
    if (dataInfo != nullptr && dataInfo->data != nullptr) {
        free(dataInfo->data);
        dataInfo->data = nullptr;
        dataInfo->length = 0;
    }
}
