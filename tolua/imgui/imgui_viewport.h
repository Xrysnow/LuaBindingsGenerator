#include "imgui.h"

namespace imgui
{
/**
* - Currently represents the Platform Window created by the application which is hosting our Dear ImGui windows.
* - With multi-viewport enabled, we extend this concept to have multiple active viewports.
* - In the future we will extend this concept further to also represent Platform Monitor and support a "no main platform window" operation mode.
* - About Main Area vs Work Area:
*   - Main Area = entire viewport.
*   - Work Area = entire viewport minus sections used by main menu bars (for platform windows), or by task bar (for platform monitor).
*   - Windows are generally trying to stay within the Work Area of their host viewport.
*/
class ImGuiViewport
{
public:
	/** Unique identifier for the viewport */
    ImGuiID             ID;
	/** See ImGuiViewportFlags_ */
    ImGuiViewportFlags  Flags;
	/** Main Area: Position of the viewport (Dear ImGui coordinates are the same as OS desktop/native coordinates) */
    ImVec2              Pos;
	/** Main Area: Size of the viewport. */
    ImVec2              Size;
    /** Work Area: Position of the viewport minus task bars, menus bars, status bars (>= Pos) */
    ImVec2              WorkPos;
    /** Work Area: Size of the viewport minus task bars, menu bars, status bars (<= Size) */
    ImVec2              WorkSize;
	/** 1.0f = 96 DPI = No extra scale. */
    float               DpiScale;
	/** (Advanced) 0: no parent. Instruct the platform backend to setup a parent/child relationship between platform windows. */
    ImGuiID             ParentViewportId;
	/** The ImDrawData corresponding to this viewport. Valid after Render() and until the next call to NewFrame(). */
    //ImDrawData*         DrawData;

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

    // Helpers

    ImVec2              GetCenter() const;
    ImVec2              GetWorkCenter() const;
};

}
