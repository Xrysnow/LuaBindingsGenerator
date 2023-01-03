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

from clang import cindex
from Util.CursorHelper import CursorHelper
from Config.BaseConfig import BaseConfig
from Native.Base import Wrapper
from Util.DocHelper import LUA_KEYWORDS


class Enum(Wrapper):
    """枚举类型。"""

    def _GetKeyValue(self, useValue: bool = False, rename: bool = True) -> dict:
        """获取枚举的键值对字典。
        参数：
            useValue        是否直接使用字面值。
        """
        kv = {}
        for node in self._cursor.get_children():
            name = node.displayname
            name = name if not rename else self._generator.RenameMember(self._directName, name)
            name = self._generator.RenameEnumEntry(self._directName, name)
            comment = self.ParseComment(node.raw_comment)
            if not useValue:
                kv[name] = [CursorHelper.GetWholeName(node), comment]
            else:
                kv[name] = [node.enum_value, comment]
        return kv


class NamedEnum(Enum):
    """具名枚举。"""

    def _Implement(self):
        """
        具名枚举的生成。
        依照 命名空间.类型名.类型名...枚举名{...};的方式生成。
        """
        implStr = super()._Implement()
        kvMap = self._GetKeyValue()
        strList = []
        strList.append('\tLUA_ENUM_DEF("{}");'.format(self._newName))
        for key, value in kvMap.items():
            strList.append('\tLUA_ENUM_ENTRY("{}", {});'.format(key, str(value[0])))
        strList.append('\tLUA_ENUM_END();')

        if self._parent:
            s = [
                '{}\n{{'.format(self._def),
                '\n'.join(strList),
                '\treturn 0;',
                '}'
            ]
            return '\n'.join(s)
        return implStr.format("", '\n'.join(strList))

    def _Declaration(self):
        return "// NamedEnum Decl {}".format(self._newName)

    def _Documentation(self):
        kvMap = self._GetKeyValue(True)
        docs = []
        docs.append('local {} = {{'.format(self._newName))
        for key, value in kvMap.items():
            key: str
            is_idt = key.isascii() and (key[0].isalpha() or key[0] == '_')
            is_idt = is_idt and (key not in LUA_KEYWORDS)
            if value[1]:
                docs.append('\t{}'.format(value[1]))
            if is_idt:
                docs.append('\t{} = {},'.format(key, str(value[0])))
            else:
                docs.append('\t["{}"] = {},'.format(key, str(value[0])))
        docs.append('}')
        docs.append(self.Comment if self.Comment else '---')
        docs.append('---@class {}'.format('.'.join(self._nameList)))
        if self._parent:
            docs.append('{}.{} = {}'.format(self._parent.NewName, self._newName, self._newName))
        else:
            docs.append('{} = {}'.format('.'.join(self._nameList), self._newName))
        return '\n'.join(docs)


class AnonymousEnum(Enum):
    """匿名枚举类型。"""

    def __init__(self, cursor: cindex.Cursor, OG: "BaseConfig | Wrapper", using: "Wrapper" = None) -> None:
        super().__init__(cursor, OG, using)
        # 匿名枚举追加一个生成的名字。
        aeName = CursorHelper.AssumeEnumName(cursor)
        self._name = aeName
        self._newName = aeName
        self._nameList[-1] = aeName

        generator = self._generator
        if generator.AllowAnonymous == BaseConfig.AnonymousType.Ban:
            self._generatable = False
        elif generator.AllowAnonymous == BaseConfig.AnonymousType.All:
            self._generatable = True
        elif generator.AllowAnonymous == BaseConfig.AnonymousType.Class:
            if self._parent:
                self._generatable = True
        elif generator.AllowAnonymous == BaseConfig.AnonymousType.Global:
            if not self._parent:
                self._generatable = True

    def _Implement(self):
        """
        匿名枚举的生成。
        依照 命名空间.类型名...类型名{...};的方式生成。
        """
        implStr = super()._Implement()

        kvMap = self._GetKeyValue(rename=False)
        strList = []
        for key, value in kvMap.items():
            strList.append('\tLUA_CONSTANT("{}", {});'.format(key, str(value[0])))
        if self._parent:
            s = [
                '{}\n{{'.format(self._def),
                '\n'.join(strList),
                '\treturn 0;',
                '}'
            ]
            return '\n'.join(s)
        return implStr.format("", '\n'.join(strList))

    def _Declaration(self):
        return "// AnonymousEnum Decl {}".format(self._newName)

    def _Documentation(self):
        kvMap = self._GetKeyValue(True)
        docs = []
        if self._parent:
            pname = self._parent.NewName
        else:
            pname = '.'.join(self._nameList[:-1])
        for key, value in kvMap.items():
            key: str
            is_idt = key.isascii() and (key[0].isalpha() or key[0] == '_')
            is_idt = is_idt and (key not in LUA_KEYWORDS)
            if value[1]:
                docs.append('{}'.format(value[1]))
            if is_idt:
                docs.append('{}.{} = {}'.format(pname, key, str(value[0])))
            else:
                docs.append('{}["{}"] = {}'.format(pname, key, str(value[0])))
        return '\n'.join(docs)
