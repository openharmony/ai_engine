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

#include "platform/threadpool/include/thread.h"

#include "platform/time/include/time.h"
#include "platform/time/include/time_elapser.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"

namespace OHOS {
namespace AI {
IWorker::IWorker() : counter_(0), thread_(nullptr), status_(IDLE)
{
}

IWorker::~IWorker()
{
    thread_ = nullptr;
}

unsigned long IWorker::GetThreadId()
{
    CHK_RET(thread_ == nullptr, RETCODE_FAILURE);
    return thread_->GetThreadId();
}

Thread::Thread()
    : running_(false), status_(STOPPED), stackSize_(THREAD_DEFAULT_STACK_SIZE), worker_(nullptr)
{
    InitThread(thread_);
}

Thread::~Thread()
{
    CHK_RET_NONE(worker_ == nullptr);

    worker_->SetThread(nullptr);
    worker_ = nullptr;
}

void Thread::SetStackSize(const size_t size)
{
    stackSize_ = size;
}

Thread::ThreadStatus Thread::Status() const
{
    return status_;
}

const IWorker *Thread::GetWorker() const
{
    return worker_;
}

void Thread::SetWorker(IWorker *pWorker)
{
    if (worker_ != nullptr) {
        worker_->SetThread(nullptr);
    }

    worker_ = pWorker;
    if (pWorker != nullptr) {
        pWorker->SetThread(this);
    }
}

bool Thread::StartThread()
{
    if (status_ == RUNNING) {
        return false;
    }

    size_t stackSize = stackSize_;
    if (worker_ != nullptr && worker_->GetStackSize() > 0) {
        stackSize = worker_->GetStackSize();
    }

    PthreadAttr attr;
    PthreadAttr *pattr = nullptr;
    if ((stackSize >= THREAD_MIN_STACK_SIZE) && (stackSize < THREAD_MAX_STACK_SIZE)) {
        (void)InitThreadAttr(attr);
        (void)SetThreadAttrStackSize(attr, stackSize);
        pattr = &attr;
    }

    status_ = PREPARED;

    int retCode = CreateOneThread(thread_, pattr, Thread::ThreadProc, this);
    if (retCode != 0) {
        status_ = STOPPED;
        return false;
    }

    while (status_ == PREPARED) {
        (void)StepSleepMs(THREAD_SLEEP_MS);
    }

    return true;
}

bool Thread::StartThread(IWorker *pWorker)
{
    if (status_ == RUNNING) {
        return false;
    }
    if (pWorker == nullptr) {
        return false;
    }
    worker_ = pWorker;

    (void)pWorker->SetThread(this);

    return StartThread();
}

void Thread::StopThread()
{
    if (!IsActive()) {
        return;
    }

    running_ = false;
    while (status_ == RUNNING) {
        (void)StepSleepMs(THREAD_SLEEP_MS);
    }
    WaitForEnd();
}

bool Thread::StopThread(int timeOut)
{
    if (!IsActive()) {
        return true;
    }

    running_ = false;

    // if timeOut==0 means just set stop flag
    if (timeOut == 0) {
        return true;
    }

    TimeElapser elapser;
    while ((status_ == RUNNING) && (static_cast<int32_t>(elapser.ElapseMilli()) < timeOut)) {
        (void)StepSleepMs(THREAD_SLEEP_MS);
    }

    if (status_ == RUNNING) {
        return false;
    }

    WaitForEnd();
    return true;
}

void Thread::Run()
{
    if (worker_ == nullptr) {
        return;
    }

    IWorker *pWorker = worker_;
    if (!pWorker->Initialize()) {
        pWorker->Uninitialize();
        return;
    }

    while (running_) {
        if (!pWorker->OneAction()) {
            break;
        }
        ++worker_->counter_;
    }
    status_ = STOPPED;
    pWorker->Uninitialize();
}

bool Thread::IsActive() const
{
    if (status_ == RUNNING) {
        return IsThreadRunning(GetThreadId());
    }

    return false;
}

bool Thread::IsHung(time_t now) const
{
    if (worker_ != nullptr) {
        return worker_->isHung(now);
    }
    return false;
}

unsigned long Thread::GetThreadId() const
{
    return GetThreadIdUnix(thread_);
}

bool Thread::IsRunning() const
{
    return running_;
}

void Thread::Execute()
{
    running_ = true;
    status_ = RUNNING;
    Run();
    status_ = STOPPED;
}

void Thread::WaitForEnd()
{
    (void)WaitThread(thread_);
}

void *Thread::ThreadProc(void *pObj)
{
    auto *pThis = reinterpret_cast<Thread*>(pObj);
    pThis->Execute();
    auto retCode = RETCODE_SUCCESS;
    return (void*)retCode;
}
} // namespace AI
} // namespace OHOS
