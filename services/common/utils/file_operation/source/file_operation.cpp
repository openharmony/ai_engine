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

#include "utils/file_operation/include/file_operation.h"

#include <cstdio>
#include <cstdlib>

#include "utils/log/aie_log.h"

namespace OHOS {
namespace AI {
namespace {
    const int FILE_OPERATION_SUCCESS = 0;
}
int GetFileSize(const char *fileName, long int &fileSize)
{
    FILE *fp = fopen(fileName, "rb");
    if (fp == nullptr) {
        HILOGE("[FileOperation]Fopen file failed!");
        return RETCODE_FAILURE;
    }

    if (fseek(fp, 0L, SEEK_END) != FILE_OPERATION_SUCCESS) {
        HILOGE("[FileOperation]Fseek file failed!");
        fclose(fp);
        return RETCODE_FAILURE;
    }

    fileSize = ftell(fp);
    if (fileSize <= 0) {
        HILOGE("[FileOperation]Ftell file failed!");
        fclose(fp);
        return RETCODE_FAILURE;
    }

    if (fseek(fp, 0L, SEEK_SET) != FILE_OPERATION_SUCCESS) {
        HILOGE("[FileOperation]Fseek file failed!");
        fclose(fp);
        return RETCODE_FAILURE;
    }
    int retCode = fclose(fp);
    if (retCode != FILE_OPERATION_SUCCESS) {
        HILOGE("[FileOperation]Fclose file failed!");
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}

int ReadFile(const char *fileName, long long fileSize, long long countNumber, char *buffer)
{
    if (buffer == nullptr) {
        HILOGE("[FileOperation]Fopen file failed! Input buffer is null!");
        return RETCODE_FAILURE;
    }
    FILE *fp = fopen(fileName, "rb");
    if (fp == nullptr) {
        HILOGE("[FileOperation]Fopen file failed!");
        return RETCODE_FAILURE;
    }

    int readNumber = fread(buffer, fileSize, countNumber, fp);
    if (readNumber != countNumber) {
        HILOGE("[FileOperation]Fread file failed, readNumber is %d!", readNumber);
        fclose(fp);
        return RETCODE_FAILURE;
    }

    int retCode = fclose(fp);
    if (retCode != FILE_OPERATION_SUCCESS) {
        HILOGE("[FileOperation]Fclose file failed!");
        return RETCODE_FAILURE;
    }
    return RETCODE_SUCCESS;
}
} // namespace AI
} // namespace OHOS