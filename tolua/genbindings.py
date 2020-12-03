#!/usr/bin/python

# This script is used to generate luabinding glue codes.
# Android ndk version must be ndk-r9b.


import sys
import os, os.path
import shutil
import ConfigParser
import subprocess
import re
from contextlib import contextmanager


def _check_ndk_root_env():
    ''' Checking the environment NDK_ROOT, which will be used for building
    '''

    try:
        NDK_ROOT = os.environ['NDK_ROOT']
    except Exception:
        print "NDK_ROOT not defined. Please define NDK_ROOT in your environment."
        sys.exit(1)

    return NDK_ROOT

def _check_python_bin_env():
    ''' Checking the environment PYTHON_BIN, which will be used for building
    '''

    try:
        PYTHON_BIN = os.environ['PYTHON_BIN']
    except Exception:
        print "PYTHON_BIN not defined, use current python."
        PYTHON_BIN = sys.executable

    return PYTHON_BIN


class CmdError(Exception):
    pass


@contextmanager
def _pushd(newDir):
    previousDir = os.getcwd()
    os.chdir(newDir)
    yield
    os.chdir(previousDir)

def _run_cmd(command):
    ret = subprocess.call(command, shell=True)
    if ret != 0:
        message = "Error running command"
        raise CmdError(message)

