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
from copy import deepcopy
from typing import List
from clang import cindex
from Config.BaseConfig import BaseConfig
from Util.CursorHelper import CursorHelper
from Util.DocHelper import lua_typename_from_natve, parse_function_comment, add_lua_type, LUA_KEYWORDS


class Type(object):
    """类，结构体，枚举，方法，成员变量等的基类。"""

    def __init__(self, cursor, generator: BaseConfig, using: "Wrapper" = None) -> None:
        object.__init__(self)
        self._cursor: cindex.Cursor = cursor
        self._generator = generator
        # 生成的c++实现部分字符串。
        self._impl = ""
        self._decl = ""
        self._doc = ""
        # 原始名。
        self._name = CursorHelper.GetName(cursor)
        # 原始全名。
        if not using:
            self._wholeName = CursorHelper.GetWholeName(cursor)
        else:
            self._wholeName = CursorHelper.GetWholeName(using._cursor) + "::" + self._name
        # 直接名。
        if not using:
            self._directName = CursorHelper.GetDirectName(cursor)
        else:
            self._directName = CursorHelper.GetDirectName(using._cursor) + "::" + self._name
        # 所属类名。
        if not using:
            self._classesName = CursorHelper.GetClassesName(cursor)
        else:
            self._classesName = CursorHelper.GetDirectName(using._cursor)
        # 前缀名。
        if not using:
            self._prefixName = CursorHelper.GetPrefixName(cursor)
        else:
            self._prefixName = CursorHelper.GetPrefixName(using._cursor) + "::" + using._name
        # 新名字，可能在继承类中更改（此处直接等于原始名）。
        self._newName = self._name
        # 该对象的所属类是否是可生成的。
        # 如果该对象属于某个类/结构体，那么一旦可以确定该类/结构体不可生成，
        # 即该对象也一定不能生成；
        # 如果该对象不属于某个类/结构体，则暂时认为是可生成的，
        # 直到在继承类中判断是否真的需要生成。
        self._generatable = generator.Generatable(self._classesName) if self._classesName else True

        if self._generatable:
            nameSpace = CursorHelper.GetNameSpace(cursor if not using else using._cursor)
            if nameSpace:
                # 检查是否位于可生成的命名空间中。
                # 有时在内部生成缓存对象时，可能绕过命名空间的判断，在此再次限定一次，但不判断没有命名空间的类型。
                self._generatable = nameSpace in generator.NameSpace

        self._comment: str = self.ParseComment(cursor.raw_comment)

    def UsingFor(self, *args):
        """
        为某个类/结构体复制一份使用using语句定义的成员。
        """
        return self.__class__(self._cursor, *args)

    def _Implement(self):
        # 使用空实现。
        return self._impl

    def _Declaration(self):
        # 使用空实现。
        return self._decl

    def _Documentation(self):
        # 使用空实现。
        return self._doc

    @property
    def WholeName(self):
        return self._wholeName

    @property
    def DirectName(self):
        return self._directName

    @property
    def Generatable(self):
        return self._generatable

    @property
    def Generator(self):
        return self._generator

    @property
    def Name(self):
        return self._name

    @property
    def Cursor(self):
        return self._cursor

    @property
    def NewName(self):
        return self._newName

    @property
    def Comment(self):
        return self._comment

    @Comment.setter
    def Comment(self, value):
        self._comment = value

    @property
    def Implement(self):
        if not self._impl:
            self._impl = self._Implement()
        return self._impl

    @property
    def Declaration(self):
        if not self._decl:
            self._decl = self._Declaration()
        return self._decl

    @property
    def Documentation(self):
        if not self._doc:
            self._doc = self._Documentation()
        return self._doc

    def ParseComment(self, comment):
        replaceStr = comment
        if comment is None:
            return ""
        regular_replace_list = [
            ("\r\n", "\n"),
            (r"[ \t\f]*//!", ""),
            (r"[ \t\f]*//", ""),
            (r"[ \t\f]*/\*\*", ""),
            (r"[ \t\f]*/\*", ""),
            (r"[ \t\f]*/", ""),
            (r"\n[ \t\f]*\*[ \t\f]", "\n"),
            (r"\n[ \t\f]*\*\n", "\n\n"),
            (r"\n[ \t\f]*\*$", ""),
            (r"[ \t\f]*[\*]*$", ""),
            (r"^[ \t\f]*", ""),
            ("\n", "\n--- "),
        ]
        for item in regular_replace_list:
            replaceStr = re.sub(item[0], item[1], replaceStr)
        return '--- ' + replaceStr

    def ParseDocType(self, type_name, is_ret=False):
        ns_map = {}
        for k, v in self.Generator.NameSpace.items():
            ns_map[k + '.'] = v + '.'
        t = lua_typename_from_natve(ns_map, type_name, is_ret)
        if t == 'void':
            t = 'any'
        return t


