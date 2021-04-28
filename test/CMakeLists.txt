cmake_minimum_required(VERSION 3.16)
project(engine)

set(CMAKE_CXX_STANDARD 14)

include_directories(../../../../base/hiviewdfx/hilog_lite/interfaces/native/kits/hilog)
include_directories(../../../../foundation/ai/engine/interfaces)
include_directories(../../../../foundation/ai/engine/services/client)
include_directories(../../../../foundation/ai/engine/services/common)
include_directories(../../../../foundation/ai/engine/services/server)
include_directories(../../../../foundation/ai/engine/test)
include_directories(../../../../foundation/ai/engine/test/common/dl_operation/dl_operation_so/include)
include_directories(../../../../foundation/ai/engine/test/performance)
include_directories(../../../../foundation/ai/engine/test/utils)
include_directories(../../../../foundation/communication/ipc_lite/frameworks/liteipc/include)
include_directories(../../../../foundation/communication/ipc_lite/interfaces/kits)
include_directories(../../../../foundation/distributedschedule/samgr_lite/interfaces/innerkits/registry)
include_directories(../../../../foundation/distributedschedule/samgr_lite/interfaces/innerkits/samgr)
include_directories(../../../../foundation/distributedschedule/samgr_lite/interfaces/kits/registry)
include_directories(../../../../foundation/distributedschedule/samgr_lite/interfaces/kits/samgr)
include_directories(../../../../third_party/bounds_checking_function/include)
include_directories(../../../../third_party/googletest/googletest/include)
include_directories(../../../../third_party/googletest/googletest/src)
include_directories(../../../../utils/native/lite/include)

add_executable(test
        common/dl_operation/dl_operation_so/include/dl_operation_fun.h
        common/dl_operation/dl_operation_so/source/dl_operation_fun.cpp
        common/dl_operation/dl_operation_test.cpp
        common/encdec/encdec_test.cpp
        common/event/event_test.cpp
        common/queuepool/queuepool_test.cpp
        common/semaphore/semaphore_test.cpp
        common/threadpool/thread_pool_test.cpp
        common/time/time_test.cpp
        function/async_process/async_process_function_test.cpp
        function/death_callback/death_callback_test.cpp
        function/destroy/destroy_function_test.cpp
        function/init/init_function_test.cpp
        function/plugin_manager/plugin_manager_test.cpp
        function/prepare/prepare_function_test.cpp
        function/release/release_function_test.cpp
        function/set_get_option/option_function_test.cpp
        function/share_memory/share_memory_test.cpp
        function/sync_process/sync_process_function_test.cpp
        performance/delay/async_process/async_process_delay_test.cpp
        performance/delay/sync_process/sync_process_delay_test.cpp
        performance/reliability/aie_client/aie_client_reliability_test.cpp
        sample/include/sample_plugin_1.h
        sample/include/sample_plugin_2.h
        sample/source/sample_plugin_1.cpp
        sample/source/sample_plugin_2.cpp
        utils/client_callback.h
        utils/service_dead_cb.h
)
