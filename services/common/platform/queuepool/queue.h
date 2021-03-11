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

#ifndef QUEUE_H
#define QUEUE_H

#include <atomic>

#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
template<class TYPE>
class Queue {
    FORBID_COPY_AND_ASSIGN(Queue);
public:
    explicit Queue(size_t maxQueueSize);

    virtual ~Queue();

public:
    /**
     * Check if the queue is empty.
     *
     * @return true if empty or false if not empty.
     */
    bool IsEmpty() const;

    /**
     * Check if the queue is full.
     *
     * @return true if full or false if not full.
     */
    bool IsFull() const;

    /**
     * Push a message at the rear.
     *
     * @param [in] msgBlock message to push.
     * @return 1011 if the queue is full or 0 if success.
     */
    int PushBack(TYPE &msgBlock);

    /**
     * Pop a message from the head.
     *
     * @param [out] msgBlock message from the head.
     * @return 1012 if the queue is empty or 0 if success.
     */
    int PopFront(TYPE &msgBlock);

    /**
     * Query the number of elements in the queue.
     *
     * @return the number of elements in the queue.
     */
    size_t Count() const;

    /**
     * Reset the queue.
     */
    void Reset();

private:
    /**
     * Next position to push elements to.
     */
    std::atomic<size_t> pushPos_;

    /**
     * Next position to pop message from.
     */
    std::atomic<size_t> popPos_;

    /**
     * Maximum message of the queue.
     */
    size_t totalNum_;

    /**
     * Writable message number.
     */
    std::atomic<size_t> writeAbleCount_;

    /**
     * Readable message number.
     */
    std::atomic<size_t> readAbleCount_;

    struct QueueNode {
        bool empty = true;
        TYPE node;
    };

    QueueNode *queue_;
};
} // namespace AI
} // namespace OHOS

#include "platform/queuepool/queue.inl"

#endif // QUEUE_H