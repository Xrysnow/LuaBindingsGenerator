import os
from Generator.BaseGenerator import BaseGenerator


class LstgGenerator(BaseGenerator):
    def __init__(self, outputPath: str = None, clearOldFile: bool = True):
        self.CocosRoot = "../frameworks/cocos2d-x"
        root = "../frameworks"
        suffix = 'Test'
        fileName = "lua_" + suffix + "_auto"
        if not outputPath:
            outputPath = os.path.join(os.path.abspath(os.curdir), "out")
        super().__init__(outputPath, fileName, clearOldFile)

        self.NameSpace |= {
            "lstg": "lstg"
        }
        # self.ToNameSpace = "cocos2d"
        self.Tag = suffix
        self.SearchPaths += [
            os.path.join(self.CocosRoot, "cocos"),
            os.path.join(self.CocosRoot, "extensions"),
            root
        ]
        self.Headers += [
            "{}/Test.h".format(root)
        ]
        self.InstanceMethods |= {
            # ".*": ("(g|G)etInstance", "(d|D)estroyInstance")
        }

        # 解析时的依赖头文件路径等。
        self.ExtraArgs += [
            f"-I{self.CocosRoot}/cocos",
            f"-I{self.CocosRoot}/extensions",
            f"-I{self.CocosRoot}/external/lua/luajit/include",
            f"-I{root}",
            "-DANDROID"
        ]
        self.Classes += [
            ".*"
        ]
        self.RenameMembers |= {
            # ".*": {"(C|c)reate": "__new"}
        }
        self.SkipMembers |= {
            # ".*": ["xxx"]
        }
        self.StripEnumEntries |= {
            # "xxx": "xxx"
        }
