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

#include "utils/encdec/include/data_encoder.h"

#include <climits>

#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
namespace {
constexpr size_t CHUNK_SIZE = 1024U * 15U;
constexpr unsigned BUFF_EXTENSION_FACTOR = 2U;

MemBlock *AllocateMemBlock(const size_t sz)
{
    size_t size = sz + sizeof(MemBlock);
    unsigned char *memBlock = nullptr;
    AIE_NEW(memBlock, unsigned char[size]);
    CHK_RET((memBlock == nullptr), nullptr);

    auto *block = reinterpret_cast<MemBlock*>(memBlock);
    block->blockSize = sz;
    return block;
}

void FreeMemBlock(MemBlock *block)
{
    auto *memBlock = reinterpret_cast<unsigned char*>(block);
    AIE_DELETE_ARRAY(memBlock);
}
}

DataEncoder::DataEncoder() : buffer_(nullptr), pos_(0), allocSuccess_(false)
{
}

DataEncoder::~DataEncoder()
{
    CHK_RET_NONE(!buffer_);
    FreeMemBlock(buffer_);
    buffer_ = nullptr;
}

int DataEncoder::Init(size_t sz)
{
    buffer_ = AllocateMemBlock(sz);
    CHK_RET(buffer_ == nullptr, RETCODE_OUT_OF_MEMORY);
    allocSuccess_ = true;
    return RETCODE_SUCCESS;
}

int DataEncoder::GetSerializedData(DataInfo &dataInfo)
{
    CHK_RET(EncodeOneParameter(pos_) != RETCODE_SUCCESS, RETCODE_FAILURE);
    CHK_RET(buffer_ == nullptr || pos_ == 0, RETCODE_FAILURE);
    if (pos_ > INT_MAX) { // pos_ is unsigned but data.length is signed, avoid implicit conversion.
        HILOGE("[Encdec]The encoded data length exceed DataInfo's capacity, whose max is INT_MAX.");
        return RETCODE_FAILURE;
    }
    dataInfo.length = pos_;
    dataInfo.data = (unsigned char*)malloc(dataInfo.length);
    if (dataInfo.data == nullptr) {
        return RETCODE_OUT_OF_MEMORY;
    }
    errno_t res = memcpy_s(dataInfo.data, dataInfo.length, buffer_->data, pos_);
    if (res != EOK) {
        free(dataInfo.data);
        dataInfo.data = nullptr;
        dataInfo.length = 0;
        return RETCODE_MEMORY_COPY_FAILURE;
    }
    FreeMemBlock(buffer_);
    buffer_ = nullptr;
    pos_ = 0;
    allocSuccess_ = false;

    return RETCODE_SUCCESS;
}

bool DataEncoder::Ensure(const size_t incSize)
{
    if (buffer_->blockSize >= pos_ + incSize) {
        return true;
    }
    return ReallocBuffer(buffer_->blockSize * BUFF_EXTENSION_FACTOR + incSize);
}

bool DataEncoder::ReallocBuffer(const size_t newSize)
{
    CHK_RET(!allocSuccess_, false);

    MemBlock *p = AllocateMemBlock(newSize);
    if (p == nullptr) {
        allocSuccess_ = false;
        return false;
    }
    unsigned char *dest = p->data;
    unsigned char *src = buffer_->data;
    size_t leftSize = pos_;
    for (; leftSize >= CHUNK_SIZE; leftSize -= CHUNK_SIZE) {
        errno_t err = memcpy_s(dest, CHUNK_SIZE, src, CHUNK_SIZE);
        if (err != EOK) {
            HILOGE("[Encdec]The memcpy_s error in the encdec process, err = %d.", err);
            allocSuccess_ = false;
            FreeMemBlock(p);
            return false;
        }
        dest += CHUNK_SIZE;
        src += CHUNK_SIZE;
    }
    if (leftSize > 0) {
        errno_t errLeft = memcpy_s(dest, leftSize, src, leftSize);
        if (errLeft != EOK) {
            HILOGE("[Encdec]The memcpy_s error in the encdec process, err = %d.", errLeft);
            allocSuccess_ = false;
            FreeMemBlock(p);
            return false;
        }
    }
    FreeMemBlock(buffer_);
    buffer_ = p;
    return true;
}

template<>
int DataEncoder::EncodeOneParameter(const std::string &val)
{
    if (!allocSuccess_) {
        return RETCODE_FAILURE;
    }

    if (val.length() == 0) { // 0 length string, save length only.
        return EncodeOneParameter(val.length());
    }
    if (EncodeOneParameter(val.length()) != RETCODE_SUCCESS) {
        return RETCODE_FAILURE;
    }
    if (!Ensure(val.length())) {
        HILOGE("[Encdec]ReallocBuffer failed.");
        return RETCODE_FAILURE;
    }

    // assign value without memory alignment cause crash
    // so use memcpy_s to make sure success.
    if (memcpy_s(buffer_->data + pos_, val.length(), val.c_str(), val.length()) != EOK) {
        HILOGE("[Encdec]memcpy_s failed.");
        return RETCODE_FAILURE;
    }
    pos_ += val.length();
    return RETCODE_SUCCESS;
}
} // namespace AI
} // namespace OHOS
