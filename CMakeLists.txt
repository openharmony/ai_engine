cmake_minimum_required(VERSION 3.16)
project(engine)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(services/server)
add_subdirectory(services/client)
add_subdirectory(services/common)
add_subdirectory(test)

add_executable(engine
    interfaces/kits/asr/keyword_spotting/kws_callback.h
    interfaces/kits/asr/keyword_spotting/kws_constants.h
    interfaces/kits/asr/keyword_spotting/kws_retcode.h
    interfaces/kits/asr/keyword_spotting/kws_sdk.h
    interfaces/kits/cv/image_classification/ic_callback.h
    interfaces/kits/cv/image_classification/ic_constants.h
    interfaces/kits/cv/image_classification/ic_retcode.h
    interfaces/kits/cv/image_classification/ic_sdk.h
    interfaces/kits/ai_datatype.h
    interfaces/kits/ai_retcode.h
)

set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)
