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

/**
 * @addtogroup ai_datatype
 * @{
 *
 * @brief Defines the basic array structure, including the array type and size.
 *
 * @since 2.2
 * @version 1.0
 */

/**
 * @file ai_datatype.h
 *
 * @brief Declares the basic array structure, including the array type and size.
 *
 * @since 2.2
 * @version 1.0
 */

#ifndef AI_DATATYPE_H
#define AI_DATATYPE_H

#include <cstddef>
#include <cstdint>

namespace OHOS {
namespace AI {
/**
 * @brief Defines the basic array structure, which is used to represent various data in the AI subsystem.
 *
 * @since 2.2
 * @version 1.0
 */
template<typename T>
struct Array {
    /* Pointer to the start address of the array */
    T *data;
    /* Array size */
    size_t size;
};
}  // namespace AI
}  // namespace OHOS
#endif // AI_DATATYPE_H
/** @} */