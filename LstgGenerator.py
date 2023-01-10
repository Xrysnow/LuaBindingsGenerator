import os
from Generator.BaseGenerator import BaseGenerator


class LstgGenerator(BaseGenerator):
    def __init__(self, tag: str, outputPath: str = None, clearOldFile: bool = False):
        self.CocosRoot = "../../frameworks/cocos2d-x"
        self.EngineRoot = "../../frameworks"
        fileName = "lua_" + tag + "_auto"
        if not outputPath:
            outputPath = os.path.join(os.path.abspath(os.curdir), "out")
        super().__init__(outputPath, fileName, clearOldFile)

        self.GroupCount = 100
        self.NameSpace |= {
            'lstg::symbol': 'lstg_sym',
            "lstg": "lstg",
            "cocos2d::backend": "ccb",
            "cocos2d": "cc",
        }
        self.Tag = tag
        self.SearchPaths += [
            os.path.join(self.CocosRoot, "cocos"),
            os.path.join(self.CocosRoot, "extensions"),
            os.path.join(self.EngineRoot, "Classes"),
            os.path.join(self.EngineRoot, "LSTG"),
        ]
        self.Headers += [
        ]
        # 解析时的依赖头文件路径等。
        self.ExtraArgs += [
            f"-I{self.CocosRoot}/cocos",
            f"-I{self.CocosRoot}/extensions",
            f"-I{self.CocosRoot}/extensions/scripting/lua-bindings/manual",
            f"-I{self.CocosRoot}/external",
            f"-I{self.CocosRoot}/external/glad/include",
            f"-I{self.CocosRoot}/external/glfw/include/GLFW",
            f"-I{self.CocosRoot}/external/jni.hpp/include",
            f"-I{self.CocosRoot}/external/lua/luajit/include",
            f"-I{self.CocosRoot}/external/lua/tolua",
            f"-I{self.CocosRoot}/external/openal/include",
            f"-I{self.CocosRoot}/external/robin-map/include",
            f"-I{self.EngineRoot}/Classes",
            f"-I{self.EngineRoot}/LSTG",
            f"-I{self.EngineRoot}/external",
            f"-I{self.EngineRoot}/external/libzippp/src/src",
            f"-I{self.EngineRoot}/external/libzippp/libzip/include",
            "-DANDROID",
            "-DCC_USE_GFX=1"
        ]
        self.Classes += []
        self.AllowConstructor = []
        self.RenameMembers |= {
        }
        self.SkipMembers |= {
            '.*': ['^operator.*']
        }
        self.StripEnumEntries |= {
            # "xxx": "pre_"
        }


class BentLaser(LstgGenerator):
    def __init__(self):
        super().__init__('BentLaser')
        self.Headers += [
            f'{self.EngineRoot}/LSTG/Object/GameObjectBentLaser.h',
        ]
        self.Classes += [
            'GameObjectBentLaser.*',
        ]
        self.SkipMembers |= {
            'GameObjectBentLaser': ['AllocInstance', 'FreeInstance']
        }


class Buffer(LstgGenerator):
    def __init__(self):
        super().__init__('Buffer')
        self.Headers += [
            f'{self.EngineRoot}/Classes/XBuffer.h',
        ]
        self.Classes += ['Buffer', ]
        self.SkipMembers |= {'Buffer': ['data']}


class FileDialog(LstgGenerator):
    def __init__(self):
        super().__init__('FileDialog')
        self.Headers += [
            f'{self.EngineRoot}/Classes/XFileDialog.h',
        ]
        self.Classes += ['FileDialog', ]
        self.SkipMembers |= {}


class Latex(LstgGenerator):
    def __init__(self):
        super().__init__('Latex')
        self.Headers += [
            f'{self.EngineRoot}/Classes/XLatex.h',
        ]
        self.Classes += ['Latex', ]
        self.SkipMembers |= {}


class LogSystem(LstgGenerator):
    def __init__(self):
        super().__init__('LogSystem')
        self.Headers += [
            f'{self.EngineRoot}/LSTG/LogSystem.h',
        ]
        self.Classes += ['LogSystem', ]
        self.SkipMembers |= {'LogSystem': ['setOnWrite', '^Log$', '^LogSystem$']}


class ObjSym(LstgGenerator):
    def __init__(self):
        super().__init__('ObjSym')
        self.NameSpace |= {'lstg::symbol': 'lstg_sym'}
        self.Headers += [
            f'{self.EngineRoot}/LSTG/Symbol/GameObjectSymbol.h',
            f'{self.EngineRoot}/LSTG/Symbol/GOSymbolNumber.h',
            f'{self.EngineRoot}/LSTG/Symbol/GOSymbolBoolean.h',
        ]
        self.Classes += 'ComputeNode Operand Operator Number Boolean Statement Assigment Block'.split(' ')
        self.SkipMembers |= {
            'Operator': ['regist$', 'execute', 'reset'],
            '.*': ['getNumber', 'getBoolean', 'exe$', 'pushLua', 'getHash',
                   'isAtomic', 'requireLuaObj', '_set', 'operator.*'],
        }


