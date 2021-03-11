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
template<typename RwLockable>
WriteGuard<RwLockable>::WriteGuard(RwLockable &rwLockable)
    : rwLockable_(rwLockable)
{
    rwLockable_.LockWrite();
}

template<typename RwLockable>
WriteGuard<RwLockable>::~WriteGuard()
{
    rwLockable_.UnLockWrite();
}

template<typename RwLockable>
ReadGuard<RwLockable>::ReadGuard(RwLockable &rwLockable)
    : rwLockable_(rwLockable)
{
    rwLockable_.LockRead();
}

template<typename RwLockable>
ReadGuard<RwLockable>::~ReadGuard()
{
    rwLockable_.UnLockRead();
}
} // namespace AI
} // namespace OHOS