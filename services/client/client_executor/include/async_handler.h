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

#ifndef ASYNC_HANDLER_H
#define ASYNC_HANDLER_H

#include <map>
#include <mutex>

#include "client_executor/include/i_client_cb.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
class AsyncHandler {
public:
    AsyncHandler();
    virtual ~AsyncHandler();

    int RegisterCb(int sessionId, IClientCb *cb);
    int UnRegisterCb(int sessionId);

    int RegisterDeadCb(int sessionId, IServiceDeadCb *cb);
    int UnRegisterDeadCb(int sessionId);

    void OnResult(int sessionId, const DataInfo &result, int resultCode, int requestId);
    void OnDead();

    void FindCb(int sessionId, IClientCb *&cb);

    bool IsCallbackEmpty();
    int GetAsyncCbSize();

private:
    virtual int RegisterAsyncClientCb() = 0;
    virtual int UnRegisterAsyncClientCb() = 0;
    virtual int RegisterServiceDeathCb() = 0;
    virtual int UnRegisterServiceDeathCb() = 0;

private:
    static std::mutex cbMutex_;
    using ClientCbs = std::map<int, IClientCb *>;
    ClientCbs mapCbMsg_;
    std::mutex deadCbMutex_;
    using DeadCbs = std::map<int, IServiceDeadCb *>;
    DeadCbs deadCbs_;
};
} // namespace AI
} // namespace OHOS

#endif // ASYNC_HANDLER_H