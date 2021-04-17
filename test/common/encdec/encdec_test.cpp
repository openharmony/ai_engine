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

#include "gtest/gtest.h"

#include <cstddef>
#include <string>

#include "utils/aie_guard.h"
#include "utils/aie_macros.h"
#include "utils/encdec/include/data_decoder.h"
#include "utils/encdec/include/data_encoder.h"
#include "utils/encdec/include/encdec_facade.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;
namespace {
constexpr size_t ARRAY_LEN = 10;
constexpr int g_int = 12;
constexpr char g_char = 'd';
constexpr float g_float = 0.99f;
constexpr long long g_long = 123456789L;
const std::string g_string = "random string";
const std::string g_emptyString = "";

typedef struct {
    char foo;
    int bar;
} StructWithPadding; // definition of struct with padding(blank memory) between members

typedef struct {
    int foo;
    StructWithPadding bar;
} NestedStruct;

typedef struct {
    size_t objectsNum;
    StructWithPadding* objects;
} StructWithPointer;

typedef struct {
    size_t foo;
    int barArray[ARRAY_LEN];
} StructWithArray;

template<typename Type>
bool CompareData(const Type &input, const Type &output)
{
    return input == output;
}

bool CompareData(const StructWithPadding &input, const StructWithPadding &output)
{
    return (input.foo == output.foo) && (input.bar == output.bar);
}

bool CompareData(const NestedStruct &input, const NestedStruct &output)
{
    return (input.foo == output.foo) && CompareData(input.bar, output.bar);
}

bool CompareData(const StructWithPointer &input, const StructWithPointer &output)
{
    if (input.objectsNum != output.objectsNum) {
        return false;
    }
    for (unsigned int i = 0; i < input.objectsNum; ++i) {
        if (!CompareData(input.objects[i], output.objects[i])) {
            return false;
        }
    }
    return true;
}

bool CompareData(const StructWithArray &input, const StructWithArray &output)
{
    if (input.foo != output.foo) {
        return false;
    }
    for (unsigned int i = 0; i < ARRAY_LEN; ++i) {
        if (!CompareData(input.barArray[i], output.barArray[i])) {
            return false;
        }
    }
    return true;
}

StructWithPadding GenStructWithPadding(const int myNum, const char myChar)
{
    StructWithPadding structWithPadding = {
        .foo = myChar,
        .bar = myNum
    };
    return structWithPadding;
}

NestedStruct GenNestedStruct()
{
    NestedStruct nestedStruct = {
        .foo = g_int,
        .bar = GenStructWithPadding(g_int, g_char)
    };
    return nestedStruct;
}

StructWithPointer GenStructWithPointer(const size_t objectsNum)
{
    StructWithPointer structWithPointer {
        .objectsNum = objectsNum
    };
    if (objectsNum > 0) {
        AIE_NEW(structWithPointer.objects, StructWithPadding[objectsNum]);
        for (size_t i = 0; i < objectsNum; ++i) {
            structWithPointer.objects[i] = GenStructWithPadding(g_int, g_char);
        }
    }
    return structWithPointer;
}

void ReleaseStructWithPointer(StructWithPointer &structWithPointer)
{
    AIE_DELETE_ARRAY(structWithPointer.objects);
}

StructWithArray GenStructWithArray(const size_t num)
{
    StructWithArray structWithArray {
        .foo = num
    };
    for (size_t i = 0; i < num && i < ARRAY_LEN; ++i) {
        structWithArray.barArray[i] = g_int;
    }
    return structWithArray;
}

void PolluteData(DataInfo &dataInfo)
{
    *(reinterpret_cast<size_t*>(dataInfo.data + dataInfo.length - sizeof(size_t))) = 0U;
}

void BasicTypesCheck(bool normalMode = true)
{
    DataInfo dataInfo {};
    int retCode = EncdecFacade::ProcessEncode(dataInfo, g_int, g_char, g_float, g_long, g_string, g_emptyString);
    MallocPointerGuard<unsigned char> dataInfoGuard(dataInfo.data);
    EXPECT_EQ(retCode, RETCODE_SUCCESS);
    if (!normalMode) {
        PolluteData(dataInfo);
    }
    int outInt {};
    char outChar {};
    float outFloat {};
    long long outLong {};
    std::string outString {};
    std::string outEmptyString {};

    retCode = EncdecFacade::ProcessDecode(dataInfo, outInt, outChar, outFloat, outLong, outString,
        outEmptyString);
    if (normalMode) {
        EXPECT_EQ(retCode, RETCODE_SUCCESS);
        EXPECT_TRUE(CompareData(g_int, outInt));
        EXPECT_TRUE(CompareData(g_char, outChar));
        EXPECT_TRUE(CompareData(g_float, outFloat));
        EXPECT_TRUE(CompareData(g_long, outLong));
        EXPECT_TRUE(CompareData(g_string, outString));
        EXPECT_TRUE(CompareData(g_emptyString, outEmptyString));
    } else {
        EXPECT_NE(retCode, RETCODE_SUCCESS);
    }
}

void StructCheck(bool normalMode = true)
{
    DataInfo dataInfo {};
    StructWithPadding structWithPadding = GenStructWithPadding(g_int, g_char);
    NestedStruct nestedStruct = GenNestedStruct();
    StructWithArray structWithArray = GenStructWithArray(ARRAY_LEN);

    int retCode = EncdecFacade::ProcessEncode(dataInfo, structWithPadding, nestedStruct, structWithArray);
    MallocPointerGuard<unsigned char> dataInfoGuard(dataInfo.data);
    EXPECT_EQ(retCode, RETCODE_SUCCESS);
    if (!normalMode) {
        PolluteData(dataInfo);
    }
    StructWithPadding outStructWithPadding {};
    NestedStruct outNestedStruct {};
    StructWithArray outStructWithArray {};

    retCode = EncdecFacade::ProcessDecode(dataInfo, outStructWithPadding, outNestedStruct, outStructWithArray);
    if (normalMode) {
        EXPECT_EQ(retCode, RETCODE_SUCCESS);
        EXPECT_TRUE(CompareData(structWithPadding, outStructWithPadding));
        EXPECT_TRUE(CompareData(nestedStruct, outNestedStruct));
        EXPECT_TRUE(CompareData(structWithArray, outStructWithArray));
    } else {
        EXPECT_NE(retCode, RETCODE_SUCCESS);
    }
}

void StructWithPointerCheck(bool normalMode = true)
{
    for (size_t classNum = 0; classNum <= ARRAY_LEN; ++classNum) {
        HILOGD ("[Test]Normal test StructWithPointer, classNum = %d.************", classNum);
        StructWithPointer structWithPointer = GenStructWithPointer(classNum);
        DataInfo dataInfo {};
        int retCode = EncdecFacade::ProcessEncode(dataInfo, structWithPointer);
        MallocPointerGuard<unsigned char> dataInfoGuard(dataInfo.data);
        EXPECT_EQ(retCode, RETCODE_SUCCESS);
        if (!normalMode) {
            PolluteData(dataInfo);
        }
        StructWithPointer res {};
        retCode = EncdecFacade::ProcessDecode(dataInfo, res);
        if (normalMode) {
            EXPECT_EQ(retCode, RETCODE_SUCCESS);
            EXPECT_TRUE(CompareData(structWithPointer, res));
            ReleaseStructWithPointer(res);
        } else {
            EXPECT_NE(retCode, RETCODE_SUCCESS);
        }
        ReleaseStructWithPointer(structWithPointer);
    }
}
}

