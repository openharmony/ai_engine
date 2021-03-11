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

#include <unistd.h>

#include "samgr_lite.h"

#include "utils/log/aie_log.h"

void __attribute__((weak)) HOS_SystemInit(void)
{
    HILOGI("[StartServer]Provider HOS System init");
    SAMGR_Bootstrap();
};

int main()
{
    HILOGD("[StartServer]Start Ai Provider System Start.");
    HOS_SystemInit();
    HILOGD("[StartServer]Start Ai Provider System End.");
    pause();
}