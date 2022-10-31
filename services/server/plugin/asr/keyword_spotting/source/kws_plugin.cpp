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

#include "kws_plugin.h"

#include "aie_log.h"
#include "aie_retcode_inner.h"
#include "encdec_facade.h"
#include "norm_processor.h"
#include "plugin_helper.h"
#include "slide_window_processor.h"
#include "type_converter.h"

#ifdef USE_NNIE
#include "nnie_adapter.h"
#endif

using namespace OHOS::AI;
using namespace OHOS::AI::Feature;
namespace {
    const std::string PLUGIN_MODEL_PATH = "/storage/data/keyword_spotting.wk";
    const std::string DEFAULT_INFER_MODE = "SYNC";
    const std::string ALGORITHM_NAME_KWS = "KWS";
    const int32_t OPTION_GET_INPUT_SIZE = 1001;
    const int32_t OPTION_GET_OUTPUT_SIZE = 1002;
    const intptr_t EMPTY_UINTPTR = 0;
    const int32_t MODEL_INPUT_NODE_ID = 0;
    const int32_t MODEL_OUTPUT_NODE_ID = 0;
}

static int32_t InitWorkplace(KWSWorkplace &worker, SlideWindowProcessorConfig &slideCfg,
    TypeConverterConfig &convertCfg, NormProcessorConfig &normCfg)
{
    worker.slideProcessor = std::make_shared<SlideWindowProcessor>();
    worker.typeConverter = std::make_shared<TypeConverter>();
    worker.normProcessor = std::make_shared<NormProcessor>();
    if (worker.slideProcessor == nullptr ||
        worker.typeConverter == nullptr ||
        worker.normProcessor == nullptr) {
        HILOGE("[KWSPlugin]Fail to allocate workplaces");
        return RETCODE_FAILURE;
    }
    if (worker.slideProcessor->Init(&slideCfg) != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]Fail to init slideProcessor");
        return RETCODE_FAILURE;
    }
    if (worker.typeConverter->Init(&convertCfg) != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]Fail to init typeConverter");
        return RETCODE_FAILURE;
    }
    if (worker.normProcessor->Init(&normCfg) != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]Fail to init normConfig");
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

KWSPlugin::KWSPlugin()
{
    HILOGD("[KWSPlugin]ctor");
    handles_.clear();
}

KWSPlugin::~KWSPlugin()
{
    ReleaseAllHandles();
    HILOGD("[KWSPlugin]dtor");
}

void KWSPlugin::ReleaseAllHandles()
{
    for (auto iter = handles_.begin(); iter != handles_.end(); ++iter) {
        (void)adapter_->ReleaseHandle(iter->first);
    }
    adapter_->Deinit();
    handles_.clear();
}

int32_t KWSPlugin::Prepare(long long transactionId, const DataInfo &inputInfo, DataInfo &outputInfo)
{
    HILOGI("[KWSPlugin]Begin to prepare, transactionId = %lld", transactionId);
    std::lock_guard<std::mutex> lock(mutex_);
    if (adapter_ ==  nullptr) {
#ifdef USE_NNIE
        adapter_ = std::make_shared<NNIEAdapter>();
#endif
        if (adapter_ == nullptr) {
            HILOGE("[KWSPlugin]Fail to create engine adapter");
            return RETCODE_FAILURE;
        }
    }
    intptr_t handle = 0;
    if (adapter_->Init(PLUGIN_MODEL_PATH.c_str(), handle) != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]NNIEAdapterInit failed");
        return RETCODE_FAILURE;
    }
    const auto iter = handles_.find(handle);
    if (iter != handles_.end()) {
        HILOGE("[KWSPlugin]handle=%lld has already existed", (long long)handle);
        return RETCODE_SUCCESS;
    }
    PluginConfig config;
    if (BuildConfig(handle, config) != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]BuildConfig failed");
        return RETCODE_FAILURE;
    }
    KWSWorkplace worker = {
        .config = config,
        .normProcessor = nullptr,
        .typeConverter = nullptr,
        .slideProcessor = nullptr
    };
    if (InitComponents(worker) != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]InitComponents failed");
        return RETCODE_FAILURE;
    }
    handles_.emplace(handle, worker);
    return EncdecFacade::ProcessEncode(outputInfo, handle);
}

const long long KWSPlugin::GetVersion() const
{
    return ALGOTYPE_VERSION_KWS;
}

const char *KWSPlugin::GetName() const
{
    return ALGORITHM_NAME_KWS.c_str();
}

const char *KWSPlugin::GetInferMode() const
{
    return DEFAULT_INFER_MODE.c_str();
}