namespace OHOS {
namespace AI {
template<>
int DataEncoder::EncodeOneParameter(const StructWithPointer &val)
{
    if (RecursiveEncode(val.objectsNum) != RETCODE_SUCCESS) {
        HILOGE("[EncdecTest]Serialize memory error.");
        return RETCODE_FAILURE;
    }
    for (size_t i = 0; i < val.objectsNum; ++i) {
        if (RecursiveEncode(val.objects[i]) != RETCODE_SUCCESS) {
            HILOGE("[EncdecTest]Serialize memory error.");
            return RETCODE_FAILURE;
        }
    }
    return RETCODE_SUCCESS;
}

template<>
int DataDecoder::DecodeOneParameter(StructWithPointer &val)
{
    if (RecursiveDecode(val.objectsNum) != RETCODE_SUCCESS) {
        HILOGE("[EncdecTest]Unserialize failed.");
        return RETCODE_FAILURE;
    }
    if (val.objectsNum > 0) {
        AIE_NEW(val.objects, StructWithPadding[val.objectsNum]);
        for (size_t i = 0; i < val.objectsNum; ++i) {
            if (RecursiveDecode(val.objects[i]) != RETCODE_SUCCESS) {
                AIE_DELETE_ARRAY(val.objects);
                return RETCODE_FAILURE;
            }
        }
    }
    return RETCODE_SUCCESS;
}
} // namespace AI
} // namespace OHOS

