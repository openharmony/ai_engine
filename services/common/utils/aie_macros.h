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

#ifndef AIE_MACROS_H
#define AIE_MACROS_H

#include <cstdlib>
#include <new>

namespace OHOS {
namespace AI {
/**
 * If x is true, return ret, pass otherwise
 */
#define CHK_RET(x, ret) \
    do { \
        if (x) { \
            return ret; \
        } \
    } while (0)

/**
 * If x is true, return void, pass otherwise
 */
#define CHK_RET_NONE(x) \
    do { \
        if (x) { \
            return; \
        } \
    } while (0)

/**
 * Try assign classname to var, assign nullptr if failed
 */
#define AIE_NEW(var, classname) \
    do { \
        var = new(std::nothrow) classname; \
    } while (0)

/**
 * Try delete the pointer p, and set p to nullptr.
 */
#define AIE_DELETE(p) \
    do { \
        if (p ==  nullptr) { \
            break; \
        } \
        delete p; \
        p = nullptr; \
    } while (0)

/**
 * Try delete the pointer p[], and set p to nullptr.
 */
#define AIE_DELETE_ARRAY(p) \
    do { \
        if (p == nullptr) { \
            break; \
        } \
        delete[] p; \
        p = nullptr; \
    } while (0)

#undef FORBID_COPY_AND_ASSIGN
/**
 * Forbid copy and assign classname, for singleton pattern.
 */
#define FORBID_COPY_AND_ASSIGN(CLASSNAME) \
private: \
    CLASSNAME(const CLASSNAME&) = delete; \
    CLASSNAME& operator = (const CLASSNAME&) = delete; \
    CLASSNAME(CLASSNAME&&) = delete; \
    CLASSNAME& operator = (CLASSNAME&&) = delete

#undef FORBID_CREATE_BY_SELF
/**
 * Forbid create classname, for singleton pattern.
 */
#define FORBID_CREATE_BY_SELF(CLASSNAME) \
private: \
    CLASSNAME(); \
    ~CLASSNAME()
} // namespace AI
} // namespace OHOS

#endif // AIE_MACROS_H
