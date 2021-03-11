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

#ifndef I_SYNC_TASK_MANAGER_H
#define I_SYNC_TASK_MANAGER_H

#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"

namespace OHOS {
namespace AI {
class ISyncTaskManager {
public:
    virtual ~ISyncTaskManager() = default;

    /**
     * Interface to sync execute request
     *
     * @param [in] request Input info.
     * @param [out] response Output info.
     * @return Returns RETCODE_SUCCESS(0) if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int SyncExecute(IRequest *request, IResponse *&response) = 0;
};

/**
 * Get the singleton of ServerExecutor.
 *
 * @return Pointer to the singleton.
 */
ISyncTaskManager *GetSyncTaskManager();
} // namespace AI
} // namespace OHOS

#endif // I_SYNC_TASK_MANAGER_H