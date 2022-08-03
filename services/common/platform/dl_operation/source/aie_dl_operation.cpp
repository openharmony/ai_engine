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

#include "platform/dl_operation/include/aie_dl_operation.h"

#include <climits>
#include <cstdlib>
#include <cstring>
#include <dlfcn.h>

#include "utils/aie_macros.h"
#include "utils/log/aie_log.h"

namespace {
const int LOCAL = (RTLD_NOW | RTLD_LOCAL);
const int GLOBAL = (RTLD_NOW | RTLD_GLOBAL);
const char *LIB_PATH = "/usr/";
}

void *AieDlopen(const char *libName, int local)
{
    int flag = local ? LOCAL : GLOBAL;
    unsigned int length = strlen(LIB_PATH);
    if ((libName == nullptr) || (strlen(libName) < length)) {
        HILOGD("[AieDlOperation] libName is invalid.");
        return nullptr;
    }

    char realLibPath[PATH_MAX + 1] = {0};
    if (realpath(libName, realLibPath) == nullptr) {
        HILOGD("[AieDlOperation] get libName realpath failed.");
        return nullptr;
    }

    int retCode = strncmp(realLibPath, LIB_PATH, length);
    if (retCode != 0) {
        HILOGD("[AieDlOperation] lib path is error.");
        return nullptr;
    }

    void *res = dlopen(realLibPath, flag);
    return res;
}

void AieDlclose(void *lib)
{
    dlclose(lib);
}

void *AieDlsym(void *handle, const char *funName)
{
    return dlsym(handle, funName);
}

const char *AieDlerror()
{
    return dlerror();
}
