#include "imgui.h"

#define IMGUI_API

namespace imgui
{

//-----------------------------------------------------------------------------
// Draw List API (ImDrawCmd, ImDrawIdx, ImDrawVert, ImDrawChannel, ImDrawListSplitter, ImDrawListFlags, ImDrawList, ImDrawData)
// Hold a series of drawing commands. The user provides a renderer for ImDrawData which essentially contains an array of ImDrawList.
//-----------------------------------------------------------------------------

typedef void (*ImDrawCallback)(const ImDrawList* parent_list, const ImDrawCmd* cmd);
#define ImDrawCallback_ResetRenderState     (ImDrawCallback)(-1)
typedef unsigned short ImDrawIdx;

enum ImDrawCornerFlags_
{
    ImDrawCornerFlags_TopLeft   = 1 << 0, // 0x1
    ImDrawCornerFlags_TopRight  = 1 << 1, // 0x2
    ImDrawCornerFlags_BotLeft   = 1 << 2, // 0x4
    ImDrawCornerFlags_BotRight  = 1 << 3, // 0x8
    ImDrawCornerFlags_Top       = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_TopRight,   // 0x3
    ImDrawCornerFlags_Bot       = ImDrawCornerFlags_BotLeft | ImDrawCornerFlags_BotRight,   // 0xC
    ImDrawCornerFlags_Left      = ImDrawCornerFlags_TopLeft | ImDrawCornerFlags_BotLeft,    // 0x5
    ImDrawCornerFlags_Right     = ImDrawCornerFlags_TopRight | ImDrawCornerFlags_BotRight,  // 0xA
    ImDrawCornerFlags_All       = 0xF     // In your function calls you may use ~0 (= all bits sets) instead of ImDrawCornerFlags_All, as a convenience
};

// Flags for ImDrawList. Those are set automatically by ImGui:: functions from ImGuiIO settings, and generally not manipulated directly.
// It is however possible to temporarily alter flags between calls to ImDrawList:: functions.
enum ImDrawListFlags_
{
    ImDrawListFlags_None                    = 0,
    ImDrawListFlags_AntiAliasedLines        = 1 << 0,  // Enable anti-aliased lines/borders (*2 the number of triangles for 1.0f wide line or lines thin enough to be drawn using textures, otherwise *3 the number of triangles)
    ImDrawListFlags_AntiAliasedLinesUseTex  = 1 << 1,  // Enable anti-aliased lines/borders using textures when possible. Require backend to render with bilinear filtering.
    ImDrawListFlags_AntiAliasedFill         = 1 << 2,  // Enable anti-aliased edge around filled shapes (rounded rectangles, circles).
    ImDrawListFlags_AllowVtxOffset          = 1 << 3   // Can emit 'VtxOffset > 0' to allow large meshes. Set when 'ImGuiBackendFlags_RendererHasVtxOffset' is enabled.
};

/** Draw command list
* This is the low-level list of polygons that ImGui:: functions are filling. At the end of the frame,
* all command lists are passed to your ImGuiIO::RenderDrawListFn function for rendering.
* Each dear imgui window contains its own ImDrawList. You can use ImGui::GetWindowDrawList() to 
* access the current window draw list and draw custom primitives.
* You can interleave normal ImGui:: calls and adding primitives to the current draw list.
* All positions are generally in pixel coordinates (generally top-left at 0,0, bottom-right at io.DisplaySize, unless multiple viewports are used), but you are totally free to apply whatever transformation matrix to want to the data (if you apply such transformation you'll want to apply it to ClipRect as well)
* Important: Primitives are always added to the list and not culled (culling is done at higher-level by ImGui:: functions), if you use this API a lot consider coarse culling your drawn objects.*/
class ImDrawList
{
    /*
    // This is what you have to render
    ImVector<ImDrawCmd>     CmdBuffer;          // Draw commands. Typically 1 command = 1 GPU draw call, unless the command is a callback.
    ImVector<ImDrawIdx>     IdxBuffer;          // Index buffer. Each command consume ImDrawCmd::ElemCount of those
    ImVector<ImDrawVert>    VtxBuffer;          // Vertex buffer.
    ImDrawListFlags         Flags;              // Flags, you may poke into these to adjust anti-aliasing settings per-primitive.

    // [Internal, used while building lists]
    unsigned int            _VtxCurrentIdx;     // [Internal] generally == VtxBuffer.Size unless we are past 64K vertices, in which case this gets reset to 0.
    const ImDrawListSharedData* _Data;          // Pointer to shared draw data (you can use ImGui::GetDrawListSharedData() to get the one from current ImGui context)
    const char*             _OwnerName;         // Pointer to owner window's name for debugging
    ImDrawVert*             _VtxWritePtr;       // [Internal] point within VtxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImDrawIdx*              _IdxWritePtr;       // [Internal] point within IdxBuffer.Data after each add command (to avoid using the ImVector<> operators too much)
    ImVector<ImVec4>        _ClipRectStack;     // [Internal]
    ImVector<ImTextureID>   _TextureIdStack;    // [Internal]
    ImVector<ImVec2>        _Path;              // [Internal] current path building
    ImDrawCmdHeader         _CmdHeader;         // [Internal] template of active commands. Fields should match those of CmdBuffer.back().
    ImDrawListSplitter      _Splitter;          // [Internal] for channels api (note: prefer using your own persistent instance of ImDrawListSplitter!)

    // If you want to create ImDrawList instances, pass them ImGui::GetDrawListSharedData() or create and use your own ImDrawListSharedData (so you can use ImDrawList without ImGui)
    ImDrawList(const ImDrawListSharedData* shared_data) { memset(this, 0, sizeof(*this)); _Data = shared_data; }

    ~ImDrawList() { _ClearFreeMemory(); }
    */
public:
	/** Flags, you may poke into these to adjust anti-aliasing settings per-primitive. */
    ImDrawListFlags         Flags;
	/** Pointer to owner window's name for debugging */
    const char*             _OwnerName;

