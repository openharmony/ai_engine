cmake_minimum_required(VERSION 3.16)
project(engine)

set(CMAKE_CXX_STANDARD 14)

include_directories(../../../../../base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog)
include_directories(../../../../../foundation/ai/engine/interfaces)
include_directories(../../../../../foundation/ai/engine/interfaces/kits)
include_directories(../../../../../foundation/ai/engine/interfaces/kits/asr/keyword_spotting)
include_directories(../../../../../foundation/ai/engine/interfaces/kits/cv/image_classification)
include_directories(../../../../../foundation/ai/engine/services/client)
include_directories(../../../../../foundation/ai/engine/services/client/algorithm_sdk/asr/keyword_spotting/include)
include_directories(../../../../../foundation/ai/engine/services/client/client_executor/include)
include_directories(../../../../../foundation/ai/engine/services/common)
include_directories(../../../../../foundation/ai/engine/services/common/platform/os_wrapper/feature/interfaces)
include_directories(../../../../../foundation/ai/engine/services/common/platform/os_wrapper/utils)
include_directories(../../../../../foundation/ai/engine/services/common/protocol/plugin_config)
include_directories(../../../../../foundation/ai/engine/services/common/protocol/retcode_inner)
include_directories(../../../../../foundation/ai/engine/services/common/protocol/struct_definition)
include_directories(../../../../../foundation/ai/engine/services/common/utils)
include_directories(../../../../../foundation/ai/engine/services/common/utils/constants)
include_directories(../../../../../foundation/ai/engine/services/common/utils/encdec/include)
include_directories(../../../../../foundation/ai/engine/services/common/utils/log)
include_directories(../../../../../foundation/systemabilitymgr/samgr_lite/interfaces/kits/registry)
include_directories(../../../../../foundation/systemabilitymgr/samgr_lite/interfaces/kits/samgr)
include_directories(../../../../../third_party/bounds_checking_function/include)
include_directories(../../../../../utils/native/lite/include)

add_executable(client
        algorithm_sdk/asr/keyword_spotting/include/kws_sdk_impl.h
        algorithm_sdk/asr/keyword_spotting/include/pcm_iterator.h
        algorithm_sdk/asr/keyword_spotting/source/kws_sdk.cpp
        algorithm_sdk/asr/keyword_spotting/source/kws_sdk_impl.cpp
        algorithm_sdk/asr/keyword_spotting/source/pcm_iterator.cpp
        algorithm_sdk/cv/image_classification/include/ic_sdk_impl.h
        algorithm_sdk/cv/image_classification/source/ic_sdk.cpp
        algorithm_sdk/cv/image_classification/source/ic_sdk_impl.cpp
        client_executor/include/async_handler.h
        client_executor/include/client_factory.h
        client_executor/include/i_aie_client.inl
        client_executor/include/i_client_cb.h
        client_executor/source/async_handler.cpp
        client_executor/source/client_factory.cpp
        communication_adapter/include/sa_async_handler.h
        communication_adapter/include/sa_client.h
        communication_adapter/include/sa_client_adapter.h
        communication_adapter/include/sa_client_proxy.h
        communication_adapter/source/sa_async_handler.cpp
        communication_adapter/source/sa_client.cpp
        communication_adapter/source/sa_client_adapter.cpp
        communication_adapter/source/sa_client_proxy.cpp
)
