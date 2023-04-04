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

#include "platform/event/include/i_event.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

#include "utils/constants/constants.h"
#include "utils/inf_cast_impl.h"

namespace OHOS {
namespace AI {
class Event {
public:
    Event() : flag_(false)
    {
    }

    ~Event()
    {
    }

    inline void Wait() const
    {
        std::unique_lock<std::mutex> lock(mutex_);
        condition_.wait(lock, [&]()->bool { return flag_; });
    }

    inline bool Wait(const int milliSeconds) const
    {
        using milliseconds_type = std::chrono::duration<int, std::milli>;
        std::unique_lock<std::mutex> lock(mutex_);
        return condition_.wait_for(lock, milliseconds_type(milliSeconds), [&]()->bool { return flag_; });
    }

    inline bool Signal()
    {
        mutex_.lock();
        bool signalled = flag_;
        flag_ = true;
        mutex_.unlock();
        condition_.notify_all();
        return !signalled;
    }

    inline bool Reset()
    {
        mutex_.lock();
        bool signalled = flag_;
        flag_ = false;
        mutex_.unlock();
        return signalled;
    }

    inline bool IsSet() const
    {
        return flag_;
    }

private:
    bool flag_;
    mutable std::mutex mutex_;
    mutable std::condition_variable condition_;
};

DEFINE_IMPL_CLASS_CAST(EventCast, IEvent, Event);

class EventDeleter {
public:
    void operator ()(IEvent *p) const
    {
        EventCast::Destroy(p);
    }
};

std::shared_ptr<IEvent> IEvent::MakeShared()
{
    std::shared_ptr<IEvent> sp(EventCast::Create(), EventDeleter());
    return sp;
}

bool IEvent::Wait(const int milliSeconds) const
{
    if (milliSeconds <= 0) {
        EventCast::Ref(this).Wait();
        return true;
    }
    return EventCast::Ref(this).Wait(milliSeconds);
}

bool IEvent::Signal()
{
    return EventCast::Ref(this).Signal();
}

bool IEvent::Reset()
{
    return EventCast::Ref(this).Reset();
}

bool IEvent::IsSet() const
{
    return EventCast::Ref(this).IsSet();
}
} // namespace AI
} // namespace OHOS
