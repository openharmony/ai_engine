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

#ifndef ENCDEC_FACADE_H
#define ENCDEC_FACADE_H

#include "utils/encdec/include/data_encoder.h"
#include "utils/encdec/include/data_decoder.h"

namespace OHOS {
namespace AI {
/**
 * Facade pattern for DataEncoder and DataDecoder.
 */
class EncdecFacade {
public:
/**
 * @brief encode arbitrary number args.
 *
 * Please note,
 *
 * 1. the memory in dataInfo should be released manually by the caller,
 *    recommend to use guard macro immediately after ProcessEncode.
 *    e.g.
 *         MallocPointerGuard<unsigned char> dataInfoGuard(dataInfo.data);
 *
 * 2. the allowed input args contains:
 *      - basic data types(int, double, boolean etc.), fix length arrays and structs containing them only.
 *      - std::string
 *
 *    otherwise you should implement your own version of encoding/decoding
 *    for pointers or struct containing pointers.
 *    Specifically, you should instantiate template function (@link DataEncoder::EncodeOneParameter)
 *    and (@link DataDecoder::DecodeOneParameter) for your own data type.
 *
 * 3. class and container are not supported or tested.
 *
 * @param [in] arg argument need to be serialized. Receive any number of input.
 * @return Return 0 if encode successfully, returns a non-zero value otherwise.
 */
template<typename Type, typename... Types>
static int ProcessEncode(DataInfo &dataInfo, const Type &arg, const Types &...args)
{
    DataEncoder dataEncoder;
    int retCode = dataEncoder.Init();
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[Encdec]Init encoder failed!");
        return retCode;
    }
    retCode = dataEncoder.RecursiveEncode(arg, args...);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[Encdec]Serialize failed.");
        return retCode;
    }
    retCode = dataEncoder.GetSerializedData(dataInfo);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[Encdec]Memory error when GetSerializedData.");
        return retCode;
    }

    return RETCODE_SUCCESS;
}

/**
 * @brief decode arbitrary number args.
 *
 * Please note,
 * 1. the encode and decode order should be matched strictly.
 *
 * @param [out] arg argument need to be unserialized. Receive any number of input.
 * @return Return 0 if decode successfully, returns a non-zero value otherwise.
 */
template<typename Type, typename... Types>
static int ProcessDecode(const DataInfo &dataInfo, Type &arg, Types &...args)
{
    if (dataInfo.data == nullptr || dataInfo.length <= 0) {
        return RETCODE_FAILURE;
    }

    DataDecoder dataDecoder(dataInfo);
    int retCode = dataDecoder.RecursiveDecode(arg, args...);
    if (retCode != RETCODE_SUCCESS) {
        HILOGE("[Encdec]Unserialize Argument failed.");
        return RETCODE_FAILURE;
    }

    if (!dataDecoder.CheckDataEnd()) {
        HILOGE("[Encdec]The size of decode data does not equal to the original one.");
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}
};
} // namespace AI
} // namespace OHOS

#endif // ENCDEC_FACADE_H
