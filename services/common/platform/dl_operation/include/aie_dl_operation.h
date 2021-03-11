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

#ifndef AIE_DL_OPERATION_H
#define AIE_DL_OPERATION_H

#ifdef __cplusplus
extern "C" {
#endif

const int TYPE_DEFAULT_LOCAL = 1;

/**
 * Load dynamic library.
 *
 * @param libPath Dynamic library name, including path.
 * @param type true：RTLD_LOCAL，false: RTLD_GLOBAL.
 * @return if the result is null that means fail and if the result is not null that means success.
 */
void *AieDlopen(const char *libPath, int type = TYPE_DEFAULT_LOCAL);

/**
 * Get symbol.
 *
 * @param libHandle The result which is obtained by AiDlopen.
 * @param functionName Symbol name.
 * @return if the result is null that means fail and if the result is not null that means success.
 */
void *AieDlsym(void *libHandle, const char *functionName);

/**
 * Unload dynamic library.
 *
 * @param libHandle The result which is obtained by AiDlopen.
 */
void AieDlclose(void *libHandle);

/**
 * Get error information.
 *
 * @return error information.
 */
const char *AieDlerror();

#ifdef __cplusplus
}
#endif

#endif // AIE_DL_OPERATION_H