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

#ifndef I_SEMAPHORE_H
#define I_SEMAPHORE_H

#include "utils/aie_macros.h"
#include "utils/constants/constants.h"

#include <memory>

namespace OHOS {
namespace AI {
class ISemaphore {
    FORBID_COPY_AND_ASSIGN(ISemaphore);
    FORBID_CREATE_BY_SELF(ISemaphore);
public:

    /**
     * create method
     *
     * @param count The init num of signal.
     */
    static std::shared_ptr<ISemaphore> MakeShared(unsigned int count);

    /**
     * wait method
     *
     * @param milliSeconds The response time(ms).
     * @return success or fail
    */
    bool Wait(const int milliSeconds = -1);

    /**
     *  wake up a waiting signal
     */
    void Signal();
};
} // namespace AI
} // namespace OHOS

#endif // I_SEMAPHORE_H
