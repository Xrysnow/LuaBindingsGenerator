// functions can auto bind

#include <cstdint>
#include <float.h>

#define IMGUI_API static
#define IM_FMTARGS(FMT)
#define IM_FMTLIST(FMT)

//-----------------------------------------------------------------------------
// Forward declarations and basic types
//-----------------------------------------------------------------------------

// Forward declarations
struct ImDrawChannel;               // Temporary storage to output draw commands out of order, used by ImDrawListSplitter and ImDrawList::ChannelsSplit()
struct ImDrawCmd;                   // A single draw command within a parent ImDrawList (generally maps to 1 GPU draw call, unless it is a callback)
struct ImDrawData;                  // All draw command lists required to render the frame + pos/size coordinates to use for the projection matrix.
struct ImDrawList;                  // A single draw command list (generally one per window, conceptually you may see this as a dynamic "mesh" builder)
struct ImDrawListSharedData;        // Data shared among multiple draw lists (typically owned by parent ImGui context, but you may create one yourself)
struct ImDrawListSplitter;          // Helper to split a draw list into different layers which can be drawn into out of order, then flattened back.
struct ImDrawVert;                  // A single vertex (pos + uv + col = 20 bytes by default. Override layout with IMGUI_OVERRIDE_DRAWVERT_STRUCT_LAYOUT)
struct ImFont;                      // Runtime data for a single font within a parent ImFontAtlas
struct ImFontAtlas;                 // Runtime data for multiple fonts, bake multiple fonts into a single texture, TTF/OTF font loader
struct ImFontBuilderIO;             // Opaque interface to a font builder (stb_truetype or FreeType).
struct ImFontConfig;                // Configuration data when adding a font or merging fonts
struct ImFontGlyph;                 // A single font glyph (code point + coordinates within in ImFontAtlas + offset)
struct ImFontGlyphRangesBuilder;    // Helper to build glyph ranges from text/string data
struct ImColor;                     // Helper functions to create a color that can be converted to either u32 or float4 (*OBSOLETE* please avoid using)
struct ImGuiContext;                // Dear ImGui context (opaque structure, unless including imgui_internal.h)
struct ImGuiIO;                     // Main configuration and I/O between your application and ImGui
struct ImGuiInputTextCallbackData;  // Shared state of InputText() when using custom ImGuiInputTextCallback (rare/advanced use)
struct ImGuiListClipper;            // Helper to manually clip large list of items
struct ImGuiOnceUponAFrame;         // Helper for running a block of code not more than once a frame, used by IMGUI_ONCE_UPON_A_FRAME macro
struct ImGuiPayload;                // User data payload for drag and drop operations
struct ImGuiPlatformIO;             // Multi-viewport support: interface for Platform/Renderer backends + viewports to render
struct ImGuiPlatformMonitor;        // Multi-viewport support: user-provided bounds for each connected monitor/display. Used when positioning popups and tooltips to avoid them straddling monitors
struct ImGuiSizeCallbackData;       // Callback data when using SetNextWindowSizeConstraints() (rare/advanced use)
struct ImGuiStorage;                // Helper for key->value storage
struct ImGuiStyle;                  // Runtime data for styling/colors
struct ImGuiTableSortSpecs;         // Sorting specifications for a table (often handling sort specs for a single column, occasionally more)
struct ImGuiTableColumnSortSpecs;   // Sorting specification for one column of a table
struct ImGuiTextBuffer;             // Helper to hold and append into a text buffer (~string builder)
struct ImGuiTextFilter;             // Helper to parse and apply text filters (e.g. "aaaaa[,bbbbb][,ccccc]")
struct ImGuiViewport;               // A Platform Window (always 1 unless multi-viewport are enabled. One per platform window to output to). In the future may represent Platform Monitor
struct ImGuiWindowClass;            // Window class (rare/advanced uses: provide hints to the platform backend via altered viewport flags and parent/child info)

// Enums/Flags (declared as int for compatibility with old C++, to allow using as flags and to not pollute the top of this file)
// - Tip: Use your programming IDE navigation facilities on the names in the _central column_ below to find the actual flags/enum lists!
//   In Visual Studio IDE: CTRL+comma ("Edit.NavigateTo") can follow symbols in comments, whereas CTRL+F12 ("Edit.GoToImplementation") cannot.
//   With Visual Assist installed: ALT+G ("VAssistX.GoToImplementation") can also follow symbols in comments.
typedef int ImGuiCol;               // -> enum ImGuiCol_             // Enum: A color identifier for styling
typedef int ImGuiCond;              // -> enum ImGuiCond_            // Enum: A condition for many Set*() functions
typedef int ImGuiDataType;          // -> enum ImGuiDataType_        // Enum: A primary data type
typedef int ImGuiDir;               // -> enum ImGuiDir_             // Enum: A cardinal direction
typedef int ImGuiKey;               // -> enum ImGuiKey_             // Enum: A key identifier (ImGui-side enum)
typedef int ImGuiNavInput;          // -> enum ImGuiNavInput_        // Enum: An input identifier for navigation
typedef int ImGuiMouseButton;       // -> enum ImGuiMouseButton_     // Enum: A mouse button identifier (0=left, 1=right, 2=middle)
typedef int ImGuiMouseCursor;       // -> enum ImGuiMouseCursor_     // Enum: A mouse cursor identifier
typedef int ImGuiSortDirection;     // -> enum ImGuiSortDirection_   // Enum: A sorting direction (ascending or descending)
typedef int ImGuiStyleVar;          // -> enum ImGuiStyleVar_        // Enum: A variable identifier for styling
typedef int ImGuiTableBgTarget;     // -> enum ImGuiTableBgTarget_   // Enum: A color target for TableSetBgColor()
typedef int ImDrawFlags;            // -> enum ImDrawFlags_          // Flags: for ImDrawList functions
typedef int ImDrawListFlags;        // -> enum ImDrawListFlags_      // Flags: for ImDrawList instance
typedef int ImFontAtlasFlags;       // -> enum ImFontAtlasFlags_     // Flags: for ImFontAtlas build
typedef int ImGuiBackendFlags;      // -> enum ImGuiBackendFlags_    // Flags: for io.BackendFlags
typedef int ImGuiButtonFlags;       // -> enum ImGuiButtonFlags_     // Flags: for InvisibleButton()
typedef int ImGuiColorEditFlags;    // -> enum ImGuiColorEditFlags_  // Flags: for ColorEdit4(), ColorPicker4() etc.
typedef int ImGuiConfigFlags;       // -> enum ImGuiConfigFlags_     // Flags: for io.ConfigFlags
typedef int ImGuiComboFlags;        // -> enum ImGuiComboFlags_      // Flags: for BeginCombo()
typedef int ImGuiDockNodeFlags;     // -> enum ImGuiDockNodeFlags_   // Flags: for DockSpace()
typedef int ImGuiDragDropFlags;     // -> enum ImGuiDragDropFlags_   // Flags: for BeginDragDropSource(), AcceptDragDropPayload()
typedef int ImGuiFocusedFlags;      // -> enum ImGuiFocusedFlags_    // Flags: for IsWindowFocused()
typedef int ImGuiHoveredFlags;      // -> enum ImGuiHoveredFlags_    // Flags: for IsItemHovered(), IsWindowHovered() etc.
typedef int ImGuiInputTextFlags;    // -> enum ImGuiInputTextFlags_  // Flags: for InputText(), InputTextMultiline()
typedef int ImGuiKeyModFlags;       // -> enum ImGuiKeyModFlags_     // Flags: for io.KeyMods (Ctrl/Shift/Alt/Super)
typedef int ImGuiPopupFlags;        // -> enum ImGuiPopupFlags_      // Flags: for OpenPopup*(), BeginPopupContext*(), IsPopupOpen()
typedef int ImGuiSelectableFlags;   // -> enum ImGuiSelectableFlags_ // Flags: for Selectable()
typedef int ImGuiSliderFlags;       // -> enum ImGuiSliderFlags_     // Flags: for DragFloat(), DragInt(), SliderFloat(), SliderInt() etc.
typedef int ImGuiTabBarFlags;       // -> enum ImGuiTabBarFlags_     // Flags: for BeginTabBar()
typedef int ImGuiTabItemFlags;      // -> enum ImGuiTabItemFlags_    // Flags: for BeginTabItem()
typedef int ImGuiTableFlags;        // -> enum ImGuiTableFlags_      // Flags: For BeginTable()
typedef int ImGuiTableColumnFlags;  // -> enum ImGuiTableColumnFlags_// Flags: For TableSetupColumn()
typedef int ImGuiTableRowFlags;     // -> enum ImGuiTableRowFlags_   // Flags: For TableNextRow()
typedef int ImGuiTreeNodeFlags;     // -> enum ImGuiTreeNodeFlags_   // Flags: for TreeNode(), TreeNodeEx(), CollapsingHeader()
typedef int ImGuiViewportFlags;     // -> enum ImGuiViewportFlags_   // Flags: for ImGuiViewport
typedef int ImGuiWindowFlags;       // -> enum ImGuiWindowFlags_     // Flags: for Begin(), BeginChild()