int32_t KWSPlugin::SyncProcess(IRequest *request, IResponse *&response)
{
    HILOGI("[KWSPlugin]SyncProcess start");
    std::lock_guard<std::mutex> lock(mutex_);
    if (request == nullptr) {
        HILOGE("[KWSPlugin]SyncProcess request is nullptr");
        return RETCODE_NULL_PARAM;
    }
    DataInfo inputInfo = request->GetMsg();
    if (inputInfo.data == nullptr || inputInfo.length <= 0) {
        HILOGE("[KWSPlugin]SyncProcess inputInfo data is nullptr");
        return RETCODE_NULL_PARAM;
    }
    intptr_t handle = 0;
    Array<uint16_t> audioInput = {
        .data = nullptr,
        .size = 0
    };
    int32_t ret = EncdecFacade::ProcessDecode(inputInfo, handle, audioInput);
    if (ret != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]SyncProcess load inputData failed");
        return RETCODE_FAILURE;
    }
    const auto iter = handles_.find(handle);
    if (iter == handles_.end()) {
        HILOGE("[KWSPlugin]SyncProcess no matched handle [%lld]", (long long)handle);
        return RETCODE_NULL_PARAM;
    }
    Array<int32_t> processorOutput = {
        .data = nullptr,
        .size = 0
    };
    ret = GetNormedFeatures(audioInput, processorOutput, iter->second);
    if (ret != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]Fail to get normed features");
        return RETCODE_FAILURE;
    }
    DataInfo outputInfo = {
        .data = nullptr,
        .length = 0
    };
    ret = MakeInference(handle, processorOutput, iter->second.config, outputInfo);
    if (ret != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]SyncProcess MakeInference failed");
        return RETCODE_FAILURE;
    }
    response = IResponse::Create(request);
    response->SetResult(outputInfo);
    return RETCODE_SUCCESS;
}

int32_t KWSPlugin::GetNormedFeatures(const Array<uint16_t> &input, Array<int32_t> &output, const KWSWorkplace &worker)
{
    FeatureData inputData = {
        .dataType = UINT16,
        .data = static_cast<void *>(input.data),
        .size = input.size
    };
    FeatureData normedOutput = {
        .dataType = FLOAT,
        .data = nullptr,
        .size = 0
    };
    int32_t retCode = worker.normProcessor->Process(inputData, normedOutput);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]Fail to get nomred output via normProcessor");
        return RETCODE_FAILURE;
    }
    FeatureData convertedOutput = {
        .dataType = INT32,
        .data = nullptr,
        .size = 0
    };
    retCode = worker.typeConverter->Process(normedOutput, convertedOutput);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]Fail to convert normed output via typeConverter");
        return RETCODE_FAILURE;
    }
    FeatureData slideOutput = {
        .dataType = INT32,
        .data = nullptr,
        .size = 0
    };
    retCode = worker.slideProcessor->Process(convertedOutput, slideOutput);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]Fail to get slided output via slideProcessor");
        return RETCODE_FAILURE;
    }
    output.data = static_cast<int32_t *>(slideOutput.data);
    output.size = slideOutput.size;
    return RETCODE_SUCCESS;
}

int32_t KWSPlugin::AsyncProcess(IRequest *request, IPluginCallback *callback)
{
    return RETCODE_SUCCESS;
}

int32_t KWSPlugin::SetOption(int optionType, const DataInfo &inputInfo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (inputInfo.data == nullptr) {
        HILOGE("[KWSPlugin]SetOption inputInfo data is [NULL]");
        return RETCODE_FAILURE;
    }
    int retCode = RETCODE_SUCCESS;
    switch (optionType) {
        default:
            HILOGE("[KWSPlugin]SetOption optionType[%d] undefined", optionType);
            break;
    }
    return retCode;
}

int32_t KWSPlugin::GetOption(int optionType, const DataInfo &inputInfo, DataInfo &outputInfo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (inputInfo.data == nullptr || inputInfo.length <= 0) {
        HILOGE("[KWSPlugin]GetOption failed for empty inputInfo");
        return RETCODE_FAILURE;
    }
    intptr_t handle = 0;
    int32_t ret = EncdecFacade::ProcessDecode(inputInfo, handle);
    if (ret != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]GetOption get handle from inputInfo failed");
        return RETCODE_FAILURE;
    }
    const auto &iter = handles_.find(handle);
    if (iter == handles_.end()) {
        HILOGE("[KWSPlugin]GetOption no matched handle [%lld]", (long long)handle);
        return RETCODE_FAILURE;
    }
    outputInfo.length = 0;
    switch (optionType) {
        case OPTION_GET_INPUT_SIZE:
            return EncdecFacade::ProcessEncode(outputInfo, handle, iter->second.config.inputSize);
        case OPTION_GET_OUTPUT_SIZE:
            return EncdecFacade::ProcessEncode(outputInfo, handle, iter->second.config.outputSize);
        default:
            HILOGE("[KWSPlugin]GetOption optionType[%d] undefined", optionType);
            return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int32_t KWSPlugin::Release(bool isFullUnload, long long transactionId, const DataInfo &inputInfo)
{
    if (adapter_ == nullptr) {
        HILOGE("[KWSPlugin]The engine adapter has not been created");
        return RETCODE_FAILURE;
    }
    HILOGI("[KWSPlugin]Begin to release, transactionId = %lld", transactionId);
    intptr_t handle = 0;
    int32_t ret = EncdecFacade::ProcessDecode(inputInfo, handle);
    if (ret != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]UnSerializeHandle Failed");
        return RETCODE_FAILURE;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    ret = adapter_->ReleaseHandle(handle);
    if (ret != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]ReleaseHandle failed");
        return RETCODE_FAILURE;
    }
    FreeHandle(handle);
    if (isFullUnload) {
        ret = adapter_->Deinit();
        if (ret != RETCODE_SUCCESS) {
            HILOGE("[KWSPlugin]Engine adapter deinit failed");
            return RETCODE_FAILURE;
        }
    }
    return RETCODE_SUCCESS;
}