def main():

    cur_platform= '??'
    llvm_path = '??'
    ndk_root = _check_ndk_root_env()
    # del the " in the path
    ndk_root = re.sub(r"\"", "", ndk_root)
    python_bin = _check_python_bin_env()

    platform = sys.platform
    if platform == 'win32':
        cur_platform = 'windows'
    elif platform == 'darwin':
        cur_platform = platform
    elif 'linux' in platform:
        cur_platform = 'linux'
    else:
        print 'Your platform is not supported!'
        sys.exit(1)

    x86_llvm_path = ""
    x64_llvm_path = os.path.abspath(os.path.join(ndk_root, 'toolchains/llvm/prebuilt', '%s-%s' % (cur_platform, 'x86_64')))
    if not os.path.exists(x64_llvm_path):
        x86_llvm_path = os.path.abspath(os.path.join(ndk_root, 'toolchains/llvm/prebuilt', '%s' % (cur_platform)))
    if not os.path.exists(x86_llvm_path):
        x86_llvm_path = os.path.abspath(os.path.join(ndk_root, 'toolchains/llvm/prebuilt', '%s-%s' % (cur_platform, 'x86')))

    if os.path.isdir(x64_llvm_path):
        llvm_path = x64_llvm_path
    elif os.path.isdir(x86_llvm_path):
        llvm_path = x86_llvm_path
    else:
        print 'llvm toolchain not found!'
        print 'path: %s or path: %s are not valid! ' % (x86_llvm_path, x64_llvm_path)
        sys.exit(1)

    x86_gcc_toolchain_path = ""
    x64_gcc_toolchain_path = os.path.abspath(os.path.join(ndk_root, 'toolchains/arm-linux-androideabi-4.9/prebuilt', '%s-%s' % (cur_platform, 'x86_64')))
    if not os.path.exists(x64_gcc_toolchain_path):
        x86_gcc_toolchain_path = os.path.abspath(os.path.join(ndk_root, 'toolchains/arm-linux-androideabi-4.9/prebuilt', '%s' % (cur_platform)))
    if not os.path.exists(x86_gcc_toolchain_path):
        x86_gcc_toolchain_path = os.path.abspath(os.path.join(ndk_root, 'toolchains/arm-linux-androideabi-4.9/prebuilt', '%s-%s' % (cur_platform, 'x86')))

    if os.path.isdir(x64_gcc_toolchain_path):
        gcc_toolchain_path = x64_gcc_toolchain_path
    elif os.path.isdir(x86_gcc_toolchain_path):
        gcc_toolchain_path = x86_gcc_toolchain_path
    else:
        print 'gcc toolchain not found!'
        print 'path: %s or path: %s are not valid! ' % (x64_gcc_toolchain_path, x86_gcc_toolchain_path)
        sys.exit(1)


    project_root = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
    cocos_root = os.path.abspath(os.path.join(project_root, ''))
    cxx_generator_root = os.path.abspath(os.path.join(project_root, 'tools/bindings-generator'))

    # save config to file
    config = ConfigParser.ConfigParser()
    config.set('DEFAULT', 'androidndkdir', ndk_root)
    config.set('DEFAULT', 'clangllvmdir', llvm_path)
    config.set('DEFAULT', 'gcc_toolchain_dir', gcc_toolchain_path)
    config.set('DEFAULT', 'cocosdir', cocos_root)
    config.set('DEFAULT', 'cxxgeneratordir', cxx_generator_root)
    config.set('DEFAULT', 'extra_flags', '')

    conf_ini_file = os.path.abspath(os.path.join(os.path.dirname(__file__), 'userconf.ini'))

    print 'generating userconf.ini...'
    with open(conf_ini_file, 'w') as configfile:
      config.write(configfile)


    # set proper environment variables
    if 'linux' in platform or platform == 'darwin':
        os.putenv('LD_LIBRARY_PATH', '%s/libclang' % cxx_generator_root)
    if platform == 'win32':
        path_env = os.environ['PATH']
        os.putenv('PATH', r'%s;%s\libclang;%s\tools\win32;' % (path_env, cxx_generator_root, cxx_generator_root))


    try:

        tolua_root = '%s/tools/tolua' % project_root
        output_dir = '%s/tools/tolua/out' % project_root

        cmd_args = {
            #'cocos2dx_ui.ini' : ('cocos2dx_ui', 'lua_cocos2dx_ui_auto'),
            #'cc_font.ini' : ('cc_font', 'lua_cocos2dx_font_auto'),
            #'cocos2dx_configuration.ini' : ('cocos2dx_configuration', 'lua_cocos2dx_configuration_auto'),
            #'x_L2D.ini' : ('L2D', 'lua_L2D_auto'),
            #'x_Latex.ini' : ('x_Latex', 'lua_Latex_auto'),
            #'x_Resource.ini' : ('x_Resource', 'lua_Resource_auto'),
            #'x_RenderMode.ini' : ('x_RenderMode', 'lua_RenderMode_auto'),
            #'x_LogSystem.ini' : ('x_LogSystem', 'lua_LogSystem_auto'),
            #'x_BentLaser.ini' : ('x_BentLaser', 'lua_BentLaser_auto'),
            #'x_Random.ini' : ('x_Random', 'lua_Random_auto'),
            #'x_ObjSym.ini' : ('x_ObjSym', 'lua_ObjSym_auto'),
            #'x_FileDialog.ini' : ('x_FileDialog', 'lua_FileDialog_auto'),
            #'x_WindowHelper.ini' : ('x_WindowHelper', 'lua_WindowHelper_auto'),
            #'x_ParticlePool.ini' : ('x_ParticlePool', 'lua_ParticlePool_auto'),
            #'x_ResourceMgr.ini' : ('x_ResourceMgr', 'lua_ResourceMgr_auto'),
            #'x_Triangles.ini' : ('x_Triangles', 'lua_Triangles_auto'),
            #'x_Stream.ini' : ('x_Stream', 'lua_Stream_auto'),
            #'x_Buffer.ini' : ('x_Buffer', 'lua_Buffer_auto'),
            'imgui/x_imgui.ini' : ('x_imgui', 'lua_imgui_auto'),
            'imgui/x_imguiStyle.ini' : ('x_imguiStyle', 'lua_imguiStyle_auto'),
            'imgui/x_imguiIO.ini' : ('x_imguiIO', 'lua_imguiIO_auto'),
            'imgui/x_imguiFont.ini' : ('x_imguiFont', 'lua_imguiFont_auto'),
            'imgui/x_imguiDrawList.ini' : ('x_imguiDrawList', 'lua_imguiDrawList_auto'),
            'imgui/x_imguiViewport.ini' : ('x_imguiViewport', 'lua_imguiViewport_auto'),
            #'imgui/x_implot.ini' : ('x_implot', 'lua_implot_auto'),
            #'x_ImGuiColorTextEdit.ini' : ('x_ImGuiColorTextEdit', 'lua_ImGuiColorTextEdit_auto'),
            #'x_Audio.ini' : ('x_Audio', 'lua_Audio_auto'),
            #'x_StopWatch.ini' : ('x_StopWatch', 'lua_StopWatch_auto'),
            #'x_ZipArchive.ini' : ('x_ZipArchive', 'lua_ZipArchive_auto'),
            #'cc_video.ini' : ('cc_video', 'lua_video_auto'),
            #'cc_ext.ini' : ('cc_ext', 'lua_cc_ext_auto'),
            #'cocos2dx_extension.ini' : ('cocos2dx_extension', 'lua_cocos2dx_extension_auto'),
            #'x_cq.ini' : ('x_cq', 'lua_cq_auto'),
            }
        target = 'lua'
        generator_py = '%s/generator.py' % cxx_generator_root
        for key in cmd_args.keys():
            args = cmd_args[key]
            cfg = '%s/%s' % (tolua_root, key)
            print 'Generating bindings for %s...' % (key[:-4])
            out_dir = output_dir + args[0]
            command = '%s %s %s -s %s -t %s -o %s -n %s' % (python_bin, generator_py, cfg, args[0], target, output_dir, args[1])
            _run_cmd(command)

        print '---------------------------------'
        print 'Generating lua bindings succeeds.'
        print '---------------------------------'

    except Exception as e:
        if e.__class__.__name__ == 'CmdError':
            print '---------------------------------'
            print 'Generating lua bindings fails.'
            print '---------------------------------'
            sys.exit(1)
        else:
            raise


# -------------- main --------------
if __name__ == '__main__':
    main()