// Other types
#ifndef ImTextureID                 // ImTextureID [configurable type: override in imconfig.h with '#define ImTextureID xxx']
typedef void* ImTextureID;          // User data for rendering backend to identify a texture. This is whatever to you want it to be! read the FAQ about ImTextureID for details.
#endif
typedef unsigned int ImGuiID;       // A unique ID used by widgets, typically hashed from a stack of string.
typedef int (*ImGuiInputTextCallback)(ImGuiInputTextCallbackData* data);    // Callback function for ImGui::InputText()
typedef void (*ImGuiSizeCallback)(ImGuiSizeCallbackData* data);             // Callback function for ImGui::SetNextWindowSizeConstraints()
typedef void* (*ImGuiMemAllocFunc)(size_t sz, void* user_data);             // Function signature for ImGui::SetAllocatorFunctions()
typedef void (*ImGuiMemFreeFunc)(void* ptr, void* user_data);               // Function signature for ImGui::SetAllocatorFunctions()

// Character types
// (we generally use UTF-8 encoded string in the API. This is storage specifically for a decoded character used for keyboard input and display)
typedef unsigned short ImWchar16;   // A single decoded U16 character/code point. We encode them as multi bytes UTF-8 when used in strings.
typedef unsigned int ImWchar32;     // A single decoded U32 character/code point. We encode them as multi bytes UTF-8 when used in strings.
#ifdef IMGUI_USE_WCHAR32            // ImWchar [configurable type: override in imconfig.h with '#define IMGUI_USE_WCHAR32' to support Unicode planes 1-16]
typedef ImWchar32 ImWchar;
#else
typedef ImWchar16 ImWchar;
#endif

// Basic scalar data types
typedef signed char         ImS8;   // 8-bit signed integer
typedef unsigned char       ImU8;   // 8-bit unsigned integer
typedef signed short        ImS16;  // 16-bit signed integer
typedef unsigned short      ImU16;  // 16-bit unsigned integer
typedef signed int          ImS32;  // 32-bit signed integer == int
typedef unsigned int        ImU32;  // 32-bit unsigned integer (often used to store packed colors)
#if defined(_MSC_VER) && !defined(__clang__)
typedef signed   __int64    ImS64;  // 64-bit signed integer (pre and post C++11 with Visual Studio)
typedef unsigned __int64    ImU64;  // 64-bit unsigned integer (pre and post C++11 with Visual Studio)
#elif (defined(__clang__) || defined(__GNUC__)) && (__cplusplus < 201100)
#include <stdint.h>
typedef int64_t             ImS64;  // 64-bit signed integer (pre C++11)
typedef uint64_t            ImU64;  // 64-bit unsigned integer (pre C++11)
#else
typedef signed   long long  ImS64;  // 64-bit signed integer (post C++11)
typedef unsigned long long  ImU64;  // 64-bit unsigned integer (post C++11)
#endif

// 2D vector (often used to store positions, sizes, etc.)
struct ImVec2
{
    float     x, y;
    ImVec2()  { x = y = 0.0f; }
    ImVec2(float _x, float _y) { x = _x; y = _y; }
};

// 4D vector (often used to store floating-point colors)
struct ImVec4
{
    float     x, y, z, w;
    ImVec4()  { x = y = z = w = 0.0f; }
    ImVec4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
};

namespace imgui {

class ImFont;
class ImFontAtlas;

class ImGui
{
	
public:

	/**  Main */
	
	/**  start a new Dear ImGui frame, you can submit any command from this point until Render()/EndFrame(). */
    IMGUI_API void          NewFrame();
	/**  ends the Dear ImGui frame. automatically called by Render(). If you don't need to render data (skipping rendering) you may call EndFrame() without Render()... but you'll have wasted CPU already! If you don't need to render, better to not create any windows and not call NewFrame() at all! */
    IMGUI_API void          EndFrame();
	/**  ends the Dear ImGui frame, finalize the draw data. You can then get call GetDrawData(). */
    IMGUI_API void          Render();


    // Demo, Debug, Information

	/**  add style selector block (not a window), essentially a combo listing the default styles. */
    IMGUI_API bool          ShowStyleSelector(const char* label);
	/**  add font selector block (not a window), essentially a combo listing the loaded fonts. */
    IMGUI_API void          ShowFontSelector(const char* label);
	/**  add basic help/info block (not a window): how to manipulate ImGui as a end-user (mouse/keyboard controls). */
    IMGUI_API void          ShowUserGuide();
	/**  get the compiled version string e.g. "1.80 WIP" (essentially the value for IMGUI_VERSION from the compiled version of imgui.cpp) */
    IMGUI_API const char*   GetVersion();

    // Windows
    // - Begin() = push window to the stack and start appending to it. End() = pop window from the stack.
    // - You may append multiple times to the same window during the same frame.
    // - Passing 'bool* p_open != NULL' shows a window-closing widget in the upper-right corner of the window,
    //   which clicking will set the boolean to false when clicked.
    // - Begin() return false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting
    //   anything to the window. Always call a matching End() for each Begin() call, regardless of its return value!
    //   [Important: due to legacy reason, this is inconsistent with most other functions such as BeginMenu/EndMenu,
    //    BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function
    //    returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]
    // - Note that the bottom of window stack always contains a window called "Debug".


    // Child Windows
    // - Use child windows to begin into a self-contained independent scrolling/clipping regions within a host window. Child windows can embed their own child.
    // - For each independent axis of 'size': ==0.0f: use remaining host window size / >0.0f: fixed size / <0.0f: use remaining window size minus abs(size) / Each axis can use a different mode, e.g. ImVec2(0,400).
    // - BeginChild() returns false to indicate the window is collapsed or fully clipped, so you may early out and omit submitting anything to the window.
    //   Always call a matching EndChild() for each BeginChild() call, regardless of its return value.
    //   [Important: due to legacy reason, this is inconsistent with most other functions such as BeginMenu/EndMenu,
    //    BeginPopup/EndPopup, etc. where the EndXXX call should only be called if the corresponding BeginXXX function
    //    returned true. Begin and BeginChild are the only odd ones out. Will be fixed in a future update.]

	
    // Windows Utilities
    // - 'current window' = the window we are appending into while inside a Begin()/End() block. 'next window' = next window we will Begin() into.

    IMGUI_API bool          IsWindowAppearing();
    IMGUI_API bool          IsWindowCollapsed();
	/**  is current window focused? or its root/child, depending on flags. see flags for options. */
    IMGUI_API bool          IsWindowFocused(ImGuiFocusedFlags flags=0);
	/**  is current window hovered (and typically: not blocked by a popup/modal)? see flags for options. NB: If you are trying to check whether your mouse should be dispatched to imgui or to your app, you should use the 'io.WantCaptureMouse' boolean for that! Please read the FAQ! */
    IMGUI_API bool          IsWindowHovered(ImGuiHoveredFlags flags=0);
    /** get draw list associated to the current window, to append your own drawing primitives */
    IMGUI_API ImDrawList*   GetWindowDrawList();
    /** get DPI scale currently associated to the current window's viewport. */
	IMGUI_API float         GetWindowDpiScale();
	/**  get current window position in screen space (useful if you want to do your own drawing via the DrawList API) */
    IMGUI_API ImVec2        GetWindowPos();
	/**  get current window size */
    IMGUI_API ImVec2        GetWindowSize();
	/**  get current window width (shortcut for GetWindowSize().x) */
    IMGUI_API float         GetWindowWidth();
	/**  get current window height (shortcut for GetWindowSize().y) */
    IMGUI_API float         GetWindowHeight();
	/** get viewport currently associated to the current window. */
    IMGUI_API ImGuiViewport*GetWindowViewport();

	//  Prefer using SetNextXXX functions (before Begin) rather that SetXXX functions (after Begin).

