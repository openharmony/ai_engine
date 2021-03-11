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

#ifndef DATA_ENCODER_H
#define DATA_ENCODER_H

#include <string>

#include "securec.h"

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
struct MemBlock {
    size_t blockSize;
    unsigned char data[0];
};

class DataEncoder {
public:
    DataEncoder();
    ~DataEncoder();

    /**
     * Delete copy constructor/assignment to avoid misuse.
     */
    DataEncoder(const DataEncoder &other) = delete;
    DataEncoder& operator=(const DataEncoder &other) = delete;

    /**
     * Initializes memory for encoding data.
     *
     * @param [in] sz The size of the data which needs to be encoded.
     * @return Return 0 if initialize successfully, returns a non-zero value otherwise.
     */
    int Init(size_t sz = INIT_BUFFER_SIZE);

    /**
     * Encodes arguments.
     *
     * Please note,
     * 1. the allowed input args contains:
     *      - basic data types(int, double, boolean etc.), fix length arrays and structs containing them only.
     *      - std::string
     *
     *    otherwise you should implement your own version of encoding/decoding
     *    for pointers / struct containing pointers / struct containing std:string.
     *    Specifically, you should instantiate template function (@link DataEncoder::EncodeOneParameter)
     *    and (@link DataDecoder::DecodeOneParameter).
     *
     * 2.  class and container are not supported or tested.
     *
     * @param [in] arg argument need to be serialized. Receive any number of input.
     * @return Return 0 if encode successfully, returns a non-zero value otherwise.
     */
    template<typename Type, typename... Types>
    int RecursiveEncode(const Type &arg, const Types &...args)
    {
        int retCode = EncodeOneParameter<Type>(arg);
        if (retCode != RETCODE_SUCCESS) {
            return retCode;
        }
        return RecursiveEncode(args...);
    }

    /**
     * Returns serialized data.
     *
     * Please note,
     * 1. the memory in dataInfo should be released manually by the caller,
     *    recommend to use guard macro immediately after ProcessEncode.
     *    e.g.
     *         MallocPointerGuard<unsigned char> dataInfoGuard(dataInfo.data);
     *
     * @param [out] dataInfo The serialized data.
     * @return Return 0 if get data successfully, returns a non-zero value otherwise.
     */
    int GetSerializedData(DataInfo &dataInfo);

private:
    /**
     * Ensure left memory length is bigger than incSize, otherwise expand it.
     *
     * @param [in] incSize Size to check.
     * @return Return false if remaining memory is less than incSize, returns true otherwise.
     */
    bool Ensure(const size_t incSize);

    /**
     * Write the data into the buffer. if the data size exceeds the buffer size, it will extend the buffer first.
     * instantiate it for your own data type if needed.
     *
     * @param [in] val The data that needs to be encoded.
     * @return Returns 0 if the data has been written into the buffer successfully, otherwise, it will return -1.
     */
    template<typename T>
    int EncodeOneParameter(const T &val)
    {
        if (!allocSuccess_) {
            return RETCODE_FAILURE;
        }

        size_t len = sizeof(T);
        // on gcc, sizeof(T) is 0 when on gcc when T is defined as 0 length array.
        // but it may not apply to other compilers.
        // try a better type definition to avoid ambiguity.
        if (len == 0) {
            HILOGE("[Encdec]sizeof(T) is 0.");
            return RETCODE_FAILURE;
        }
        if (!Ensure(len)) {
            HILOGE("[Encdec]ReallocBuffer failed.");
            return RETCODE_FAILURE;
        }

        // assign value without memory alignment cause crash
        // so use memcpy_s to make sure success.
        if (memcpy_s(buffer_->data + pos_, len, &val, sizeof(val)) != EOK) {
            HILOGE("[Encdec]memcpy_s failed.");
            return RETCODE_FAILURE;
        }
        pos_ += len;
        return RETCODE_SUCCESS;
    }

    bool ReallocBuffer(const size_t newSize);

    /**
     * it offers a terminal call for RecursiveEncode.
     *
     * @return Returns 0.
     */
    int RecursiveEncode()
    {
        return RETCODE_SUCCESS;
    }

private:
    static constexpr size_t INIT_BUFFER_SIZE = 256U;

    MemBlock *buffer_ {nullptr};
    size_t pos_ {0};
    bool allocSuccess_ {false};
};

/**
 * Decode std::string. It's a instantiation of template function (@link DataDecoder::DecodeOneParameter).
 *
 * @param [in] val The data that needs to be encoded.
 * @return Returns 0 if the data has been written into the buffer successfully, otherwise, it will return -1.
 */
template<>
int DataEncoder::EncodeOneParameter(const std::string &val);
} // namespace AI
} // namespace OHOS

#endif // DATA_ENCODER_H
