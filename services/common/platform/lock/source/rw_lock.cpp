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

#include "platform/lock/include/rw_lock.h"

namespace OHOS {
namespace AI {
RwLock::RwLock()
    : readCnt_(0), writeCnt_(0), writeFlag_(false)
{
}

RwLock::~RwLock() = default;

void RwLock::LockRead()
{
    std::unique_lock<std::mutex> guard(mutex_);
    condRead_.wait(guard, [=]()->bool { return writeCnt_ == 0; });
    ++readCnt_;
}

void RwLock::LockWrite()
{
    std::unique_lock<std::mutex> guard(mutex_);
    ++writeCnt_;
    condWrite_.wait(guard, [=]()->bool { return (readCnt_ == 0) && (!writeFlag_); });
    writeFlag_ = true;
}

void RwLock::UnLockRead()
{
    std::unique_lock<std::mutex> guard(mutex_);
    if ((--readCnt_ == 0) && (writeCnt_ > 0)) {
        condWrite_.notify_one();
    }
}

void RwLock::UnLockWrite()
{
    std::unique_lock<std::mutex> guard(mutex_);
    if (--writeCnt_ == 0) {
        condRead_.notify_all();
    } else {
        condWrite_.notify_one();
    }

    writeFlag_ = false;
}
} // namespace AI
} // namespace OHOS