	/**  set next window position. call before Begin(). use pivot=(0.5f,0.5f) to center on given point, etc. */
    IMGUI_API void          SetNextWindowPos(const ImVec2& pos, ImGuiCond cond = 0, const ImVec2& pivot = ImVec2(0,0));
	/**  set next window size. set axis to 0.0f to force an auto-fit on this axis. call before Begin() */
    IMGUI_API void          SetNextWindowSize(const ImVec2& size, ImGuiCond cond = 0);
	/**  set next window size limits. use -1,-1 on either X/Y axis to preserve the current size. Sizes will be rounded down. Use callback to apply non-trivial programmatic constraints. */
    IMGUI_API void          SetNextWindowSizeConstraints(const ImVec2& size_min, const ImVec2& size_max);
	/**  set next window content size (~ enforce the range of scrollbars). not including window decorations (title bar, menu bar, etc.). set an axis to 0.0f to leave it automatic. call before Begin() */
    IMGUI_API void          SetNextWindowContentSize(const ImVec2& size);
	/**  set next window collapsed state. call before Begin() */
    IMGUI_API void          SetNextWindowCollapsed(bool collapsed, ImGuiCond cond = 0);
	/**  set next window to be focused / front-most. call before Begin() */
    IMGUI_API void          SetNextWindowFocus();
	/**  set next window background color alpha. helper to easily modify ImGuiCol_WindowBg/ChildBg/PopupBg. you may also use ImGuiWindowFlags_NoBackground. */
    IMGUI_API void          SetNextWindowBgAlpha(float alpha);
    /** set next window viewport */
    IMGUI_API void          SetNextWindowViewport(ImGuiID viewport_id);
	/**  set font scale. Adjust IO.FontGlobalScale if you want to scale all windows */
    IMGUI_API void          SetWindowFontScale(float scale);
	/**  set named window position. */
    IMGUI_API void          SetWindowPos(const char* name, const ImVec2& pos, ImGuiCond cond = 0);
	/**  set named window size. set axis to 0.0f to force an auto-fit on this axis. */
    IMGUI_API void          SetWindowSize(const char* name, const ImVec2& size, ImGuiCond cond = 0);
	/**  set named window collapsed state */
    IMGUI_API void          SetWindowCollapsed(const char* name, bool collapsed, ImGuiCond cond = 0);
	/**  set named window to be focused / front-most. use NULL to remove focus. */
    IMGUI_API void          SetWindowFocus(const char* name);

    // Content region
    // - Those functions are bound to be redesigned soon (they are confusing, incomplete and return values in local window coordinates which increases confusion)

	/**  == GetContentRegionMax() - GetCursorPos() */
    IMGUI_API ImVec2        GetContentRegionAvail();
	/**  current content boundaries (typically window boundaries including scrolling, or current column boundaries), in windows coordinates */
    IMGUI_API ImVec2        GetContentRegionMax();
	/**  content boundaries min (roughly (0,0)-Scroll), in window coordinates */
    IMGUI_API ImVec2        GetWindowContentRegionMin();
	/**  content boundaries max (roughly (0,0)+Size-Scroll) where Size can be override with SetNextWindowContentSize(), in window coordinates */
    IMGUI_API ImVec2        GetWindowContentRegionMax();
	/**  */
    IMGUI_API float         GetWindowContentRegionWidth();

    // Windows Scrolling

	/**  get scrolling amount [0..GetScrollMaxX()] */
    IMGUI_API float         GetScrollX();
	/**  get scrolling amount [0..GetScrollMaxY()] */
    IMGUI_API float         GetScrollY();
	/**  set scrolling amount [0..GetScrollMaxX()] */
    IMGUI_API void          SetScrollX(float scroll_x);
	/**  set scrolling amount [0..GetScrollMaxY()] */
    IMGUI_API void          SetScrollY(float scroll_y);
	/**  get maximum scrolling amount ~~ ContentSize.x - WindowSize.x */
    IMGUI_API float         GetScrollMaxX();
	/**  get maximum scrolling amount ~~ ContentSize.y - WindowSize.y */
    IMGUI_API float         GetScrollMaxY();
    /**
    * adjust scrolling amount to make current cursor position visible. center_x_ratio=0.0: left, 0.5: center, 1.0: right. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead.
    */
    IMGUI_API void          SetScrollHereX(float center_x_ratio = 0.5f);
	/**  adjust scrolling amount to make current cursor position visible. center_y_ratio=0.0: top, 0.5: center, 1.0: bottom. When using to make a "default/current item" visible, consider using SetItemDefaultFocus() instead. */
    IMGUI_API void          SetScrollHereY(float center_y_ratio = 0.5f);
    /**
    * adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position.
    */
    IMGUI_API void          SetScrollFromPosX(float local_x, float center_x_ratio = 0.5f);
	/**  adjust scrolling amount to make given position visible. Generally GetCursorStartPos() + offset to compute a valid position. */
    IMGUI_API void          SetScrollFromPosY(float local_y, float center_y_ratio = 0.5f);

    // Parameters stacks (shared)

	//IMGUI_API void          PushFont(ImFont* font);   // use NULL as a shortcut to push default font

    IMGUI_API void          PopFont();

    //IMGUI_API void          PushStyleColor(ImGuiCol idx, ImU32 col);    // modify a style color. always use this if you modify the style after NewFrame().

    //IMGUI_API void          PushStyleColor(ImGuiCol idx, const ImVec4& col);

    IMGUI_API void          PopStyleColor(int count = 1);

    //IMGUI_API void          PushStyleVar(ImGuiStyleVar idx, float val);                     // modify a style float variable. always use this if you modify the style after NewFrame().

    //IMGUI_API void          PushStyleVar(ImGuiStyleVar idx, const ImVec2& val);             // modify a style ImVec2 variable. always use this if you modify the style after NewFrame().

    IMGUI_API void          PopStyleVar(int count = 1);
	/**  == tab stop enable. Allow focusing using TAB/Shift-TAB, enabled by default but you can disable it for certain widgets */
    IMGUI_API void          PushAllowKeyboardFocus(bool allow_keyboard_focus);

    IMGUI_API void          PopAllowKeyboardFocus();
	/**  in 'repeat' mode, Button*() functions return repeated true in a typematic manner (using io.KeyRepeatDelay/io.KeyRepeatRate setting). Note that you can call IsItemActive() after any Button() to tell if the button is held in the current frame. */
    IMGUI_API void          PushButtonRepeat(bool repeat);
	
    IMGUI_API void          PopButtonRepeat();

    // Parameters stacks (current window)

	/**  push width of items for common large "item+label" widgets. >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -FLT_MIN always align width to the right side). */
    IMGUI_API void          PushItemWidth(float item_width);
    IMGUI_API void          PopItemWidth();
	/**  set width of the _next_ common large "item+label" widget. >0.0f: width in pixels, <0.0f align xx pixels to the right of window (so -FLT_MIN always align width to the right side) */
    IMGUI_API void          SetNextItemWidth(float item_width);
	/**  width of item given pushed settings and current cursor position */
    IMGUI_API float         CalcItemWidth();
	/**  push word-wrapping position for Text*() commands. < 0.0f: no wrapping; 0.0f: wrap to end of window (or column); > 0.0f: wrap at 'wrap_pos_x' position in window local space */
    IMGUI_API void          PushTextWrapPos(float wrap_local_pos_x = 0.0f);
    IMGUI_API void          PopTextWrapPos();

    // Style read access

    //IMGUI_API ImFont*       GetFont();                                                      // get current font
	/**  get current font size (= height in pixels) of current font with current scale applied */
    IMGUI_API float         GetFontSize();
	/**  get UV coordinate for a while pixel, useful to draw custom shapes via the ImDrawList API */
    IMGUI_API ImVec2        GetFontTexUvWhitePixel();

    //IMGUI_API ImU32         GetColorU32(ImGuiCol idx, float alpha_mul = 1.0f);              // retrieve given style color with style alpha applied and optional extra alpha multiplier, packed as a 32-bit value suitable for ImDrawList

    //IMGUI_API ImU32         GetColorU32(const ImVec4& col);                                 // retrieve given color with style alpha applied, packed as a 32-bit value suitable for ImDrawList
 
    //IMGUI_API ImU32         GetColorU32(ImU32 col);                                         // retrieve given color with style alpha applied, packed as a 32-bit value suitable for ImDrawList
	/**  retrieve style color as stored in ImGuiStyle structure. use to feed back into PushStyleColor(), otherwise use GetColorU32() to get style color with style alpha baked in. */
    IMGUI_API const ImVec4& GetStyleColorVec4(ImGuiCol idx);


    // Cursor / Layout
    // - By "cursor" we mean the current output position.
    // - The typical widget behavior is to output themselves at the current cursor position, then move the cursor one line down.
	// - You can call SameLine() between widgets to undo the last carriage return and output at the right of the preceeding widget.

