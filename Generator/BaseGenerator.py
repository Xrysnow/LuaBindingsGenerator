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
import sys
import time
from typing import Dict
from clang import cindex
from Native.Base import Exposure
from Native.Variable import Variable
from Native.Function import Function, FunctionCollection
from Native.Enum import AnonymousEnum, NamedEnum
from Native.Object import Class, Struct
from Util.Functions import FindAllFilesMatch
from Util.CursorHelper import CursorHelper
from Config.BaseConfig import BaseConfig


class BaseGenerator(BaseConfig):
    def __init__(self, outputPath: str, outputFile: str, clearOldFile: bool = True):
        """
        参数：

            outputPath          生成代码的目录。

            outputFile          生成代码的文件名，不需要带有后缀。
                                比如文件名为"gFile"，则将生成：
                                outputPath/gFile.hpp
                                outputPath/gFile.cpp
                                outputPath/gFile_01.cpp
                                outputPath/gFile_02.cpp
                                ...

            clearOldFile        是否清除原来的旧文件再生成，还是采取覆盖的方式。
                                该参数在文件总数有变化时很有用。
        """
        super().__init__(os.path.join(os.path.abspath(os.curdir), "src").replace("\\", '/'))
        self._outputPath = outputPath
        self._outputFile = outputFile
        self._clearOldFile = clearOldFile
        self._filePrefix = os.path.join(self._outputPath, self._outputFile)
        self._exposures: Dict[str, Exposure] = {}

        # 缓存的所有访问过的节点，避免重复访问。
        self.AllExposures = {}

        self.__inited = False

        if not cindex.Config.loaded:
            cindex.Config.set_library_path(os.path.abspath(os.path.join(os.path.dirname(__file__), "..", "libclang")))
        self._cindex = cindex.Index.create()

    def __LazyInit(self):
        if not self.__inited:
            self.__inited = True
            self.ClangArgs += self.ExtraArgs
            if sys.platform == "win32":
                self.ClangArgs += self.Win32ClangFlags

    def _CheckOldFile(self):
        """检查是否需要删除旧文件。"""
        if not self._clearOldFile:
            return

        matchFiles = FindAllFilesMatch(self._outputPath, lambda x: -1 != x.find(self._outputFile))
        for file in matchFiles:
            os.remove(file)

    def _GenerateHeadCode(self):
        """生成c++头文件。"""
        if not os.path.exists(self._outputPath):
            os.makedirs(self._outputPath)
        with open(self._filePrefix + ".hpp", "w") as hFile:
            headerStr = [
                '#pragma once\n',
                '#include "base/ccConfig.h"\n',
            ]
            for s in self.HeadCodeHeaders:
                headerStr.append('#include "{}"\n'.format(s))
            if self.ToNameSpace:
                headerStr.append("namespace " + self.ToNameSpace + " {\n")
            headerStr.append((self.CxxConfig.Define + ";\n").format(self.Tag))
            if self.ToNameSpace:
                headerStr.append("}")
            hFile.write(''.join(headerStr))

    def _GenerateImplCode(self):
        """生成c++实现文件。"""
        # with open(self._filePrefix + ".cpp", "w") as iFile:
        with open(self._filePrefix + ".hpp", "w") as iFile:
            # strList = []
            strList = [
                '#pragma once\n',
                '#include "base/ccConfig.h"\n',
            ]
            for s in self.HeadCodeHeaders:
                strList.append('#include "{}"\n'.format(s))
            strList.append('\n')
            if self.ToNameSpace:
                strList.append("namespace " + self.ToNameSpace + " {\n")
            for c in self._exposures.values():
                if c.Define:
                    strList.append(c.Define + "\n")

            strList.append('\n')
            strList.append('inline ' + (self.CxxConfig.Define + "\n{{\n").format(self.Tag))
            if self.MacroJudgement:
                strList.append(self.MacroJudgement + "\n")
            # for ns in self.NameSpace.values():
            #     strList.append('lua["{0}"]=lua.get_or("{0}",lua.create_table());\n'.format(ns))
            for c in self._exposures.values():
                if c.Called:
                    strList.append('\t{}\n'.format(c.Called))
            if self.MacroJudgement:
                strList.append("#endif\n")
            strList.append("\treturn 0;\n")
            strList.append("}\n")
            if self.ToNameSpace:
                strList.append("}")
            iFile.write(''.join(strList))

    def _GenerateObjectCode(self):
        """为避免产生编译器C1060错误，将每10个类/枚举/结构体放置于一个实现文件。
        """
        groupIndex = self.GroupCount
        idx = 0
        strList = []
        exposures = list(self._exposures.values())
        print('Top exposures: {}'.format(len(exposures)))

        def DoEnd(self: BaseGenerator):
            if not strList:
                return
            # if self.ToNameSpace:
            #     strList.append("}\n")
            if self.MacroJudgement:
                strList.append("#endif\n")

            num = 0 if groupIndex == 0 else (idx // groupIndex)
            postfix = "%02d.cpp" % num
            if num == 0:
                postfix = ".cpp"
            with open(self._filePrefix + postfix, "w") as implFile:
                implFile.write(''.join(strList))
                strList.clear()

        for obj in exposures:
            if (groupIndex == 0 and idx == 0) or (idx % groupIndex == 0):
                strList.append('#include "{}.hpp"\n'.format(self._outputFile))
                if self.MacroJudgement:
                    strList.append(self.MacroJudgement + "\n")
                if self.ImplCodeHeaders is not None:
                    # 直接使用self.ImplCodeHeaders
                    for s in self.ImplCodeHeaders:
                        strList.append('#include "{}"\n'.format(s))
                else:
                    # 从self.Headers生成
                    for header in self.Headers:
                        relative = ".."
                        for searchPath in self.SearchPaths:
                            relative = os.path.relpath(header, searchPath)
                            if ".." not in relative:
                                break
                        if ".." not in relative:
                            strList.append('#include "{}"\n'.format(relative.replace(os.path.sep, '/')))
                        else:
                            strList.append('#include "{}"\n'.format(os.path.basename(header)))
                strList.append("\n")
                if self.ToNameSpace:
                    # strList.append("namespace " + self.ToNameSpace + " {\n")
                    strList.append("using namespace " + self.ToNameSpace + ";\n\n")
                if self.ExtraImpl:
                    strList.append(self.ExtraImpl)
                    strList.append('\n\n')
            strList.append(obj.Implement)
            strList.append("\n")

            if groupIndex != 0 and (idx % groupIndex == groupIndex - 1):
                DoEnd(self)

            idx += 1
        else:
            DoEnd(self)

    def _GenerateObjectDoc(self):
        groupIndex = 0
        idx = 0
        strList = []
        exposures = list(self._exposures.values())

        def DoEnd(self: BaseGenerator):
            if not strList:
                return
            # if self.ToNameSpace:
            #     strList.append("}\n")

            num = 0 if groupIndex == 0 else (idx // groupIndex)
            postfix = "%02d.lua" % num
            if num == 0:
                postfix = ".lua"
            with open(self._filePrefix + postfix, "w", encoding='utf-8') as implFile:
                content = ''.join(strList)
                content = self.DocPostprocess(content)
                implFile.write(content)
                strList.clear()

        allNames = []
        for obj in exposures:
            allNames.append(obj.NewName)
        for obj in exposures:
            if groupIndex == 0 and idx == 0:
                strList.append('------------------------------\n')
                strList.append('--- Tag: {}\n'.format(self.Tag))
                # strList.append('--- Exposures: {}\n'.format(', '.join(allNames)))
                strList.append('--- Date: {}\n'.format(time.strftime("%Y-%m-%d", time.localtime())))
                strList.append('------------------------------\n')
                strList.append("\n")
                if self.ExtraDoc:
                    strList.append(self.ExtraDoc)
                    strList.append('\n\n')
            # strList.append("\n")
            strList.append(obj.Documentation)
            strList.append("\n\n")
            # if groupIndex != 0 and (idx % groupIndex == groupIndex - 1):
            #     DoEnd(self)
            idx += 1
        else:
            DoEnd(self)

    def DocPostprocess(self, content: str):
        return content

    def _ProcessGlobalExposure(self):
        # 合并全局函数的注册
        global_functions = {}
        for key, obj in self._exposures.copy().items():
            if isinstance(obj, Function):
                ns = obj.NameList[0]
                if ns in global_functions:
                    global_functions[ns].append(obj)
                else:
                    global_functions[ns] = [obj]
                del self._exposures[key]
        for k, v in global_functions.items():
            c = FunctionCollection(v)
            self._exposures['{}/functions'.format(c.NameList[0])] = c

    def _GenerateCode(self):
        self._ProcessGlobalExposure()
        self._GenerateHeadCode()
        self._GenerateImplCode()
        self._GenerateObjectCode()
        self._GenerateObjectDoc()

    def Generate(self):
        self.__LazyInit()
        self._exposures.clear()
        print("\n生成 {} 代码...".format(self._outputFile))
        self._ParseHeaders()
        self._CheckOldFile()
        self._GenerateCode()
        print("完成 {} 生成。".format(self._outputFile))

    def _CheckDiagnostics(self, diagnostics: list[cindex.Diagnostic]):
        """检查并打印诊断信息。"""
        errors = []
        for idx, d in enumerate(diagnostics):
            if d.severity > cindex.Diagnostic.Warning:
                errors.append(d)
        if not errors:
            return
        print("====\n解析头文件发生错误：")
        severities = ["忽略", "注意", "警告", "错误", "致命"]
        for idx, d in enumerate(errors):
            print("{}.  严重性 = {},\n    位于 ： [{}] {!r},\n    详细 ： {!r}".format(
                idx + 1, severities[d.severity], self.Tag, d.location, d.spelling))
            for f in d.fixits:
                print('-- fix:')
                print(f)
            for c in d.children:
                print('--')
                print(c)
        print("====\n")
        print("*** 发现错误，无法继续。")
        raise Exception("Fatal error in parsing headers")

    def _ParseHeaders(self):
        for header in self.Headers:
            print("{} -> {}".format(self.Tag, header))
            tu = self._cindex.parse(header, self.ClangArgs)
            self._CheckDiagnostics(tu.diagnostics)
            self._DeepIterate(tu.cursor)

    def _DeepIterate(self, cursor):
        if next(cursor.get_children(), None) is None \
                and cursor.kind != cindex.CursorKind.FUNCTION_DECL:
            return

        if (cursor.kind == cindex.CursorKind.CLASS_DECL
            or (self.AllowStruct and cursor.kind == cindex.CursorKind.STRUCT_DECL)) \
                or cursor.kind == cindex.CursorKind.ENUM_DECL \
                or cursor.kind == cindex.CursorKind.VAR_DECL \
                or cursor.kind == cindex.CursorKind.FUNCTION_DECL:
            if cursor.kind == cindex.CursorKind.VAR_DECL or cursor.kind == cindex.CursorKind.FUNCTION_DECL or cursor == cursor.type.get_declaration():
                if not self.Generatable(CursorHelper.GetDirectName(cursor)):
                    return

                # 筛选那些没有命名空间却位于self.AllowFiles中需要生成的对象。
                allow = CursorHelper.GetNameSpace(cursor) != ""
                if not allow and self.AllowFiles:
                    f = CursorHelper.GetFile(cursor)
                    for file in self.AllowFiles:
                        if os.path.samefile(f, file):
                            allow = True
                            break
                if not allow:
                    return
                wholeName = CursorHelper.GetWholeName(cursor)
                exposure: Exposure = None
                # 匿名判断不使用cursor.is_anonymous()获取，此处直接简单判断名字作为匿名标准。
                if CursorHelper.GetName(cursor):
                    # 具名枚举、类、全局函数和全局变量。
                    if wholeName in self.AllExposures.keys() and cursor.kind != cindex.CursorKind.FUNCTION_DECL:
                        # 函数类型较特殊，由于存在重载，所以重复的不能被排除。
                        return

                    if wholeName not in self._exposures.keys() or cursor.kind == cindex.CursorKind.FUNCTION_DECL:
                        if cursor.kind == cindex.CursorKind.ENUM_DECL:
                            exposure = NamedEnum(cursor, self)
                            print('Enum     {}'.format(wholeName))
                        elif cursor.kind == cindex.CursorKind.CLASS_DECL:
                            exposure = Class(cursor, self)
                            print('Class    {}'.format(wholeName))
                        elif cursor.kind == cindex.CursorKind.STRUCT_DECL:
                            exposure = Struct(cursor, self)
                            print('Struct   {}'.format(wholeName))
                        elif cursor.kind == cindex.CursorKind.VAR_DECL:
                            exposure = Variable(cursor, self)
                            print('Variable {}'.format(wholeName))
                        elif cursor.kind == cindex.CursorKind.FUNCTION_DECL:
                            if wholeName not in self._exposures.keys():
                                exposure = Function(cursor, self)
                                print('Function {}'.format(wholeName))
                            else:
                                # 增加重载实现。
                                self._exposures[wholeName] << cursor
                                return
                elif cursor.kind == cindex.CursorKind.ENUM_DECL:
                    # 匿名枚举。
                    exposure = AnonymousEnum(cursor, self)
                    wholeName = exposure.WholeName
                    if wholeName in self.AllExposures.keys():
                        return
                    print('AnonEnum {}'.format(wholeName))
                if exposure:
                    if exposure.Generatable:
                        # 在可生成的情况下加入self._exposures
                        self._exposures[wholeName] = exposure
                    # 但无论如何都会加入self.AllExposures以作为缓存和去重
                    self.AllExposures[wholeName] = exposure

        elif cursor.kind == cindex.CursorKind.NAMESPACE or cursor.kind == cindex.CursorKind.TRANSLATION_UNIT:
            if cursor.kind == cindex.CursorKind.NAMESPACE:
                ns = CursorHelper.GetWholeName(cursor)
                allow = False
                for NS in self.NameSpace.keys():
                    if ns == NS or NS.startswith(ns + "::"):
                        # 如果命名空间符合条件，或该命名空间是目标命名空间的上级，则允许解析。
                        allow = True
                        break
                if not allow:
                    return
            for node in cursor.get_children():
                self._DeepIterate(node)
