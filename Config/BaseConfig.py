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
import re
from typing import Dict, List
from Util.EnvChecker import EnvChecker
from Config.CxxConfig import CxxConfig


class NotFoundFileException(Exception):
    def __str__(self) -> str:
        return "未找到某些关键文件或环境变量。"


class BaseConfig(object):
    class AnonymousType():
        # 不允许匿名枚举。
        Ban = 0
        # 允许所有的匿名枚举。
        All = 1
        # 仅允许类中的匿名枚举。
        Class = 2
        # 仅允许全局的匿名枚举。
        Global = 3

    def __init__(self, luaConfigRoot):
        ndkRoot = EnvChecker.Check_NDK_RootEnv()
        defaultInclude = EnvChecker.DefaultIncludePath()
        llvmToolChain = EnvChecker.Find_LLVM_ToolChain()
        if not ndkRoot or not defaultInclude or not llvmToolChain or not luaConfigRoot:
            raise NotFoundFileException()

        self.CxxConfig = CxxConfig(luaConfigRoot)

        # Tag用于生成的注册函数中，作为和其它生成文件的区分。
        self.Tag = ""
        # 指定一组c++命名空间，所有c++类将在符合的命名空间中查找，且可以和相同的类名区分开来。
        # 比如："cocos2d::Label" <-> "cocos2d::ui::Label"
        # 会将键对应的名字替换为值对应的名字。
        self.NameSpace: Dict[str, str] = {}
        # 指定一组文件，某些类或函数即便不处于CppNameSpace中，但仍然处于给定的文件中，仍然可以被生成。
        self.AllowFiles: List[str] = []
        # 希望生成到的命名空间，所有生成的代码将被包装到该命名空间中。
        self.ToNameSpace = ""
        # 允许生成匿名枚举（属于全局或仅属于命名空间的匿名枚举可能在被include时多次生成，谨慎开启）。
        self.AllowAnonymous = BaseConfig.AnonymousType.Class
        # 允许生成结构体类型（结构体普遍被转换为一个table，没有必要为每个结构体转换为用户类）。
        self.AllowStruct = False
        # 宏判断，用于某些情况下的条件编译。
        self.MacroJudgement = ""
        # 使用大驼峰命名法（在最后阶段生效，不会影响命名空间，只对成员函数生效）。
        self.UpperMethod = False
        # 使用大驼峰命名法（在最后阶段生效，不会影响命名空间，只对成员变量/枚举值生效）。
        self.UpperField = False
        self.UpperFunction = False
        #
        self.LowerMethod = False
        self.LowerField = False
        self.LowerFunction = False
        # 是否自动生成属性。
        # 当一个方法能够接受一个参数，且前缀符合SetPrefix时，生成set属性；
        # 当一个方法能够返回一个非void值，且接受0个参数，前缀也还符合GetPrefix时，生成get属性。
        self.AutoProperties = True

        self.AndroidHeaders: List[str] = []
        self.AndroidFlags = [
            "-target", "armv7-none-linux-androideabi",
            "-D_SIZE_T_DEFINED_",
            "-D_LIBCPP_DISABLE_VISIBILITY_ANNOTATIONS",
            "-DANDROID",
            "-D__ANDROID_API__=18",
            "--sysroot={}/platforms/android-18/arch-arm".format(ndkRoot),
            "-idirafter", "{}/sources/android/support/include".format(ndkRoot),
            "-idirafter", "{}/sysroot/usr/include".format(llvmToolChain),
            "-idirafter", "{}/sysroot/usr/include/arm-linux-androideabi".format(llvmToolChain),
            "-idirafter", "{}/lib64/clang/14.0.6/include".format(llvmToolChain),
            "-idirafter", "{}/../../build-tools/33.0.1/renderscript/clang-include".format(ndkRoot),
            "-I{}/sources/cxx-stl/llvm-libc++/include".format(ndkRoot)
        ]
        dl = os.listdir("{}/lib64/clang".format(llvmToolChain))
        if dl:
            self.AndroidFlags.append("-idirafter")
            self.AndroidFlags.append("{}/lib64/clang/{}/include".format(llvmToolChain, dl[0]))

        self.ClangHeaders: List[str] = []
        self.ClangFlags = ["-nostdinc", "-x", "c++", "-std=c++17", "-fsigned-char", "-U__SSE__"]
        self.Win32ClangFlags: List[str] = []

        # 额外参数，用于用户扩展。
        self.ExtraArgs: List[str] = []

        # clang 参数。
        self.ClangArgs: List[str] = [
            *self.AndroidHeaders,
            *self.ClangHeaders,
            *self.AndroidFlags,
            *self.ClangFlags,
            *defaultInclude
        ]

        # 欲解析的头文件。
        self.Headers: List[str] = []

        # 头文件的查找路径。
        self.SearchPaths: List[str] = []

        # 欲生成代码的类（也适用于结构体和枚举类型），可以使用正则表达式。
        # 会使用"^$"括起来再进行正则匹配，如："^Menu*$"。
        self.Classes: List[str] = []

        # 欲跳过的类。
        self.SkipClasses: List[str] = []

        # 指示哪些类的哪些成员需要被生成，默认全部生成（除构造函数外）。
        # 格式见self.SkipMembers
        self.Members: Dict[str, List[str]] = {
            ".*": [".*"]
        }

        # 生成构造函数的优先级：
        # 1. 强制指定某个静态方法命名为__new；
        # 2. 指定某些类一定会生成构造函数（仍然以命名为__new的静态方法优先）。
        self.AllowConstructor: List[str] = [".*"]
        self.BanConstructor: List[str] = []

        # 欲跳过的类方法（也适用于成员变量）。
        # 格式：
        # self.SkipMembers = {
        #   "SkipName" : ["method1","method2"],
        #   "Skip.*" : ["method.*"]
        # }
        # 类名和方法名均可使用正则表达式。
        self.SkipMembers: Dict[str, List[str]] = {}

        # 欲重命名的方法（也适用于成员变量）。
        # 格式：
        # self.RenameMembers = {
        #   "renameClass.*" : {"renameMethod" : "newMethod"},
        # }
        # 类名可以使用正则表达式。
        self.RenameMembers: Dict[str, Dict[str, str]] = {}

        # 欲重命名的类。
        # 格式：
        # self.RenameClasses = {
        #   "renameClass" : "newClass",
        #   "rename.*" : "newClass1"
        # }
        # 类名可使用正则表达式。
        self.RenameClasses: Dict[str, str] = {}

        # 欲去除前缀的枚举。
        # 格式：
        # self.StripEnumEntries = {
        #   "renameEnum" : "prefix",
        #   "rename.*" : "prefix1"
        # }
        self.StripEnumEntries: Dict[str, str] = {}

        # 欲将单例方法变更为单例属性的对应关系。
        # 格式：
        # self.InstanceMethods = {
        #   ".*Test.*" : ("getInstance","destroyInstance"),
        #   ".*NoDestroyTest.*" : ("getInstance",None)
        # }
        # 类名和方法名均可使用正则表达式。
        # 只有静态方法才生效。
        # 变换后，可直接使用单例属性访问单例（原单例方法仍然可以使用），如：
        # local inst = MyTest.getInstance();
        # local inst = MyTest.Instance;
        # MyTest.Instance = nil;--相当于调用MyTest.destroyInstance();
        self.InstanceMethods: Dict[str, tuple] = {}

        # 当一个类在作为基类时，希望被跳过的类型列表。
        self.BaseClassesSkip: List[str] = []

        # 生成get属性的前缀。
        self.GetPrefix: List[str] = []
        # 生成set属性的前缀。
        self.SetPrefix: List[str] = []

        # 具有只读属性的成员变量（用于一些特殊情况）。
        # 格式：
        # self.ReadOnlyFields = {
        #   "TestClass.*" : ["a.?","b"],
        # }
        self.ReadOnlyFields: Dict[str, List[str]] = {}

        # 积极模式。
        # 在积极模式下将会认为所有基类的虚方法都已经绑定过。
        self.Aggressive = True

        # 多少个为一组？
        self.GroupCount = 10

        self.ExtraImpl: str = ''
        self.ExtraDoc: str = ''

    def RenameMember(self, className: str, funcName: str) -> str:
        """重命名给定的函数。

        参数：

            className           类名，可也以是结构体名。

            funcName            函数名，也可以是成员名。

        返回：
            若没有对应的重命名规则，返回原函数名。
        """
        for k, v in self.RenameMembers.items():
            if re.match("^" + k + "$", className):
                for rName, nName in v.items():
                    nName = re.sub("^" + rName + "$", nName, funcName)
                    if nName != funcName:
                        return nName
        return funcName

    def RenameClass(self, className: str) -> str:
        """重命名给定的类（结构体，枚举，全局函数）。

        参数：

            className           类名，可也以是结构体/枚举名。

        返回：
            若没有对应的重命名规则，返回原类型名。
        """
        for k, v in self.RenameClasses.items():
            nName = re.sub("^" + k + "$", v, className)
            if nName != className:
                return nName
        return className

    def RenameEnumEntry(self, enumName: str, entryName: str) -> str:
        """重命名给定的枚举值。
        """
        for k, v in self.StripEnumEntries.items():
            if entryName.startswith(v) and len(entryName) > len(v):
                return entryName[len(v):]
        # 避免数字开头
        if entryName[0].isdigit():
            entryName = '_' + entryName
        return entryName

    def Generatable(self, className: str, memberName: str = None) -> bool:
        """判断是否应当生成该类（结构体/全局变量）或类成员。

        参数：

            className           类名，可也以是结构体名或枚举名或全局函数名。
            funcName            成员函数名/成员变量名，可以为空，此时只检查类名。
        """
        if not className:
            return False
        for cName in self.Classes:
            if re.match("^" + cName + "$", className):
                # 在生成列表中。
                break
        else:
            return False

        for kName in self.SkipClasses:
            if re.match("^" + kName + "$", className):
                # 在跳过列表中。
                return False
        if not memberName:
            return True

        findMem = False
        for clz, members in self.Members.items():
            if re.match("^" + clz + "$", className):
                for mem in members:
                    if re.match("^" + mem + "$", memberName):
                        findMem = True
                        break
                if findMem:
                    break
        if not findMem:
            if not (className.endswith("::" + memberName) or className.endswith(memberName)):
                # 构造函数始终能够生成。
                return False
        for skipCls, skipMem in self.SkipMembers.items():
            if re.match("^" + skipCls + "$", className):
                for skipFunc in skipMem:
                    if re.match("^" + skipFunc + "$", memberName):
                        return False

        return True