	/**  separator, generally horizontal. inside a menu bar or in horizontal layout mode, this becomes a vertical separator. */
    IMGUI_API void          Separator();
	/**  call between widgets or groups to layout them horizontally. X position given in window coordinates. */
    IMGUI_API void          SameLine(float offset_from_start_x=0.0f, float spacing=-1.0f);
	/**  undo a SameLine() or force a new line when in an horizontal-layout context. */
    IMGUI_API void          NewLine();
	/**  add vertical spacing. */
    IMGUI_API void          Spacing();
	/**  add a dummy item of given size. unlike InvisibleButton(), Dummy() won't take the mouse click or be navigable into. */
    IMGUI_API void          Dummy(const ImVec2& size);
	/**  move content position toward the right, by indent_w, or style.IndentSpacing if indent_w <= 0 */
    IMGUI_API void          Indent(float indent_w = 0.0f);
	/**  move content position back to the left, by indent_w, or style.IndentSpacing if indent_w <= 0 */
    IMGUI_API void          Unindent(float indent_w = 0.0f);
	/**  lock horizontal starting position */
    IMGUI_API void          BeginGroup();
	/**  unlock horizontal starting position + capture the whole group bounding box into one "item" (so you can use IsItemHovered() or layout primitives such as SameLine() on whole group, etc.) */
    IMGUI_API void          EndGroup();
	/**  cursor position in window coordinates (relative to window position) */
    IMGUI_API ImVec2        GetCursorPos();
	/**    (some functions are using window-relative coordinates, such as: GetCursorPos, GetCursorStartPos, GetContentRegionMax, GetWindowContentRegion* etc. */
    IMGUI_API float         GetCursorPosX();
	/**     other functions such as GetCursorScreenPos or everything in ImDrawList:: */
    IMGUI_API float         GetCursorPosY();
	/**     are using the main, absolute coordinate system. */
    IMGUI_API void          SetCursorPos(const ImVec2& local_pos);
	/**     GetWindowPos() + GetCursorPos() == GetCursorScreenPos() etc.) */
    IMGUI_API void          SetCursorPosX(float local_x);
	/**  */
    IMGUI_API void          SetCursorPosY(float local_y);
	/**  initial cursor position in window coordinates */
    IMGUI_API ImVec2        GetCursorStartPos();
	/**  cursor position in absolute coordinates (useful to work with ImDrawList API). generally top-left == GetMainViewport()->Pos == (0,0) in single viewport mode, and bottom-right == GetMainViewport()->Pos+Size == io.DisplaySize in single-viewport mode. */
    IMGUI_API ImVec2        GetCursorScreenPos();
	/**  cursor position in absolute coordinates */
    IMGUI_API void          SetCursorScreenPos(const ImVec2& pos);
	/**  vertically align upcoming text baseline to FramePadding.y so that it will align properly to regularly framed items (call if you have text on a line before a framed item) */
    IMGUI_API void          AlignTextToFramePadding();
	/**  ~ FontSize */
    IMGUI_API float         GetTextLineHeight();
	/**  ~ FontSize + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of text) */
    IMGUI_API float         GetTextLineHeightWithSpacing();
	/**  ~ FontSize + style.FramePadding.y * 2 */
    IMGUI_API float         GetFrameHeight();
	/**  ~ FontSize + style.FramePadding.y * 2 + style.ItemSpacing.y (distance in pixels between 2 consecutive lines of framed widgets) */
    IMGUI_API float         GetFrameHeightWithSpacing();

    // ID stack/scopes
    // - Read the FAQ for more details about how ID are handled in dear imgui. If you are creating widgets in a loop you most
    //   likely want to push a unique identifier (e.g. object pointer, loop index) to uniquely differentiate them.
    // - The resulting ID are hashes of the entire stack.
    // - You can also use the "Label##foobar" syntax within widget label to distinguish them from each others.
    // - In this header file we use the "label"/"name" terminology to denote a string that will be displayed and used as an ID,
    //   whereas "str_id" denote a string that is only used as an ID and not normally displayed.

	/**  pop from the ID stack. */
    IMGUI_API void          PopID();
	/**  calculate unique ID (hash of whole ID stack + given parameter). e.g. if you want to query into ImGuiStorage yourself */
    IMGUI_API ImGuiID       GetID(const char* str_id);
	/**  Widgets: Text */
    IMGUI_API ImGuiID       GetID(const char* str_id_begin, const char* str_id_end);
	/**  raw text without formatting. Roughly equivalent to Text("%s", text) but: A) doesn't require null terminated string if 'text_end' is specified, B) it's faster, no memory copy is done, no buffer size limits, recommended for long chunks of text. */
    IMGUI_API void          TextUnformatted(const char* text, const char* text_end = NULL);
	/**  simple formatted text */
    IMGUI_API void          Text(const char* fmt);
	/**  shortcut for PushStyleColor(ImGuiCol_Text, col); Text(fmt); PopStyleColor(); */
    IMGUI_API void          TextColored(const ImVec4& col, const char* fmt)            IM_FMTARGS(2);
	/**  shortcut for PushStyleColor(ImGuiCol_Text, style.Colors[ImGuiCol_TextDisabled]); Text(fmt); PopStyleColor(); */
    IMGUI_API void          TextDisabled(const char* fmt)                              IM_FMTARGS(1);
	/**  shortcut for PushTextWrapPos(0.0f); Text(fmt); PopTextWrapPos();. Note that this won't work on an auto-resizing window if there's no other widgets to extend the window width, yoy may need to set a size using SetNextWindowSize(). */
    IMGUI_API void          TextWrapped(const char* fmt)                               IM_FMTARGS(1);
	/**  display text+label aligned the same way as value+label widgets */
    IMGUI_API void          LabelText(const char* label, const char* fmt)              IM_FMTARGS(2);
	/**  shortcut for Bullet()+Text() */
    IMGUI_API void          BulletText(const char* fmt)                                IM_FMTARGS(1);

    // Widgets: Main
    // - Most widgets return true when the value has been changed or when pressed/selected
    // - You may also use one of the many IsItemXXX functions (e.g. IsItemActive, IsItemHovered, etc.) to query widget state.

	/**  button */
    IMGUI_API bool          Button(const char* label, const ImVec2& size = ImVec2(0,0));
	/**  button with FramePadding=(0,0) to easily embed within text */
    IMGUI_API bool          SmallButton(const char* label);
	/**  flexible button behavior without the visuals, frequently useful to build custom behaviors using the public api (along with IsItemActive, IsItemHovered, etc.) */
    IMGUI_API bool          InvisibleButton(const char* str_id, const ImVec2& size, ImGuiButtonFlags flags = 0);
	/**  square button with an arrow shape */
    IMGUI_API bool          ArrowButton(const char* str_id, ImGuiDir dir);

	/**  use with e.g. if (RadioButton("one", my_value==1)) { my_value = 1; } */
    IMGUI_API bool          RadioButton(const char* label, bool active);

    IMGUI_API void          ProgressBar(float fraction, const ImVec2& size_arg = ImVec2(-FLT_MIN,0), const char* overlay = NULL);
	/**  draw a small circle + keep the cursor on the same line. advance cursor x position by GetTreeNodeToLabelSpacing(), same distance that TreeNode() uses */
    IMGUI_API void          Bullet();

    // Widgets: Combo Box
    // - The new BeginCombo()/EndCombo() api allows you to manage your contents and selection state however you want it, by creating e.g. Selectable() items.
    // - The old Combo() api are helpers over BeginCombo()/EndCombo() which are kept available for convenience purpose. This is analogous to how ListBox are created.
	
    IMGUI_API bool          BeginCombo(const char* label, const char* preview_value, ImGuiComboFlags flags = 0);
	/**  only call EndCombo() if BeginCombo() returns true! */
    IMGUI_API void          EndCombo();

    // Widgets: Drag Sliders

    // Widgets: Regular Sliders

    // Widgets: Input with Keyboard

    // Widgets: Color Editor/Picker (tip: the ColorEdit* functions have a little colored preview square that can be left-clicked to open a picker, and right-clicked to open an option menu.)
    // - Note that in C++ a 'float v[X]' function argument is the _same_ as 'float* v', the array syntax is just a way to document the number of elements that are expected to be accessible.
    // - You can pass the address of a first float element out of a contiguous structure, e.g. &myvector.x

    //IMGUI_API bool          ColorEdit3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
    //IMGUI_API bool          ColorEdit4(const char* label, float col[4], ImGuiColorEditFlags flags = 0);
    //IMGUI_API bool          ColorPicker3(const char* label, float col[3], ImGuiColorEditFlags flags = 0);
    //IMGUI_API bool          ColorPicker4(const char* label, float col[4], ImGuiColorEditFlags flags = 0, const float* ref_col = NULL);
	/**  display a color square/button, hover for details, return true when pressed. */
    IMGUI_API bool          ColorButton(const char* desc_id, const ImVec4& col, ImGuiColorEditFlags flags = 0, ImVec2 size = ImVec2(0,0));
	/**  initialize current options (generally on application startup) if you want to select a default format, picker type, etc. User will be able to change many settings, unless you pass the _NoOptions flag to your calls. */
    IMGUI_API void          SetColorEditOptions(ImGuiColorEditFlags flags);

    // Widgets: Trees
    // - TreeNode functions return true when the node is open, in which case you need to also call TreePop() when you are finished displaying the tree node contents.
 
    IMGUI_API bool          TreeNode(const char* label);
	/**  helper variation to easily decorelate the id from the displayed string. Read the FAQ about why and how to use ID. to align arbitrary text at the same level as a TreeNode() you can use Bullet(). */
    IMGUI_API bool          TreeNode(const char* str_id, const char* fmt);
    IMGUI_API bool          TreeNodeEx(const char* label, ImGuiTreeNodeFlags flags = 0);
    IMGUI_API bool          TreeNodeEx(const char* str_id, ImGuiTreeNodeFlags flags, const char* fmt);
	/**  ~ Indent()+PushId(). Already called by TreeNode() when returning true, but you can call TreePush/TreePop yourself if desired. */
    IMGUI_API void          TreePush(const char* str_id);
    // IMGUI_API void          TreePush(const void* ptr_id = NULL);
	/**  ~ Unindent()+PopId() */
    IMGUI_API void          TreePop();
	/**  horizontal distance preceding label when using TreeNode*() or Bullet() == (g.FontSize + style.FramePadding.x*2) for a regular unframed TreeNode */
    IMGUI_API float         GetTreeNodeToLabelSpacing();
	/**  set next TreeNode/CollapsingHeader open state. */
    IMGUI_API void          SetNextItemOpen(bool is_open, ImGuiCond cond = 0);

