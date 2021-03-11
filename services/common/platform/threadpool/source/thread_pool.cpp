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

#include "platform/threadpool/include/thread_pool.h"

#include "platform/time/include/time.h"
#include "platform/time/include/time_elapser.h"

namespace OHOS {
namespace AI {
std::mutex ThreadPool::mutex_;
ThreadPool *ThreadPool::instance_ = nullptr;

ThreadPool *ThreadPool::GetInstance()
{
    CHK_RET(instance_ != nullptr, instance_);

    std::lock_guard<std::mutex> lock(mutex_);
    CHK_RET(instance_ != nullptr, instance_);

    AIE_NEW(instance_, ThreadPool);

    return instance_;
}

void ThreadPool::ReleaseInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);

    AIE_DELETE(instance_);
}

ThreadPool::ThreadPool()
    : stackSize_(THREAD_DEFAULT_STACK_SIZE)
{
}

ThreadPool::~ThreadPool()
{
    StopThreads();
}

void ThreadPool::SetStackSize(size_t size)
{
    stackSize_ = size;
}

int ThreadPool::getStackSize()
{
    return stackSize_;
}

std::shared_ptr<Thread> ThreadPool::Pop()
{
    std::lock_guard<std::mutex> guard(mutex4Inner_);

    std::shared_ptr<Thread> thread;
    if (idleThreads_.empty()) {
        Thread *ptr = nullptr;
        AIE_NEW(ptr, Thread);
        CHK_RET(ptr == nullptr, nullptr);

        ptr->SetStackSize(stackSize_);
        thread.reset(ptr);
    } else {
        thread = idleThreads_.front();
        idleThreads_.pop_front();
    }

    busyThreads_.push_back(thread);

    return thread;
}

void ThreadPool::Push(std::shared_ptr<Thread> &thread)
{
    thread->StopThread();

    std::lock_guard<std::mutex> guard(mutex4Inner_);
    for (auto iter = busyThreads_.begin(); iter != busyThreads_.end(); ++iter) {
        if ((*iter)->GetThreadId() == thread->GetThreadId()) {
            busyThreads_.erase(iter);
            break;
        }
    }

    idleThreads_.push_back(thread);
}

void ThreadPool::StopThreads()
{
    (void)StopThreads(THREAD_STOP_TIMEOUT);
}

bool ThreadPool::StopThreads(int32_t timeOut)
{
    std::lock_guard<std::mutex> guard(mutex4Inner_);

    for (auto &thread : busyThreads_) {
        thread->running_ = false;
    }

    time_t now = GetCurTimeSec();

    TimeElapser elapser;

    while (true) {
        // judge if the wait time over timeOut
        CHK_RET(static_cast<int32_t>(elapser.ElapseMilli()) > timeOut, false);

        // count the num of busy threads
        int32_t activeThreadCount = 0;
        for (auto &thread : busyThreads_) {
            if (!thread->IsActive()) {
                continue;
            }
            // if thread is Hung, stop failed
            CHK_RET(thread->IsHung(now), false);
            ++activeThreadCount;
        }

        if (activeThreadCount == 0) {
            break;
        }

        (void)StepSleepMs(THREAD_SLEEP_TIME);
    }

    // WaitForEnd
    for (auto &thread : busyThreads_) {
        thread->WaitForEnd();
        idleThreads_.push_back(thread);
    }

    busyThreads_.clear();

    return true;
}
} // namespace AI
} // namespace OHOS