class EncdecTest : public testing::Test {
public:
    // SetUpTestCase:The preset action of the test suite is executed before the first TestCase
    static void SetUpTestCase() {};

    // TearDownTestCase:The test suite cleanup action is executed after the last TestCase
    static void TearDownTestCase() {};

    // SetUp:Execute before each test case
    void SetUp() override {};

    // TearDown:Execute after each test case
    void TearDown() override {};
};

/**
 * @tc.name: EncdecNormalCheck001
 * @tc.desc: Test encode decode function for non-pointer type in normal mode.
 * @tc.type: FUNC
 * @tc.require: AR000F77MR
 */
HWTEST_F(EncdecTest, EncdecNormalCheck001, TestSize.Level0)
{
    HILOGD ("**********[Test]Normal test start, all the result should return 0************");
    BasicTypesCheck();
    HILOGD ("**********[Test]Normal test end************");
}

/**
 * @tc.name: EncdecNormalCheck002
 * @tc.desc: Test encode decode function for struct without pointer.
 * @tc.type: FUNC
 * @tc.require: AR000F77MR
 */
HWTEST_F(EncdecTest, EncdecNormalCheck002, TestSize.Level0)
{
    HILOGD ("**********[Test]Normal test start, all the result should return 0************");
    StructCheck();
    HILOGD ("**********[Test]Normal test end************");
}

/**
 * @tc.name: EncdecNormalCheck003
 * @tc.desc: Test encode decode function for types which include pointers in normal mode.
 * @tc.type: FUNC
 * @tc.require: AR000F77MR
 */
HWTEST_F(EncdecTest, EncdecNormalCheck003, TestSize.Level0)
{
    HILOGD ("**********[Test]Normal test start, all the result should return 0************");
    StructWithPointerCheck();
    HILOGD ("**********[Test]Normal test end************");
}

/**
 * @tc.name: EncdecAbnormalCheck001
 * @tc.desc: Test encode decode function for non-pointer type in abnormal mode.
 * @tc.type: FUNC
 * @tc.require: AR000F77MR
 */
HWTEST_F(EncdecTest, EncdecAbnormalCheck001, TestSize.Level0)
{
    HILOGD ("**********[Test]abnormal test start************");
    BasicTypesCheck(false);
    HILOGD ("**********[Test]abnormal test end************");
}

/**
 * @tc.name: EncdecAbnormalCheck002
 * @tc.desc: Test encode decode function for types which include pointers in abnormal mode.
 * @tc.type: FUNC
 * @tc.require: AR000F77MR
 */
HWTEST_F(EncdecTest, EncdecAbnormalCheck002, TestSize.Level0)
{
    HILOGD ("**********[Test]abnormal test start************");
    StructCheck(false);
    HILOGD ("**********[Test]abnormal test end************");
}

/**
 * @tc.name: EncdecAbnormalCheck003
 * @tc.desc: Test encode decode function for types which include pointers in normal mode.
 * @tc.type: FUNC
 * @tc.require: AR000F77MR
 */
HWTEST_F(EncdecTest, EncdecAbnormalCheck003, TestSize.Level0)
{
    HILOGD ("**********[Test]Normal test start, all the result should return 0************");
    StructWithPointerCheck(false);
    HILOGD ("**********[Test]Normal test end************");
}
