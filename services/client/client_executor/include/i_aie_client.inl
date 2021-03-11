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

#ifndef I_AIE_CLIENT_INL
#define I_AIE_CLIENT_INL

#include <vector>

#include "client_factory.h"
#include "i_client_cb.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/aie_macros.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
inline int AieClientInit(const ConfigInfo &configInfo, ClientInfo &clientInfo,
    const AlgorithmInfo &algorithmInfo, IServiceDeadCb *cb)
{
    HILOGI("[IAieClient]AieClientInit");
    ClientFactory *client = GetClient();
    CHK_RET(client == nullptr, RETCODE_NULL_PARAM);
    return client->ClientInit(configInfo, clientInfo, algorithmInfo, cb);
}

inline int AieClientPrepare(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo, IClientCb *cb)
{
    HILOGI("[IAieClient]AieClientPrepare");
    ClientFactory *client = GetClient();
    CHK_RET(client == nullptr, RETCODE_NULL_PARAM);
    return client->ClientPrepare(clientInfo, algorithmInfo, inputInfo, outputInfo, cb);
}

inline int AieClientAsyncProcess(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo)
{
    HILOGI("[IAieClient]AieClientAsyncProcess");
    ClientFactory *client = GetClient();
    CHK_RET(client == nullptr, RETCODE_NULL_PARAM);
    return client->ClientAsyncProcess(clientInfo, algorithmInfo, inputInfo);
}

inline int AieClientDestroy(ClientInfo &clientInfo)
{
    HILOGI("[IAieClient]AieClientDestroy");
    ClientFactory *client = GetClient();
    CHK_RET(client == nullptr, RETCODE_NULL_PARAM);
    return client->ClientDestroy(clientInfo);
}

inline int AieClientSetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo)
{
    HILOGI("[IAieClient]AieClientSetOption");
    ClientFactory *client = GetClient();
    CHK_RET(client == nullptr, RETCODE_NULL_PARAM);
    return client->ClientSetOption(clientInfo, optionType, inputInfo);
}

inline int AieClientGetOption(const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo,
    DataInfo &outputInfo)
{
    HILOGI("[IAieClient]AieClientGetOption");
    ClientFactory *client = GetClient();
    CHK_RET(client == nullptr, RETCODE_NULL_PARAM);
    return client->ClientGetOption(clientInfo, optionType, inputInfo, outputInfo);
}

inline int AieClientRelease(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo)
{
    HILOGI("[IAieClient]AieClientRelease");
    ClientFactory *client = GetClient();
    CHK_RET(client == nullptr, RETCODE_NULL_PARAM);
    return client->ClientRelease(clientInfo, algorithmInfo, inputInfo);
}

inline int AieClientSyncProcess(const ClientInfo &clientInfo, const AlgorithmInfo &algorithmInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo)
{
    HILOGI("[IAieClient]AieClientSyncProcess.");
    ClientFactory *client = GetClient();
    CHK_RET(client == nullptr, RETCODE_NULL_PARAM);
    return client->ClientSyncProcess(clientInfo, algorithmInfo, inputInfo, outputInfo);
}
} // namespace AI
} // namespace OHOS

#endif // I_AIE_CLIENT_INL