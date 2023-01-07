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


import re
from clang import cindex
from Native.Base import Exposure
from Util.CursorHelper import CursorHelper
from Util.DocHelper import lua_typename_from_natve


class Variable(Exposure):
    """
    全局变量类型。
    """

    def __init__(self, cursor, generator):
        super().__init__(cursor, generator)
        self._const = cursor.type.is_const_qualified()
        self._type = CursorHelper.GetArgName(self._cursor.type, True)
        self._pointer = cursor.type.kind == cindex.TypeKind.POINTER
        # const即是readonly
        self._readonly = self._const
        if not self._readonly:
            # 尝试查找手动指定的只读属性。
            for clz, fields in self._generator.ReadOnlyFields.items():
                if re.match("^" + clz + "$", self._classesName):
                    for field in fields:
                        if re.match("^" + field + "$", self._name):
                            self._readonly = True
                            break
                    if self._readonly:
                        break
        name = "_".join(self._nameList)
        self._getterName = name + "_getter"
        self._setterName = name + "_setter"

    def _Implement(self):
        """
        全局变量生成。
        按照 命名空间.名字=全局变量 的方式生成。
        """
        define = self.Generator.CxxConfig.Define
        hintName = ".".join(self._nameList)
        getterHeader = define.format(self._getterName) + "\n{"
        getter = [getterHeader]
        getter.append("LUA_CUR_FNAME(\"{}\");".format(hintName + "/getter"))
        getter.append("LUA_PUSH_NATIVE({});".format(self._wholeName))
        getter.append("return 1;")
        getterImpl = "\n\t".join(getter) + "\n}"
        setterImpl = ""
        if not self._readonly:
            setterHeader = define.format(self._setterName) + "\n{"
            setter = [setterHeader]
            setter.append("LUA_CUR_FNAME(\"{}\");".format(hintName + "/setter"))
            setter.append("LUA_NATIVE_SETTER({}, 1);".format(self._wholeName))
            setter.append("return 0;")
            setterImpl = "\n\t".join(getter) + "\n}"

        impl = [
            self._def + '\n{',
        ]
        for parent in self._nameList[:-1]:
            impl.append('LUA_ENTRY("{}");'.format(parent))

        func = self.Generator.CxxConfig.Func
        getter = func.format(self._getterName)
        setter = "nullptr"
        if not self._readonly:
            setter = func.format(self._setterName)

        impl.append('LUA_VARIABLE("{}", {}, {})'.format(self._newName, getter, setter))

        impl.append("LUA_ENTRY_END({});".format(len(self._nameList) - 1))
        implStr = '\n' + '\n\t'.join(impl) + '\n}'
        return getterImpl + setterImpl + implStr

    def _Declaration(self):
        called = self.Generator.CxxConfig.Called
        getter = called.format(self._getterName)
        setter = "nullptr"
        if not self._readonly:
            setter = called.format(self._setterName)
        return '// LUA_VARIABLE("{}", {}, {});'.format(self._newName, getter, setter)

    def _Documentation(self):
        docs = []
        luaName = ".".join(self._nameList)
        luaType = self.ParseDocType(self._type)
        if self.Comment:
            docs.append(self.Comment)
        if self._readonly:
            docs.append('---@type {} ({}, readonly)'.format(luaType, self._type))
        else:
            docs.append('---@type {} ({})'.format(luaType, self._type))
        return '\n'.join(docs) + '\n{} = nil'.format(luaName)
