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

#ifndef SA_CLIENT_PROXY_H
#define SA_CLIENT_PROXY_H

#include <unistd.h>

#include "iproxy_client.h"
#include "iunknown.h"
#include "ipc_skeleton.h"
#include "registry.h"
#include "securec.h"

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"

namespace OHOS {
namespace AI {
#ifdef __cplusplus
extern "C" {
#endif

void HosInit();
IClientProxy *GetRemoteIUnknown(void);

/**
 * Invoke SA server, to connect the server and get the client ID and server uid.
 *
 * @param [in] proxy SA proxy to call ai server interfaces.
 * @param [in] configInfo Engine configuration information.
 * @param [out] clientInfo Client information.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int InitSaEngine(IClientProxy &proxy, const ConfigInfo &configInfo, ClientInfo &clientInfo);

/**
 * Invoke SA server, to load algorithm plugin and model.
 *
 * @param [in] proxy SA proxy to call ai server interfaces.
 * @param [in] clientInfo Client information.
 * @param [in] algorithmInfo Algorithm information.
 * @param [in] inputInfo Data information needed to load algorithm plugin.
 * @param [out] outputInfo The returned data information after loading the algorithm plugin.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int LoadAlgorithmProxy(IClientProxy &proxy, const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo);

/**
 * Invoke SA server, to execute algorithm inference synchronously.
 *
 * @param [in] proxy SA proxy to call ai server interfaces.
 * @param [in] clientInfo Client information.
 * @param [in] AlgorithmInfo Algorithm information.
 * @param [in] inputInfo Data information needed to synchronous execution algorithm.
 * @param [out] outputInfo Algorithm inference results.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int SyncExecAlgorithmProxy(IClientProxy &proxy, const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo, DataInfo &outputInfo);

/**
 * Invoke SA server, to execute algorithm inference asynchronously.
 *
 * @param [in] proxy SA proxy to call ai server interfaces.
 * @param [in] clientInfo Client information.
 * @param [in] algorithmInfo Algorithm information.
 * @param [in] inputInfo Data information needed to asynchronous execution algorithm.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int AsyncExecuteAlgorithmProxy(IClientProxy &proxy, const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo);

/**
 * Invoke SA server, to unload algorithm plugin and model based on algorithm information and client information.
 *
 * @param [in] proxy SA proxy to call ai server interfaces.
 * @param [in] clientInfo Client information.
 * @param [in] algorithmInfo Algorithm information.
 * @param [in] inputInfo Data information needed to load algorithm plugin.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int UnloadAlgorithmProxy(IClientProxy &proxy, const ClientInfo &clientInfo, const AlgorithmInfo &algoInfo,
    const DataInfo &inputInfo);

/**
 * Invoke SA server, to disconnect the client from the server, release and destroy information of the client.
 *
 * @param [in] proxy SA proxy to call ai server interfaces.
 * @param [in] clientInfo Client information.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int DestroyEngineProxy(IClientProxy &proxy, const ClientInfo &clientInfo);

/**
 * Release SA client proxy.
 *
 * @param [in] proxy SA proxy acquired from SA manager.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
void ReleaseIUnknown(IUnknown &proxy);

/**
 * Invoke SA server, to set the configuration parameters of the engine or plugin.
 *
 * @param [in] proxy SA proxy to call ai server interfaces.
 * @param [in] clientInfo Client information.
 * @param [in] optionType The type of setting option.
 * @param [in] inputInfo Configuration parameter needed to set up the engine or plugin.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int SetOptionProxy(IClientProxy &proxy, const ClientInfo &clientInfo, int optionType, const DataInfo &inputInfo);

/**
 * Invoke SA server, to get the configuration parameters of the engine or plugin.
 *
 * @param [in] proxy SA proxy to call ai server interfaces.
 * @param [in] clientInfo Client information.
 * @param [in] optionType The type of getting option.
 * @param [in] inputInfo Parameter information for getting options.
 * @param [out] outputInfo The configuration parameter information.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int GetOptionProxy(IClientProxy &proxy, const ClientInfo &clientInfo, int optionType,
    const DataInfo &inputInfo, DataInfo &outputInfo);

/**
 * Invoke SA server, to register listener for async processing.
 *
 * @param [in] proxy SA proxy to call ai server interfaces.
 * @param [in] clientInfo Client information.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int RegisterCallbackProxy(IClientProxy &proxy, const ClientInfo &clientInfo, OnRemoteRequest asyncCallback);

/**
 * Invoke SA server, to unregister listener for async processing.
 *
 * @param [in] proxy SA proxy to call ai server interfaces.
 * @param [in] clientInfo Client information.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
int UnregisterCallbackProxy(IClientProxy &proxy, const ClientInfo &clientInfo);

#ifdef __cplusplus
}
#endif
} // namespace AI
} // namespace OHOS

#endif // SA_CLIENT_PROXY_H