    // Widgets: List Boxes
    // - This is essentially a thin wrapper to using BeginChild/EndChild with some stylistic changes.
    // - The BeginListBox()/EndListBox() api allows you to manage your contents and selection state however you want it, by creating e.g. Selectable() or any items.
    // - The simplified/old ListBox() api are helpers over BeginListBox()/EndListBox() which are kept available for convenience purpose. This is analoguous to how Combos are created.
    // - Choose frame width:   size.x > 0.0f: custom  /  size.x < 0.0f or -FLT_MIN: right-align   /  size.x = 0.0f (default): use current ItemWidth
    // - Choose frame height:  size.y > 0.0f: custom  /  size.y < 0.0f or -FLT_MIN: bottom-align  /  size.y = 0.0f (default): arbitrary default height which can fit ~7 items

    /** open a framed scrolling region */
    IMGUI_API bool          BeginListBox(const char* label, const ImVec2& size = ImVec2(0, 0));
    /** only call EndListBox() if BeginListBox() returned true! */
    IMGUI_API void          EndListBox();

    // Widgets: Data Plotting

    // Widgets: Value() Helpers.

    // Widgets: Menus
	// - Use BeginMenuBar() on a window ImGuiWindowFlags_MenuBar to append to its menu bar.
    // - Use BeginMainMenuBar() to create a menu bar at the top of the screen.
    // - Not that MenuItem() keyboardshortcuts are displayed as a convenience but _not processed_ by Dear ImGui at the moment.

	/**  append to menu-bar of current window (requires ImGuiWindowFlags_MenuBar flag set on parent window). */
    IMGUI_API bool          BeginMenuBar();
	/**  only call EndMenuBar() if BeginMenuBar() returns true! */
    IMGUI_API void          EndMenuBar();
	/**  create and append to a full screen menu-bar. */
    IMGUI_API bool          BeginMainMenuBar();
	/**  only call EndMainMenuBar() if BeginMainMenuBar() returns true! */
    IMGUI_API void          EndMainMenuBar();
	/**  create a sub-menu entry. only call EndMenu() if this returns true! */
    IMGUI_API bool          BeginMenu(const char* label, bool enabled = true);
	/**  only call EndMenu() if BeginMenu() returns true! */
    IMGUI_API void          EndMenu();

    // Tooltips
    // - Tooltip are windows following the mouse. They do not take focus away.

	/**  begin/append a tooltip window. to create full-featured tooltip (with any kind of items). */
    IMGUI_API void          BeginTooltip();
    IMGUI_API void          EndTooltip();
	/**  set a text-only tooltip, typically use with ImGui::IsItemHovered(). override any previous call to SetTooltip(). */
    IMGUI_API void          SetTooltip(const char* fmt);

    // Popups, Modals
    //  - They block normal mouse hovering detection (and therefore most mouse interactions) behind them.
    //  - If not modal: they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    //  - Their visibility state (~bool) is held internally instead of being held by the programmer as we are used to with regular Begin*() calls.
    //  - The 3 properties above are related: we need to retain popup visibility state in the library because popups may be closed as any time.
    //  - You can bypass the hovering restriction by using ImGuiHoveredFlags_AllowWhenBlockedByPopup when calling IsItemHovered() or IsWindowHovered().
    //  - IMPORTANT: Popup identifiers are relative to the current ID stack, so OpenPopup and BeginPopup generally needs to be at the same level of the stack.
    //    This is sometimes leading to confusing mistakes. May rework this in the future.
    // Popups: begin/end functions
    //  - BeginPopup(): query popup state, if open start appending into the window. Call EndPopup() afterwards. ImGuiWindowFlags are forwarded to the window.
    //  - BeginPopupModal(): block every interactions behind the window, cannot be closed by user, add a dimming background, has a title bar.

	/**  return true if the popup is open, and you can start outputting to it. */
    IMGUI_API bool          BeginPopup(const char* str_id, ImGuiWindowFlags flags = 0);
	/**  only call EndPopup() if BeginPopupXXX() returns true! */
    IMGUI_API void          EndPopup();

    // Popups: open/close functions
    //  - OpenPopup(): set popup state to open. ImGuiPopupFlags are available for opening options.
    //  - If not modal: they can be closed by clicking anywhere outside them, or by pressing ESCAPE.
    //  - CloseCurrentPopup(): use inside the BeginPopup()/EndPopup() scope to close manually.
    //  - CloseCurrentPopup() is called by default by Selectable()/MenuItem() when activated (FIXME: need some options).
    //  - Use ImGuiPopupFlags_NoOpenOverExistingPopup to avoid opening a popup if there's already one at the same level. This is equivalent to e.g. testing for !IsAnyPopupOpen() prior to OpenPopup().
    //  - Use IsWindowAppearing() after BeginPopup() to tell if a window just opened.
	
	/**  call to mark popup as open (don't call every frame!). */
    IMGUI_API void          OpenPopup(const char* str_id, ImGuiPopupFlags popup_flags = 0);
    //IMGUI_API void          OpenPopup(ImGuiID id, ImGuiPopupFlags popup_flags = 0);                             // id overload to facilitate calling from nested stacks
	/**  helper to open popup when clicked on last item. Default to ImGuiPopupFlags_MouseButtonRight == 1. (note: actually triggers on the mouse _released_ event to be consistent with popup behaviors) */
    IMGUI_API void          OpenPopupOnItemClick(const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1);
	/**  manually close the popup we have begin-ed into. */
    IMGUI_API void          CloseCurrentPopup();

    // Popups: open+begin combined functions helpers
    //  - Helpers to do OpenPopup+BeginPopup where the Open action is triggered by e.g. hovering an item and right-clicking.
    //  - They are convenient to easily create context menus, hence the name.
    //  - IMPORTANT: Notice that BeginPopupContextXXX takes ImGuiPopupFlags just like OpenPopup() and unlike BeginPopup(). For full consistency, we may add ImGuiWindowFlags to the BeginPopupContextXXX functions in the future.
    //  - IMPORTANT: we exceptionally default their flags to 1 (== ImGuiPopupFlags_MouseButtonRight) for backward compatibility with older API taking 'int mouse_button = 1' parameter, so if you add other flags remember to re-add the ImGuiPopupFlags_MouseButtonRight.

	/**  open+begin popup when clicked on last item. Use str_id==NULL to associate the popup to previous item. If you want to use that on a non-interactive item such as Text() you need to pass in an explicit ID here. read comments in .cpp! */
    IMGUI_API bool          BeginPopupContextItem(const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1);
	/**  open+begin popup when clicked on current window. */
    IMGUI_API bool          BeginPopupContextWindow(const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1);
	/**  open+begin popup when clicked in void (where there are no windows). */
    IMGUI_API bool          BeginPopupContextVoid(const char* str_id = NULL, ImGuiPopupFlags popup_flags = 1);

    // Popups: query functions
    //  - IsPopupOpen(): return true if the popup is open at the current BeginPopup() level of the popup stack.
    //  - IsPopupOpen() with ImGuiPopupFlags_AnyPopupId: return true if any popup is open at the current BeginPopup() level of the popup stack.
    //  - IsPopupOpen() with ImGuiPopupFlags_AnyPopupId + ImGuiPopupFlags_AnyPopupLevel: return true if any popup is open.

	/**  return true if the popup is open. */
    IMGUI_API bool          IsPopupOpen(const char* str_id, ImGuiPopupFlags flags = 0);

    // Tables
    // [BETA API] API may evolve slightly! If you use this, please update to the next version when it comes out!
    // - Full-featured replacement for old Columns API.
    // - See Demo->Tables for demo code.
    // - See top of imgui_tables.cpp for general commentary.
    // - See ImGuiTableFlags_ and ImGuiTableColumnFlags_ enums for a description of available flags.
    // The typical call flow is:
    // - 1. Call BeginTable().
    // - 2. Optionally call TableSetupColumn() to submit column name/flags/defaults.
    // - 3. Optionally call TableSetupScrollFreeze() to request scroll freezing of columns/rows.
    // - 4. Optionally call TableHeadersRow() to submit a header row. Names are pulled from TableSetupColumn() data.
    // - 5. Populate contents:
    //    - In most situations you can use TableNextRow() + TableSetColumnIndex(N) to start appending into a column.
    //    - If you are using tables as a sort of grid, where every columns is holding the same type of contents,
    //      you may prefer using TableNextColumn() instead of TableNextRow() + TableSetColumnIndex().
    //      TableNextColumn() will automatically wrap-around into the next row if needed.
    //    - IMPORTANT: Comparatively to the old Columns() API, we need to call TableNextColumn() for the first column!
    //    - Summary of possible call flow:
    //        --------------------------------------------------------------------------------------------------------
    //        TableNextRow() -> TableSetColumnIndex(0) -> Text("Hello 0") -> TableSetColumnIndex(1) -> Text("Hello 1")  // OK
    //        TableNextRow() -> TableNextColumn()      -> Text("Hello 0") -> TableNextColumn()      -> Text("Hello 1")  // OK
    //                          TableNextColumn()      -> Text("Hello 0") -> TableNextColumn()      -> Text("Hello 1")  // OK: TableNextColumn() automatically gets to next row!
    //        TableNextRow()                           -> Text("Hello 0")                                               // Not OK! Missing TableSetColumnIndex() or TableNextColumn()! Text will not appear!
    //        --------------------------------------------------------------------------------------------------------
    // - 5. Call EndTable()