class ParticlePool(LstgGenerator):
    def __init__(self):
        super().__init__('ParticlePool')
        self.Headers += [
            f'{self.EngineRoot}/LSTG/Resource/ResParticle.h',
        ]
        self.Classes += ['ResParticle', '.*ParticlePool.*']
        self.SkipMembers |= {
            'ResParticle': ['.*'],
            'ParticlePool': ['^update', '^render', 'getParticleInstance'],
        }


class Random(LstgGenerator):
    def __init__(self):
        super().__init__('Random')
        self.Headers += [
            f'{self.EngineRoot}/Classes/XRand.h',
        ]
        self.Classes += ['Random', 'Random::.*']
        self.AllowConstructor = ['Random']
        self.SkipMembers |= {
        }


class RenderMode(LstgGenerator):
    def __init__(self):
        super().__init__('RenderMode')
        self.Headers += [
            f'{self.EngineRoot}/LSTG/RenderMode.h',
        ]
        self.Classes += ['^RenderMode$']
        self.SkipMembers |= {
        }


class Resource(LstgGenerator):
    def __init__(self):
        super().__init__('Resource')
        self.Headers += [
            f'{self.EngineRoot}/LSTG/Resource/GameResources.h',
        ]
        self.Classes += ['Resource', 'ResourceColliable', 'ResourceQuad', 'ResTexture',
                         'ResAnimation', 'ResSprite', 'ResParticle', 'ResAudio', 'ResSound',
                         'ResMusic', 'ResFX', 'ResFont', 'ResRenderTarget', 'ResParticle::.*']
        self.SkipMembers |= {
            'ResParticle': ['newPool', 'getParticleInfo'],
            'ResRenderTarget': ['checkTarget'],
            'ResFont': ['ReadHGEDefine', 'syncLabelSetting'],
            'ResTexture': ['getVertexData', 'getIndexData'],
            'ResParticle::ParticlePool': ['^update', '^render', 'getParticleInstance'],
            '.*': ['pushLua', 'fromLua', 'operator.*'],
        }


class ResourceMgr(LstgGenerator):
    def __init__(self):
        super().__init__('ResourceMgr')
        self.Headers += [
            f'{self.EngineRoot}/LSTG/ResourceMgr.h',
        ]
        self.Classes += ['ResourcePack', 'ResourceMgr']


class StopWatch(LstgGenerator):
    def __init__(self):
        super().__init__('StopWatch')
        self.Headers += [
            f'{self.EngineRoot}/Classes/XStopWatch.h',
        ]
        self.Classes += ['StopWatch']
        self.AllowConstructor = ['StopWatch']


class Stream(LstgGenerator):
    def __init__(self):
        super().__init__('Stream')
        self.Headers += [
            f'{self.EngineRoot}/Classes/XStream.h',
        ]
        self.Classes += ['Stream', 'StreamMemory', 'StreamFile', 'Stream::.*']
        self.SkipMembers |= {
            'Stream': ['read', 'write'],
            'StreamMemory': ['read', 'write', '^data'],
            'StreamFile': ['read', 'write'],
        }


class Triangles(LstgGenerator):
    def __init__(self):
        super().__init__('Triangles')
        self.Headers += [
            f'{self.EngineRoot}/Classes/XTriangles.h',
        ]
        self.Classes += ['Triangles']
        self.SkipMembers |= {
            'Triangles': ['getVertexData', 'getIndexData', 'operator.*'],
        }


class WindowHelper(LstgGenerator):
    def __init__(self):
        super().__init__('WindowHelper')
        self.Headers += [
            f'{self.EngineRoot}/Classes/WindowHelper.h',
        ]
        self.Classes += ['WindowHelper', 'WindowHelper::.*']
        self.SkipMembers |= {
            'WindowHelperDesktop': ['setVideoMode', 'getVideoMode'],
        }


class ZipArchive(LstgGenerator):
    def __init__(self):
        super().__init__('ZipArchive')
        self.Headers += [
            f'{self.EngineRoot}/Classes/XZipArchive.h',
        ]
        self.Classes += ['ZipArchive', 'ZipArchive::.*']
        self.SkipMembers |= {
            # '.*': ['operator.*'],
        }


class Audio(LstgGenerator):
    def __init__(self):
        super().__init__('audio')
        self.NameSpace |= {
            'audio': 'audio'
        }
        self.Headers += [
            f'{self.EngineRoot}/Audio/AudioEngine.h',
        ]
        self.Classes += ['Decoder', 'Engine', 'RecordingDevice', 'SoundData', 'Source', 'Stream']
        self.SkipMembers |= {
            'Source': ['queueData', '.*Atomic', 'copyBuffer', 'getBufferOffset', 'update'],
            'Decoder': ['^getBuffer$', 'getBufferOffset', 'decode', 'setLoopingPoint'],
            'SoundData': ['^getData$'],
            'Stream': ['read'],
            'Engine': ['^init$', '^end$']
        }
        self.AllowConstructor = []
        self.BanConstructor = ['.*']


