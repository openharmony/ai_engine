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

#include<cstring>
#include<string>

#include "gtest/gtest.h"

#include "platform/threadpool/include/thread.h"
#include "platform/threadpool/include/thread_pool.h"
#include "platform/time/include/time.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace std;
using namespace testing::ext;

namespace {
    const int STOP_TIME = 100;
    const time_t HUNG_TIME = 15;
    const int SIZE = 70000;
    const int SIZE_MIN = 0;
    const int INVALID_THREAD_ID = -1;
    const string WORKER_DEFAULT_NAME("WorkerDefaultName");
}

class ThreadPoolTest : public testing::Test {
public:
    // SetUpTestCase:The preset action of the test suite is executed before the first TestCase
    static void SetUpTestCase() {};

    // TearDownTestCase:The test suite cleanup action is executed after the last TestCase
    static void TearDownTestCase() {};

    // SetUp:Execute before each test case
    void SetUp() {};

    // TearDown:Execute after each test case
    void TearDown() {};
};

class CWorker : public IWorker {
public:
    const char *GetName() const override
    {
        return "CWorker";
    }

    bool OneAction() override
    {
        StepSleepMs(THREAD_SLEEP_MS);
        HILOGD("[Test]Worker OneAction after sleep");
        return true;
    }

    bool Initialize() override
    {
        HILOGD("[Test]Worker Initialize success");
        return true;
    }
};

class MultiWorker : public IWorker {
public:
    explicit MultiWorker(const string &workerName) : workerName_(workerName)
    {
    }

    ~MultiWorker() override = default;

    const char *GetName() const override
    {
        return workerName_.c_str();
    }

    bool OneAction() override
    {
        StepSleepMs(THREAD_SLEEP_MS);
        HILOGD("[Test]Worker(%s) OneAction success.", GetName());
        return true;
    }

    bool Initialize() override
    {
        HILOGD("[Test]Worker(%s) Initialize success.", GetName());
        return true;
    }

    void Uninitialize() override
    {
        HILOGD("[Test]Worker(%s) Uninitialize success.", GetName());
    }

private:
    string workerName_ = WORKER_DEFAULT_NAME;
};

/**
 * @tc.name: TestWorker001
 * @tc.desc: Test worker Initialize/GetStackSize/Status/GetThreadId functions.
 * @tc.type: FUNC
 * @tc.require: AR000F77TL
 */
HWTEST_F(ThreadPoolTest, TestWorker001, TestSize.Level1)
{
    HILOGD("[Test]Test worker begin");
    CWorker worker;
    worker.Initialize();
    ASSERT_FALSE(worker.isHung(HUNG_TIME));
    ASSERT_EQ(worker.GetStackSize(), THREAD_DEFAULT_STACK_SIZE);
    ASSERT_EQ(worker.Status(), 0);
    ASSERT_EQ((int)worker.GetThreadId(), INVALID_THREAD_ID);
    Thread thread;
    worker.SetThread(&thread);
    HILOGD("[Test]Test worker end, GetThreadId is %lu.", worker.GetThreadId());
}

/**
 * @tc.name: TestThread001
 * @tc.desc: Test thread StartThread/IsRunning/IsActive/StopThread functions.
 * @tc.type: FUNC
 * @tc.require: AR000F77TL
 */
HWTEST_F(ThreadPoolTest, TestThread001, TestSize.Level1)
{
    HILOGD("[Test]Test thread1 begin");
    Thread testThread;
    unsigned long id = testThread.GetThreadId();
    HILOGD("[Test]Thread id %lu.", id);
    testThread.SetStackSize(SIZE);
    CWorker worker;
    ASSERT_TRUE(testThread.StartThread(&worker));
    id = testThread.GetThreadId();
    HILOGD("[Test]Thread id %lu.", id);
    ASSERT_TRUE(testThread.IsRunning());
    ASSERT_TRUE(testThread.IsActive());
    ASSERT_TRUE(testThread.StopThread(STOP_TIME));
    ASSERT_FALSE(testThread.IsRunning());
    ASSERT_FALSE(testThread.IsActive());
    HILOGD("[Test]Test thread1 end.");
}

/**
 * @tc.name: ThreadPoolTest001
 * @tc.desc: Test ThreadPool SetStackSize/getStackSize functions.
 * @tc.type: FUNC
 * @tc.require: AR000F77TL
 */
HWTEST_F(ThreadPoolTest, ThreadPoolTest001, TestSize.Level1)
{
    HILOGD("[Test]ThreadPoolTest001 begin");
    ThreadPool *threadPool = ThreadPool::GetInstance();
    ASSERT_EQ(threadPool->getStackSize(), 0);
    threadPool->SetStackSize(SIZE);
    ASSERT_EQ(threadPool->getStackSize(), SIZE);
    threadPool->SetStackSize(SIZE_MIN);
    HILOGD("[Test]ThreadPoolTest001 end");
}

/**
 * @tc.name: ThreadPoolTest002
 * @tc.desc: Test ThreadPool Pop method.
 * @tc.type: FUNC
 * @tc.require: AR000F77TL
 */
HWTEST_F(ThreadPoolTest, ThreadPoolTest002, TestSize.Level1)
{
    HILOGD("[Test]ThreadPoolTest002 begin");
    ThreadPool *threadPool = ThreadPool::GetInstance();
    ASSERT_NE(threadPool, nullptr);
    std::shared_ptr<Thread> oneThread = threadPool->Pop();
    ASSERT_NE(oneThread, nullptr);
    HILOGD("[Test]ThreadPoolTest002 end");
}

/**
 * @tc.name: ThreadPoolTest003
 * @tc.desc: Test ThreadPool Push method.
 * @tc.type: FUNC
 * @tc.require: AR000F77TL
 */
HWTEST_F(ThreadPoolTest, ThreadPoolTest003, TestSize.Level1)
{
    HILOGD("[Test]ThreadPoolTest003 begin");
    ThreadPool *threadPool = ThreadPool::GetInstance();
    ASSERT_NE(threadPool, nullptr);
    std::shared_ptr<Thread> oneThread = threadPool->Pop();
    threadPool->Push(oneThread);
    HILOGD("[Test]ThreadPoolTest003 end");
}

/**
 * @tc.name: TestThreadWithManyWorker001
 * @tc.desc: Test pop/push thread, and start/stop thread
 * @tc.type: FUNC
 * @tc.require: AR000F77TL
 */
HWTEST_F(ThreadPoolTest, TestThreadWithManyWorker001, TestSize.Level1)
{
    HILOGD("[Test]Test worker begin");
    string workerName("MultiWorker");
    int maxThreadNum = 10;
    MultiWorker *workerList[maxThreadNum];
    std::shared_ptr<Thread> threadList[maxThreadNum];
    ThreadPool *threadPool = ThreadPool::GetInstance();
    ASSERT_NE(threadPool, nullptr);
    for (int i = 0; i < maxThreadNum; i++) {
        string tempWorkerName = workerName + to_string(i);
        AIE_NEW(workerList[i], MultiWorker(tempWorkerName));
        threadList[i] = threadPool->Pop();
        ASSERT_TRUE(threadList[i]->StartThread(workerList[i]));
    }
    for (int i = 0; i < maxThreadNum; i++) {
        threadList[i]->StopThread();
        threadPool->Push(threadList[i]);
        AIE_DELETE(workerList[i]);
    }
    threadPool->ReleaseInstance();
}
