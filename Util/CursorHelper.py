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

import hashlib
from enum import Enum
from clang import cindex


class CursorHelper:
    """一个获取cindex.TranslationUnit.cusor信息的类。"""

    class Availability(Enum):
        """指示游标的可用性。"""
        AVAILABLE = 0
        DEPRECATED = 1
        NOT_AVAILABLE = 2
        NOT_ACCESSIBLE = 3

    @staticmethod
    def _BuildNameSpace(cursor, includeClassName: bool = True, nameSpace: list = None) -> list:
        """构建命名空间到所属类名的列表。"""
        if None is nameSpace:
            nameSpace = []
        parent = cursor.semantic_parent
        if parent:
            if parent.kind == cindex.CursorKind.NAMESPACE or \
                    (parent.kind == cindex.CursorKind.CLASS_DECL or
                     parent.kind == cindex.CursorKind.STRUCT_DECL or
                     parent.kind == cindex.CursorKind.ENUM_DECL):
                CursorHelper._BuildNameSpace(parent, includeClassName, nameSpace)
                if parent.displayname and (
                        parent.kind == cindex.CursorKind.NAMESPACE or includeClassName) and parent.displayname != "__ndk1":
                    nameSpace.append(parent.displayname)
        return nameSpace

    @staticmethod
    def GetAvailability(cursor) -> Availability:
        """获取可用性。"""
        if not hasattr(cursor, "_availability"):
            cursor._availability = cindex.conf.lib.clang_getCursorAvailability(cursor)

        return CursorHelper.Availability(cursor._availability)

    @staticmethod
    def GetName(cursor, useDisplayName=False) -> str:
        """获取名字。
        """
        if useDisplayName and cursor.displayname:
            return cursor.displayname
        cName = cursor.spelling
        if not cName:
            cName = cursor.type.spelling
        if cName:
            # spelling名字中包含(的可能是匿名的。
            if cName.find("(") != -1:
                cName = ""
            else:
                cList = cName.split("::")
                if cList:
                    return cList[len(cList) - 1]
        return cName

    @staticmethod
    def GetParentName(cursor) -> str:
        """获取直接父级的名字。"""
        parent = cursor.semantic_parent
        if parent:
            return CursorHelper.GetName(parent)
        return ""

    @staticmethod
    def GetWholeName(cursor, useDisplayName=False) -> str:
        """获得全名，包含命名空间名和类名。"""
        ns = CursorHelper._BuildNameSpace(cursor)
        ns.append(CursorHelper.GetName(cursor, useDisplayName))
        return "::".join(ns)

    @staticmethod
    def GetDirectName(cursor) -> str:
        """获得直接名，包含类名，但不包含命名空间名。"""
        dn = CursorHelper.GetClassesNameList(cursor)
        dn.append(CursorHelper.GetName(cursor))
        return "::".join(dn)

    @staticmethod
    def GetClassesNameList(cursor, l=None) -> list:
        """获得所属类的全名列表，不包括命名空间名。"""
        if l is None:
            l = []
        parent = cursor.semantic_parent
        if parent and \
                parent.kind == cindex.CursorKind.CLASS_DECL or parent.kind == cindex.CursorKind.STRUCT_DECL:
            name = parent.displayname
            if name != "__ndk1":
                l.insert(0, name)
            CursorHelper.GetClassesNameList(parent, l)
        return l

    @staticmethod
    def GetClassesName(cursor) -> str:
        """获得所属类的全名，不包括命名空间名。"""
        return "::".join(CursorHelper.GetClassesNameList(cursor))

    @staticmethod
    def GetPrefixName(cursor) -> str:
        """获得前缀名，包含命名空间名和所属类名。"""
        return "::".join(CursorHelper._BuildNameSpace(cursor))

    @staticmethod
    def GetNameSpace(cursor) -> str:
        """获得命名空间名。"""
        return "::".join(CursorHelper._BuildNameSpace(cursor, False))

    @staticmethod
    def AssumeEnumName(cursor) -> str:
        """为匿名枚举假设一个名字。"""
        name = CursorHelper.GetName(cursor)
        if cursor.kind != cindex.CursorKind.ENUM_DECL or name:
            return name
        nsName = CursorHelper.GetPrefixName(cursor)
        signList = [nsName]
        for node in cursor.get_children():
            if node.kind == cindex.CursorKind.ENUM_CONSTANT_DECL:
                signList.append(node.displayname)
                signList.append(";")
        retStr = ''.join(signList)
        md5 = hashlib.md5()
        md5.update(retStr.encode())
        return ("_AE_" + md5.hexdigest() + "_")

    @staticmethod
    def GetArgName(ntype, origin: bool = False) -> str:
        """获得参数名，也可用于获得返回值名（获取的是类型名，而不是变量名）。"""

        if ntype.kind == cindex.TypeKind.POINTER:
            return CursorHelper.GetArgName(ntype.get_pointee(), origin) + "*" + (
                " const" if ntype.is_const_qualified() else "")
        elif ntype.kind == cindex.TypeKind.LVALUEREFERENCE:
            return CursorHelper.GetArgName(ntype.get_pointee(), origin) + "&"
        else:
            name = ""
            decl = ntype.get_declaration()
            const = ntype.is_const_qualified()
            if decl.kind == cindex.CursorKind.NO_DECL_FOUND:
                return ntype.spelling.replace("__ndk1::", "")
            elif origin:
                # 获取原始类型名（即通过typedef和using起的别名的原始名称）。
                while (decl.kind == cindex.CursorKind.TYPE_ALIAS_DECL or
                       decl.kind == cindex.CursorKind.TYPEDEF_DECL):
                    typedef = decl.underlying_typedef_type
                    declaration = typedef.get_declaration()
                    if declaration.kind != cindex.CursorKind.NO_DECL_FOUND:
                        decl = declaration
                    else:
                        # 基本类型。
                        name = typedef.spelling
                        break
            if not name:
                name = CursorHelper.GetWholeName(decl, True)
            return (("const " if const else "") + name) \
                .replace("__ndk1::", "") \
                .replace("unsigned long long", "uint64_t") \
                .replace("long long", "int64_t")


    @staticmethod
    def UpperCamelCase(name: str) -> str:
        """如果可以，将参数name转换为大驼峰命名，并返回转换后的内容。
        某些名字不会被转换（如new/delete等）。
        """
        if name in ("new", "delete", "__delete", "__new"):
            return name
        first = -1
        for idx, char in enumerate(name):
            alpha = char.isalpha()
            if alpha:
                first = idx
                break

        if first != -1:
            return name[:first] + name[first].upper() + (name[first + 1:] if first < len(name) - 1 else "")

        return name

    @staticmethod
    def LowerCamelCase(name: str) -> str:
        if name in ("new", "delete", "__delete", "__new"):
            return name
        first = -1
        for idx, char in enumerate(name):
            alpha = char.isalpha()
            if alpha:
                first = idx
                break

        if first != -1:
            return name[:first] + name[first].lower() + (name[first + 1:] if first < len(name) - 1 else "")

        return name

    @staticmethod
    def GetFile(cursor, recursive: bool = False) -> str:
        if recursive:
            parent = None
            while (True):
                parent = cursor.semantic_parent
                if parent:
                    if parent.kind == cindex.CursorKind.TRANSLATION_UNIT:
                        return parent.displayname
                    cursor = parent
                else:
                    break
        else:
            parent = cursor.semantic_parent
            if parent:
                if parent.kind == cindex.CursorKind.TRANSLATION_UNIT:
                    return parent.displayname
        return ""
