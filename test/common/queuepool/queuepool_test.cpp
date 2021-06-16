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

#include "gtest/gtest.h"

#include "platform/queuepool/queue.h"
#include "platform/queuepool/queue_pool.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
    const int SINGLE_QUEUE_CAPACITY = 3;
    const int TEST_QUEUE_SINGLE_ELEMENT = 0;
}

class QueuepoolTest : public testing::Test {
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

/**
 * @tc.name: TestQueuePool001
 * @tc.desc: Get singleton of queue pool.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueuePool001, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueuePool002
 * @tc.desc: Pop a queue from queue pool.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueuePool002, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueuePool003
 * @tc.desc: Get the singleton of queue pool and test the number of busy queues.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueuePool003, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    auto busyNum = queuePool->BusyQueueNum();
    ASSERT_EQ(busyNum, 0);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueuePool004
 * @tc.desc: Test the number of busy queues when pop a queue.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueuePool004, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);
    auto busyNum = queuePool->BusyQueueNum();
    ASSERT_EQ(busyNum, 1);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueuePool005
 * @tc.desc: Pop a queue first, then push a queue to Test the busy queue number.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueuePool005, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);
    queuePool->Push(queue);
    auto busy = queuePool->BusyQueueNum();
    ASSERT_EQ(busy, 0);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueuePool006
 * @tc.desc: Release singleton of queue pool.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueuePool006, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    QueuePool<int>::ReleaseInstance();
    if (queuePool == nullptr) {
        HILOGE("[Test]QueuePool is nullptr.");
    }
}

/**
 * @tc.name: TestQueuePool007
 * @tc.desc: Test the capacity of queue pool.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueuePool007, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);

    std::shared_ptr<Queue<int>> queue;
    for (auto i = 0; i < MAX_QUEUE_COUNT; ++i) {
        queue = queuePool->Pop();
        ASSERT_NE(queue, nullptr);
    }

    queue = queuePool->Pop();
    ASSERT_EQ(queue, nullptr);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueuePool008
 * @tc.desc: Push a queue to test whether the busy queue is abnormal.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueuePool008, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);

    std::shared_ptr<Queue<int>> queue;
    queuePool->Push(queue);
    auto busyNum = queuePool->BusyQueueNum();
    ASSERT_FALSE(busyNum < 0);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueuePool009
 * @tc.desc: Pop a queue and verify it is empty.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue001, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();

    bool empty = queue->IsEmpty();
    ASSERT_TRUE(empty);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue002
 * @tc.desc: Push an element and verify it is not empty.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue002, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    int iv = TEST_QUEUE_SINGLE_ELEMENT;
    queue->PushBack(iv);
    bool notEmpty = !queue->IsEmpty();
    ASSERT_TRUE(notEmpty);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue003
 * @tc.desc: Fill up a queue and test if it is full.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue003, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    for (int i = 0; i < SINGLE_QUEUE_CAPACITY; ++i) {
        queue->PushBack(i);
    }
    ASSERT_TRUE(queue->IsFull());

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue004
 * @tc.desc: Pop a queue and verify it is not full.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue004, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);
    ASSERT_FALSE(queue->IsFull());

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue005
 * @tc.desc: Push back an element and verify the queue is not full.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue005, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    int iv = TEST_QUEUE_SINGLE_ELEMENT;
    queue->PushBack(iv);
    ASSERT_FALSE(queue->IsFull());

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue006
 * @tc.desc: Push back an element into queue.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue006, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    int iv = TEST_QUEUE_SINGLE_ELEMENT;
    int result = queue->PushBack(iv);
    ASSERT_EQ(result, RETCODE_SUCCESS);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue007
 * @tc.desc: Push back elements that exceed the capacity of queue, and verify the queue is full.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue007, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    int result;
    for (int i = 0; i < SINGLE_QUEUE_CAPACITY; ++i) {
        result = queue->PushBack(i);
        ASSERT_EQ(result, RETCODE_SUCCESS);
    }

    int iv = TEST_QUEUE_SINGLE_ELEMENT;
    result = queue->PushBack(iv);
    ASSERT_EQ(result, RETCODE_QUEUE_FULL);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue008
 * @tc.desc: Pop Front element of empty queue.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue008, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    int iv;
    int result = queue->PopFront(iv);
    ASSERT_EQ(result, RETCODE_QUEUE_EMPTY);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue009
 * @tc.desc: Push back an element into queue, then pop it and verify its value.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue009, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    int iv = TEST_QUEUE_SINGLE_ELEMENT;
    queue->PushBack(iv);
    int result = queue->PopFront(iv);
    ASSERT_EQ(result, RETCODE_SUCCESS);
    ASSERT_EQ(iv, TEST_QUEUE_SINGLE_ELEMENT);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue010
 * @tc.desc: Test the availability of queue under capacity constraints.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue010, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    int result;
    for (int i = 0; i < SINGLE_QUEUE_CAPACITY; ++i) {
        result = queue->PushBack(i);
        ASSERT_EQ(result, RETCODE_SUCCESS);
    }

    int iv = 0;
    for (int i = 0; i < SINGLE_QUEUE_CAPACITY; ++i) {
        result = queue->PopFront(iv);
        ASSERT_EQ(result, RETCODE_SUCCESS);
        ASSERT_EQ(iv, i);
    }

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue011
 * @tc.desc: Test the count of element in the empty queue.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue011, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);
    int count = queue->Count();
    ASSERT_EQ(count, 0);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue012
 * @tc.desc: Test capacity limit of queue.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue012, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    int i;
    int count;

    for (i = 0; i < SINGLE_QUEUE_CAPACITY; ++i) {
        queue->PushBack(i);
        count = queue->Count();
        ASSERT_EQ(count, i + 1);
    }

    queue->PushBack(i);
    count = queue->Count();
    ASSERT_EQ(count, SINGLE_QUEUE_CAPACITY);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue013
 * @tc.desc: Test the count of element in the queue when push back and pop repeatedly.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue013, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    for (int i = 0; i < SINGLE_QUEUE_CAPACITY; ++i) {
        queue->PushBack(i);
    }
    int iv;
    int count;

    for (int i = 0; i < SINGLE_QUEUE_CAPACITY; ++i) {
        queue->PopFront(iv);
        count = queue->Count();
        ASSERT_EQ(count, SINGLE_QUEUE_CAPACITY - i - 1);
    }

    queue->PopFront(iv);
    count = queue->Count();
    ASSERT_EQ(count, 0);

    QueuePool<int>::ReleaseInstance();
}

/**
 * @tc.name: TestQueue014
 * @tc.desc: Pop more than one queue.
 * @tc.type: FUNC
 * @tc.require: AR000F77MS
 */
HWTEST_F(QueuepoolTest, TestQueue014, TestSize.Level1)
{
    QueuePool<int> *queuePool = QueuePool<int>::GetInstance(SINGLE_QUEUE_CAPACITY);
    ASSERT_NE(queuePool, nullptr);
    std::shared_ptr<Queue<int>> queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    queuePool->Push(queue);

    queue = queuePool->Pop();
    ASSERT_NE(queue, nullptr);

    QueuePool<int>::ReleaseInstance();
}
