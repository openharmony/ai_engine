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

#ifndef SINGLE_INSTANCE_H
#define SINGLE_INSTANCE_H

#include "aie_macros.h"

namespace OHOS {
namespace AI {
#define DECLARE_SINGLE_INSTANCE_BASE(className)       \
public:                                               \
    static className& GetInstance();                  \
FORBID_COPY_AND_ASSIGN(className);

#define DECLARE_SINGLE_INSTANCE(className)            \
    DECLARE_SINGLE_INSTANCE_BASE(className)           \
private:                                              \
FORBID_CREATE_BY_SELF(className);

#define IMPLEMENT_SINGLE_INSTANCE(className)          \
className& className::GetInstance()                   \
{                                                     \
    static className instance;                        \
    return instance;                                  \
}
} // namespace AI
} // namespace OHOS
#endif // SINGLE_INSTANCE_H
