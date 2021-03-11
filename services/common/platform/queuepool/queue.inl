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
Queue<TYPE>::Queue(size_t maxQueueSize)
        : pushPos_(0), popPos_(0), totalNum_(maxQueueSize), writeAbleCount_(totalNum_), readAbleCount_(0) {
    AIE_NEW(queue_, QueueNode[maxQueueSize]);
    CHK_RET_NONE(queue_ == nullptr);
}

template<class TYPE>
Queue<TYPE>::~Queue() {
    AIE_DELETE_ARRAY(queue_);
}

template<class TYPE>
bool Queue<TYPE>::IsEmpty() const {
    return readAbleCount_ == 0;
}

template<class TYPE>
bool Queue<TYPE>::IsFull() const {
    return totalNum_ == readAbleCount_;
}

template<class TYPE>
int Queue<TYPE>::PushBack(TYPE &msgBlock) {
    CHK_RET(writeAbleCount_ == 0, RETCODE_QUEUE_FULL);

    --writeAbleCount_;
    size_t pushPos = pushPos_++ % totalNum_;

    queue_[pushPos].node = msgBlock;
    queue_[pushPos].empty = false;

    ++readAbleCount_;

    return RETCODE_SUCCESS;
}

template<class TYPE>
int Queue<TYPE>::PopFront(TYPE &msgBlock) {
    CHK_RET(readAbleCount_ == 0, RETCODE_QUEUE_EMPTY);
    --readAbleCount_;

    size_t popPos = popPos_ % totalNum_;

    if (queue_[popPos].empty) {
        ++readAbleCount_;
        return RETCODE_QUEUE_NODE_INVALID;
    }

    ++popPos_;

    msgBlock = queue_[popPos].node;
    queue_[popPos].empty = true;

    ++writeAbleCount_;

    return RETCODE_SUCCESS;
}

template<class TYPE>
size_t Queue<TYPE>::Count() const {
    return readAbleCount_;
}

template<class TYPE>
void Queue<TYPE>::Reset() {
    pushPos_ = 0;
    popPos_ = 0;
    writeAbleCount_ = totalNum_;
    readAbleCount_ = 0;

    for (size_t i = 0; i < totalNum_; ++i) {
        queue_[i].empty = true;
    }
}
} // namespace AI
} // namespace OHOS