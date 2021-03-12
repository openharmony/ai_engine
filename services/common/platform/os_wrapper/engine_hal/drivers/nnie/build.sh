#!/bin/bash
# Copyright (c) 2021 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
set -e
OUT_DIR="$1"
BOARD_NAME=$(echo $OUT_DIR | awk -F "/" '{print $(NF-1)}')

function main() {
    ROOT_DIR=$(cd $(dirname "$0");pwd)
    cd "$ROOT_DIR"
    cp -rf $BOARD_NAME/libnnie_adapter.so $OUT_DIR/
    SOURCE_LIB="$OUT_DIR/../../../device/hisilicon/$BOARD_NAME/sdk_liteos/mpp/lib"
    cp -rf $SOURCE_LIB/libnnie.so $OUT_DIR/
    cp -rf $SOURCE_LIB/libmpi.so $OUT_DIR/
    cp -rf $SOURCE_LIB/libdnvqe.so $OUT_DIR/
    cp -rf $SOURCE_LIB/libupvqe.so $OUT_DIR/
    cp -rf $SOURCE_LIB/libVoiceEngine.so $OUT_DIR/
}

