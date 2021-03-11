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

namespace OHOS {
namespace AI {
template<class TYPE>
std::mutex QueuePool<TYPE>::mutex_;

template<class TYPE>
QueuePool<TYPE> *QueuePool<TYPE>::instance_ = nullptr;

template<class TYPE>
size_t QueuePool<TYPE>::singleQueueCapacity_ = MAX_QUEUE_LENGTH;

template<class TYPE>
QueuePool<TYPE> *QueuePool<TYPE>::GetInstance(size_t singleQueueCapacity) {
    CHK_RET(instance_ != nullptr, instance_);

    std::lock_guard<std::mutex> lock(mutex_);
    CHK_RET(instance_ != nullptr, instance_);

    singleQueueCapacity_ = singleQueueCapacity;
    AIE_NEW(instance_, QueuePool<TYPE>);

    return instance_;
}

template<class TYPE>
void QueuePool<TYPE>::ReleaseInstance() {
    std::lock_guard<std::mutex> lock(mutex_);

    AIE_DELETE(instance_);
}

template<class TYPE>
QueuePool<TYPE>::QueuePool()
        : busyQueueNum_(0) {
}

template<class TYPE>
QueuePool<TYPE>::~QueuePool() = default;

template<class TYPE>
std::shared_ptr<Queue<TYPE>> QueuePool<TYPE>::Pop() {
    std::shared_ptr <Queue<TYPE>> queue = nullptr;

    if (busyQueueNum_ >= MAX_QUEUE_COUNT) {
        return queue;
    }

    ++busyQueueNum_;

    if (queues_.empty()) {
        Queue<TYPE> *ptr = nullptr;
        AIE_NEW(ptr, Queue<TYPE>(singleQueueCapacity_));
        CHK_RET(ptr == nullptr, nullptr);
        queue.reset(ptr);

        return queue;
    }

    std::lock_guard<std::mutex> guard(mutex4Inner_);
    queue = queues_.front();
    queues_.pop_front();

    return queue;
}

template<class TYPE>
void QueuePool<TYPE>::Push(std::shared_ptr<Queue<TYPE>> &queue) {
    if (busyQueueNum_ <= 0) {
        return;
    }

    busyQueueNum_--;

    std::lock_guard<std::mutex> guard(mutex4Inner_);
    queue->Reset();
    queues_.push_back(queue);
}

template<class TYPE>
size_t QueuePool<TYPE>::BusyQueueNum() const {
    return busyQueueNum_;
}
} // namespace AI
} // namespace OHOS