	/** Render-level scissoring. This is passed down to your render function but not used for CPU-side coarse clipping. Prefer using higher-level ImGui::PushClipRect() to affect logic (hit-testing and widget culling) */
    IMGUI_API void  PushClipRect(ImVec2 clip_rect_min, ImVec2 clip_rect_max, bool intersect_with_current_clip_rect = false);
    IMGUI_API void  PushClipRectFullScreen();
    IMGUI_API void  PopClipRect();
	// need manual
    // IMGUI_API void  PushTextureID(ImTextureID texture_id);

    IMGUI_API void  PopTextureID();
    inline ImVec2   GetClipRectMin() const;
    inline ImVec2   GetClipRectMax() const;

    // Primitives
    // - For rectangular primitives, "p_min" and "p_max" represent the upper-left and lower-right corners.
    // - For circle primitives, use "num_segments == 0" to automatically calculate tessellation (preferred).
    //   In older versions (until Dear ImGui 1.77) the AddCircle functions defaulted to num_segments == 12.
    //   In future versions we will use textures to provide cheaper and higher-quality circles.
    //   Use AddNgon() and AddNgonFilled() functions if you need to guaranteed a specific number of sides.

    IMGUI_API void  AddLine(const ImVec2& a, const ImVec2& b, ImU32 col, float thickness = 1.0f);
	/** a: upper-left, b: lower-right (== upper-left + size), rounding_corners_flags: 4-bits corresponding to which corner to round */
    IMGUI_API void  AddRect(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All, float thickness = 1.0f);
	/** a: upper-left, b: lower-right (== upper-left + size) */
    IMGUI_API void  AddRectFilled(const ImVec2& a, const ImVec2& b, ImU32 col, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);
    IMGUI_API void  AddRectFilledMultiColor(const ImVec2& a, const ImVec2& b, ImU32 col_upr_left, ImU32 col_upr_right, ImU32 col_bot_right, ImU32 col_bot_left);
    IMGUI_API void  AddQuad(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col, float thickness = 1.0f);
    IMGUI_API void  AddQuadFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, ImU32 col);
    IMGUI_API void  AddTriangle(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col, float thickness = 1.0f);
    IMGUI_API void  AddTriangleFilled(const ImVec2& a, const ImVec2& b, const ImVec2& c, ImU32 col);
    IMGUI_API void  AddCircle(const ImVec2& centre, float radius, ImU32 col, int num_segments = 0, float thickness = 1.0f);
    IMGUI_API void  AddCircleFilled(const ImVec2& centre, float radius, ImU32 col, int num_segments = 0);
    IMGUI_API void  AddNgon(const ImVec2 &center, float radius, ImU32 col, int num_segments, float thickness = 1.0f);
    IMGUI_API void  AddNgonFilled(const ImVec2 &center, float radius, ImU32 col, int num_segments);
	
