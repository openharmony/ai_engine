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

#ifndef QUEUE_POOL_H
#define QUEUE_POOL_H

#include <atomic>
#include <list>
#include <memory>
#include <mutex>

#include "platform/queuepool/queue.h"

namespace OHOS {
namespace AI {
const size_t MAX_QUEUE_LENGTH = 1024U;
const size_t MAX_QUEUE_COUNT = 10U;

template<class TYPE>
class QueuePool {
    FORBID_COPY_AND_ASSIGN(QueuePool);
    FORBID_CREATE_BY_SELF(QueuePool);

    typedef std::list<std::shared_ptr<Queue<TYPE>>> Queues;
public:
    /**
     * Acquire singleton instance.
     *
     * @param [in] singleQueueCapacity Capacity for single queue.
     * @return A pointer to the singleton instance.
     */
    static QueuePool *GetInstance(size_t singleQueueCapacity = MAX_QUEUE_LENGTH);

    /**
     * Release the singleton instance.
     */
    static void ReleaseInstance();

    /**
     * Obtain a queue from the available queue pool.
     *
     * @return A shared pointer to the queue obtained.
     */
    std::shared_ptr<Queue<TYPE>> Pop();

    /**
     * Return the queue to the pool for management.
     *
     * @param [in] queue A queue to push back.
     */
    void Push(std::shared_ptr<Queue<TYPE>> &queue);

    /**
     * Query the number of currently used queues.
     *
     * @return The number of currently used queues.
     */
    size_t BusyQueueNum() const;

private:
    static std::mutex mutex_;
    static QueuePool *instance_;
    static size_t singleQueueCapacity_;

private:
    std::mutex mutex4Inner_;
    Queues queues_;
    std::atomic<size_t> busyQueueNum_;
};
} // namespace AI
} // namespace OHOS

#include "platform/queuepool/queue_pool.inl"

#endif // QUEUE_POOL_H