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

#include "utils/encdec/include/data_decoder.h"

namespace OHOS {
namespace AI {
template<>
int DataDecoder::DecodeOneParameter(std::string &val)
{
    size_t len = 0;
    if (DecodeOneParameter(len) != RETCODE_SUCCESS) {
        return RETCODE_FAILURE;
    }
    if (len == 0) {
        val = "";
        return RETCODE_SUCCESS;
    }
    if (!Ensure(len)) {
        HILOGE("[Encdec]Memory read out of boundary");
        return RETCODE_FAILURE;
    }
    val.assign(buffer_ + pos_, buffer_ + pos_ + len);

    pos_ += len;
    return RETCODE_SUCCESS;
}
} // namespace AI
} // namespace OHOS