	// need manual
    // IMGUI_API void  AddText(const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL);
    // IMGUI_API void  AddText(const ImFont* font, float font_size, const ImVec2& pos, ImU32 col, const char* text_begin, const char* text_end = NULL, float wrap_width = 0.0f, const ImVec4* cpu_fine_clip_rect = NULL);
    // IMGUI_API void  AddImage(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a = ImVec2(0,0), const ImVec2& uv_b = ImVec2(1,1), ImU32 col = IM_COL32_WHITE);
    // IMGUI_API void  AddImageQuad(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a = ImVec2(0,0), const ImVec2& uv_b = ImVec2(1,0), const ImVec2& uv_c = ImVec2(1,1), const ImVec2& uv_d = ImVec2(0,1), ImU32 col = IM_COL32_WHITE);
    // IMGUI_API void  AddImageRounded(ImTextureID user_texture_id, const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col, float rounding, int rounding_corners = ImDrawCornerFlags_All);
    // IMGUI_API void  AddPolyline(const ImVec2* points, int num_points, ImU32 col, bool closed, float thickness);
	/** Note: Anti-aliased filling requires points to be in clockwise order. */
    // IMGUI_API void  AddConvexPolyFilled(const ImVec2* points, int num_points, ImU32 col);
	
    IMGUI_API void  AddBezierCurve(const ImVec2& pos0, const ImVec2& cp0, const ImVec2& cp1, const ImVec2& pos1, ImU32 col, float thickness, int num_segments = 0);

    // Stateful path API, add points then finish with PathFillConvex() or PathStroke()
	
    inline    void  PathClear();
    inline    void  PathLineTo(const ImVec2& pos);
    inline    void  PathLineToMergeDuplicate(const ImVec2& pos);
	/** Note: Anti-aliased filling requires points to be in clockwise order. */
    inline    void  PathFillConvex(ImU32 col);
    inline    void  PathStroke(ImU32 col, bool closed, float thickness = 1.0f);
    IMGUI_API void  PathArcTo(const ImVec2& centre, float radius, float a_min, float a_max, int num_segments = 10);
	/** Use precomputed angles for a 12 steps circle */
    IMGUI_API void  PathArcToFast(const ImVec2& centre, float radius, int a_min_of_12, int a_max_of_12);
    IMGUI_API void  PathBezierCurveTo(const ImVec2& p1, const ImVec2& p2, const ImVec2& p3, int num_segments = 0);
    IMGUI_API void  PathRect(const ImVec2& rect_min, const ImVec2& rect_max, float rounding = 0.0f, int rounding_corners_flags = ImDrawCornerFlags_All);

    // Advanced
	/** Your rendering function must check for 'UserCallback' in ImDrawCmd and call the function instead of rendering triangles. */
    // IMGUI_API void  AddCallback(ImDrawCallback callback, void* callback_data);
	
	/** This is useful if you need to forcefully create a new draw call (to allow for dependent rendering / blending). Otherwise primitives are merged into the same draw-call as much as possible */
    IMGUI_API void  AddDrawCmd();
	/** Create a clone of the CmdBuffer/IdxBuffer/VtxBuffer. */
    IMGUI_API ImDrawList* CloneOutput() const;

    // Advanced: Channels
    // - Use to split render into layers. By switching channels to can render out-of-order (e.g. submit foreground primitives before background primitives)
    // - Use to minimize draw calls (e.g. if going back-and-forth between multiple non-overlapping clipping rectangles, prefer to append into separate channels then merge at the end)

    void     ChannelsSplit(int count);
    void     ChannelsMerge();
    void     ChannelsSetCurrent(int n);

    // Advanced: Primitives allocations
    // - We render triangles (three vertices)
    // - All primitives needs to be reserved via PrimReserve() beforehand.

    IMGUI_API void  PrimReserve(int idx_count, int vtx_count);
    IMGUI_API void  PrimUnreserve(int idx_count, int vtx_count);

    // Axis aligned rectangle (composed of two triangles)

    IMGUI_API void  PrimRect(const ImVec2& a, const ImVec2& b, ImU32 col);
    IMGUI_API void  PrimRectUV(const ImVec2& a, const ImVec2& b, const ImVec2& uv_a, const ImVec2& uv_b, ImU32 col);
    IMGUI_API void  PrimQuadUV(const ImVec2& a, const ImVec2& b, const ImVec2& c, const ImVec2& d, const ImVec2& uv_a, const ImVec2& uv_b, const ImVec2& uv_c, const ImVec2& uv_d, ImU32 col);
    void  PrimWriteVtx(const ImVec2& pos, const ImVec2& uv, ImU32 col);
    void  PrimWriteIdx(ImDrawIdx idx);
    
    // Write vertex with unique index

    void  PrimVtx(const ImVec2& pos, const ImVec2& uv, ImU32 col);

};


}
