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

#include "platform/threadpool/include/aie_thread_unix.h"

#include <csignal>
#include <unistd.h>

#include "securec.h"

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/log/aie_log.h"

typedef struct {
    pthread_t handle;
} PthreadHandle;

int InitThread(PthreadData &tr)
{
    errno_t retCode = memset_s(&tr, sizeof(PthreadData), 0, sizeof(PthreadData));
    if (retCode != EOK) {
        HILOGE("[AieThreadUnix]Failed to memory set, retCode[%d].", retCode);
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

unsigned long GetThreadIdUnix(const PthreadData &tr)
{
    auto *rt = reinterpret_cast<const PthreadHandle *>(&tr);
    return static_cast<unsigned long>(rt->handle);
}

bool IsThreadRunning(unsigned long tid)
{
    const int checkThreadAliveSig = 0;
    return pthread_kill(tid, checkThreadAliveSig) != ESRCH;
}

void InitThreadAttr(PthreadAttr &attr)
{
    attr.stackSize = THREAD_DEFAULT_STACK_SIZE;
    attr.scope = THREAD_SCOPE_SYSTEM;
}

void SetThreadAttrStackSize(PthreadAttr &attr, size_t size)
{
    attr.stackSize = size;
}

void SetThreadAttrScope(PthreadAttr &attr, int32_t scope)
{
    attr.scope = scope;
}

int CreateOneThread(PthreadData &tr, PthreadAttr *attr, PthreadRoutine func, void *param)
{
    pthread_attr_t pthreadAttr;

    int retCode = pthread_attr_init(&pthreadAttr);
    if (retCode != 0) {
        HILOGE("[AieThreadUnix]Failed to run pthread_attr_init, retCode[%d].", retCode);
        return retCode;
    }
    if (attr != nullptr && attr->scope == THREAD_SCOPE_PROCESS) {
        pthread_attr_setscope(&pthreadAttr, PTHREAD_SCOPE_PROCESS);
    } else {
        pthread_attr_setscope(&pthreadAttr, PTHREAD_SCOPE_SYSTEM);
    }

    if (attr != nullptr && attr->stackSize > 0) {
        pthread_attr_setstacksize(&pthreadAttr, attr->stackSize);
    }

    pthread_attr_setdetachstate(&pthreadAttr, PTHREAD_CREATE_JOINABLE);
    auto *pthreadHandleT = reinterpret_cast<PthreadHandle *>(reinterpret_cast<void *>(&tr));
    retCode = pthread_create(&(pthreadHandleT->handle), &pthreadAttr, func, param);
    pthread_attr_destroy(&pthreadAttr);
    return retCode;
}

int WaitThread(PthreadData &tr)
{
    auto *pthreadHandle = reinterpret_cast<PthreadHandle *>(reinterpret_cast<void *>(&tr));
    if (pthreadHandle->handle == 0) {
        return RETCODE_SUCCESS;
    }

    pthread_join(pthreadHandle->handle, nullptr);
    pthreadHandle->handle = 0;
    return RETCODE_SUCCESS;
}
