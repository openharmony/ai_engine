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

#include "plugin_manager/include/aie_plugin_info.h"

namespace OHOS {
namespace AI {
const std::string GetAlgorithmIdByType(int type)
{
    int algorithmTypeNum = static_cast<int>(ALGORITHM_TYPE_ID_LIST.size());
    CHK_RET(type >= algorithmTypeNum || type < 0, ALGORITHM_ID_INVALID);
    return ALGORITHM_TYPE_ID_LIST[type];
}
} // namespace AI
} // namespace OHOS