class Exposure(Type):
    """
    外部暴露类型。
    类、结构体、枚举、全局变量、全局函数等属于“暴露的”；
    成员函数、成员变量等属于“非暴露的”。
    """

    def __init__(self, cursor, generator: BaseConfig, using: "Wrapper" = None) -> None:
        Type.__init__(self, cursor, generator, using)

        if self._generatable:
            self._generatable = generator.Generatable(self._directName)
        # 获取可能的重命名。
        self._newName = generator.RenameClass(self._newName)

        wholeName = self._wholeName
        for kNS, vNS in generator.NameSpace.items():
            if wholeName.startswith(kNS + "::"):
                # 将命名空间简化为目标命名空间名。
                wholeName = wholeName.replace(kNS, vNS, 1)
                break

        # 从命名空间开始的一系列名称列表。
        # 该列表中的名字最终会转换为已重命名的/已简化的。
        nameList = wholeName.split("::")
        values = generator.NameSpace.values()
        if nameList[0] not in values:
            # 保证首个名称一定是以命名空间开头的，由于generator.AllowFiles可能存在，
            # 避免无命名空间的类型被生成到全局空间中。
            nameList.insert(0, list(values)[0])
        # 重命名列表中的类名为新名称。
        for idx, name in enumerate(nameList[1:]):
            linkedName = "::".join(nameList[1:idx + 2])
            rename = generator.RenameClass(linkedName)
            # name == name if rename == linkedName else rename
            name = name if rename == linkedName else rename
            nameList[idx + 1] = name

        self._nameList = nameList
        listName = "".join(nameList)
        self._listName = listName
        self._def = None
        self._define = None
        self._called = None
        self.UpdateDefine(self._nameList)

    def UpdateDefine(self, nameList):
        # 暴露在外部的类型所具有的定义代码和调用代码。
        cxxConfig = self._generator.CxxConfig
        tag = self._generator.Tag
        listName = "".join(nameList)
        self._def = cxxConfig.Define.format(tag + "_" + listName)
        self._define = cxxConfig.Extern + self._def + ";"
        self._called = cxxConfig.Called.format(tag + "_" + listName)

    @property
    def NameList(self):
        return self._nameList

    @property
    def Define(self):
        return self._define

    @property
    def Called(self):
        return self._called

    def _Implement(self):
        self.UpdateDefine(self._nameList)
        # cxxConfig = self._generator.CxxConfig
        # define = self._define[len(cxxConfig.Extern):-1]
        define = self._def

        # strProg = ['sol::table pTable = lua["{}"];'.format(self._nameList[0])]
        strProg = ['\tLUA_ENTRY("{}");'.format(self._nameList[0])]
        for parent in self._nameList[1:-1]:
            # strProg.append('pTable = pTable["{}"];'.format(parent))
            strProg.append('\tLUA_ENTRY("{}");'.format(parent))

        progressive = "\n".join(strProg)
        return "".join([
            "{}",
            "{}\n{{{{\n".format(define),
            progressive,
            "\n{}",
            "\n\tLUA_ENTRY_END({});".format(len(self._nameList) - 1),
            "\n\treturn 0;"
            "\n}}"
        ])

    def _Declaration(self):
        return ""


class Wrapper(Exposure):
    """
    包装类型（类/结构体/枚举）。
    """

    def __init__(self, cursor: cindex.Cursor, OG: "BaseConfig | Wrapper", using: "Wrapper" = None) -> None:
        if isinstance(OG, BaseConfig):
            super().__init__(cursor, OG, using)
            self._parent = None
        else:
            super().__init__(cursor, OG._generator, using)
            self._parent = OG
        add_lua_type(self.WholeName, '.'.join(self.NameList))


