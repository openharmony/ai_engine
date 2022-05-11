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

#ifndef ADAPTER_WRAPPER_H
#define ADAPTER_WRAPPER_H

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "ipc_skeleton.h"
#include "protocol/struct_definition/aie_info_define.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Allocate client ID and generate adapter for client.
 *
 * @return Returns client ID if the operation is successful, returns INVALID_CLIENT_ID otherwise.
 */
extern int GenerateClient();

/**
 * Generate transaction ID and load certain algorithm based on the information transferred from client.
 *
 * @param [in] clientInfo Client information.
 * @param [in] algorithmInfo Algorithm information.
 * @param [in] inputInfo Data information needed to load algorithm plugin.
 * @param [out] outputInfo The returned data information after loading the algorithm plugin.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
extern int LoadAlgoWrapper(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo,
    const DataInfo *inputInfo, DataInfo *outputInfo);

/**
 * Execute algorithm inference synchronously.
 *
 * @param [in] clientInfo Client information.
 * @param [in] AlgorithmInfo Algorithm information.
 * @param [in] inputInfo Data information needed to synchronous execution algorithm.
 * @param [out] outputInfo Algorithm inference results.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
extern int SyncExecAlgoWrapper(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo,
    const DataInfo *inputInfo, DataInfo *outputInfo);

/**
 * Execute algorithm inference asynchronously.
 *
 * @param [in] clientInfo Client information.
 * @param [in] AlgorithmInfo Algorithm information.
 * @param [in] inputInfo Data information needed to synchronous execution algorithm.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
extern int AsyncExecAlgoWrapper(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo, const DataInfo *inputInfo);

/**
 * Unload algorithm plugin and model based on algorithm information and client information.
 *
 * @param [in] clientInfo Client information.
 * @param [in] algorithmInfo Algorithm information.
 * @param [in] inputInfo Data information needed to load algorithm plugin.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
extern int UnloadAlgoWrapper(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo, const DataInfo *inputInfo);

/**
 * Delete client adapter.
 *
 * @param [in] clientInfo Client information.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
extern int RemoveAdapterWrapper(const ClientInfo *clientInfo);

/**
 * Set the configuration parameters of the engine or plugin.
 *
 * @param [in] clientInfo Client information.
 * @param [in] optionType The type of setting option.
 * @param [in] inputInfo Configuration parameter needed to set up the engine or plugin.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
extern int SetOptionWrapper(const ClientInfo *clientInfo, int optionType, const DataInfo *inputInfo);

/**
 * Get the configuration parameters of the engine or plugin.
 *
 * @param [in] clientInfo Client information.
 * @param [in] optionType The type of getting option.
 * @param [in] inputInfo Parameter information for getting options.
 * @param [out] outputInfo The configuration parameter information.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
extern int GetOptionWrapper(const ClientInfo *clientInfo, int optionType, const DataInfo *inputInfo,
    DataInfo *outputInfo);

/**
 * Save listener to call client async process, and register server async handler.
 *
 * @param [in] clientInfo Client information.
 * @param [in] sid Client async callback SVC handle identity
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
extern int RegisterCallbackWrapper(const ClientInfo *clientInfo, SvcIdentity *sid);

/**
 * Delete listener to call client async process, and stop server async handler.
 *
 * @param [in] clientInfo Client information.
 * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
 */
extern int UnregisterCallbackWrapper(const ClientInfo *clientInfo);

#ifdef __cplusplus
};
#endif

#endif // ADAPTER_WRAPPER_H
