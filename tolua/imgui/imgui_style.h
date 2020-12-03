#include "imgui.h"

namespace imgui
{

class ImGuiStyle
{
public:
    /** Global alpha applies to everything in ImGui. */
    float       Alpha;
    /** Padding within a window. */
    ImVec2      WindowPadding;
    /** Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended. */
    float       WindowRounding;
    /** Thickness of border around windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly). */
    float       WindowBorderSize;
    /** Minimum window size. This is a global setting. If you want to constraint individual windows, use SetNextWindowSizeConstraints(). */
    ImVec2      WindowMinSize;
    /** Alignment for title bar text. Defaults to (0.0f,0.5f) for left-aligned,vertically centered. */
    ImVec2      WindowTitleAlign;
    /** Side of the collapsing/docking button in the title bar (left/right). Defaults to ImGuiDir_Left. */
    ImGuiDir    WindowMenuButtonPosition;
    /** Radius of child window corners rounding. Set to 0.0f to have rectangular windows. */
    float       ChildRounding;
    /** Thickness of border around child windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly). */
    float       ChildBorderSize;
    /** Radius of popup window corners rounding. (Note that tooltip windows use WindowRounding) */
    float       PopupRounding;
    /** Thickness of border around popup/tooltip windows. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly). */
    float       PopupBorderSize;
    /** Padding within a framed rectangle (used by most widgets). */
    ImVec2      FramePadding;
    /** Radius of frame corners rounding. Set to 0.0f to have rectangular frame (used by most widgets). */
    float       FrameRounding;
    /** Thickness of border around frames. Generally set to 0.0f or 1.0f. (Other values are not well tested and more CPU/GPU costly). */
    float       FrameBorderSize;
    /** Horizontal and vertical spacing between widgets/lines. */
    ImVec2      ItemSpacing;
    /** Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label). */
    ImVec2      ItemInnerSpacing;
    /** Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much! */
    ImVec2      TouchExtraPadding;
    /** Horizontal indentation when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2). */
    float       IndentSpacing;
    /** Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1). */
    float       ColumnsMinSpacing;
    /** Width of the vertical scrollbar, Height of the horizontal scrollbar. */
    float       ScrollbarSize;
    /** Radius of grab corners for scrollbar. */
    float       ScrollbarRounding;
    /** Minimum width/height of a grab box for slider/scrollbar. */
    float       GrabMinSize;
    /** Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs. */
    float       GrabRounding;
    /** The size in pixels of the dead-zone around zero on logarithmic sliders that cross zero. */
    float       LogSliderDeadzone;
    /** Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs. */
    float       TabRounding;
    /** Thickness of border around tabs. */
    float       TabBorderSize;
    /** Minimum width for close button to appears on an unselected tab when hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to never show close button unless selected. */
    float       TabMinWidthForCloseButton;
	/** Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right. */
    ImGuiDir ColorButtonPosition;
    /** Alignment of button text when button is larger than text. Defaults to (0.5f, 0.5f) (centered). */
    ImVec2      ButtonTextAlign;
    /** Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line. */
    ImVec2      SelectableTextAlign;
    /** Window position are clamped to be visible within the display area by at least this amount. Only applies to regular windows. */
    ImVec2      DisplayWindowPadding;
    /** If you cannot see the edges of your screen (e.g. on a TV) increase the safe area padding. Apply to popups/tooltips as well regular windows. NB: Prefer configuring your TV sets correctly! */
    ImVec2      DisplaySafeAreaPadding;
    /** Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). We apply per-monitor DPI scaling over this scale. May be removed later. */
    float       MouseCursorScale;
    /** Enable anti-aliased lines/borders. Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList). */
    bool        AntiAliasedLines;
    /** Enable anti-aliased lines/borders using textures where possible. Require backend to render with bilinear filtering. Latched at the beginning of the frame (copied to ImDrawList). */
    bool        AntiAliasedLinesUseTex;
    /** Enable anti-aliased edges around filled shapes (rounded rectangles, circles, etc.). Disable if you are really tight on CPU/GPU. Latched at the beginning of the frame (copied to ImDrawList). */
    bool        AntiAliasedFill;
    /** Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality. */
    float       CurveTessellationTol;
    /**
    * Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.
    */
    float CircleSegmentMaxError;

    //ImVec4      Colors[ImGuiCol_COUNT];
	
    void ScaleAllSizes(float scale_factor);
};

}