void KWSPlugin::FreeHandle(intptr_t handle)
{
    const auto iter = handles_.find(handle);
    if (iter != handles_.end()) {
        (void)handles_.erase(iter);
    }
}

int32_t KWSPlugin::InitComponents(KWSWorkplace &worker)
{
    SlideWindowProcessorConfig slideWindowConfig;
    slideWindowConfig.dataType = INT32;
    slideWindowConfig.stepSize = DEFAULT_SLIDE_STEP_SIZE;
    slideWindowConfig.windowSize = DEFAULT_SLIDE_WINDOW_SIZE;
    TypeConverterConfig convertConfig;
    convertConfig.dataType = INT32;
    convertConfig.size = DEFAULT_SLIDE_STEP_SIZE;
    NormProcessorConfig normConfig;
    normConfig.meanFilePath = DEFAULT_NORM_MEAN_FILE_PATH;
    normConfig.stdFilePath = DEFAULT_NORM_STD_FILE_PATH;
    normConfig.numChannels = DEFAULT_NORM_NUM_CHANNELS;
    normConfig.inputSize = DEFAULT_NORM_INPUT_SIZE;
    normConfig.scale = DEFAULT_NORM_SCALE;
    if (InitWorkplace(worker, slideWindowConfig, convertConfig, normConfig) != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]Fail to init workplace");
        worker.slideProcessor = nullptr;
        worker.typeConverter = nullptr;
        worker.normProcessor = nullptr;
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int32_t KWSPlugin::BuildConfig(intptr_t handle, PluginConfig &config)
{
    if (adapter_ == nullptr) {
        return RETCODE_NULL_PARAM;
    }
    int32_t retcode = adapter_->GetInputAddr(handle, MODEL_INPUT_NODE_ID, config.inputAddr, config.inputSize);
    if (retcode != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]NNIEAdapter GetInputAddr failed with [%d]", retcode);
        return RETCODE_NULL_PARAM;
    }
    retcode = adapter_->GetOutputAddr(handle, MODEL_OUTPUT_NODE_ID, config.outputAddr, config.outputSize);
    if (retcode != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]NNIEAdapter GetOutputAddr failed with [%d]", retcode);
        return RETCODE_NULL_PARAM;
    }
    return RETCODE_SUCCESS;
}

int32_t KWSPlugin::MakeInference(intptr_t handle, Array<int32_t> &input, PluginConfig &config, DataInfo &outputInfo)
{
    HILOGI("[KWSPlugin]start with handle = %lld", (long long)handle);
    if (adapter_ == nullptr || config.inputAddr == EMPTY_UINTPTR || config.outputAddr == EMPTY_UINTPTR) {
        HILOGE("[KWSPlugin]MakeInference inference engine is not ready");
        return RETCODE_NULL_PARAM;
    }
    if (input.data == nullptr || input.size != config.inputSize) {
        HILOGE("[KWSPlugin]The input size is not equal to the size of model input");
        return RETCODE_FAILURE;
    }
    int32_t *inputData = reinterpret_cast<int32_t *>(config.inputAddr);
    size_t bufferSize = config.inputSize * sizeof(input.data[0]);
    errno_t retCode = memcpy_s(inputData, bufferSize, input.data, bufferSize);
    if (retCode != EOK) {
        HILOGE("[KWSPlugin]MakeInference memory copy failed");
        return RETCODE_NULL_PARAM;
    }
    int32_t ret = adapter_->Invoke(handle);
    if (ret != RETCODE_SUCCESS) {
        HILOGE("[KWSPlugin]MakeInference failed");
        return RETCODE_FAILURE;
    }
    Array<int32_t> result;
    result.size = config.outputSize;
    result.data = reinterpret_cast<int32_t *>(config.outputAddr);
    return EncdecFacade::ProcessEncode(outputInfo, handle, result);
}

PLUGIN_INTERFACE_IMPL(KWSPlugin);