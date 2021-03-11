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

#ifndef FILE_OPERATION_H
#define FILE_OPERATION_H

#include "protocol/retcode_inner/aie_retcode_inner.h"

namespace OHOS {
namespace AI {
int GetFileSize(const char *fileName, long int &fileSize);
int ReadFile(const char *fileName, long long fileSize, long long countNumber, char *buffer);
} // namespace AI
} // namespace OHOS

#endif // FILE_OPERATION_H