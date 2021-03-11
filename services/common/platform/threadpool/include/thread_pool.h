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

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <list>
#include <mutex>

#include "platform/threadpool/include/thread.h"
#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
const int THREAD_STOP_TIMEOUT = 3600 * 24 * 1000;
const int THREAD_SLEEP_TIME = 10;

class ThreadPool {
    FORBID_COPY_AND_ASSIGN(ThreadPool);
    FORBID_CREATE_BY_SELF(ThreadPool);

    typedef std::list<std::shared_ptr<Thread>> Threads;
public:
    static ThreadPool *GetInstance();

    static void ReleaseInstance();

    /**
     * get a Thread from thread pool
     *
     * @return thread
     */
    std::shared_ptr<Thread> Pop();

    /**
     * push a to thread to thread pool
     */
    void Push(std::shared_ptr<Thread>& thread);

    /**
     * attention must set before start thread
     *
     * @param stack size: 16384B ~ 8192KB,
     *  SetStackSize(0) will set the default stack size
     */
    void SetStackSize(size_t size);

    int getStackSize();

private:
    bool StopThreads(int32_t timeOut);
    void StopThreads();

private:
    static std::mutex mutex_;
    static ThreadPool *instance_;
    size_t stackSize_;
    std::mutex mutex4Inner_;
    Threads busyThreads_;
    Threads idleThreads_;
};
} // namespace AI
} // namespace OHOS

#endif // THREAD_POOL_H