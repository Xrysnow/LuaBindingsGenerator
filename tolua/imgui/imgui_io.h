#include "imgui.h"

#ifdef IMGUI_API
#undef IMGUI_API
#define IMGUI_API
#endif

namespace imgui
{

/** ImGuiIO
* Communicate most settings and inputs/outputs to Dear ImGui using this structure.
* Access via ImGui::GetIO(). Read 'Programmer guide' section in .cpp file for general usage.
*/
class ImGuiIO
{
public:
    //------------------------------------------------------------------
    // Configuration (fill once)                // Default value
    //------------------------------------------------------------------

	/** See ImGuiConfigFlags_ enum. Set by user/application. Gamepad/keyboard navigation options, etc.
	* = 0 */
    ImGuiConfigFlags   ConfigFlags;
	/** See ImGuiBackendFlags_ enum. Set by backend (imgui_impl_xxx files or custom backend) to communicate features supported by the backend.
	* = 0 */
    ImGuiBackendFlags  BackendFlags;
	/** Main display size, in pixels.
	* <unset>         */
    ImVec2      DisplaySize;
	/** Time elapsed since last frame, in seconds.
	* = 1.0f/60.0f    */
    float       DeltaTime;
	/** Minimum time between saving positions/sizes to .ini file, in seconds.
	* = 5.0f          */
    float       IniSavingRate;
	/** Path to .ini file. Set NULL to disable automatic .ini loading/saving, if e.g. you want to manually load/save from memory.
	* = "imgui.ini"   */
    const char* IniFilename;
	/** Path to .log file (default parameter to ImGui::LogToFile when no file is specified).
	* = "imgui_log.txt" */
    const char* LogFilename;
	/** Time for a double-click, in seconds.
	* = 0.30f         */
    float       MouseDoubleClickTime;
	/** Distance threshold to stay in to validate a double-click, in pixels.
	* = 6.0f          */
    float       MouseDoubleClickMaxDist;
	/** Distance threshold before considering we are dragging.
	* = 6.0f          */
    float       MouseDragThreshold;

	/** <unset>          // Map of indices into the KeysDown[512] entries array which represent your "native" keyboard state.*/
	//int         KeyMap[ImGuiKey_COUNT];
	
	/** When holding a key/button, time before it starts repeating, in seconds (for buttons in Repeat mode, etc.).
	* = 0.250f        */
    float       KeyRepeatDelay;
	/** When holding a key/button, rate at which it repeats, in seconds.
	* = 0.050f        */
    float       KeyRepeatRate;
	
	// Store your own data for retrieval by callbacks.
	// = NULL          
    //void*       UserData;

	/** Load, rasterize and pack one or more fonts into a single texture.
	* <auto>          */
    ImFontAtlas*Fonts;
	/** Global scale all fonts
	* = 1.0f          */
    float       FontGlobalScale;
	/** Allow user scaling text of individual window with CTRL+Wheel.
	* = false         */
    bool        FontAllowUserScaling;
	/** Font to use on NewFrame(). Use NULL to uses Fonts->Fonts[0].
	* = NULL          */
    ImFont*     FontDefault;
	/** For retina display or other situations where window coordinates are different from framebuffer coordinates. This generally ends up in ImDrawData::FramebufferScale.
	* = (1, 1)        */
    ImVec2      DisplayFramebufferScale;

    // Docking options (when ImGuiConfigFlags_DockingEnable is set)
	
	/** Simplified docking mode: disable window splitting, so docking is limited to merging multiple windows together into tab-bars.
	* = false */
    bool        ConfigDockingNoSplit;
	/** Enable docking with holding Shift key (reduce visual noise, allows dropping in wider space)
	* = false */
    bool        ConfigDockingWithShift;
	/**
    * [BETA] [FIXME: This currently creates regression with auto-sizing and general overhead] Make every single floating window display within a docking node.
    * = false
    */
    bool        ConfigDockingAlwaysTabBar;
	/** [BETA] Make window or viewport transparent when docking and only display docking boxes on the target viewport. Useful if rendering of multiple viewport cannot be synced. Best used with ConfigViewportsNoAutoMerge.
	* = false */
    bool        ConfigDockingTransparentPayload;
    
    // Viewport options (when ImGuiConfigFlags_ViewportsEnable is set)
	