    IMGUI_API bool          BeginTable(const char* str_id, int column, ImGuiTableFlags flags = 0, const ImVec2& outer_size = ImVec2(0.0f, 0.0f), float inner_width = 0.0f);
    /** only call EndTable() if BeginTable() returns true! */
    IMGUI_API void          EndTable();
    /** append into the first cell of a new row. */
    IMGUI_API void          TableNextRow(ImGuiTableRowFlags row_flags = 0, float min_row_height = 0.0f);
    /** append into the next column (or first column of next row if currently in last column). Return true when column is visible. */
    IMGUI_API bool          TableNextColumn();
    /** append into the specified column. Return true when column is visible. */
    IMGUI_API bool          TableSetColumnIndex(int column_n);

    // Tables: Headers & Columns declaration
    // - Use TableSetupColumn() to specify label, resizing policy, default width/weight, id, various other flags etc.
    // - Use TableHeadersRow() to create a header row and automatically submit a TableHeader() for each column.
    //   Headers are required to perform: reordering, sorting, and opening the context menu.
    //   The context menu can also be made available in columns body using ImGuiTableFlags_ContextMenuInBody.
    // - You may manually submit headers using TableNextRow() + TableHeader() calls, but this is only useful in
    //   some advanced use cases (e.g. adding custom widgets in header row).
    // - Use TableSetupScrollFreeze() to lock columns/rows so they stay visible when scrolled.

    IMGUI_API void          TableSetupColumn(const char* label, ImGuiTableColumnFlags flags = 0, float init_width_or_weight = 0.0f, ImGuiID user_id = 0);
    /** lock columns/rows so they stay visible when scrolled. */
    IMGUI_API void          TableSetupScrollFreeze(int cols, int rows);
    /** submit all headers cells based on data provided to TableSetupColumn() + submit context menu */
    IMGUI_API void          TableHeadersRow();
    /** submit one header cell manually (rarely used) */
    IMGUI_API void          TableHeader(const char* label);

    // Tables: Sorting
    // - Call TableGetSortSpecs() to retrieve latest sort specs for the table. NULL when not sorting.
    // - When 'SpecsDirty == true' you should sort your data. It will be true when sorting specs have changed
    //   since last call, or the first time. Make sure to set 'SpecsDirty = false' after sorting, else you may
    //   wastefully sort your data every frame!
    // - Lifetime: don't hold on this pointer over multiple frames or past any subsequent call to BeginTable().

    /** get latest sort specs for the table (NULL if not sorting). */
    // IMGUI_API ImGuiTableSortSpecs*  TableGetSortSpecs();

    // Tables: Miscellaneous functions
    // - Functions args 'int column_n' treat the default value of -1 as the same as passing the current column index.

    /** return number of columns (value passed to BeginTable) */
    IMGUI_API int                   TableGetColumnCount();
    /** return current column index. */
    IMGUI_API int                   TableGetColumnIndex();
    /** return current row index. */
    IMGUI_API int                   TableGetRowIndex();
    /** return "" if column didn't have a name declared by TableSetupColumn(). Pass -1 to use current column. */
    IMGUI_API const char*           TableGetColumnName(int column_n = -1);
    /** return column flags so you can query their Enabled/Visible/Sorted/Hovered status flags. Pass -1 to use current column. */
    IMGUI_API ImGuiTableColumnFlags TableGetColumnFlags(int column_n = -1);
    /** change enabled/disabled state of a column, set to false to hide the column. Note that end-user can use the context menu to change this themselves (right-click in headers, or right-click in columns body with ImGuiTableFlags_ContextMenuInBody) */
    IMGUI_API void                  TableSetColumnEnabled(int column_n, bool v);
    /** change the color of a cell, row, or column. See ImGuiTableBgTarget_ flags for details. */
    IMGUI_API void                  TableSetBgColor(ImGuiTableBgTarget target, ImU32 color, int column_n = -1);

    // Legacy Columns API (2020: prefer using Tables!)
    // - You can also use SameLine(pos_x) to mimic simplified columns.

    IMGUI_API void          Columns(int count = 1, const char* id = NULL, bool border = true);
	/**  next column, defaults to current row or next row if the current row is finished */
    IMGUI_API void          NextColumn();
	/**  get current column index */
    IMGUI_API int           GetColumnIndex();
	/**  get column width (in pixels). pass -1 to use current column */
    IMGUI_API float         GetColumnWidth(int column_index = -1);
	/**  set column width (in pixels). pass -1 to use current column */
    IMGUI_API void          SetColumnWidth(int column_index, float width);
	/**  get position of column line (in pixels, from the left side of the contents region). pass -1 to use current column, otherwise 0..GetColumnsCount() inclusive. column 0 is typically 0.0f */
    IMGUI_API float         GetColumnOffset(int column_index = -1);
	/**  set position of column line (in pixels, from the left side of the contents region). pass -1 to use current column */
    IMGUI_API void          SetColumnOffset(int column_index, float offset_x);
	/**  Tab Bars, Tabs */
    IMGUI_API int           GetColumnsCount();

    // Tab Bars, Tabs
    // Note: Tabs are automatically created by the docking system. Use this to create tab bars/tabs yourself without docking being involved.

	/**  create and append into a TabBar */
    IMGUI_API bool          BeginTabBar(const char* str_id, ImGuiTabBarFlags flags = 0);
	/**  only call EndTabBar() if BeginTabBar() returns true! */
    IMGUI_API void          EndTabBar();

	/**  only call EndTabItem() if BeginTabItem() returns true! */
    IMGUI_API void          EndTabItem();
    /**  create a Tab behaving like a button. return true when clicked. cannot be selected in the tab bar. */
    IMGUI_API bool          TabItemButton(const char* label, ImGuiTabItemFlags flags = 0);
	/**  notify TabBar or Docking system of a closed tab/window ahead (useful to reduce visual flicker on reorderable tab bars). For tab-bar: call after BeginTabBar() and before Tab submissions. Otherwise call with a window name. */
    IMGUI_API void          SetTabItemClosed(const char* tab_or_docked_window_label);

    // Docking
    // [BETA API] Enable with io.ConfigFlags |= ImGuiConfigFlags_DockingEnable.
    // Note: You can use most Docking facilities without calling any API. You DO NOT need to call DockSpace() to use Docking!
    // - Drag from window title bar or their tab to dock/undock. Hold SHIFT to disable docking.
    // - Drag from window menu button (upper-left button) to undock an entire node (all windows).
    // About DockSpace:
    // - Use DockSpace() to create an explicit dock node _within_ an existing window. See Docking demo for details.
    // - DockSpace() needs to be submitted _before_ any window they can host. If you use a dockspace, submit it early in your app.
    
	IMGUI_API ImGuiID       DockSpace(ImGuiID id, const ImVec2& size = ImVec2(0, 0), ImGuiDockNodeFlags flags = 0);
    IMGUI_API ImGuiID       DockSpaceOverViewport(const ImGuiViewport* viewport = NULL, ImGuiDockNodeFlags flags = 0);
	/** set next window dock id (FIXME-DOCK) */
    IMGUI_API void          SetNextWindowDockID(ImGuiID dock_id, ImGuiCond cond = 0);
	/** set next window class (rare/advanced uses: provide hints to the platform backend via altered viewport flags and parent/child info) */
    //IMGUI_API void          SetNextWindowClass(const ImGuiWindowClass* window_class);
    IMGUI_API ImGuiID       GetWindowDockID();
	/** is current window docked into another window?  */
    IMGUI_API bool          IsWindowDocked();

    // Logging/Capture
    // - All text output from the interface can be captured into tty/file/clipboard. By default, tree nodes are automatically opened during logging.
	/**  start logging to tty (stdout) */
    IMGUI_API void          LogToTTY(int auto_open_depth = -1);
	/**  start logging to file */
    IMGUI_API void          LogToFile(int auto_open_depth = -1, const char* filename = NULL);
	/**  start logging to OS clipboard */
    IMGUI_API void          LogToClipboard(int auto_open_depth = -1);
	/**  stop logging (close file, etc.) */
    IMGUI_API void          LogFinish();
	/**  helper to display buttons for logging to tty/file/clipboard */
    IMGUI_API void          LogButtons();
	/**  pass text data straight to log (without being displayed) */
    IMGUI_API void          LogText(const char* fmt);

