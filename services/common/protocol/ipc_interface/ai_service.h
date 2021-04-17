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

#ifndef AI_SERVICE_H
#define AI_SERVICE_H

#include "iproxy_client.h"
#include "iproxy_server.h"

#ifdef __cplusplus
extern "C" {
#endif

#define AI_SERVICE "ai_service"
#define AI_FEATURE "ai_feature"

enum FUNC_ID {
    ID_INIT_ENGINE = 0,
    ID_LOAD_ALGORITHM,
    ID_SYNC_EXECUTE_ALGORITHM,
    ID_ASYNC_EXECUTE_ALGORITHM,
    ID_UNLOAD_ALGORITHM,
    ID_DESTROY_ENGINE,
    ID_SET_OPTION,
    ID_GET_OPTION,
    ID_REGISTER_CALLBACK,
    ID_UNREGISTER_CALLBACK,
};

enum CALLBACK_ID {
    ON_ASYNC_PROCESS_CODE = 0,
};

typedef struct AiInterface {
    INHERIT_SERVER_IPROXY;

    /**
     * @brief Initialize engine configuration and get the client ID from ai server.
     *
     * @param [in] configInfo Engine configuration information.
     * @return Client ID.
     */
    int (*InitEngine)(const ConfigInfo *configInfo);

    /**
     * @brief Load algorithm plugin and model based on algorithm information and client information.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algoInfo Algorithm information.
     * @param [in] inputInfo Data information needed to load algorithm plugin.
     * @param [out] outputInfo The returned data information after loading the algorithm plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int (*LoadAlgorithm)(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo,
        const DataInfo *inputInfo, DataInfo *outputInfo);

    /**
     * @brief Algorithmic inference interface for synchronous tasks.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algoInfo Algorithm information.
     * @param [in] inputInfo Data information needed to synchronous execution algorithm.
     * @param [out] outputInfo Algorithm inference results.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int (*SyncExecuteAlgorithm)(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo,
        const DataInfo *inputInfo, DataInfo *outputInfo);

    /**
     * @brief Algorithmic inference interface for asynchronous tasks.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algoInfo Algorithm information.
     * @param [in] inputInfo Data information needed to asynchronous execution algorithm.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int (*AsyncExecuteAlgorithm)(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo,
        const DataInfo *inputInfo);

    /**
     * @brief Unload algorithm model and plugin based on algorithm information and client information.
     *
     * @param [in] clientInfo Client information.
     * @param [in] algoInfo Algorithm information.
     * @param [in] inputInfo Data information needed to unload algorithm plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int (*UnloadAlgorithm)(const ClientInfo *clientInfo, const AlgorithmInfo *algoInfo, const DataInfo *inputInfo);

    /**
     * @brief Disconnect the link between the client and the server, and destroy the engine information
     *        corresponding to the client.
     *
     * @param [in] clientInfo Client information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int (*DestroyEngine)(const ClientInfo *clientInfo);

    /**
     * @brief Set the configuration parameters of the engine or plugin.
     *
     * @param [in] clientInfo Client information.
     * @param [in] optionType The type of setting option.
     * @param [in] inputInfo Configuration parameter needed to set up the engine or plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int (*SetOption)(const ClientInfo *clientInfo, int optionType, const DataInfo *inputInfo);

    /**
     * @brief Get the configuration parameters of the engine or plugin.
     *
     * @param [in] clientInfo Client information.
     * @param [in] optionType The type of getting option.
     * @param [in] inputInfo Parameter information for getting options.
     * @param [out] outputInfo The configuration parameter information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int (*GetOption)(const ClientInfo *clientInfo, int optionType,
        const DataInfo *inputInfo, DataInfo *outputInfo);

    /**
     * @brief Unregister callback function of client.
     *
     * @param [in] clientInfo Client information.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    int (*UnregisterCallback)(const ClientInfo *clientInfo);
} AiInterface;

#ifdef __cplusplus
}
#endif

#endif // AI_SERVICE_H
