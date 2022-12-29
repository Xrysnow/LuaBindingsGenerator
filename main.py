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

import multiprocessing
import sys
import getopt
import importlib
import os
from Generator.BaseGenerator import BaseGenerator

from LstgGenerator import LstgGenerator

# 是否使用多进程。
_UseMultiProcessing = False


def _ProcessWork(ele):
    if ele[1]:
        gen: BaseGenerator = ele[0](outputPath=ele[1])
    else:
        gen: BaseGenerator = ele[0]()
    gen.Generate()


if __name__ == "__main__":
    if os.environ.get("ANDROID_NDK") is None:
        raise RuntimeError('no ANDROID_NDK')
    opt, _ = getopt.getopt(sys.argv[1:], "eE:")
    E = ""
    for k, v in opt:
        if k == "-E":
            E = v

    genList = []
    if not E:
        genList += [
            [LstgGenerator, ""],
        ]
    else:
        try:
            path = os.path.dirname(E)
            if path not in sys.path:
                sys.path.append(path)
            name = os.path.basename(E)
            module = importlib.import_module(name)
            genList.append([module.__dict__[name], ""])
        except:
            print(f"无法生成 {E} 文件指定的lua绑定。")

    if _UseMultiProcessing and len(genList) > 1:
        with multiprocessing.Pool() as pPool:
            pPool.map(_ProcessWork, genList)
    else:
        for ele in genList:
            _ProcessWork(ele)
