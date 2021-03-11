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

#ifndef AIE_INFO_DEFINE_H
#define AIE_INFO_DEFINE_H

typedef struct ConfigInfo {
    const char *description;
} ConfigInfo;

typedef struct ClientInfo {
    long long clientVersion;
    int clientId;
    int sessionId;
    int extendLen;
    unsigned char *extendMsg;
} ClientInfo;

typedef struct AlgorithmInfo {
    long long clientVersion;
    bool isAsync;
    int algorithmType;
    long long algorithmVersion;
    bool isCloud;
    int operateId;
    int requestId;
    int extendLen;
    unsigned char *extendMsg;
} AlgorithmInfo;

typedef struct DataInfo {
    unsigned char *data;
    int length;
} DataInfo;

#endif // AIE_INFO_DEFINE_H