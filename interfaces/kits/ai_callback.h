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

#ifndef AI_CALLBACK_H
#define AI_CALLBACK_H

namespace OHOS {
namespace AI {
template <typename T>
class ICallback {
public:
    /**
     * @brief Defines the destructor for the callback.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual ~ICallback() = default;

    /**
     * @brief Defines the callback for prediction results.
     *
     * @param result Indicates the prediction result.
     *
     * @since 2.2
     * @version 1.0
     */
    virtual void OnDetectedResult(const int resultCode, const T &result) = 0;
};
} // namespace AI
} // namespace OHOS

#endif // AI_CALLBACK_H
/** @} */
