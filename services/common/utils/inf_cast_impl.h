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

#ifndef INF_CAST_IMPL_H
#define INF_CAST_IMPL_H

#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
template<class I, class C>
class InfCastImpl {
public:
    /**
     * Interface class implement create class pointer function.
     *
     * @return I* Pointer of the class.
     */
    static I* Create()
    {
        C *p = nullptr;
        AIE_NEW(p, C);
        CHK_RET(p == nullptr, nullptr);
        return reinterpret_cast<I*>(p);
    }

    /**
     * Interface class implement create class pointer function.
     *
     * @param [in] type The int type param for the class.
     * @return I* Pointer of the class.
     */
    static I* Create(int type)
    {
        C *p = nullptr;
        AIE_NEW(p, C(type));
        CHK_RET(p == nullptr, nullptr);
        return reinterpret_cast<I*>(p);
    }

    /**
     * Interface class implement create class pointer function.
     *
     * @param [in] t The template type param for the class.
     * @return I* Pointer of the class.
     */
    template<class T>
    static I* Create(T& t)
    {
        C *p = nullptr;
        AIE_NEW(p, C(t));
        CHK_RET(p == nullptr, nullptr);
        return reinterpret_cast<I*>(p);
    }

    /**
     * Interface class implement create class pointer function.
     *
     * @param [in] T1 The first template type param for the class.
     * @param [in] T2 The second template type param for the class.
     * @return I* Pointer of the class.
     */
    template<class T1, class T2>
    static I* Create(T1& t1, T2& t2)
    {
        C *p = nullptr;
        AIE_NEW(p, C(t1, t2));
        CHK_RET(p == nullptr, nullptr);
        return reinterpret_cast<I*>(p);
    }

    /**
     * Interface class implement create class pointer function.
     *
     * @param [in] T1 The first template type param for the class.
     * @param [in] T2 The second template type param for the class.
     * @param [in] T3 The third template type param for the class.
     * @return I* Pointer of the class.
     */
    template<class T1, class T2, class T3>
    static I* Create(T1& t1, T2& t2, T3& t3)
    {
        C *p = nullptr;
        AIE_NEW(p, C(t1, t2, t3));
        CHK_RET(p == nullptr, nullptr);
        return reinterpret_cast<I*>(p);
    }

    /**
     * Interface class implement delete class pointer, and set the pointer to nullptr.
     */
    static void Destroy(I*& pi)
    {
        C *pc = reinterpret_cast<C*>(pi);
        AIE_DELETE(pc);
        pi = nullptr;
    }

    /**
     * Convert The I* pointer to C*.
     *
     * @return C* pointer of the class.
     */
    static C& Ref(I* p)
    {
        return *reinterpret_cast<C*>(p);
    }

    static const C& Ref(const I* p)
    {
        return *reinterpret_cast<const C*>(p);
    }
};

#define DEFINE_IMPL_CLASS_CAST(CastClass, InterfaceClass, ImplementationClass) \
class CastClass : public InfCastImpl<InterfaceClass, ImplementationClass> {};
} // namespace AI
} // namespace OHOS

#endif // INF_CAST_IMPL_H