class Callable(Type):
    """
    可调用类型，成员函数，普通函数等的基类。
    可调用类型可能包含多种实现。
    """

    class Implementation(object):
        """
        实现类型，用于可调用类型。
        实现类型没有名字，也没有实体，总是附加于可调用类型而产生作用。
        """
        __LiteralList = [
            cindex.CursorKind.INTEGER_LITERAL, cindex.CursorKind.FLOATING_LITERAL,
            cindex.CursorKind.IMAGINARY_LITERAL, cindex.CursorKind.STRING_LITERAL,
            cindex.CursorKind.CHARACTER_LITERAL, cindex.CursorKind.CXX_BOOL_LITERAL_EXPR,
            cindex.CursorKind.CXX_NULL_PTR_LITERAL_EXPR, cindex.CursorKind.GNU_NULL_EXPR,
            cindex.CursorKind.CALL_EXPR,
            # An expression that refers to some value declaration, such as a function,
            # varible, or enumerator.
            cindex.CursorKind.DECL_REF_EXPR]

        def __init__(self, callable: "Callable",
                     args: List[str], originArgs: List[str], argNames: List[str],
                     res: str,
                     default: bool,
                     supported: bool) -> None:
            super().__init__()
            self._callable = callable
            self._args = args
            self._originArgs = originArgs
            self._argNames = argNames
            self._result = res
            self._cursor = self._callable.Cursor
            # 该实现是否基于默认参数。
            self._default: bool = default
            self._supported: bool = supported
            self._comment: str = ''
            self._pcomment: dict[str, str] = {}
            #
            docArgNames = []
            for a in argNames:
                if a in LUA_KEYWORDS:
                    docArgNames.append(a + '_')
                else:
                    docArgNames.append(a)
            self._docArgNames = docArgNames

        @classmethod
        def _CheckDefaultArg(cls, paramNode) -> bool:
            """检查是否有默认参数。"""
            for node in paramNode.get_children():
                if node.kind in cls.__LiteralList:
                    return True
                if node.kind == cindex.CursorKind.TEMPLATE_REF:
                    # 跳过模板内的字面值
                    return False
                if cls._CheckDefaultArg(node):
                    return True
            return False

        def __eq__(self, o: "Callable.Implementation") -> bool:
            return self._originArgs == o._originArgs

        def Implement(self, simple=False, noCast=False) -> str:
            """
            参数：
                    simple      是否使用简便生成（直接使用函数指针）。
            """
            if not self.Supported:
                return ""
            implList = []
            if noCast:
                implList.append("&" + self._callable._wholeName)
            elif simple:
                implList.append("static_cast<{}(*)({})>(".format(self._result, ", ".join(self._args)))
                implList.append("&" + self._callable._wholeName)
                implList.append(")")
            else:
                implList.append("[](")
                argList = []
                for idx, arg in enumerate(self._args):
                    argList.append(arg + " " + "arg" + str(idx))
                implList.append(",".join(argList))
                implList.append("){{return ")
                implList.append(self._callable._wholeName)

                argList.clear()
                implList.append("(")
                for idx, arg in enumerate(self._args):
                    pr = arg[-1] == "&" or arg[-1] == "*"
                    if pr:
                        # 使用指针或引用时省略移动语义。
                        argList.append("arg" + str(idx))
                    else:
                        argList.append("std::move(arg" + str(idx) + ")")
                implList.append(",".join(argList))
                implList.append(");}}")
            return "".join(implList)

        def GetArgumentDocs(self):
            doc = []
            pcomment = self.ParamComments
            for t, ori, name in zip(self.Args, self.OriginArgs, self._docArgNames):
                docType = self._callable.ParseDocType(ori)
                if t.startswith('const ') and t.endswith('&'):
                    t = t[6:-1]
                if name in pcomment:
                    doc.append('---@param {} {} @({}) {}'.format(name, docType, t, pcomment[name]))
                else:
                    doc.append('---@param {} {} @({})'.format(name, docType, t))
            return doc

        def GetResultDoc(self):
            retType = 'nil'
            retTypeDirect = CursorHelper.GetArgName(self.Cursor.result_type)
            pcomment = self.ParamComments
            if self.Result != 'void':
                retType = self._callable.ParseDocType(self.Result, True)
            if 'return' in pcomment:
                return '---@return {} @({}) {}'.format(retType, retTypeDirect, pcomment['return'])
            else:
                return '---@return {} @({})'.format(retType, retTypeDirect)

        @property
        def Args(self):
            return self._args

        @property
        def OriginArgs(self):
            return self._originArgs

        @property
        def ArgNames(self):
            return self._argNames

        @property
        def ArgNamesDoc(self):
            return self._docArgNames

        @property
        def Result(self):
            return self._result

        @property
        def Supported(self):
            return self._supported and self.IsGeneratable()

        @property
        def Default(self):
            return self._default

        @property
        def Comment(self):
            return self._comment

        @Comment.setter
        def Comment(self, value):
            self._comment = value

        def AddParamComment(self, key: str, value: str):
            self._pcomment[key] = value

        @property
        def ParamComments(self):
            return self._pcomment

        @property
        def Cursor(self):
            return self._cursor

        def IsGeneratable(self):
            return True

    _ImplType = Implementation

    def __init__(self, cursor, generator: BaseConfig, using: "Wrapper" = None) -> None:
        Type.__init__(self, cursor, generator, using)

        self._implementations = self.CreateImplementations(cursor)

    def CreateImplementations(self, cursor):
        """
        由一个游标，自动创建一组实现。
        一般的，有默认值的函数游标，将生成多个实现。
        """
        retImpl: List[Callable.Implementation] = []
        supported = True
        args: List[str] = []
        originArgs: List[str] = []
        argNames: List[str] = []
        result = "void"
        minArgs = 0
        comment = ''
        pcomment = {}
        clz = self.__class__
        try:
            if cursor.type.is_function_variadic():
                # 不定参不可用。
                supported = False

            for arg in cursor.type.argument_types():
                if arg.kind == cindex.TypeKind.RVALUEREFERENCE:
                    # 右值引用不可用。
                    supported = False
                args.append(CursorHelper.GetArgName(arg))
                originArgs.append(CursorHelper.GetArgName(arg, True))
            argNames = CursorHelper.GetArgSpellings(cursor)
            result = CursorHelper.GetArgName(cursor.result_type, True)
            # comment = self.ParseComment(cursor.raw_comment)
            comment, pcomment = parse_function_comment(cursor.raw_comment)

            # 最小参数数量。
            minArgs = len(args)
            idx = 0
            for node in cursor.get_children():
                if node.kind == cindex.CursorKind.PARM_DECL:
                    if clz._ImplType._CheckDefaultArg(node):
                        # 当最小参数数量小于参数列表数时，表示该实现有默认参数。
                        minArgs = idx
                        break
                    idx = idx + 1

            if cursor.access_specifier == cindex.AccessSpecifier.PRIVATE:
                supported = False
        except:
            supported = False
            args = []
            result = "void"
            minArgs = 0

        # 具有（最大参数数量-最小参数数量+1）种实现。
        # 且参数数量越长的实现在越靠前（sol重载以先查找先匹配的原则决定重载）。
        for index in range(len(args) - minArgs + 1):
            impl = clz._ImplType(
                self,
                args[:len(args) - index],
                originArgs[:len(originArgs) - index],
                argNames[:len(argNames) - index],
                result,
                minArgs < len(args), supported
            )
            impl.Comment = comment
            for k, v in pcomment.items():
                impl.AddParamComment(k, v)
            retImpl.append(impl)

        return retImpl

    def __lshift__(self, p: "cindex.Cursor | Implementation | Callable"):
        if isinstance(p, cindex.Cursor):
            for impl in self.CreateImplementations(p):
                self << impl
        elif isinstance(p, Callable):
            for impl in p._implementations:
                self << impl
        elif isinstance(p, Callable.Implementation):
            if p in self._implementations:
                # 已经有的实现，不需要再次插入。
                return
            else:
                if p._callable._wholeName != self._wholeName:
                    # 名字不同，不允许插入。
                    return

            for idx, impl in enumerate(self._implementations):
                if len(p.Args) >= len(impl.Args):
                    # 按参数数量插入，参数越多的越靠前（为了适应sol的重载规则）。
                    self._implementations.insert(idx, p)
                    return
            self._implementations.append(p)
        return self

    @property
    def Supported(self):
        for impl in self._implementations:
            if impl.Supported:
                return True
        return False

    @property
    def Overload(self):
        # 是否是可重载的（实现数大于1）。
        count = 0
        for impl in self._implementations:
            if impl.Supported:
                count += 1
            if count > 1:
                return True
        return False

    @property
    def Implementations(self):
        return self._implementations

    @property
    def Generatable(self):
        return super().Generatable and self.Supported
