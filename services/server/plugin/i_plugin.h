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

#ifndef I_PLUGIN_H
#define I_PLUGIN_H

#include "plugin/i_plugin_callback.h"
#include "protocol/data_channel/include/i_request.h"
#include "protocol/data_channel/include/i_response.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"

namespace OHOS {
namespace AI {
const char * const PLUGIN_INTERFACE_NAME = "PLUGIN_INTERFACE";

#define PLUGIN_INTERFACE_IMPL(PluginName) \
    extern "C" IPlugin* PLUGIN_INTERFACE() \
    { \
        return new PluginName(); \
    } \

class IPlugin {
public:
    virtual ~IPlugin() = default;

    virtual const long long GetVersion() const = 0;

    virtual const char *GetName() const = 0;

    /**
     * Get plugin inference mode.
     *
     * @return Inference mode, synchronous or asynchronous.
     */
    virtual const char *GetInferMode() const = 0;

    /**
     * Algorithmic inference interface for synchronous tasks.
     *
     * @param [in] request Request task which contains the specific information of the task.
     * @param [out] response Results of encapsulated algorithmic inference.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int SyncProcess(IRequest *request, IResponse *&response) = 0;

    /**
     * Algorithmic inference interface for asynchronous tasks.
     *
     * @param [in] request Request task which contains the specific information of the task.
     * @param [in] callback Callback which is used to return the result of asynchronous inference.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int AsyncProcess(IRequest *request, IPluginCallback *callback) = 0;

    /**
     * Initialize plugin.
     *
     * @param [in] transactionId Transaction ID.
     * @param [in] inputInfo Data information needed to initialize plugin.
     * @param [out] outputInfo The returned data information of initializing plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int Prepare(long long transactionId, const DataInfo &inputInfo, DataInfo &outputInfo) = 0;

    /**
     * Unload model and plugin.
     *
     * @param [in] isFullUnload Whether to unload completely.
     * @param [in] transactionId Transaction ID.
     * @param [in] inputInfo Data information needed to unload model and plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int Release(bool isFullUnload, long long transactionId, const DataInfo &inputInfo) = 0;

    /**
     * Set the configuration parameters of the plugin.
     *
     * @param [in] optionType The type of setting option.
     * @param [in] inputInfo Configuration parameter needed to set up the plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int SetOption(int optionType, const DataInfo &inputInfo) = 0;

    /**
     * Get the configuration parameters of plugin.
     *
     * @param [in] optionType The type of getting option.
     * @param [in] inputInfo Parameter information for getting options.
     * @param [out] outputInfo The configuration information of plugin.
     * @return Returns 0 if the operation is successful, returns a non-zero value otherwise.
     */
    virtual int GetOption(int optionType, const DataInfo &inputInfo, DataInfo &outputInfo) = 0;
};

typedef IPlugin *(*IPLUGIN_INTERFACE)();
} // namespace AI
} // namespace OHOS

#endif // I_PLUGIN_H