    // Drag and Drop
    // - On source items, call BeginDragDropSource(), if it returns true also call SetDragDropPayload() + EndDragDropSource().
    // - On target candidates, call BeginDragDropTarget(), if it returns true also call AcceptDragDropPayload() + EndDragDropTarget().
    // - If you stop calling BeginDragDropSource() the payload is preserved however it won't have a preview tooltip (we currently display a fallback "..." tooltip, see #1725)
    // - An item can be both drag source and drop target.
	
	/**  call after submitting an item which may be dragged. when this return true, you can call SetDragDropPayload() + EndDragDropSource() */
    IMGUI_API bool          BeginDragDropSource(ImGuiDragDropFlags flags = 0);
    //IMGUI_API bool          SetDragDropPayload(const char* type, const void* data, size_t sz, ImGuiCond cond = 0);  // type is a user defined string of maximum 32 characters. Strings starting with '_' are reserved for dear imgui internal types. Data is copied and held by imgui.
	/**  only call EndDragDropSource() if BeginDragDropSource() returns true! */
    IMGUI_API void          EndDragDropSource();
	/**  call after submitting an item that may receive a payload. If this returns true, you can call AcceptDragDropPayload() + EndDragDropTarget() */
    IMGUI_API bool                  BeginDragDropTarget();
    //IMGUI_API const ImGuiPayload*   AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags = 0);          // accept contents of a given type. If ImGuiDragDropFlags_AcceptBeforeDelivery is set you can peek into the payload before the mouse button is released.
	/**  only call EndDragDropTarget() if BeginDragDropTarget() returns true! */
    IMGUI_API void                  EndDragDropTarget();
    //IMGUI_API const ImGuiPayload*   GetDragDropPayload();                                                           // peek directly into the current payload from anywhere. may return NULL. use ImGuiPayload::IsDataType() to test for the payload type.

    // Clipping
	// - Mouse hovering is affected by ImGui::PushClipRect() calls, unlike direct calls to ImDrawList::PushClipRect() which are render only.

    IMGUI_API void          PushClipRect(const ImVec2& clip_rect_min, const ImVec2& clip_rect_max, bool intersect_with_current_clip_rect);
    IMGUI_API void          PopClipRect();

    // Focus, Activation
    // - Prefer using "SetItemDefaultFocus()" over "if (IsWindowAppearing()) SetScrollHereY()" when applicable to signify "this is the default item"
	/**  make last item the default focused item of a window. */
    IMGUI_API void          SetItemDefaultFocus();
	/**  focus keyboard on the next widget. Use positive 'offset' to access sub components of a multiple component widget. Use -1 to access previous widget. */
    IMGUI_API void          SetKeyboardFocusHere(int offset = 0);

    // Item/Widgets Utilities and Query Functions
    // - Most of the functions are referring to the previous Item that has been submitted.
    // - See Demo Window under "Widgets->Querying Status" for an interactive visualization of most of those functions.

	/**  is the last item hovered? (and usable, aka not blocked by a popup, etc.). See ImGuiHoveredFlags for more options. */
    IMGUI_API bool          IsItemHovered(ImGuiHoveredFlags flags = 0);
	/**  is the last item active? (e.g. button being held, text field being edited. This will continuously return true while holding mouse button on an item. Items that don't interact will always return false) */
    IMGUI_API bool          IsItemActive();
	/**  is the last item focused for keyboard/gamepad navigation? */
    IMGUI_API bool          IsItemFocused();
	/**  is the last item hovered and mouse clicked on? (**)  == IsMouseClicked(mouse_button) && IsItemHovered()Important. (**) this it NOT equivalent to the behavior of e.g. Button(). Read comments in function definition. */
    IMGUI_API bool          IsItemClicked(ImGuiMouseButton mouse_button = 0);
	/**  is the last item visible? (items may be out of sight because of clipping/scrolling) */
    IMGUI_API bool          IsItemVisible();
	/**  did the last item modify its underlying value this frame? or was pressed? This is generally the same as the "bool" return value of many widgets. */
    IMGUI_API bool          IsItemEdited();
	/**  was the last item just made active (item was previously inactive). */
    IMGUI_API bool          IsItemActivated();
	/**  was the last item just made inactive (item was previously active). Useful for Undo/Redo patterns with widgets that requires continuous editing. */
    IMGUI_API bool          IsItemDeactivated();
	/**  was the last item just made inactive and made a value change when it was active? (e.g. Slider/Drag moved). Useful for Undo/Redo patterns with widgets that requires continuous editing. Note that you may get false positives (some widgets such as Combo()/ListBox()/Selectable() will return true even when clicking an already selected item). */
    IMGUI_API bool          IsItemDeactivatedAfterEdit();
    /**
    * was the last item open state toggled? set by TreeNode().
    */
    IMGUI_API bool          IsItemToggledOpen();
	/**  is any item hovered? */
    IMGUI_API bool          IsAnyItemHovered();
	/**  is any item active? */
    IMGUI_API bool          IsAnyItemActive();
	/**  is any item focused? */
    IMGUI_API bool          IsAnyItemFocused();
	/**  get upper-left bounding rectangle of the last item (screen space) */
    IMGUI_API ImVec2        GetItemRectMin();
	/**  get lower-right bounding rectangle of the last item (screen space) */
    IMGUI_API ImVec2        GetItemRectMax();
	/**  get size of last item */
    IMGUI_API ImVec2        GetItemRectSize();
	/**  allow last item to be overlapped by a subsequent item. sometimes useful with invisible buttons, selectables, etc. to catch unused area. */
    IMGUI_API void          SetItemAllowOverlap();

    // Viewports
    // - Currently represents the Platform Window created by the application which is hosting our Dear ImGui windows.
    // - In 'docking' branch with multi-viewport enabled, we extend this concept to have multiple active viewports.
    // - In the future we will extend this concept further to also represent Platform Monitor and support a "no main platform window" operation mode.

    /** return primary/default viewport. This can never be NULL. */
    IMGUI_API ImGuiViewport* GetMainViewport();

    // Miscellaneous Utilities
	/**  test if rectangle (of given size, starting from cursor position) is visible / not clipped. */
    IMGUI_API bool          IsRectVisible(const ImVec2& size);
	/**  test if rectangle (in screen space) is visible / not clipped. to perform coarse clipping on user's side. */
    IMGUI_API bool          IsRectVisible(const ImVec2& rect_min, const ImVec2& rect_max);
	/**  get global imgui time. incremented by io.DeltaTime every frame. */
    IMGUI_API double        GetTime();
	/**  get global imgui frame count. incremented by 1 every frame. */
    IMGUI_API int           GetFrameCount();
    /** get background draw list for the viewport associated to the current window. this draw list will be the first rendering one. Useful to quickly draw shapes/text behind dear imgui contents. */
    IMGUI_API ImDrawList*   GetBackgroundDrawList();
	/** get foreground draw list for the viewport associated to the current window. this draw list will be the last rendered one. Useful to quickly draw shapes/text over dear imgui contents. */
    IMGUI_API ImDrawList*   GetForegroundDrawList();
	/** get background draw list for the given viewport. this draw list will be the first rendering one. Useful to quickly draw shapes/text behind dear imgui contents. */
    IMGUI_API ImDrawList*   GetBackgroundDrawList(ImGuiViewport* viewport);
	/** get foreground draw list for the given viewport. this draw list will be the last rendered one. Useful to quickly draw shapes/text over dear imgui contents. */
    IMGUI_API ImDrawList*   GetForegroundDrawList(ImGuiViewport* viewport);

    //IMGUI_API ImDrawListSharedData* GetDrawListSharedData();

	/**  get a string corresponding to the enum value (for display, saving, etc.). */
    IMGUI_API const char*   GetStyleColorName(ImGuiCol idx);

    //IMGUI_API void          SetStateStorage(ImGuiStorage* storage);
    //IMGUI_API ImGuiStorage* GetStateStorage();

    IMGUI_API ImVec2        CalcTextSize(const char* text, const char* text_end = NULL, bool hide_text_after_double_hash = false, float wrap_width = -1.0f);

	/**  helper to create a child window / scrolling region that looks like a normal widget frame */
    IMGUI_API bool          BeginChildFrame(ImGuiID id, const ImVec2& size, ImGuiWindowFlags flags = 0);
	/**  always call EndChildFrame() regardless of BeginChildFrame() return values (which indicates a collapsed/clipped window) */
    IMGUI_API void          EndChildFrame();

    // Color Utilities
    IMGUI_API ImVec4        ColorConvertU32ToFloat4(ImU32 in);
    IMGUI_API ImU32         ColorConvertFloat4ToU32(const ImVec4& in);

    // Inputs Utilities: Keyboard
    // - For 'int user_key_index' you can use your own indices/enums according to how your backend/engine stored them in io.KeysDown[].
    // - We don't know the meaning of those value. You can use GetKeyIndex() to map a ImGuiKey_ value into the user index.
 
