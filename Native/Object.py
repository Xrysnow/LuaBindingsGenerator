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
from clang.cindex import AccessSpecifier, Cursor, CursorKind
from typing import Dict, List, Set
from Config.BaseConfig import BaseConfig
from Native.Base import Type, Wrapper, Callable
from Native.Enum import AnonymousEnum, NamedEnum
from Util.CursorHelper import CursorHelper
from Util.DocHelper import LUA_KEYWORDS, add_lua_type


class Object(Wrapper):
    """类，结构体的基类。"""

    class Member(Type):
        """成员函数，成员变量的基类。"""

        def __init__(self, cursor, obj: "Object", using: "Object" = None) -> None:
            super().__init__(cursor, obj._generator, using)
            self._obj = obj if not using else using
            if not self._generatable:
                return
            # 在所属类/结构体将被生成的情况下，获取成员是否可能被跳过。
            self._generatable = self._generator.Generatable(self._classesName, self._name)
            if self._generatable:
                # 重命名以获得新名字。
                self._newName = self._generator.RenameMember(self._classesName, self._name)

    class Variable(Member):
        """类变量类型。"""

        def __init__(self, cursor, obj: "Object", using: "Object" = None) -> None:
            super().__init__(cursor, obj, using)
            if self._generator.UpperField:
                # 尝试获取大驼峰名字。
                self._newName = CursorHelper.UpperCamelCase(self._newName)
            elif self._generator.LowerField:
                self._newName = CursorHelper.LowerCamelCase(self._newName)
            self._const = cursor.type.is_const_qualified()
            self._type = CursorHelper.GetArgName(self._cursor.type)
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
            self._fname = 'lua_' + self._prefixName.replace('::', '_') + '_' + self._newName
            self._getterName = self._fname + '_getter'
            self._setterName = self._fname + '_setter'

    class Field(Variable):
        """成员变量。"""

        def _Implement(self):
            """
            成员变量的生成。
            只读成员按照：
                mt["get"]["self._newName"] = static_cast<const type Class::*>(&Class::member);
            可写成员按照：
                mt["set"]["self._newName"] = &Class::member;
            """
            cxxConfig = self._generator.CxxConfig
            hintName = '.'.join(self._obj.NameList) + '.' + self.NewName
            s = [
                'int {}({} lua_S)'.format(self._getterName, cxxConfig.LuaType),
                '{',
                '\tLUA_CUR_FNAME("{}/getter");'.format(hintName),
                '\tauto cobj = LUA_TO_COBJ({}*, 1);'.format(self._obj.WholeName),
                '\tLUA_PUSH_NATIVE(cobj->{});'.format(self._name),
                '\treturn 1;',
                '}',
            ]
            ret = "\n".join(s)
            if not self._readonly:
                s = [
                    'int {}({} lua_S)'.format(self._setterName, cxxConfig.LuaType),
                    '{',
                    '\tLUA_CUR_FNAME("{}/setter");'.format(hintName),
                    '\tauto cobj = LUA_TO_COBJ({}*, 1);'.format(self._obj.WholeName),
                    '\tLUA_NATIVE_SETTER(cobj->{}, 2);'.format(self._name),
                    '\treturn 0;',
                    '}',
                ]
                ret += "\n{}".format('\n'.join(s))
            return ret

        def _Declaration(self):
            if self._readonly:
                return 'LUA_FIELD("{}", {}, nullptr);'.format(self._newName, self._getterName)
            return 'LUA_FIELD("{}", {}, {});'.format(self._newName, self._getterName, self._setterName)

        def _Documentation(self):
            docs = []
            luaName = self._obj.NewName + '.' + self.NewName
            luaType = self.ParseDocType(self._type)
            if self.Comment:
                docs.append(self.Comment)
            if self._readonly:
                docs.append('---@type {} ({}, readonly)'.format(luaType, self._type))
            else:
                docs.append('---@type {} ({})'.format(luaType, self._type))
            return '\n'.join(docs) + '\n{} = nil'.format(luaName)

    class StaticField(Variable):
        """静态成员变量。"""

        def _Implement(self):
            """
            静态成员变量的生成。
            依照 mt["get"|"set"]["self._newName"] = [](...){...};的方式生成。
            """
            cxxConfig = self._generator.CxxConfig
            hintName = '.'.join(self._obj.NameList) + '.' + self.NewName
            s = [
                'int {}({} lua_S)'.format(self._getterName, cxxConfig.LuaType),
                '{',
                '\tLUA_CUR_FNAME("{}/getter");'.format(hintName),
                '\tLUA_PUSH_NATIVE({});'.format(self.WholeName),
                '\treturn 1;',
                '}',
            ]
            ret = "\n".join(s)
            if not self._readonly:
                s = [
                    'int {}({} lua_S)'.format(self._setterName, cxxConfig.LuaType),
                    '{',
                    '\tLUA_CUR_FNAME("{}/setter");'.format(hintName),
                    '\tLUA_NATIVE_SETTER({}, 2);'.format(self.WholeName),
                    '\treturn 0;',
                    '}',
                ]
                ret += "\n{}".format('\n'.join(s))
            return ret

        def _Declaration(self):
            if self._readonly:
                return 'LUA_SFIELD("{}", {}, nullptr);'.format(self._newName, self._getterName)
            return 'LUA_SFIELD("{}", {}, {});'.format(self._newName, self._getterName, self._setterName)

        def _Documentation(self):
            docs = []
            luaName = self._obj.NewName + '.' + self.NewName
            luaType = self.ParseDocType(self._type)
            if self.Comment:
                docs.append(self.Comment)
            if self._readonly:
                docs.append('---@type {} ({}, static, readonly)'.format(luaType, self._type))
            else:
                docs.append('---@type {} ({}, static)'.format(luaType, self._type))
            return '\n'.join(docs) + '\n{} = nil'.format(luaName)

    class Function(Member, Callable):
        def __init__(self, cursor, obj: "Object", using: "Object" = None) -> None:
            Callable.__init__(self, cursor, obj._generator, using)
            Object.Member.__init__(self, cursor, obj, using)
            self._using = using
            if self._generator.UpperMethod:
                # 尝试获取大驼峰名字。
                self._newName = CursorHelper.UpperCamelCase(self._newName)
            elif self._generator.LowerMethod:
                self._newName = CursorHelper.LowerCamelCase(self._newName)
            self._fname = 'lua_' + self._prefixName.replace('::', '_') + '_' + self._newName

        def _Property(self):
            """
            由自己的名字决定自己是否可以转化为set或get修饰的属性。
            返回：
                    str     "get"或"set"或None；
                    str     去除get或set剩余的属性名。
                    impl    最后匹配的实现。
            """
            if self._implementations and self._generator.AutoProperties:
                newName = self._newName
                # 取最后的那个实现（为了适应sol的重载规则，越靠后的参数越少）。
                lastImpl = self._implementations[-1]

                # get属性的实现至少保证返回值不为空且不需要参数（这是前置条件）。
                if lastImpl.Result != "void" and len(lastImpl.Args) == 0:
                    # 再判断前缀是否符合。
                    for prefix in self._generator.GetPrefix:
                        m = re.match("^" + prefix + "(.*)$", newName)
                        if m:
                            return self._generator.CxxConfig.get, m.group(2), lastImpl

                # set属性参数长度为1，且具有匹配的set前缀。
                if len(lastImpl.Args) == 1:
                    for prefix in self._generator.SetPrefix:
                        m = re.match("^" + prefix + "(.*)$", newName)
                        if m:
                            return self._generator.CxxConfig.set, m.group(2), lastImpl
            return None, None, None

        def _Implement(self):
            implStr = super()._Implement()
            strList = []
            luaTName = ".".join(self._obj._nameList)
            luaFName = '{}:{}'.format(luaTName, self._newName)
            template = [
                'int {}(lua_State* lua_S)'.format(self._fname),
                '{',
                '\tLUA_INVOKE_HEADER("{}", "{}");'.format(luaTName, luaFName),
                '\t'
            ]
            strList.append('\n'.join(template))

            implList = []
            argc = {}
            noCast = len(self._implementations) == 1
            for idx, impl in enumerate(self._implementations):
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
            strList.append('\n\tLUA_INVOKE_FOOTER("{}");'.format(','.join(expected)))
            strList.append("\n}")
            return implStr + "".join(strList)

        def _Declaration(self):
            return 'LUA_METHOD("{}", {});'.format(self._newName, self._fname)

        def _Doc(self, isStatic, isCtor):
            docs = []
            luaTName = self._obj.NewName
            luaFName = '{}:{}'.format(luaTName, self._newName)
            for idx, impl in enumerate(self._implementations):
                ii = impl.Implement(idx == 0 or not impl.Default)
                if not ii:
                    continue
                comment = impl.Comment
                #
                doc = []
                if comment:
                    doc.append(comment)
                if isStatic:
                    doc.append('--- (static)')
                if isCtor:
                    ctorDesc = 'constructor'
                    if self._cursor.is_default_constructor():
                        ctorDesc += ', default'
                    if self._cursor.is_copy_constructor():
                        ctorDesc += ', copy'
                    if self._cursor.is_move_constructor():
                        ctorDesc += ', move'
                    if self._cursor.is_converting_constructor():
                        ctorDesc += ', convert'
                    doc.append('--- ({})'.format(ctorDesc))
                doc += impl.GetArgumentDocs()
                doc.append(impl.GetResultDoc())
                args = ', '.join(impl.ArgNamesDoc)
                doc.append('function {}({})\nend'.format(luaFName, args))
                docs.append('\n'.join(doc))
            return '\n\n'.join(docs)

        def UsingFor(self, *args):
            f = super().UsingFor(*args)
            f._implementations = []
            for impl in self._implementations:
                f._implementations.append(self._ImplType(f, impl._args, impl._originArgs,
                                                         impl._result, impl._default, impl._supported))
            return f

        def Clone(self):
            """
            有时，需要在不同的不相关的数据结构中保存方法。
            避免修改其中一个引起所有引用数据的改变，通常使用保存克隆的方式。
            """
            return self.__class__(self._cursor, self._obj, self._using)

    class Method(Function):
        """成员函数。"""

        class Implementation(Callable.Implementation):
            """成员函数的实现类。"""

            def __init__(self, callable: "Callable", args: List[str], originArgs: List[str], argNames: List[str],
                         res: str, default: bool,
                         supported: bool) -> None:
                super().__init__(callable, args, originArgs, argNames, res, default, supported)
                cursor = callable.Cursor
                self._virtual: bool = cursor.is_virtual_method()
                self._pureVirtual: bool = cursor.is_pure_virtual_method()
                self._const = cursor.is_const_method()

            def IsOverridden(self, other: "Object.Method") -> bool:
                """
                判断当前实现是否重写了另一方法中的某个实现。
                返回：
                        bool    True表示是重写的。
                """
                if self._callable._name != other._name:
                    return False

                if self.Virtual:
                    for oImpl in other._implementations:
                        if oImpl.Virtual and self == oImpl:
                            return True
                return False

            @property
            def Virtual(self):
                return self._virtual

            @property
            def PureVirtual(self):
                return self._pureVirtual

            def Implement(self, simple=False, noCast=False) -> str:
                implList = []
                objName = self._callable._obj._wholeName
                if noCast:
                    implList.append("&" + self._callable._wholeName)
                elif simple or not self._default:
                    implList.append("static_cast<{}({}*)({}){}>(".format(
                        self._result,
                        objName + "::",
                        ", ".join(self._args),
                        "const" if self._const else ""
                    ))
                    implList.append("&" + self._callable._wholeName)
                    implList.append(")")
                else:
                    implList.append("[](")
                    implList.append(objName)
                    implList.append("* obj")
                    argList = []
                    if self._args:
                        implList.append(", ")
                    for idx, arg in enumerate(self._args):
                        argList.append(arg + " " + "arg" + str(idx))
                    implList.append(",".join(argList))
                    implList.append("){{return obj->")
                    implList.append(self._callable._name)

                    argList.clear()
                    implList.append("(")
                    for idx in range(len(self._args)):
                        pr = arg[-1] == "&" or arg[-1] == "*"
                        if pr:
                            # 使用指针或引用时省略移动语义。
                            argList.append("arg" + str(idx))
                        else:
                            argList.append("std::move(arg" + str(idx) + ")")
                    implList.append(",".join(argList))
                    implList.append(");}}")
                return "".join(implList)

        _ImplType = Implementation

        def _Implement(self):
            implStr = super()._Implement()
            return implStr

        def _Declaration(self):
            return 'LUA_METHOD("{}", {});'.format(self._newName, self._fname)

        def _Documentation(self):
            return self._Doc(False, False)

        def UsingFor(self, *args):
            f = super().UsingFor(*args)
            for idx, impl in enumerate(f._implementations):
                impl._virtual = self._implementations[idx]._virtual
                impl._pureVirtual = self._implementations[idx]._pureVirtual
            return f

    class StaticMethod(Function):
        """静态成员函数。"""

        def _Instance(self):
            """
            由自己的名字决定是否可以转化为单例获取和销毁的静态属性。
            返回：
                    str     "get"或"set"或None；
                    str     单例实现。
            """
            if not self._implementations:
                return None, None
            lastImpl = self._implementations[-1]
            if len(lastImpl.Args) > 0:
                # 最后的实现参数个数必须为0。
                return None, None
            name = self._name
            for className, instTuple in self._generator.InstanceMethods.items():
                if not re.match("^" + className + "$", self._classesName):
                    continue
                if instTuple[0] and re.match("^" + instTuple[0] + "$", name):
                    retStr = "[](){{return {}();}}".format(self._wholeName)
                    return self._generator.CxxConfig.get, retStr
                elif instTuple[1] and re.match("^" + instTuple[1] + "$", name):
                    retStr = "[](sol::object,std::nullptr_t){{{}();}}".format(self._wholeName)
                    return self._generator.CxxConfig.set, retStr
            return None, None

        def _Implement(self):
            implStr = super()._Implement()
            implStr = implStr.replace('LUA_INVOKE_HEADER', 'LUA_SINVOKE_HEADER')
            implStr = implStr.replace('LUA_INVOKE_FOOTER', 'LUA_SINVOKE_FOOTER')
            return implStr

        def _Declaration(self):
            return 'LUA_METHOD("{}", {});'.format(self._newName, self._fname)

        def _Documentation(self):
            return self._Doc(True, False)

    class Constructor(Function):
        """
        构造函数。
        当前使用了__new仿元方法来实现，只能返回指针。
        todo:
            实现Type.static.__call元方法以使用另一种构造方式。
        """

        class Implementation(Callable.Implementation):
            """构造函数的实现类。"""

            def Implement(self, simple=False, noCast=False) -> str:
                implList = ["[]("]
                argList = []
                for idx, arg in enumerate(self._args):
                    argList.append(arg + " " + "arg" + str(idx))
                implList.append(",".join(argList))
                implList.append("){{return new ")
                implList.append(self._callable._prefixName)

                argList.clear()
                implList.append("(")
                for idx in range(len(self._args)):
                    pr = arg[-1] == "&" or arg[-1] == "*"
                    if pr:
                        # 使用指针或引用时省略移动语义。
                        argList.append("arg" + str(idx))
                    else:
                        argList.append("std::move(arg" + str(idx) + ")")
                implList.append(",".join(argList))
                implList.append(");}}")
                return "".join(implList)

        _ImplType = Implementation

        def __init__(self, cursor, obj: "Object", using: "Object" = None) -> None:
            super().__init__(cursor, obj, using)
            # 构造函数的名字始终是__new这个仿元方法名
            self._newName = self._generator.CxxConfig.new
            self.fname = 'lua_' + self._prefixName.replace('::', '_') + '_' + self._newName
            self._fname = self.fname
            self.dname = 'lua_' + self._prefixName.replace('::', '_') + '_delete'
            if cursor.availability == cindex.AvailabilityKind.NOT_AVAILABLE:
                self._generatable = False
            if self._generatable:
                # 构造函数默认不生成，除非位于AllowConstructor中且不在BanConstructor中。
                self._generatable = False
                for c in self._generator.AllowConstructor:
                    if re.match("^" + c + "$", self._classesName):
                        self._generatable = True
                        for ban in self._generator.BanConstructor:
                            if re.match("^" + ban + "$", self._classesName):
                                self._generatable = False
                                break
                        break
            if self._generatable:
                print('Ctor     {}'.format(cursor.displayname))

        def RequiresDtor(self):
            if not self.Supported or self._obj.PureVirtualMethods:
                return False
            found = False
            if not self._obj._dtor:
                for base in self._obj.Bases:
                    if base._dtor:
                        found = True
                        break
                if not found:
                    # 自己和基类都没有__delete仿元方法时生成一个默认的。
                    return True
            return False

        def _Implement(self):
            if self.Supported and not self._obj.PureVirtualMethods:
                # 有纯虚函数未实现时不支持构造函数。
                implStr = super()._Implement()
                implStr = implStr.replace('LUA_TRY_INVOKE', 'LUA_TRY_CTOR')
                implStr = implStr.replace('LUA_INVOKE_HEADER', 'LUA_SINVOKE_HEADER')
                implStr = implStr.replace('LUA_INVOKE_FOOTER', 'LUA_SINVOKE_FOOTER')
                # delete = self._generator.CxxConfig.delete
                if not self.RequiresDtor():
                    return "{}\n// Impl No Dtor".format(implStr)
                else:
                    dtor = [
                        'int {}(lua_State* lua_S)'.format(self.dname),
                        '{',
                        '\tauto cobj = LUA_TO_COBJ({}*, 1);'.format(self._obj.WholeName),
                        '\tdelete cobj;',
                        '\treturn 0;',
                        '}',
                    ]
                    dtor = '\n'.join(dtor)
                    return "{}\n{}".format(implStr, dtor)
            # 不支持的实现使用特殊的生成方式。
            # return 'mt["{}"]=[](){{return nullptr;}};\n'.format(newName)
            return "// Impl No Ctor Dtor"

        def _Declaration(self):
            if self.Supported and not self._obj.PureVirtualMethods:
                # 有纯虚函数未实现时不支持构造函数。
                implStr = super()._Declaration()
                return implStr
            return "// Decl No Ctor Dtor"

        def _Documentation(self):
            return self._Doc(False, True)

        @property
        def Generatable(self) -> bool:
            if self._generatable and not self.Supported:
                # 即便不支持，也使用另一种生成方式（返回nullptr）。
                return True
            return super().Generatable

    def __init__(self, cursor: Cursor, OG: "BaseConfig | Object", using: "Object" = None) -> None:
        super().__init__(cursor, OG, using)
        self._variables: List[Object.Variable] = []

        # 由于存在重载，为了快速查找，方法使用字典保存。
        # 由于静态方法没有虚函数的情况，所以和非静态方法分开保存
        self._staticMethods: Dict[str, Object.StaticMethod] = {}  # 保存将要生成的静态方法。
        self._methods: Dict[str, Object.Method] = {}  # 保存将要生成的非静态方法。

        # 由于存在using语句，所以protected修饰的域将需要另外保存。
        self._allProtected: Dict[str, Type] = {}  # 保存所有的protected修饰的域。

        # 由于存在using语句，所以public修饰的方法需要另外保存。
        self._allPublicMethod: Dict[str, Object.Method] = {}  # 保存所有的public方法。

        self._wrappers: List[Wrapper] = []
        # 继承的基类（包含基类的基类）。
        self._bases: List[Object] = []
        # 直接public基类，用于在生成lua类时继承。
        self._directlyBases: List[Object] = []
        # 在该集合中的，都是未实现的纯虚函数。
        self._pvMethods: Set[str] = set()
        # 所有的虚函数，包括纯虚函数和非纯虚函数。
        # 用于子类继承时，检查继承的虚函数是否已经重写。
        self._vMethods: Dict[str, Object.Method] = {}
        # 用于保存那些虽然已经在基类绑定过，
        # 但仍然可能在之后由于存在同名且未重写的方法而导致仍然需要在当前类重写的虚函数的临时字典。
        self._tvMethods: Dict[str, Object.Method] = {}
        # 不要混同self._ctor和self._new，不然无法区分静态的工厂方法和构造函数的合并实现。
        # 生成代码时，优先使用self._new，而不是self._ctor。
        # 用于lua中的__new仿元方法的生成。
        self._new: Object.StaticMethod = None
        # 和self._new不同，self._ctor仅仅指代构造函数，而不是lua的__new仿元方法。
        # 此处使用一个默认的构造函数。
        # self._ctor = 'mt["{}"]=[](){{return new {}();}};'.format(self._generator.CxxConfig.new, self._wholeName)
        self._ctor: "Object.Constructor | str" = None
        # self._ctor = 'LUA_CTOR_DEFAULT("{}", {});'.format(self._generator.CxxConfig.new, self._wholeName)
        self._dtor = None
        self.__DeepIterate()

    @property
    def PureVirtualMethods(self) -> Set[str]:
        return self._pvMethods

    @property
    def Bases(self):
        return self._bases

    @property
    def Methods(self):
        return self._methods

    def __lshift__(self, child: Type):
        if not child.Generatable:
            return
        if isinstance(child, Object.Variable):
            self._variables.append(child)
        elif isinstance(child, Object.Method) or isinstance(child, Object.StaticMethod):
            if isinstance(child, Object.Method):
                Object._PushToMethodDict(child, self._methods)
            else:
                method = Object._PushToMethodDict(child, self._staticMethods)
                if self._generator.CxxConfig.new == method._newName:
                    self._new = method
            if self._generator.CxxConfig.delete == child._newName:
                self._dtor = child.Implement
        elif isinstance(child, Object.Constructor):
            if self._ctor:
                self._ctor << child
            else:
                self._ctor = child
        elif isinstance(child, Wrapper):
            self._wrappers.append(child)

    @staticmethod
    def _PushToMethodDict(method: "Object.Function", d: Dict[str, "Object.Function"]) -> "Object.Function":
        """将一个方法推入方法字典，若有同名方法，将实现加入原方法。
        """
        name = method._name
        if name not in d:
            d[name] = method
            return method
        return d[name] << method

    def _PushToProtected(self, member: Type):
        if isinstance(member, Object.Function):
            Object._PushToMethodDict(member, self._allProtected)
        else:
            self._allProtected[member._name] = member

    def __DeepIterate(self) -> dict:
        """深度优先遍历所有子节点。"""
        for node in self._cursor.get_children():
            self.__ProcessCursor(node)

    def __ProcessBaseObject(self, cursor: Cursor):
        """处理继承基类。"""

        generator = self._generator
        baseDef = cursor.get_definition()
        baseName = CursorHelper.GetDirectName(baseDef)
        for skip in generator.BaseClassesSkip:
            if re.match("^" + skip + "$", baseName):
                # 符合跳过的基类的条件。
                return

        bWholeName = CursorHelper.GetWholeName(baseDef)
        if bWholeName not in generator.AllExposures.keys():
            # 大多数情况下，一个类或结构体的基类已经被解析且构造过了；
            # 但仍然有可能继承自不属于生成命名空间中的类或结构体，
            # 此时，为了获取基类的某些信息（比如纯虚函数），需要暂时将基类构造出来（不一定生成为代码）。
            generator.AllExposures[bWholeName] = self.__class__(baseDef, generator)

        public = cursor.access_specifier == AccessSpecifier.PUBLIC
        base: Object = generator.AllExposures[bWholeName]
        if base not in self._directlyBases and public:
            self._directlyBases.append(base)
        if base not in self._bases:
            # 统计所有父级的纯虚函数。
            self._pvMethods |= base._pvMethods
            self._bases.append(base)

        for pBase in base._bases:
            # 添加父级的父级...。
            # Sol要求c++生成lua类绑定时明确指示所有父级。
            if pBase not in self._bases:
                self._bases.append(pBase)

    def __ProcessCursor(self, cursor: Cursor):
        """处理游标信息。"""
        generator = self._generator
        kind = cursor.kind
        public = cursor.access_specifier == AccessSpecifier.PUBLIC
        protected = cursor.access_specifier == AccessSpecifier.PROTECTED
        member = None
        if kind == CursorKind.CXX_BASE_SPECIFIER:
            if public or protected:
                self.__ProcessBaseObject(cursor)
        elif kind == CursorKind.CONSTRUCTOR:
            if type(self._ctor) is str:
                self._ctor = None
            if public or protected:
                member = Object.Constructor(cursor, self)
        elif kind == CursorKind.CXX_METHOD or (public or protected):
            # 除方法外，忽略其它private域。
            if kind == CursorKind.CXX_METHOD and CursorHelper.GetAvailability(
                    cursor) == CursorHelper.Availability.DEPRECATED:
                # 忽略已经废弃的函数。
                public = False
                protected = False
            if kind == CursorKind.ENUM_DECL:
                # 匿名判断不使用cursor.is_anonymous()获取，此处直接简单判断名字作为匿名标准。
                member = NamedEnum(cursor, self) if CursorHelper.GetName(cursor) else AnonymousEnum(cursor, self)
                print('Enum     {}'.format(member.WholeName))
            elif kind == CursorKind.CLASS_DECL:
                member = Class(cursor, self)
                print('Class    {}'.format(member.WholeName))
            elif kind == CursorKind.STRUCT_DECL:
                if generator.AllowStruct:
                    member = Struct(cursor, self)
                    print('Struct   {}'.format(member.WholeName))
            elif kind == CursorKind.FIELD_DECL:
                member = Object.Field(cursor, self)
            elif kind == CursorKind.VAR_DECL:
                member = Object.StaticField(cursor, self)
            elif kind == CursorKind.CXX_METHOD:
                if cursor.is_static_method():
                    if public or protected:
                        member = Object.StaticMethod(cursor, self)
                else:
                    member = Object.Method(cursor, self)
                    name = member._name
                    impl: Object.Method.Implementation = member.Implementations[0]
                    if impl.PureVirtual or impl.Virtual:
                        if impl.PureVirtual:
                            # 统计纯虚函数。
                            self._pvMethods.add(name)
                        elif impl.Virtual:
                            # 消减已实现的纯虚函数。
                            self._pvMethods.discard(name)

                        # 只要是虚函数，就必定记录下来（用于子类验证是否重写）。
                        Object._PushToMethodDict(member.Clone(), self._vMethods)

                    if not public and not protected:
                        return

                    if name not in self._methods and impl.Virtual:
                        # 检查基类是否绑定，基类未绑定，那么自己需要绑定。
                        for base in self._bases:
                            if name in base._methods or self._generator.Aggressive:
                                # 1、具有该名字的方法将被父类生成（或处于积极模式下）；
                                for m in base._vMethods.values():
                                    if impl.IsOverridden(m):
                                        # 2、且该方法是虚方法并且被重写。
                                        # 足以证明基类已绑定。
                                        # 即使基类已绑定，并不意味着一定不需要再次绑定，
                                        # 先在临时字典中保存。
                                        Object._PushToMethodDict(member.Clone(), self._tvMethods)
                                        return
                    # 检查自己是否有在函数缓存字典中的方法。
                    if member._name in self._tvMethods:
                        member = Object._PushToMethodDict(member, self._tvMethods)
                        del self._tvMethods[member._name]
            elif kind == CursorKind.USING_DECLARATION:
                nodes = [node for node in cursor.get_children()]
                usingBaseName = CursorHelper.GetName(nodes[0].get_definition())
                usingName = CursorHelper.GetName(nodes[1])
                usingBase = None
                for base in self._bases:
                    if base._name == usingBaseName:
                        usingBase = base
                        break
                if usingBase:
                    # 检查protected成员。
                    member = usingBase._allProtected.get(usingName, None)

                if member:
                    member = member.UsingFor(self, self)
                    if isinstance(member, Object.Function):
                        usingMember = usingBase._allPublicMethod.get(usingName, None)
                        # 尝试寻找public方法并合并。
                        if type(member) is type(usingMember):
                            member << usingMember.UsingFor(self, self)
                elif usingBase:
                    # 检查public成员。
                    member = usingBase._allPublicMethod.get(usingName, None)
                    if member:
                        member = member.UsingFor(self, self)
        if member:
            if public:
                self << member
                if isinstance(member, Object.Function):
                    # 由于方法可能被改变，所以需要生成一个新的。
                    Object._PushToMethodDict(member.Clone(), self._allPublicMethod)
            if protected and not isinstance(member, Wrapper):
                # TODO:
                # 由于难以规范内部类/结构体/枚举被using后的子域的命名，
                # 暂时将它们忽略。
                if isinstance(member, Object.Function):
                    # 由于方法可能被改变，所以需要生成一个新的。
                    self._PushToProtected(member.Clone())
                else:
                    self._PushToProtected(member)

    def _Implement(self):
        sImpl = super()._Implement()

        # 生成类定义。
        allDecl = []
        allImpl = []

        ctorName = 'nullptr'
        dtorName = 'nullptr'
        if self._ctor and type(self._ctor) is not str:
            ctorName = self._ctor.fname
            if self._ctor.RequiresDtor():
                dtorName = self._ctor.dname

        baseCls = None
        baseType = ''
        for b in self._bases:
            s = '.'.join(b.NameList)
            baseCls = b.WholeName
            baseType = s
            break

        if baseCls:
            allDecl.append('\tLUA_CLS_DEF_BASE({}, "{}", "{}",\n\t\t{}, "{}",\n\t\t{}, {});'.format(
                self._wholeName, self._newName, '.'.join(self._nameList),
                baseCls, baseType, ctorName, dtorName
            ))
        else:
            allDecl.append('\tLUA_CLS_DEF({}, "{}", "{}",\n\t\t{}, {});'.format(
                self._wholeName, self._newName, '.'.join(self._nameList),
                ctorName, dtorName
            ))

        # 构造函数或__new仿元方法。
        if not self._new:
            allow = False
            for c in self._generator.AllowConstructor:
                if re.match("^" + c + "$", self._directName):
                    # 验证是否允许构造函数。
                    allow = True
                    for ban in self._generator.BanConstructor:
                        if re.match("^" + ban + "$", self._directName):
                            allow = False
                            break
                    break
            if not self._ctor or not allow or self._pvMethods:
                # allDecl.append('mt["{}"]=[](){{return nullptr;}};'.format(self._generator.CxxConfig.new))
                # allDecl.append('LUA_CTOR_NULL();')
                pass
            else:
                if type(self._ctor) is str:
                    allDecl.append(self._ctor)
                    found = False
                    delete = self._generator.CxxConfig.delete
                    for base in self._bases:
                        if base._dtor or delete in base._allPublicMethod:
                            found = True
                            break
                    if not found and not self._dtor:
                        # 自己和基类没有__delete仿元方法时生成一个默认的。
                        # self._dtor = 'mt["{}"]=[]({}* obj){{delete obj;}};'.format(delete, self._wholeName)
                        self._dtor = 'LUA_DTOR_DEFAULT("{}", {});'.format(delete, self._wholeName)
                        allDecl.append(self._dtor)
                else:
                    # allDecl.append(self._ctor.Implement)
                    allDecl.append(self._ctor.Declaration)
                    allImpl.append(self._ctor.Implement)

        # 内部类/结构体或内部枚举等。
        wrapperImpls = [wrapper.Implement for wrapper in self._wrappers]
        # if wrapperImpls:
        #     # 新增一个换行以间隔。
        #     wrapperImpls.append("")

        # 成员变量和方法

        self._variables.sort(key=lambda x: x.NewName)
        methods = list(self._methods.values())
        methods.sort(key=lambda x: x.NewName)
        staticMethods = list(self._staticMethods.values())
        staticMethods.sort(key=lambda x: x.NewName)
        self._wrappers.sort(key=lambda x: x.NewName)

        varImpls = [var.Implement for var in self._variables]
        methodImpls = [method.Implement for method in methods]
        staticMethodImpls = [method.Implement for method in staticMethods]

        varDecls = [var.Declaration for var in self._variables]
        methodDecls = [method.Declaration for method in methods]
        staticMethodDecls = [method.Declaration for method in staticMethods]

        # 调用内部类/结构体/枚举生成。
        # called = self._generator.CxxConfig.Called
        # wrapperCalleds = [called.format(self._generator.Tag + "".join(wrapper._nameList)) for wrapper in self._wrappers]
        wrapperCalleds = [wrapper.Called for wrapper in self._wrappers]

        if varDecls:
            allDecl += varDecls
        if methodDecls:
            allDecl += methodDecls
        if staticMethodDecls:
            allDecl += staticMethodDecls
        if wrapperCalleds:
            allDecl += wrapperCalleds

        allDecl.append("LUA_CLS_END();")

        if varDecls:
            allImpl += varImpls
        if methodDecls:
            allImpl += methodImpls
        if staticMethodDecls:
            allImpl += staticMethodImpls
        # if wrapperCalleds:
        #     allImpl += wrapperCalleds

        s1 = "\n".join(wrapperImpls)
        if wrapperImpls:
            s1 += "\n"
        ret = sImpl.format(
            s1 + "\n".join(allImpl) + "\n",
            "\n\t".join(allDecl)
        )
        return ret

    def _Documentation(self):
        docs = []
        docs.append(self.Comment)
        ns = '.'.join(self._nameList[:-1])
        name = ns + '.' + self._newName
        if self._bases:
            base = self._bases[0]
            bname = '.'.join(base.NameList)
            docs.append('---@class {}:{}'.format(name, bname))
        else:
            docs.append('---@class {}'.format(name))
        docs.append('local {} = '.format(self._newName) + '{}')
        docs.append('{} = {}'.format(name, self._newName))
        clsDoc = '\n'.join(docs)
        #
        allDoc = []
        # 构造函数或__new仿元方法。
        if not self._new:
            allow = False
            for c in self._generator.AllowConstructor:
                if re.match("^" + c + "$", self._directName):
                    # 验证是否允许构造函数。
                    allow = True
                    for ban in self._generator.BanConstructor:
                        if re.match("^" + ban + "$", self._directName):
                            allow = False
                            break
                    break
            if not self._ctor or not allow or self._pvMethods:
                # allDoc.append('-- LUA_CTOR_NULL();')
                pass
            else:
                if type(self._ctor) is str:
                    pass
                else:
                    allDoc.append(self._ctor.Documentation)

        # 内部类/结构体或内部枚举等。
        wrapperDocs = [wrapper.Documentation for wrapper in self._wrappers]

        # 成员变量和方法

        self._variables.sort(key=lambda x: x.NewName)
        methods = list(self._methods.values())
        methods.sort(key=lambda x: x.NewName)
        staticMethods = list(self._staticMethods.values())
        staticMethods.sort(key=lambda x: x.NewName)
        self._wrappers.sort(key=lambda x: x.NewName)

        varDocs = [var.Documentation for var in self._variables]
        methodDocs = [method.Documentation for method in methods]
        staticMethodDocs = [method.Documentation for method in staticMethods]

        if varDocs:
            allDoc += varDocs
        if methodDocs:
            allDoc += methodDocs
        if staticMethodDocs:
            allDoc += staticMethodDocs
        if wrapperDocs:
            allDoc += wrapperDocs

        return clsDoc + '\n\n' + '\n\n'.join(allDoc)


class Class(Object):
    """类（目前类和结构体没有区别）。"""


class Struct(Object):
    """结构体（目前类和结构体没有区别）。"""
