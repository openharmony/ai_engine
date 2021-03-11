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

#ifndef THREAD_H
#define THREAD_H

#include <thread>

#include "platform/threadpool/include/aie_thread_unix.h"
#include "utils/aie_macros.h"
#include "utils/constants/constants.h"

namespace OHOS {
namespace AI {
const size_t THREAD_MIN_STACK_SIZE = (1024 * 64);
const size_t THREAD_MAX_STACK_SIZE = (1024 * 1024 * 64);
const int THREAD_SLEEP_MS = 1;
class Thread;

class IWorker {
public:
    IWorker();

    virtual ~IWorker();

    /**
     * attention sub class need to realize this, and cannot return null
     *
     * @return Worker name
     */
    virtual const char *GetName() const = 0;

    /**
     * attention thread will call OneAction loop, until the thread is stopped. or return false
     *
     * @return true thread is running, false thread is stop
     */
    virtual bool OneAction() = 0;

    /**
     * attention this method is called in "thread" first, if return false, thread stop auto.
     *
     * @return true success, false failure and the thread stop
     */
    virtual bool Initialize()
    {
        return true;
    }

    /**
     * attention this method is called when thread stop, it is still run in thread.
     */
    virtual void Uninitialize() {}

    /**
     * judge if the thread is Hung
     *
     * @return true is Hung, false is not Hung
     */
    virtual bool isHung(const time_t) const
    {
        return false;
    }

    /**
     * get the stack size
     *
     * @return 0 no request on size(default), greater than 0 means the size of stack size
     */
    virtual size_t GetStackSize()
    {
        return THREAD_DEFAULT_STACK_SIZE;
    }

    enum WorkerStatus {
        IDLE,
        BUSY
    };

    /**
     * get worker status
     *
     * @return the status on thread
     */
    WorkerStatus Status()
    {
        return status_;
    }

    /**
     * attention set status
     *
     * @param [in] IDLE or BUSY
     */
    void Status(WorkerStatus status)
    {
        status_ = status;
    }

    void SetThread(Thread *thread)
    {
        thread_ = thread;
    }

    volatile int counter_;

    unsigned long GetThreadId();

protected:
    Thread *thread_;

private:
    WorkerStatus status_;
};

class ThreadPool;
class Thread {
    friend class ThreadPool;

public:

    Thread();
    virtual ~Thread();

    /**
     * set stack size, attention: must be set before StartThread
     *
     * @param [in] >64k is valid
     * @return A pointer to the singleton instance.
     */
    void SetStackSize(const size_t size);

    /**
     * attention: need to make sure run() method is not overloaded before
     *
     * @return true success , false failure
     */
    bool StartThread(IWorker *pWorker);

    void StopThread();

    bool StopThread(int timeOut);

    // wait until thread is stop
    void WaitForEnd();

    enum ThreadStatus {
        PREPARED,
        RUNNING,
        STOPPED
    };

    /**
     * attention: get thread status
     *
     * @return PREPARED, RUNNING, STOPPED
     */
    ThreadStatus Status() const;

    const IWorker *GetWorker() const;

    void SetWorker(IWorker *pWorker);

    bool IsActive() const;

    bool IsHung(time_t now) const;

    unsigned long GetThreadId() const;

    bool IsRunning() const;

private:
    bool StartThread();
    void Execute();
    virtual void Run();

    static void *ThreadProc(void*);

private:
    volatile bool running_;
    volatile ThreadStatus status_;

    size_t stackSize_;
    PthreadData thread_;
    IWorker *worker_;
};
} // namespace AI
} // namespace OHOS

#endif // THREAD_H
