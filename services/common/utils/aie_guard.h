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

#ifndef AIE_GUARD_H
#define AIE_GUARD_H

#include "utils/aie_macros.h"
#include "utils/constants/constants.h"

namespace OHOS {
namespace AI {
/**
 * Delete the pointer which create with new T
 * when the PointerGuard class is destructed.
 */
template<class T>
class PointerGuard {
public:
    explicit PointerGuard(T *&t) : t_(t), isValid_(true) {}
    ~PointerGuard()
    {
        CHK_RET_NONE(t_ == nullptr || !isValid_);
        AIE_DELETE(t_);
        t_ = nullptr;
    }

    /**
     * Detach the pointer guard.
     */
    void Detach()
    {
        isValid_ = false;
    }
private:
    T *&t_;
    bool isValid_;
};

/**
 * Free the pointer which create with malloc when the MallocPointerGuard class is destructed.
 */
template<class T>
class MallocPointerGuard {
public:
    MallocPointerGuard() : t_(nullptr), isValid_(true)
    {}

    explicit MallocPointerGuard(T *t) : t_(t), isValid_(true)
    {}

    ~MallocPointerGuard()
    {
        CHK_RET_NONE(t_ == nullptr || !isValid_);
        free(t_);
        t_ = nullptr;
    }

    void setPointer(T *t)
    {
        t_ = t;
        isValid_ = true;
    }
    /**
     * Detach the pointer guard.
     */
    void Detach()
    {
        isValid_ = false;
    }

private:
    T *t_ = nullptr;
    bool isValid_ = false;
};

/**
 * Delete the array pointer which create with new T[] when the ArrayPointerGuard class is destructed.
 */
template<class T>
class ArrayPointerGuard {
public:
    explicit ArrayPointerGuard(T *&t) : t_(t), isValid_(true) {}
    ~ArrayPointerGuard()
    {
        CHK_RET_NONE(t_ == nullptr || !isValid_);
        AIE_DELETE_ARRAY(t_);
        t_ = nullptr;
    }

    /**
     * Detach the array pointer guard.
     */
    void Detach()
    {
        isValid_ = false;
    }
private:
    T *&t_;
    bool isValid_;
};

/**
 * The Destroy method of the T is used to delete T when the ResGuard class is destructed.
 */
template<class T>
class ResGuard {
public:
    explicit ResGuard(T *&t) : t_(t), isValid_(true) {}
    ~ResGuard()
    {
        CHK_RET_NONE(t_ == nullptr || !isValid_);
        T::Destroy(t_);
        t_ = nullptr;
    }

    /**
     * Detach the point guard.
     */
    void Detach()
    {
        isValid_ = false;
    }
private:
    T *&t_;
    bool isValid_;
};
} // namespace AI
} // namespace OHOS

#endif // AIE_GUARD_H