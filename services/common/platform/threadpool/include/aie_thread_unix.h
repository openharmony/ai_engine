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

#ifndef AIE_THREAD_UNIX_H
#define AIE_THREAD_UNIX_H

#include <chrono>
#include <pthread.h>

#include "protocol/retcode_inner/aie_retcode_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

#define THREAD_SCOPE_PROCESS 0x1
#define THREAD_SCOPE_SYSTEM 0x2
#define THREAD_DEFAULT_STACK_SIZE 0

typedef struct {
    size_t stack_size;
    int scope;
} PthreadAttr;

const size_t THREAD_DATA_LEN = 32;
typedef struct {
    char data[THREAD_DATA_LEN];
} PthreadData;

typedef void *(*PthreadRoutine)(void *);

/**
 * initialize PthreadAttr
 *
 * @param [in,out]  attr
 */
void InitThreadAttr(PthreadAttr &attr);

/**
 * set thread stack size
 *
 * @param [in] attr *PthreadAttr
 * @param [in] size
 */
void SetThreadAttrStackSize(PthreadAttr &attr, size_t size);

/**
 * set thread scope
 *
 * @param [in] attr *PthreadAttr
 * @param [in] scope THREAD_SCOPE_PROCESS or THREAD_SCOPE_SYSTEM
 */
void SetThreadAttrScope(PthreadAttr &attr, int scope);

/**
 * Init thread
 *
 * @param [in,out] tr PthreadData
 * @return RETCODE_SUCCESS: init thread success, RETCODE_FAILURE: init thread failed.
 */
int InitThread(PthreadData &tr);

/**
 * create thread
 *
 * @param [out] tr PthreadData
 * @param [in] attr thread attribute
 * @param [in] func thread function
 * @param [in] param thread function params
 * @return RETCODE_SUCCESS: create success, RETCODE_FAILURE: create failed. error information
 */
int CreateOneThread(PthreadData &tr, PthreadAttr *attr, PthreadRoutine func, void *param);

/**
 * get thread id from pec_thread_t
 *
 * @param [in] tr thread info
 * @return return id
 */
unsigned long GetThreadIdUnix(const PthreadData &tr);

/**
 * judge if thread is running
 *
 * @param [in] tid thread ID
 * @return true is existed, false is not existed
 */
bool IsThreadRunning(unsigned long tid);

/**
 * wait for thread end
 *
 * @param [in] tr thread info
 * @return RETCODE_SUCCESS
 */
int WaitThread(PthreadData &tr);

#ifdef __cplusplus
}
#endif

#endif // AIE_THREAD_UNIX_H
