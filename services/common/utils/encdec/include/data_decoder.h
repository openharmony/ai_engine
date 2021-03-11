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

#ifndef DATA_DECODER_H
#define DATA_DECODER_H

#include <string>

#include "securec.h"

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "protocol/struct_definition/aie_info_define.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
class DataDecoder {
public:
    explicit DataDecoder(const DataInfo dataInfo) : buffer_(dataInfo.data), size_(dataInfo.length), pos_(0)
    {
    }

    ~DataDecoder() = default;

    /**
     * Delete copy constructor/assignment to avoid misuse.
     */
    DataDecoder(const DataDecoder &other) = delete;
    DataDecoder& operator=(const DataDecoder &other) = delete;

    /**
     * Decode all arguments in the DataInfo.
     *
     * Please note,
     * 1. the arguments' order in decoder and encoder process should match strictly.
     * 2. if decoding a pointer, the caller should allocate the memory first and handle the release.
     *
     * @param [out] arg argument need to be unserialized. Receive any number of arguments.
     * @return Return 0 if decode successfully, returns a non-zero value otherwise.
     */
    template<typename Type, typename... Types>
    int RecursiveDecode(Type &arg, Types &...args)
    {
        int retCode = DecodeOneParameter<Type>(arg);
        if (retCode != RETCODE_SUCCESS) {
            return retCode;
        }
        return RecursiveDecode(args...);
    }

    /**
     * Check whether the decode data size matches the original data when decoding ends.
     * It should be called after all data have been decoded.
     *
     * @return Returns true if decode data size matches, return false if decode data length fails to match.
     */
    bool CheckDataEnd()
    {
        size_t dataSize = 0;
        if (DecodeOneParameter(dataSize) != RETCODE_SUCCESS) {
            return false;
        }
        return (pos_ - sizeof(size_t)) == dataSize; // memory length should match dataSize.
    }

private:
    /**
     * Decode the data buffer based on the data type that was transferred in.
     * instantiate it for your own data type if needed.
     *
     * @param [out] val It provides a data type, the function will decode by it.
     * @return Returns 0 if decode successful, otherwise it failed.
     */
    template<typename T>
    int DecodeOneParameter(T &val)
    {
        // sizeof(T) is 0 when on gcc when T is defined as 0 length array.
        // but it may not apply to other compilers.
        // try a better type definition to avoid ambiguity.
        if (sizeof(T) == 0) {
            HILOGE("[Encdec]sizeof(T) is 0.");
            return RETCODE_FAILURE;
        }
        if (!Ensure(sizeof(T))) {
            HILOGE("[Encdec]Memory read out of boundary");
            return RETCODE_FAILURE;
        }

        // assign value without memory alignment cause crash
        // so use memcpy_s to make sure success.
        if (memcpy_s(&val, sizeof(val), buffer_ + pos_, sizeof(val)) != EOK) {
            HILOGE("[Encdec]memcpy_s failed");
            return RETCODE_FAILURE;
        }
        pos_ += sizeof(T);
        return RETCODE_SUCCESS;
    }

    /**
     * To make sure that passed in data type will not exceed the original data buffer
     *
     * @return Return 1 if the size of data buffer is enough for decoding, otherwise it returns 0.
     */
    inline bool Ensure(const size_t size) const
    {
        return (size <= size_ && (pos_ + size) <= size_);
    }

    /**
     * it offers a terminal call for RecursiveEncode.
     *
     * @return Returns 0.
     */
    int RecursiveDecode()
    {
        return RETCODE_SUCCESS;
    }

private:
    unsigned char *buffer_ {nullptr};
    size_t size_ {0};
    size_t pos_ {0};
};

/**
 * Encode std::string. It's a instantiation of template function (@link DataEncoder::EncodeOneParameter).
 *
 * @param [out] val The data that needs to be encoded.
 * @return Returns 0 if the data has been written into the buffer successfully, otherwise, it will return -1.
 */
template<>
int DataDecoder::DecodeOneParameter(std::string &val);
} // namespace AI
} // namespace OHOS

#endif // DATA_DECODER_H
