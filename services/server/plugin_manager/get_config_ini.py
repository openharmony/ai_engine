#!/usr/bin/env python
# -*- coding: utf-8 -*-
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
#

import sys
import os
import traceback

try:
    from configparser import ConfigParser as cfgParser
    USE_ENCODING = True
except Exception:
    from ConfigParser import ConfigParser as cfgParser
    USE_ENCODING = False

INI_LICENSE = """;
; Copyright (c) 2021 Huawei Device Co., Ltd.
; Licensed under the Apache License, Version 2.0 (the "License");
; you may not use this file except in compliance with the License.
; You may obtain a copy of the License at
;
;     http://www.apache.org/licenses/LICENSE-2.0
;
; Unless required by applicable law or agreed to in writing, software
; distributed under the License is distributed on an "AS IS" BASIS,
; WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
; See the License for the specific language governing permissions and
; limitations under the License.
;

"""
AIE_INI_CONFIG_FOLDER = \
    "foundation/ai/engine/services/common/protocol/" \
    "plugin_config/plugin_config_ini"
# Ini config file relative path in build folder
OUT_INI_CONFIG_PATH = "etc/ai_engine_plugin.ini"
# Ini config file relative path in out folder
FAIL_CODE = -1
PARA_NUM = 4
BASE_SESSION = "base"
SUPPORTED_BOARDS = "supported_boards"
RELATED_SESSIONS = "related_sessions"
ALL_BOARDS = "ALL"
DELIMITER = ","


class ConfigParserUper(cfgParser):
    def optionxform(self, optionstr):
        return optionstr


def get_ini_object(config_file_path, encoding=None):
    """
    Get the config in ini file
    getConfig().get("base", "userDataPath")
    :param config_file_path:
    :return:
    """
    config = ConfigParserUper()
    if not USE_ENCODING:
        config.read(config_file_path)
        return config
    all_encodings = ["gbk", "utf-8-sig", "utf-8"]
    if encoding is None:
        for code in all_encodings:
            try:
                config.read(config_file_path, encoding=code)
                break
            except Exception as exception:
                if not "codec can't decode byte " in str(exception):
                    raise exception
                if code == all_encodings[-1]:
                    print(traceback.format_exc())
                    print(repr(exception))
                    raise Exception("Unknown encoding(%s)" % config_file_path)
    else:
        config.read(config_file_path, encoding=encoding)
    return config


class IniManager:
    def __init__(self):
        pass

    def get_files_in_folder(self, folder_path):
        """
        Get all file path in folder
        :return: file path list
        """
        file_path_list = []
        for path in os.listdir(folder_path):
            file_path_list.append(os.path.join(folder_path, path))
        return file_path_list

    def get_config_ini(self, file_path_list, board_name):
        """
        Generate ini file
        :param file_path_list: Input files
        :param board_name: Board name
        :return: ini config object
        """
        cfg = ConfigParserUper()
        for ini_file_path in file_path_list:
            if not ini_file_path.endswith(".ini"):
                continue
            origin_cfg = get_ini_object(ini_file_path)
            if not origin_cfg.has_section(BASE_SESSION):
                continue
            if not origin_cfg.has_option(BASE_SESSION, SUPPORTED_BOARDS) or \
                not origin_cfg.has_option(BASE_SESSION, RELATED_SESSIONS):
                continue
            supported_boards = origin_cfg.get(BASE_SESSION, SUPPORTED_BOARDS)
            related_sessions = origin_cfg.get(BASE_SESSION, RELATED_SESSIONS)
            legal_boards = supported_boards.split(DELIMITER)
            if board_name not in legal_boards and \
                ALL_BOARDS not in legal_boards:
                continue
            for session in related_sessions.split(DELIMITER):
                cfg.add_section(session)
                for key in origin_cfg.options(session):
                    val = origin_cfg.get(session, key)
                    cfg.set(session, key, val)
        return cfg

    def copy_config_ini(self, build_folder, out_folder, board_name):
        """
        Copy needed config ini in AIE_INI_CONFIG_FOLDER
        :param build_folder: The root path of build scripts
        :param out_folder: The out path for all outputs
        :param board_name: The board name
        :return:
        """

        config_folder = os.path.join(build_folder, AIE_INI_CONFIG_FOLDER)
        out_config_path = os.path.join(out_folder, OUT_INI_CONFIG_PATH)
        ini_file_list = self.get_files_in_folder(config_folder)
        ini_cfg_obj = self.get_config_ini(ini_file_list, board_name)
        out_config_folder = os.path.dirname(out_config_path)
        if not os.path.exists(out_config_folder):
            os.mkdir(out_config_folder)
        with open(out_config_path, mode="w") as file_object:
            ini_cfg_obj.write(file_object)
        with open(out_config_path, "r+") as file_object:
            ini_content = file_object.read()
            file_object.seek(0)
            file_object.write(INI_LICENSE)
            file_object.write(ini_content)


if __name__ == "__main__":
    if len(sys.argv) != PARA_NUM:
        print("[ERROR]The input para number is not correct!")
        print("usage: copy_config_ini.py  [build_dir] [out_dir] [board_name]")
        sys.exit(FAIL_CODE)
    else:
        BUILD_PATH, OUT_PATH, BOARD_NAME = sys.argv[1:]
        INI_MANAGER = IniManager()
        INI_MANAGER.copy_config_ini(BUILD_PATH, OUT_PATH, BOARD_NAME)

