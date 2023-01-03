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
from platform import version
from Util.Functions import ExceptImport

try:
    import lupa
except:
    lupa = ExceptImport("lupa")


class CxxConfig(object):

    def __init__(self, luaConfigRoot: str) -> None:
        super().__init__()
        lua = lupa.LuaRuntime(unpack_returned_tuples=True)
        lua.execute('package.path = package.path .. ";{}/?.lua"'.format(luaConfigRoot))
        config, _ = lua.require("OOP.Config")
        self.get: str = config["get"]
        self.set: str = config["set"]
        self.const: str = config["Qualifiers"]["const"]
        self.static: str = config["Qualifiers"]["static"]
        self.new: str = config["__new"]
        self.delete: str = config["__delete"]

        # self.LuaType = "cocos2d::extension::Lua&"
        self.LuaType = "lua_State*"
        self.Extern = "extern "
        # self.Define = "void RegisterLua{}Auto(" + self.LuaType + " lua)"
        # self.Called = "RegisterLua{}Auto(lua);"
        self.Define = "int luaReg_{}(" + self.LuaType + " lua_S)"
        self.Called = "luaReg_{}(lua_S);"
        self.Func = "luaReg_{}"