	/** Set to make all floating imgui windows always create their own viewport. Otherwise, they are merged into the main host viewports when overlapping it.
	* = false */
    bool        ConfigViewportsNoAutoMerge;
	/** Disable default OS task bar icon flag for secondary viewports. When a viewport doesn't want a task bar icon, ImGuiViewportFlags_NoTaskBarIcon will be set on it.
	* = false */
    bool        ConfigViewportsNoTaskBarIcon;
	/** Disable default OS window decoration flag for secondary viewports. When a viewport doesn't want window decorations, ImGuiViewportFlags_NoDecoration will be set on it. Enabling decoration can create subsequent issues at OS levels (e.g. minimum window size).
	* = true */
    bool        ConfigViewportsNoDecoration;
	/** Disable default OS parenting to main viewport for secondary viewports. By default, viewports are marked with ParentViewportId = <main_viewport>, expecting the platform backend to setup a parent/child relationship between the OS windows (some backend may ignore this). Set to true if you want the default to be 0, then all viewports will be top-level OS windows.
	* = false */
    bool        ConfigViewportsNoDefaultParent;

    // Miscellaneous options
	
	/** Request ImGui to draw a mouse cursor for you (if you are on a platform without a mouse cursor). Cannot be easily renamed to 'io.ConfigXXX' because this is frequently used by backend implementations.
	* = false         */
    bool        MouseDrawCursor;
	/** OS X style: Text editing cursor movement using Alt instead of Ctrl, Shortcuts using Cmd/Super instead of Ctrl, Line/Text Start and End using Cmd+Arrows instead of Home/End, Double click selects by word instead of selecting whole text, Multi-selection in lists uses Cmd/Super instead of Ctrl (was called io.OptMacOSXBehaviors prior to 1.63)
	* = defined(__APPLE__) */
    bool        ConfigMacOSXBehaviors;
	/** Set to false to disable blinking cursor, for users who consider it distracting. (was called: io.OptCursorBlink prior to 1.63)
	* = true          */
    bool        ConfigInputTextCursorBlink;
	/** Enable resizing of windows from their edges and from the lower-left corner. This requires (io.BackendFlags & ImGuiBackendFlags_HasMouseCursors) because it needs mouse cursor feedback. (This used to be a per-window ImGuiWindowFlags_ResizeFromAnySide flag)
	* = true          */
    bool        ConfigWindowsResizeFromEdges;
	/** [BETA] Set to true to only allow moving windows when clicked+dragged from the title bar. Windows without a title bar are not affected.
	* = false      */
    bool        ConfigWindowsMoveFromTitleBarOnly;
	/**
    * [BETA] Free transient windows/tables memory buffers when unused for given amount of time. Set to -1.0f to disable.
    * = 60.0f
    */
    float       ConfigMemoryCompactTimer;

    //------------------------------------------------------------------
    // Platform Functions
    // (the imgui_impl_xxxx backend files are setting those up for you)
    //------------------------------------------------------------------

    // Optional: Platform/Renderer backend name (informational only! will be displayed in About Window) + User data for backend/wrappers to store their own stuff.
	
	/** = NULL */
    const char* BackendPlatformName;
	/** = NULL */
    const char* BackendRendererName;
    //void*       BackendPlatformUserData;        // = NULL           // User data for platform backend
    //void*       BackendRendererUserData;        // = NULL           // User data for renderer backend
    //void*       BackendLanguageUserData;        // = NULL           // User data for non C++ programming language backend

    // Optional: Access OS clipboard
    // (default to use native Win32 clipboard on Windows, otherwise uses a private clipboard. Override to access OS clipboard on other architectures)
	/*
    const char* (*GetClipboardTextFn)(void* user_data);
    void        (*SetClipboardTextFn)(void* user_data, const char* text);
    void*       ClipboardUserData;
	*/

    //------------------------------------------------------------------
    // Input - Fill before calling NewFrame()
    //------------------------------------------------------------------

	/** Mouse position, in pixels. Set to ImVec2(-FLT_MAX, -FLT_MAX) if mouse is unavailable (on another screen, etc.) */
    ImVec2      MousePos;
	
	// MMouse buttons: 0=left, 1=right, 2=middle + extras (ImGuiMouseButton_COUNT == 5). Dear ImGui mostly uses left and right buttons. Others buttons allows us to track if the mouse is being used by your application + available to user as a convenience via IsMouse** API.
	//bool        MouseDown[5];
	