class FairyGui(LstgGenerator):
    def __init__(self):
        outputPath = os.path.join(os.path.abspath(os.curdir), "out", "fairygui")
        super().__init__('fgui', outputPath)
        self.NameSpace = {
            'fairygui': 'fgui'
        }
        self.Root = f"{self.CocosRoot}/extensions/fairygui"
        self.ExtraArgs += [
            f"-I{self.Root}",
        ]
        self.Headers += [
            f'{self.Root}/FairyGUI.h',
            f'{self.Root}/GLoader3D.h',
            f'{self.Root}/tween/EaseManager.h',
            f'{self.Root}/tween/GPath.h',
            f'{self.Root}/display/FUISprite.h',
            f'{self.Root}/utils/ByteBuffer.h',
            f'{self.Root}/utils/html/HtmlElement.h',
            f'{self.Root}/utils/html/HtmlObject.h',
            f'{self.Root}/utils/html/HtmlParser.h',
        ]
        self.Classes += [
            'EventTag', 'UIEventType', 'EventCallback', 'UIEventDispatcher', 'EventContext', 'IHitTest', 'PixelHitTest',
            'PixelHitTestData', 'InputProcessor', 'CaptureEventCallback', 'InputEvent', 'TextFormat', 'EaseType',
            'EaseManager', 'TweenPropType', 'GPath', 'GTweener', 'GTweenCallback', 'GTweenCallback0', 'GTween',
            'UIPackage', 'PackageItem', 'PackageItemType', 'ObjectType', 'ButtonMode', 'ChildrenRenderOrder',
            'OverflowType', 'ScrollType', 'ScrollBarDisplayType', 'LoaderFillType', 'ProgressTitleType',
            'ListLayoutType', 'ListSelectionMode', 'GroupLayoutType', 'PopupDirection', 'AutoSizeType', 'FlipType',
            'TransitionActionType', 'FillMethod', 'FillOrigin', 'ObjectPropID', 'GController', 'GObject', 'GComponent',
            'GRoot', 'GGroup', 'GScrollBar', 'GLoader', 'GLoader3D', 'GTextField', 'GBasicTextField', 'GGraph',
            'GButton', 'GImage', 'GLabel', 'ListItemRenderer', 'ListItemProvider', 'GList', 'GMovieClip',
            'GProgressBar', 'GComboBox', 'GRichTextField', 'GSlider', 'GTextInput', 'PopupMenu', 'Relations',
            'RelationType', 'RelationItem', 'ScrollPane', 'PlayCompleteCallback', 'TransitionHook', 'Transition',
            'UIConfig', 'IUISource', 'Window', 'DragDropManager', 'GLoaderCreator', 'GComponentCreator',
            'UIObjectFactory', 'GearBase', 'GTreeNode', 'TreeNodeRenderFunction', 'TreeNodeWillExpandFunction', 'GTree',
            'FUIContainer', 'FUIInput', 'FUILabel', 'FUIRichText', 'FUISprite', 'HtmlObject', 'HtmlElement',
            'HtmlElement::Type', 'ByteBuffer'
        ]
        self.SkipMembers |= {
            'ByteBuffer': ['getStringTable', 'setStringTable', '^readSArray$', '^readS$', '^readSP$', '^readSArray$'],
            'GGraph': ['drawPolygon', 'drawRegularPolygon'],
            'GList': ['getSelection'],
            'GTextField': ['getTemplateVars', 'setTemplateVars'],
            'GTree': ['getSelectedNodes'],
            'UIPackage': ['getItemAsset$', 'getItemAssetByURL'],
            'PackageItem': ['^rawData$', 'branches', 'highResolution', '^pixelHitTestData$', '^extensionCreator$',
                            '^bitmapFont$', '^scale9Grid$', 'skeletonAnchor'],
            'UIEventDispatcher': ['dispatchEvent', 'bubbleEvent'],
            'GObject': ['^as$', 'getData', 'setData', 'getDragBounds', '_.*'],
            'GComponent': ['getChildByPath$'],
            'HtmlObject': ['objectPool$'],
            'RelationItem': ['copyFrom$'],
            'Relations': ['copyFrom$'],
            'GearBase': ['getController$', 'setController$', 'getTweenConfig$', 'updateFromRelations$', '^apply$',
                         'updateState$', '^setup$']
        }
        # self.BanConstructor += ['EventTag', 'GTextField']
        self.AllowConstructor = ['EventTag', 'TextFormat']


ALL_GENERATORS = [
    # BentLaser,
    # Buffer,
    # FileDialog,
    # Latex,
    # LogSystem,
    # ObjSym,
    # ParticlePool,
    # Random,
    # RenderMode,
    # Resource,
    # ResourceMgr,
    # StopWatch,
    # Stream,
    # Triangles,
    # WindowHelper,
    # ZipArchive,
    # Audio,
    # FairyGui,
]
