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

#include "platform/semaphore/include/i_semaphore.h"

#include <condition_variable>
#include <mutex>

#include "utils/constants/constants.h"
#include "utils/inf_cast_impl.h"

namespace OHOS {
namespace AI {
class Semaphore {
public:
    explicit Semaphore(unsigned int count): count_(count)
    {
    }

    ~Semaphore()
    {
    }

    inline void Wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [&]()->bool { return count_ > 0; });
        --count_;
    }

    bool Wait(const int milliSeconds)
    {
        using milliseconds_type = std::chrono::duration<int, std::milli>;
        std::unique_lock<std::mutex> lock(mutex_);
        if (!condition_.wait_for(lock, milliseconds_type(milliSeconds), [&]()->bool { return count_ > 0; })) {
            return false;
        }
        --count_;
        return true;
    }

    inline void Signal()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        ++count_;
        condition_.notify_one();
    }

private:
    unsigned int count_;
    std::mutex mutex_;
    std::condition_variable condition_;
};

DEFINE_IMPL_CLASS_CAST(SemaphoreCast, ISemaphore, Semaphore);

class SemaphoreDeleter {
public:
    void operator ()(ISemaphore *p) const
    {
        SemaphoreCast::Destroy(p);
    }
};

std::shared_ptr<ISemaphore> ISemaphore::MakeShared(unsigned int count)
{
    std::shared_ptr<ISemaphore> sp(SemaphoreCast::Create(count), SemaphoreDeleter());
    return sp;
}

bool ISemaphore::Wait(const int milliSeconds)
{
    if (milliSeconds <= 0) {
        SemaphoreCast::Ref(this).Wait();
        return true;
    }
    return SemaphoreCast::Ref(this).Wait(milliSeconds);
}

void ISemaphore::Signal()
{
    SemaphoreCast::Ref(this).Signal();
}
} // namespace AI
} // namespace OHOS