	/** Mouse wheel Vertical: 1 unit scrolls about 5 lines text. */
    float       MouseWheel;
	/** Mouse wheel Horizontal. Most users don't have a mouse with an horizontal wheel, may not be filled by all backends. */
    float       MouseWheelH;
	/** (Optional) When using multiple viewports: viewport the OS mouse cursor is hovering _IGNORING_ viewports with the ImGuiViewportFlags_NoInputs flag, and _REGARDLESS_ of whether another viewport is focused. Set io.BackendFlags |= ImGuiBackendFlags_HasMouseHoveredViewport if you can provide this info. If you don't imgui will infer the value using the rectangles and last focused time of the viewports it knows about (ignoring other OS windows). */
    ImGuiID     MouseHoveredViewport;
	/** Keyboard modifier pressed: Control */
    bool        KeyCtrl;
	/** Keyboard modifier pressed: Shift */
    bool        KeyShift;
	/** Keyboard modifier pressed: Alt */
    bool        KeyAlt;
	/** Keyboard modifier pressed: Cmd/Super/Windows */
    bool        KeySuper;
	
	/** Keyboard keys that are pressed (ideally left in the "native" order your engine has access to keyboard keys, so you can use your own defines/enums for keys). */
	//float       NavInputs[ImGuiNavInput_COUNT];
	/** Gamepad inputs. Cleared back to zero by EndFrame(). Keyboard keys will be auto-mapped and be written here by NewFrame(). */
	//bool        KeysDown[512];

    // Functions
	
	/** Queue new character input */
    IMGUI_API void  AddInputCharacter(unsigned int c);
	/** Queue new character input from an UTF-16 character, it can be a surrogate */
    IMGUI_API void  AddInputCharacterUTF16(ImWchar16 c);
	/** Queue new characters input from an UTF-8 string */
    IMGUI_API void  AddInputCharactersUTF8(const char* str);
	/** Clear the text input buffer manually */
    IMGUI_API void  ClearInputCharacters();

    //------------------------------------------------------------------
    // Output - Updated by NewFrame() or EndFrame()/Render()
    // (when reading from the io.WantCaptureMouse, io.WantCaptureKeyboard flags to dispatch your inputs, it is
    //  generally easier and more correct to use their state BEFORE calling NewFrame(). See FAQ for details!)
    //------------------------------------------------------------------

	/** Set when Dear ImGui will use mouse inputs, in this case do not dispatch them to your main game/application (either way, always pass on mouse inputs to imgui). (e.g. unclicked mouse is hovering over an imgui window, widget is active, mouse was clicked over an imgui window, etc.). */
    bool        WantCaptureMouse;
	/** Set when Dear ImGui will use keyboard inputs, in this case do not dispatch them to your main game/application (either way, always pass keyboard inputs to imgui). (e.g. InputText active, or an imgui window is focused and navigation is enabled, etc.). */
    bool        WantCaptureKeyboard;
	/** Mobile/console: when set, you may display an on-screen keyboard. This is set by Dear ImGui when it wants textual keyboard input to happen (e.g. when a InputText widget is active). */
    bool        WantTextInput;
	/** MousePos has been altered, backend should reposition mouse on next frame. Rarely used! Set only when ImGuiConfigFlags_NavEnableSetMousePos flag is enabled. */
    bool        WantSetMousePos;
	/** When manual .ini load/save is active (io.IniFilename == NULL), this will be set to notify your application that you can call SaveIniSettingsToMemory() and save yourself. Important: clear io.WantSaveIniSettings yourself after saving! */
    bool        WantSaveIniSettings;
	/** Keyboard/Gamepad navigation is currently allowed (will handle ImGuiKey_NavXXX events) = a window is focused and it doesn't use the ImGuiWindowFlags_NoNavInputs flag. */
    bool        NavActive;
	/** Keyboard/Gamepad navigation is visible and allowed (will handle ImGuiKey_NavXXX events). */
    bool        NavVisible;
	/** Application framerate estimate, in frame per second. Solely for convenience. Rolling average estimation based on io.DeltaTime over 120 frames. */
    float       Framerate;
	/** Vertices output during last call to Render() */
    int         MetricsRenderVertices;
	/** Indices output during last call to Render() = number of triangles * 3 */
    int         MetricsRenderIndices;
	/** Number of visible windows */
    int         MetricsRenderWindows;
	/** Number of active windows */
    int         MetricsActiveWindows;
	/** Number of active allocations, updated by MemAlloc/MemFree based on current context. May be off if you have multiple imgui contexts. */
    int         MetricsActiveAllocations;
	/** Mouse delta. Note that this is zero if either current or previous position are invalid (-FLT_MAX,-FLT_MAX), so a disappearing/reappearing mouse won't have a huge delta. */
    ImVec2      MouseDelta;

};

}
