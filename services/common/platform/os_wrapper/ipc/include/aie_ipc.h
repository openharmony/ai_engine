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

#ifndef AIE_IPC_H
#define AIE_IPC_H

#include "protocol/struct_definition/aie_info_define.h"
#include "serializer.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Use ipc to transfer memory.
 *
 * Memory larger than IPC_MAX_TRANS_CAPACITY(200) would be transferred by shared memory, otherwise by ipc.
 *
 * @param [in] request Ipc handle.
 * @param [in] dataInfo Data to transfer.
 * @param [in] receiverUid receiver's uid.
 */
void ParcelDataInfo(IpcIo *request, const DataInfo *dataInfo, const uid_t receiverUid);

/**
 * Use ipc to receive memory.
 * Note: the returned dataInfo must release by {@link FreeDataInfo}.
 *
 * @param [in] request Ipc handle.
 * @param [out] dataInfo Data received.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int UnParcelDataInfo(IpcIo *request, DataInfo *dataInfo);

/**
 * Free dataInfo.
 *
 * @param [in] dataInfo data to be freed.
 */
void FreeDataInfo(DataInfo *dataInfo);

#ifdef __cplusplus
}
#endif

#endif // AIE_IPC_H