	/**  map ImGuiKey_* values into user's key index. == io.KeyMap[key] */
    IMGUI_API int           GetKeyIndex(ImGuiKey imgui_key);
	/**  is key being held. == io.KeysDown[user_key_index]. note that imgui doesn't know the semantic of each entry of io.KeysDown[]. Use your own indices/enums according to how your backend/engine stored them into io.KeysDown[]! */
    IMGUI_API bool          IsKeyDown(int user_key_index);
	/**  was key pressed (went from !Down to Down). if repeat=true, uses io.KeyRepeatDelay / KeyRepeatRate */
    IMGUI_API bool          IsKeyPressed(int user_key_index, bool repeat = true);
	/**  was key released (went from Down to !Down).. */
    IMGUI_API bool          IsKeyReleased(int user_key_index);
	/**  uses provided repeat rate/delay. return a count, most often 0 or 1 but might be >1 if RepeatRate is small enough that DeltaTime > RepeatRate */
    IMGUI_API int           GetKeyPressedAmount(int key_index, float repeat_delay, float rate);
	/**  attention: misleading name! manually override io.WantCaptureKeyboard flag next frame (said flag is entirely left for your application to handle). e.g. force capture keyboard when your widget is being hovered. This is equivalent to setting "io.WantCaptureKeyboard = want_capture_keyboard_value"; after the next NewFrame() call. */
    IMGUI_API void          CaptureKeyboardFromApp(bool want_capture_keyboard_value = true);

    // Inputs Utilities: Mouse
    // - To refer to a mouse button, you may use named enums in your code e.g. ImGuiMouseButton_Left, ImGuiMouseButton_Right.
    // - You can also use regular integer: it is forever guaranteed that 0=Left, 1=Right, 2=Middle.
    // - Dragging operations are only reported after mouse has moved a certain distance away from the initial clicking position (see 'lock_threshold' and 'io.MouseDraggingThreshold')

	/**  is mouse button held (0=left, 1=right, 2=middle) */
    IMGUI_API bool          IsMouseDown(ImGuiMouseButton button);
	/**  did mouse button clicked (went from !Down to Down) (0=left, 1=right, 2=middle) */
    IMGUI_API bool          IsMouseClicked(ImGuiMouseButton button, bool repeat = false);
	/**  did mouse button released (went from Down to !Down) */
    IMGUI_API bool          IsMouseReleased(ImGuiMouseButton button);
	/**  did mouse button double-clicked? (note that a double-click will also report IsMouseClicked() == true) */
    IMGUI_API bool          IsMouseDoubleClicked(ImGuiMouseButton button);
	/**  is mouse hovering given bounding rect (in screen space). clipped by current clipping settings, but disregarding of other consideration of focus/window ordering/popup-block. */
    IMGUI_API bool          IsMouseHoveringRect(const ImVec2& r_min, const ImVec2& r_max, bool clip = true);
    /**  by convention we use (-FLT_MAX,-FLT_MAX) to denote that there is no mouse available */
    //IMGUI_API bool          IsMousePosValid(const ImVec2* mouse_pos = NULL);
	/**  is any mouse button held */
    IMGUI_API bool          IsAnyMouseDown();
	/**  shortcut to ImGui::GetIO().MousePos provided by user, to be consistent with other calls */
    IMGUI_API ImVec2        GetMousePos();
	/**  retrieve backup of mouse position at the time of opening popup we have BeginPopup() into */
    IMGUI_API ImVec2        GetMousePosOnOpeningCurrentPopup();
	/** is mouse dragging? (if lock_threshold < -1.0f, uses io.MouseDraggingThreshold) */
    IMGUI_API bool          IsMouseDragging(ImGuiMouseButton button, float lock_threshold = -1.0f);
	/**  return the delta from the initial clicking position while the mouse button is pressed or was just released. This is locked and return 0.0f until the mouse moves past a distance threshold at least once. If lock_threshold < -1.0f uses io.MouseDraggingThreshold. */
    IMGUI_API ImVec2        GetMouseDragDelta(ImGuiMouseButton button = 0, float lock_threshold = -1.0f);
	/**  */
    IMGUI_API void          ResetMouseDragDelta(ImGuiMouseButton button = 0);
	/**  get desired cursor type, reset in ImGui::NewFrame(), this is updated during the frame. valid before Render(). If you use software rendering by setting io.MouseDrawCursor ImGui will render those for you */
    IMGUI_API ImGuiMouseCursor GetMouseCursor();
	/**  set desired cursor type */
    IMGUI_API void          SetMouseCursor(ImGuiMouseCursor cursor_type);
	/**  attention: misleading name! manually override io.WantCaptureMouse flag next frame (said flag is entirely left for your application to handle). This is equivalent to setting "io.WantCaptureMouse = want_capture_mouse_value;" after the next NewFrame() call. */
    IMGUI_API void          CaptureMouseFromApp(bool want_capture_mouse_value = true);

    // Clipboard Utilities
    // - Also see the LogToClipboard() function to capture GUI into clipboard, or easily output text data to the clipboard.

    IMGUI_API const char*   GetClipboardText();
    IMGUI_API void          SetClipboardText(const char* text);

	//  Settings/.Ini Utilities
    // - The disk functions are automatically called if io.IniFilename != NULL (default is "imgui.ini").
    // - Set io.IniFilename to NULL to load/save manually. Read io.WantSaveIniSettings description about handling .ini saving manually.

	/**  call after CreateContext() and before the first call to NewFrame(). NewFrame() automatically calls LoadIniSettingsFromDisk(io.IniFilename). */
    IMGUI_API void          LoadIniSettingsFromDisk(const char* ini_filename);
	/**  call after CreateContext() and before the first call to NewFrame() to provide .ini data from your own data source. */
    IMGUI_API void          LoadIniSettingsFromMemory(const char* ini_data, size_t ini_size=0);
	/**  this is automatically called (if io.IniFilename is not empty) a few seconds after any modification that should be reflected in the .ini file (and also by DestroyContext). */
    IMGUI_API void          SaveIniSettingsToDisk(const char* ini_filename);
    //IMGUI_API const char*   SaveIniSettingsToMemory(size_t* out_ini_size = NULL);               // return a zero-terminated string with the .ini data which you can save by your own mean. call when io.WantSaveIniSettings is set, then save data by your own mean and clear io.WantSaveIniSettings.

    // Debug Utilities
    // - This is used by the IMGUI_CHECKVERSION() macro.

    IMGUI_API bool          DebugCheckVersionAndDataLayout(const char* version_str, size_t sz_io, size_t sz_style, size_t sz_vec2, size_t sz_vec4, size_t sz_drawvert, size_t sz_drawidx); // This is called by IMGUI_CHECKVERSION() macro.

    // Memory Allocators
    // - Those functions are not reliant on the current context.
    // - DLL users: heaps and globals are not shared across DLL boundaries! You will need to call SetCurrentContext() + SetAllocatorFunctions()
    //   for each static/DLL boundary you are calling from. Read "Context and Memory Allocators" section of imgui.cpp for more details.

    /*
    IMGUI_API void          SetAllocatorFunctions(ImGuiMemAllocFunc alloc_func, ImGuiMemFreeFunc free_func, void* user_data = NULL);
    IMGUI_API void          GetAllocatorFunctions(ImGuiMemAllocFunc* p_alloc_func, ImGuiMemFreeFunc* p_free_func, void** p_user_data);
    IMGUI_API void*         MemAlloc(size_t size);
    IMGUI_API void          MemFree(void* ptr);
    */

    // (Optional) Platform/OS interface for multi-viewport support
    // Note: You may use GetWindowViewport() to get the current viewport of the current window.
    
	/** platform/renderer functions, for backend to setup + viewports list. */
	//IMGUI_API ImGuiPlatformIO&  GetPlatformIO();
	/** call in main loop. will call CreateWindow/ResizeWindow/etc. platform functions for each secondary viewport, and DestroyWindow for each inactive viewport. */
    IMGUI_API void              UpdatePlatformWindows();
	/** call in main loop. will call RenderWindow/SwapBuffers platform functions for each secondary viewport which doesn't have the ImGuiViewportFlags_Minimized flag set. May be reimplemented by user for custom rendering needs. */
    IMGUI_API void              RenderPlatformWindowsDefault();
	/** call DestroyWindow platform functions for all viewports. call from backend Shutdown() if you need to close platform windows before imgui shutdown. otherwise will be called by DestroyContext(). */
    IMGUI_API void              DestroyPlatformWindows();
	/** this is a helper for backends. */
    IMGUI_API ImGuiViewport*    FindViewportByID(ImGuiID id);
	/** this is a helper for backends. the type platform_handle is decided by the backend (e.g. HWND, MyWindow*, GLFWwindow* etc.) */
    //IMGUI_API ImGuiViewport*    FindViewportByPlatformHandle(void* platform_handle);

};

}

#define IMGUI_API
