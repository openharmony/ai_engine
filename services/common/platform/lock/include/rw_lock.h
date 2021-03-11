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

#ifndef RW_LOCK_H
#define RW_LOCK_H

#include <condition_variable>
#include <mutex>

#include "utils/aie_macros.h"
#include "utils/constants/constants.h"

namespace OHOS {
namespace AI {
class RwLock {
    FORBID_COPY_AND_ASSIGN(RwLock);
public:
    RwLock();
    ~RwLock();

public:
    /**
     * Lock the reading operation.
     */
    void LockRead();

    /**
     * Lock the writing operation.
     */
    void LockWrite();

    /**
     * Unlock the reading operation.
     */
    void UnLockRead();

    /**
     * Unlock the writing operation.
     */
    void UnLockWrite();

private:
    // Count of reading operations.
    volatile size_t readCnt_;

    // Count of writing operations.
    volatile size_t writeCnt_;

    // The flag of writing operations.
    volatile bool writeFlag_;

    std::mutex mutex_;
    std::condition_variable condWrite_;
    std::condition_variable condRead_;
};

template<typename RwLockable>
class WriteGuard {
    FORBID_COPY_AND_ASSIGN(WriteGuard);
public:
    /**
     * Lock to guard the writing operation.
     *
     * @param rwLockable the lock with {@code RwLock} base type.
     */
    explicit WriteGuard(RwLockable &rwLockable);

    /**
     * Unlock the guarding of writing operation.
     */
    ~WriteGuard();

private:
    RwLockable& rwLockable_;
};

template<typename RwLockable>
class ReadGuard {
    FORBID_COPY_AND_ASSIGN(ReadGuard);
public:
    /**
     * Lock to guard the reading operation.
     *
     * @param rwLockable the lock with {@code RwLock} base type.
     */
    explicit ReadGuard(RwLockable &rwLockable);

    /**
     * Unlock the guarding of reading operation.
     */
    ~ReadGuard();

private:
    RwLockable& rwLockable_;
};
} // namespace AI
} // namespace OHOS

// Include the implementation of reader-writer lock.
#include "platform/lock/include/rw_lock.inl"

#endif // RW_LOCK_H