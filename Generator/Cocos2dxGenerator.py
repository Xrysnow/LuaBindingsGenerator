#!/usr/bin/env python3
# Copyright (c) 2021 - wzhengsen

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

import os
from .BaseGenerator import BaseGenerator


class Cocos2dxGenerator(BaseGenerator):
    def __init__(self, outputPath: str = None, clearOldFile: bool = True):
        self.CocosRoot = "D:/LuaSTG-x/frameworks/cocos2d-x"
        # 由当前类名获取生成的文件名。
        suffix = self.__class__.__name__
        if suffix.find(__class__.__name__) == 0:
            suffix = suffix[len(__class__.__name__):]
        fileName = "CCRegLua" + suffix + "Auto"
        if not outputPath:
            # outputPath = "{}/extensions/Lua/auto".format(self.CocosRoot)
            outputPath = os.path.join(os.path.abspath(os.curdir), "out")
        super().__init__(outputPath, fileName, clearOldFile)

        # self.ToNameSpace = "cocos2d"
        self.Tag = suffix
        self.SearchPaths += [
            os.path.join(self.CocosRoot, "cocos"),
            os.path.join(self.CocosRoot, "extensions")
        ]

        self.InstanceMethods |= {
            # ".*": ("(g|G)etInstance", "(d|D)estroyInstance")
        }

        # 解析时的依赖头文件路径等。
        self.ExtraArgs += [
            f"-I{self.CocosRoot}/cocos",
            f"-I{self.CocosRoot}/extensions",
            f"-I{self.CocosRoot}/external/lua/luajit/include",
            "-DANDROID"
        ]

        self.RenameMembers |= {
            # ".*": {"(C|c)reate": "__new"}
        }

        self.SkipMembers |= {
            ".*": ["createInstance", "__Type"]
        }

        self.GetPrefix += ["(G|g)et", "(I|i)s"]
        self.SetPrefix += ["(S|s)et"]
