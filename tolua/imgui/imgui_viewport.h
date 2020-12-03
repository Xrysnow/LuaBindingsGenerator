#include "imgui.h"

namespace imgui
{

/// The viewports created and managed by Dear ImGui. The role of the platform back-end is to create the platform/OS windows corresponding to each viewport.
/// - Main Area = entire viewport.
/// - Work Area = entire viewport minus sections optionally used by menu bars, status bars. Some positioning code will prefer to use this. Window are also trying to stay within this area.
class ImGuiViewport
{
public:
	/** Unique identifier for the viewport */
    ImGuiID             ID;
	/** See ImGuiViewportFlags_ */
    ImGuiViewportFlags  Flags;
	/** Main Area: Position of the viewport (the imgui coordinates are the same as OS desktop/native coordinates) */
    ImVec2              Pos;
	/** Main Area: Size of the viewport. */
    ImVec2              Size;
    /** Work Area: Offset from Pos to top-left corner of Work Area. Generally (0,0) or (0,+main_menu_bar_height). Work Area is Full Area but without menu-bars/status-bars (so WorkArea always fit inside Pos/Size!) */
    ImVec2              WorkOffsetMin;
    /** Work Area: Offset from Pos+Size to bottom-right corner of Work Area. Generally (0,0) or (0,-status_bar_height). */
    ImVec2              WorkOffsetMax;
 
	/** 1.0f = 96 DPI = No extra scale. */
    float               DpiScale;
	/** The ImDrawData corresponding to this viewport. Valid after Render() and until the next call to NewFrame(). */
    //ImDrawData*         DrawData;
	/** (Advanced) 0: no parent. Instruct the platform backend to setup a parent/child relationship between platform windows. */
    ImGuiID             ParentViewportId;

    // Our design separate the Renderer and Platform backends to facilitate combining default backends with each others.
    // When our create your own backend for a custom engine, it is possible that both Renderer and Platform will be handled
    // by the same system and you may not need to use all the UserData/Handle fields.
    // The library never uses those fields, they are merely storage to facilitate backend implementation.

	/** void* to hold custom data structure for the renderer (e.g. swap chain, frame-buffers etc.) */
    //void*               RendererUserData;
	/** void* to hold custom data structure for the OS / platform (e.g. windowing info, render context) */
    //void*               PlatformUserData;
	/** void* for FindViewportByPlatformHandle(). (e.g. suggested to use natural platform handle such as HWND, GlfwWindow*, SDL_Window*) */
    //void*               PlatformHandle;
	/** Platform window requested closure (e.g. window was moved by the OS / host window manager, e.g. pressing ALT-F4) */
    bool                PlatformRequestClose;
	/** Platform window requested move (e.g. window was moved by the OS / host window manager, authoritative position will be OS window position) */
    bool                PlatformRequestMove;
	/** Platform window requested resize (e.g. window was resized by the OS / host window manager, authoritative size will be OS window size) */
    bool                PlatformRequestResize;

    // ImGuiViewport()     { ID = 0; Flags = 0; DpiScale = 0.0f; DrawData = NULL; ParentViewportId = 0; RendererUserData = PlatformUserData = PlatformHandle = NULL; PlatformRequestClose = PlatformRequestMove = PlatformRequestResize = false; }
    // ~ImGuiViewport()    { IM_ASSERT(PlatformUserData == NULL && RendererUserData == NULL); }

    // Access work-area rectangle with GetWorkXXX functions (see comments above)
    ImVec2              GetCenter();
    ImVec2              GetWorkPos();
    ImVec2              GetWorkSize();
};

}
