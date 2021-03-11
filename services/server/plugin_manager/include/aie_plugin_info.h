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

#ifndef AIE_PLUGIN_INFO_H
#define AIE_PLUGIN_INFO_H

#include <string>
#include <vector>

#include "utils/aie_macros.h"

namespace OHOS {
namespace AI {
const std::string ALGORITHM_ID_SAMPLE_1 = "sample_plugin_1";
const std::string ALGORITHM_ID_SAMPLE_2 = "sample_plugin_2";
const std::string ALGORITHM_ID_INVALID = "invalid algorithm id";

// Defines the key value of the table field in the .ini file.
const std::string ALGORITHM_INFO_TABLE = "AlgorithmInfo";
const std::string ALGORITHM_INFO_TABLE_FIELD_NAME_AID = "AID";
const std::string ALGORITHM_INFO_TABLE_FIELD_NAME_VERSION_CODE = "VersionCode";
const std::string ALGORITHM_INFO_TABLE_FIELD_NAME_VERSION_NAME = "VersionName";
const std::string ALGORITHM_INFO_TABLE_FIELD_NAME_XPU = "XPU";
const std::string ALGORITHM_INFO_TABLE_FIELD_NAME_CHIPSET = "Chipset";
const std::string ALGORITHM_INFO_TABLE_FIELD_NAME_DISTRICT = "District";
const std::string ALGORITHM_INFO_TABLE_FIELD_NAME_FULLPATH = "FullPath";
const std::string ALGORITHM_INFO_TABLE_FIELD_NAME_CHKSUM = "ChkSum";
const std::string ALGORITHM_INFO_TABLE_FIELD_NAME_KEY = "Key";

const std::vector<std::string> ALGORITHM_TYPE_ID_LIST = {
    ALGORITHM_ID_SAMPLE_1,
    ALGORITHM_ID_SAMPLE_2,
};

/**
 * Get algorithm id by algorithm type
 *
 * @param [in] type Algorithm type
 * @return Algorithm id
 */
const std::string GetAlgorithmIdByType(int type);
} // namespace AI
} // namespace OHOS
#endif // AIE_PLUGIN_INFO_H