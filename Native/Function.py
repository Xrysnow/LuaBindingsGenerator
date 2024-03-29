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

from Config.BaseConfig import BaseConfig
from Native.Base import Callable, Exposure, Wrapper
from Util.CursorHelper import CursorHelper
from typing import List


class Function(Exposure, Callable):
    """
    全局函数类型。
    """

    class Implementation(Callable.Implementation):
        def IsGeneratable(self):
            # 跳过不需要的情况
            parent: Function = self._callable
            return parent.Generator.FunctionGeneratable(parent.NameList[0], parent.Name, self.Args)

    _ImplType = Implementation

    def __init__(self, cursor, generator: BaseConfig, using: Wrapper = None) -> None:
        Callable.__init__(self, cursor, generator, using)
        Exposure.__init__(self, cursor, generator, using)
        if self._generator.UpperFunction:
            self._newName = CursorHelper.UpperCamelCase(self._newName)
        elif self._generator.LowerFunction:
            self._newName = CursorHelper.LowerCamelCase(self._newName)
        self._fname = 'lua_' + self._prefixName.replace('::', '_') + '_' + self._newName

    @property
    def FName(self):
        return self._fname

    def _Implement(self):
        """
        全局变量生成。
        按照 命名空间.名字=全局变量 的方式生成。
        """
        if not self.Supported:
            return ''
        strList = []
        luaTName = self._prefixName.replace('::', '.')
        luaFName = '{}.{}'.format(luaTName, self._newName)
        s = [
            'int {}(lua_State* lua_S)'.format(self._fname),
            '{',
            '\tLUA_GINVOKE_HEADER("{}", "{}");'.format(luaTName, luaFName),
            '\t'
        ]
        strList.append('\n'.join(s))
        implList = []
        argc = {}
        noCast = len(self._implementations) == 1
        for idx, impl in enumerate(self._implementations):
            if not impl.Supported:
                continue
            ii = impl.Implement(idx == 0 or not impl.Default, noCast)
            if ii:
                narg = len(impl.Args)
                argc[str(narg)] = True
                if impl.Result == 'void':
                    implList.append('LUA_TRY_INVOKE({}, {});'.format(narg, ii))
                else:
                    implList.append('LUA_TRY_INVOKE_R({}, {});'.format(narg, ii))
        strList.append("\n\t".join(implList))
        expected = list(argc.keys())
        expected.sort()
        strList.append('\n\tLUA_GINVOKE_FOOTER("{}");'.format(','.join(expected)))
        strList.append("\n}")
        return "".join(strList)  # + implStr.format("", reg)

    def _Declaration(self):
        return ''

    def _Documentation(self):
        if not self.Supported:
            return ''
        docs = []
        # luaTName = self._prefixName.replace('::', '.')
        luaTName = self.NameList[0]
        luaFName = '{}.{}'.format(luaTName, self._newName)
        for idx, impl in enumerate(self._implementations):
            if not impl.Supported:
                continue
            ii = impl.Implement(idx == 0 or not impl.Default)
            if not ii:
                continue
            comment = impl.Comment
            #
            doc = []
            if comment:
                doc.append(comment)
            doc += impl.GetArgumentDocs()
            doc.append(impl.GetResultDoc())
            args = ', '.join(impl.ArgNamesDoc)
            doc.append('function {}({})\nend'.format(luaFName, args))
            docs.append('\n'.join(doc))
        return '\n\n'.join(docs)

    @property
    def Define(self):
        return ''

    @property
    def Called(self):
        return ''


class FunctionCollection(Exposure):

    def __init__(self, functions: list[Function], using: Wrapper = None) -> None:
        Exposure.__init__(self, functions[0].Cursor, functions[0].Generator, using)
        self._functions = functions.copy()
        self._functions.sort(key=lambda x: x.NewName)
        cxxConfig = self._generator.CxxConfig
        tag = self._generator.Tag
        self._nameList = [self._nameList[0], '_global_functions']
        name = self.NameList[0] + '_global_functions'
        self.UpdateDefine([name])
        self._fname = cxxConfig.Func.format(tag + '_' + name)

    def _Implement(self):
        """
        全局变量生成。
        按照 命名空间.名字=全局变量 的方式生成。
        """
        implStr = Exposure._Implement(self)
        impl = []
        reg = []
        for f in self._functions:
            if not f.Supported:
                continue
            impl.append(f.Implement)
            reg.append('\tLUA_METHOD("{}", {});'.format(f.NewName, f.FName))
        return '\n'.join(impl) + '\n' + implStr.format("", '\n'.join(reg))

    def _Documentation(self):
        docs = []
        for f in self._functions:
            if not f.Supported:
                continue
            docs.append(f.Documentation)
        return '\n\n'.join(docs)
