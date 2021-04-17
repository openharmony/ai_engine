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

#include "liteipc.h"
#include "protocol/struct_definition/aie_info_define.h"

#ifdef __cplusplus
extern "C" {
#endif

void ParcelDataInfo(IpcIo *request, const DataInfo *dataInfo);
int UnParcelDataInfo(IpcIo *request, DataInfo *dataInfo);
void FreeDataInfo(DataInfo *dataInfo);

#ifdef __cplusplus
}
#endif //

#endif // AIE_IPC_H
