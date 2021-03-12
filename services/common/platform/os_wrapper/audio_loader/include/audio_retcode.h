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

#ifndef AUDIO_RETCODE_H
#define AUDIO_RETCODE_H

#include <cstdint>

namespace OHOS {
namespace AI {
enum AudioLoaderRetCode {
    AUDIO_SUCCESS = 0,
    AUDIO_INVALID_HANDLER = 1000,
    AUDIO_NO_KEY,
    AUDIO_ILLEGAL_TYPE,
    AUDIO_NULL_CODER,
    AUDIO_INIT_FAILURE,
    AUDIO_CONVERT_FAILURE,
    AUDIO_INVALID_OPERATION,
};
} // namespace AI
} // namespace OHOS
#endif // AUDIO_RETCODE_H