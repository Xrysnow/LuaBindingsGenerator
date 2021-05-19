// ImPlot v0.10 WIP

#pragma once
#include "imgui.h"

enum ImGuiCond_
{
    ImGuiCond_None          = 0,        /// No condition (always set the variable), same as _Always
    ImGuiCond_Always        = 1 << 0,   /// No condition (always set the variable)
    ImGuiCond_Once          = 1 << 1,   /// Set the variable once per runtime session (only the first call will succeed)
    ImGuiCond_FirstUseEver  = 1 << 2,   /// Set the variable if the object/window has no persistently saved data (no entry in .ini file)
    ImGuiCond_Appearing     = 1 << 3    // Set the variable if the object/window is appearing after being hidden/inactive (or the first time)
};
enum ImGuiKeyModFlags_
{
    ImGuiKeyModFlags_None       = 0,
    ImGuiKeyModFlags_Ctrl       = 1 << 0,
    ImGuiKeyModFlags_Shift      = 1 << 1,
    ImGuiKeyModFlags_Alt        = 1 << 2,
    ImGuiKeyModFlags_Super      = 1 << 3
};

//-----------------------------------------------------------------------------
// Macros and Defines
//-----------------------------------------------------------------------------

// Define attributes of all API symbols declarations (e.g. for DLL under Windows)
// Using ImPlot via a shared library is not recommended, because we don't guarantee
// backward nor forward ABI compatibility and also function call overhead. If you
// do use ImPlot as a DLL, be sure to call SetImGuiContext (details below).
#ifndef IMPLOT_API
#define IMPLOT_API static
#endif

// ImPlot version string
#define IMPLOT_VERSION "0.10 WIP"
// Indicates variable should deduced automatically.
#define IMPLOT_AUTO -1
// Special color used to indicate that a color should be deduced automatically.
#define IMPLOT_AUTO_COL ImVec4(0,0,0,-1)

//-----------------------------------------------------------------------------
// Forward Declarations and Basic Types
//-----------------------------------------------------------------------------

// Forward declarations
struct ImPlotContext;          // ImPlot context (opaque struct, see implot_internal.h)

// Enums/Flags
typedef int ImPlotFlags;       // -> enum ImPlotFlags_
typedef int ImPlotAxisFlags;   // -> enum ImPlotAxisFlags_
typedef int ImPlotCol;         // -> enum ImPlotCol_
typedef int ImPlotStyleVar;    // -> enum ImPlotStyleVar_
typedef int ImPlotMarker;      // -> enum ImPlotMarker_
typedef int ImPlotColormap;    // -> enum ImPlotColormap_
typedef int ImPlotLocation;    // -> enum ImPlotLocation_
typedef int ImPlotOrientation; // -> enum ImPlotOrientation_
typedef int ImPlotYAxis;       // -> enum ImPlotYAxis_;
typedef int ImPlotBin;         // -> enum ImPlotBin_

/** Double precision version of ImVec2 used by ImPlot. Extensible by end users. */
class ImPlotPoint {};

/** A range defined by a min/max value. Used for plot axes ranges. */
class ImPlotRange {};

/** Combination of two ranges for X and Y axes. */
//@lua manual
class ImPlotLimits {};

namespace implot {

/** Options for plots. */
enum ImPlotFlags_ {
    /** default */
    ImPlotFlags_None          = 0,
    /** the plot title will not be displayed (titles are also hidden if preceeded by double hashes, e.g. "##MyPlot") */
    ImPlotFlags_NoTitle       = 1 << 0,
    /** the legend will not be displayed */
    ImPlotFlags_NoLegend      = 1 << 1,
    /** the user will not be able to open context menus with right-click */
    ImPlotFlags_NoMenus       = 1 << 2,
    /** the user will not be able to box-select with right-click drag */
    ImPlotFlags_NoBoxSelect   = 1 << 3,
    /** the mouse position, in plot coordinates, will not be displayed inside of the plot */
    ImPlotFlags_NoMousePos    = 1 << 4,
    /** plot items will not be highlighted when their legend entry is hovered */
    ImPlotFlags_NoHighlight   = 1 << 5,
    /** a child window region will not be used to capture mouse scroll (can boost performance for single ImGui window applications) */
    ImPlotFlags_NoChild       = 1 << 6,
    /** primary x and y axes will be constrained to have the same units/pixel (does not apply to auxiliary y-axes) */
    ImPlotFlags_Equal         = 1 << 7,
    /** enable a 2nd y-axis on the right side */
    ImPlotFlags_YAxis2        = 1 << 8,
    /** enable a 3rd y-axis on the right side */
    ImPlotFlags_YAxis3        = 1 << 9,
    /** the user will be able to draw query rects with middle-mouse or CTRL + right-click drag */
    ImPlotFlags_Query         = 1 << 10,
    /** the default mouse cursor will be replaced with a crosshair when hovered */
    ImPlotFlags_Crosshairs    = 1 << 11,
    /** plot lines will be software anti-aliased (not recommended for high density plots, prefer MSAA) */
    ImPlotFlags_AntiAliased   = 1 << 12,

    ImPlotFlags_CanvasOnly    = ImPlotFlags_NoTitle | ImPlotFlags_NoLegend | ImPlotFlags_NoMenus | ImPlotFlags_NoBoxSelect | ImPlotFlags_NoMousePos
};

/** Options for plot axes (X and Y). */
enum ImPlotAxisFlags_ {
    /** default */
    ImPlotAxisFlags_None          = 0,
    /** the axis label will not be displayed (axis labels also hidden if the supplied string name is NULL) */
    ImPlotAxisFlags_NoLabel       = 1 << 0,
    /** no grid lines will be displayed */
    ImPlotAxisFlags_NoGridLines   = 1 << 1,
    /** no tick marks will be displayed */
    ImPlotAxisFlags_NoTickMarks   = 1 << 2,
    /** no text labels will be displayed */
    ImPlotAxisFlags_NoTickLabels  = 1 << 3,
    /** grid lines will be displayed in the foreground (i.e. on top of data) in stead of the background */
    ImPlotAxisFlags_Foreground    = 1 << 4,
    /** a logartithmic (base 10) axis scale will be used (mutually exclusive with ImPlotAxisFlags_Time) */
    ImPlotAxisFlags_LogScale      = 1 << 5,
    /** axis will display date/time formatted labels (mutually exclusive with ImPlotAxisFlags_LogScale) */
    ImPlotAxisFlags_Time          = 1 << 6,
    /** the axis will be inverted */
    ImPlotAxisFlags_Invert        = 1 << 7,
    /** axis will not be initially fit to data extents on the first rendered frame (also the case if SetNextPlotLimits explicitly called) */
    ImPlotAxisFlags_NoInitialFit  = 1 << 8,
    /** axis will be auto-fitting to data extents */
    ImPlotAxisFlags_AutoFit       = 1 << 9,
    /** axis will only fit points if the point is in the visible range of the **orthoganol** axis */
    ImPlotAxisFlags_RangeFit      = 1 << 10,
    /** the axis minimum value will be locked when panning/zooming */
    ImPlotAxisFlags_LockMin       = 1 << 11,
    /** the axis maximum value will be locked when panning/zooming */
    ImPlotAxisFlags_LockMax       = 1 << 12,
    /**  */
    ImPlotAxisFlags_Lock          = ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax,
    /**  */
    ImPlotAxisFlags_NoDecorations = ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels

};

/** Plot styling colors. */
enum ImPlotCol_ {
    // item styling colors

    /** plot line/outline color (defaults to next unused color in current colormap) */
    ImPlotCol_Line,
    /** plot fill color for bars (defaults to the current line color) */
    ImPlotCol_Fill,
    /** marker outline color (defaults to the current line color) */
    ImPlotCol_MarkerOutline,
    /** marker fill color (defaults to the current line color) */
    ImPlotCol_MarkerFill,
    /** error bar color (defaults to ImGuiCol_Text) */
    ImPlotCol_ErrorBar,

    // plot styling colors

    /** plot frame background color (defaults to ImGuiCol_FrameBg) */
    ImPlotCol_FrameBg,
    /** plot area background color (defaults to ImGuiCol_WindowBg) */
    ImPlotCol_PlotBg,
    /** plot area border color (defaults to ImGuiCol_Border) */
    ImPlotCol_PlotBorder,
    /** legend background color (defaults to ImGuiCol_PopupBg) */
    ImPlotCol_LegendBg,
    /** legend border color (defaults to ImPlotCol_PlotBorder) */
    ImPlotCol_LegendBorder,
    /** legend text color (defaults to ImPlotCol_InlayText) */
    ImPlotCol_LegendText,
    /** plot title text color (defaults to ImGuiCol_Text) */
    ImPlotCol_TitleText,
    /** color of text appearing inside of plots (defaults to ImGuiCol_Text) */
    ImPlotCol_InlayText,
    /** x-axis label and tick lables color (defaults to ImGuiCol_Text) */
    ImPlotCol_XAxis,
    /** x-axis grid color (defaults to 25% ImPlotCol_XAxis) */
    ImPlotCol_XAxisGrid,
    /** y-axis label and tick labels color (defaults to ImGuiCol_Text) */
    ImPlotCol_YAxis,
    /** y-axis grid color (defaults to 25% ImPlotCol_YAxis) */
    ImPlotCol_YAxisGrid,
    /** 2nd y-axis label and tick labels color (defaults to ImGuiCol_Text) */
    ImPlotCol_YAxis2,
    /** 2nd y-axis grid/label color (defaults to 25% ImPlotCol_YAxis2) */
    ImPlotCol_YAxisGrid2,
    /** 3rd y-axis label and tick labels color (defaults to ImGuiCol_Text) */
    ImPlotCol_YAxis3,
    /** 3rd y-axis grid/label color (defaults to 25% ImPlotCol_YAxis3) */
    ImPlotCol_YAxisGrid3,
    /** box-selection color (defaults to yellow) */
    ImPlotCol_Selection,
    /** box-query color (defaults to green) */
    ImPlotCol_Query,
    /** crosshairs color (defaults to ImPlotCol_PlotBorder) */
    ImPlotCol_Crosshairs,

    ImPlotCol_COUNT
};

/** Plot styling variables. */
enum ImPlotStyleVar_ {
    // item styling variables
    /** float,  plot item line weight in pixels */
    ImPlotStyleVar_LineWeight,
    /** int,    marker specification */
    ImPlotStyleVar_Marker,
    /** float,  marker size in pixels (roughly the marker's "radius") */
    ImPlotStyleVar_MarkerSize,
    /** float,  plot outline weight of markers in pixels */
    ImPlotStyleVar_MarkerWeight,
    /** float,  alpha modifier applied to all plot item fills */
    ImPlotStyleVar_FillAlpha,
    /** float,  error bar whisker width in pixels */
    ImPlotStyleVar_ErrorBarSize,
    /** float,  error bar whisker weight in pixels */
    ImPlotStyleVar_ErrorBarWeight,
    /** float,  digital channels bit height (at 1) in pixels */
    ImPlotStyleVar_DigitalBitHeight,
    /** float,  digital channels bit padding gap in pixels */
    ImPlotStyleVar_DigitalBitGap,

    // plot styling variables

    /** float,  thickness of border around plot area */
    ImPlotStyleVar_PlotBorderSize,
    /** float,  alpha multiplier applied to minor axis grid lines */
    ImPlotStyleVar_MinorAlpha,
    /** ImVec2, major tick lengths for X and Y axes */
    ImPlotStyleVar_MajorTickLen,
    /** ImVec2, minor tick lengths for X and Y axes */
    ImPlotStyleVar_MinorTickLen,
    /** ImVec2, line thickness of major ticks */
    ImPlotStyleVar_MajorTickSize,
    /** ImVec2, line thickness of minor ticks */
    ImPlotStyleVar_MinorTickSize,
    /** ImVec2, line thickness of major grid lines */
    ImPlotStyleVar_MajorGridSize,
    /** ImVec2, line thickness of minor grid lines */
    ImPlotStyleVar_MinorGridSize,
    /** ImVec2, padding between widget frame and plot area, labels, or outside legends (i.e. main padding) */
    ImPlotStyleVar_PlotPadding,
    /** ImVec2, padding between axes labels, tick labels, and plot edge */
    ImPlotStyleVar_LabelPadding,
    /** ImVec2, legend padding from plot edges */
    ImPlotStyleVar_LegendPadding,
    /** ImVec2, legend inner padding from legend edges */
    ImPlotStyleVar_LegendInnerPadding,
    /** ImVec2, spacing between legend entries */
    ImPlotStyleVar_LegendSpacing,
    /** ImVec2, padding between plot edge and interior info text */
    ImPlotStyleVar_MousePosPadding,
    /** ImVec2, text padding around annotation labels */
    ImPlotStyleVar_AnnotationPadding,
    /** ImVec2, additional fit padding as a percentage of the fit extents (e.g. ImVec2(0.1f,0.1f) adds 10% to the fit extents of X and Y) */
    ImPlotStyleVar_FitPadding,
    /** ImVec2, default size used when ImVec2(0,0) is passed to BeginPlot */
    ImPlotStyleVar_PlotDefaultSize,
    /** ImVec2, minimum size plot frame can be when shrunk */
    ImPlotStyleVar_PlotMinSize,

    ImPlotStyleVar_COUNT
};

/** Marker specifications. */
enum ImPlotMarker_ {
    /** no marker */
    ImPlotMarker_None = -1,
    /** a circle marker */
    ImPlotMarker_Circle,
    /** a square maker */
    ImPlotMarker_Square,
    /** a diamond marker */
    ImPlotMarker_Diamond,
    /** an upward-pointing triangle marker */
    ImPlotMarker_Up,
    /** an downward-pointing triangle marker */
    ImPlotMarker_Down,
    /** an leftward-pointing triangle marker */
    ImPlotMarker_Left,
    /** an rightward-pointing triangle marker */
    ImPlotMarker_Right,
    /** a cross marker (not fillable) */
    ImPlotMarker_Cross,
    /** a plus marker (not fillable) */
    ImPlotMarker_Plus,
    /** a asterisk marker (not fillable) */
    ImPlotMarker_Asterisk,

    ImPlotMarker_COUNT
};

/** Built-in colormaps */
enum ImPlotColormap_ {
    /**  a.k.a. seaborn deep             (qual=true,  n=10) (default) */
    ImPlotColormap_Deep     = 0,   
    /**  a.k.a. matplotlib "Set1"        (qual=true,  n=9 ) */
    ImPlotColormap_Dark     = 1,   
    /**  a.k.a. matplotlib "Pastel1"     (qual=true,  n=9 ) */
    ImPlotColormap_Pastel   = 2,   
    /**  a.k.a. matplotlib "Paired"      (qual=true,  n=12) */
    ImPlotColormap_Paired   = 3,   
    /**  a.k.a. matplotlib "viridis"     (qual=false, n=11) */
    ImPlotColormap_Viridis  = 4,   
    /**  a.k.a. matplotlib "plasma"      (qual=false, n=11) */
    ImPlotColormap_Plasma   = 5,   
    /**  a.k.a. matplotlib/MATLAB "hot"  (qual=false, n=11) */
    ImPlotColormap_Hot      = 6,   
    /**  a.k.a. matplotlib/MATLAB "cool" (qual=false, n=11) */
    ImPlotColormap_Cool     = 7,   
    /**  a.k.a. matplotlib/MATLAB "pink" (qual=false, n=11) */
    ImPlotColormap_Pink     = 8,   
    /**  a.k.a. MATLAB "jet"             (qual=false, n=11) */
    ImPlotColormap_Jet      = 9,   
    /**  a.k.a. matplotlib "twilight"    (qual=false, n=11) */
    ImPlotColormap_Twilight = 10,  
    /**  red/blue, Color Brewer          (qual=false, n=11) */
    ImPlotColormap_RdBu     = 11,  
    /**  brown/blue-green, Color Brewer  (qual=false, n=11) */
    ImPlotColormap_BrBG     = 12,  
    /**  pink/yellow-green, Color Brewer (qual=false, n=11) */
    ImPlotColormap_PiYG     = 13,  
    /**  color spectrum, Color Brewer    (qual=false, n=11) */
    ImPlotColormap_Spectral = 14,  
    /**  white/black                     (qual=false, n=2 ) */
    ImPlotColormap_Greys    = 15
};

/** Used to position items on a plot (e.g. legends, labels, etc.) */
enum ImPlotLocation_ {
    /** center-center */
    ImPlotLocation_Center    = 0,
    /** top-center */
    ImPlotLocation_North     = 1 << 0,
    /** bottom-center */
    ImPlotLocation_South     = 1 << 1,
    /** center-left */
    ImPlotLocation_West      = 1 << 2,
    /** center-right */
    ImPlotLocation_East      = 1 << 3,
    /** top-left */
    ImPlotLocation_NorthWest = ImPlotLocation_North | ImPlotLocation_West,
    /** top-right */
    ImPlotLocation_NorthEast = ImPlotLocation_North | ImPlotLocation_East,
    /** bottom-left */
    ImPlotLocation_SouthWest = ImPlotLocation_South | ImPlotLocation_West,
    /** bottom-right */
    ImPlotLocation_SouthEast = ImPlotLocation_South | ImPlotLocation_East
};

/** Used to orient items on a plot (e.g. legends, labels, etc.) */
enum ImPlotOrientation_ {
    /** left/right */
    ImPlotOrientation_Horizontal,
    /** up/down */
    ImPlotOrientation_Vertical
};

/** Enums for different y-axes. */
enum ImPlotYAxis_ {
    /** left (default) */
    ImPlotYAxis_1 = 0,
    /** first on right side */
    ImPlotYAxis_2 = 1,
    /** second on right side */
    ImPlotYAxis_3 = 2
};

/** Enums for different automatic histogram binning methods (k = bin count or w = bin width) */
enum ImPlotBin_ {
    /**  k = sqrt(n) */
    ImPlotBin_Sqrt    = -1, 
    /**  k = 1 + log2(n) */
    ImPlotBin_Sturges = -2, 
    /**  k = 2 * cbrt(n) */
    ImPlotBin_Rice    = -3, 
    /**  w = 3.49 * sigma / cbrt(n) */
    ImPlotBin_Scott   = -4, 
};

//

/** Plot style structure */
class ImPlotStyle
{
public:
    // item styling variables

	/** item line weight in pixels
    * = 1 */
    float        LineWeight;
	/** marker specification *
	* = ImPlotMarker_None */
    int Marker;
	/** marker size in pixels (roughly the marker's "radius") *
	* = 4 */
    float        MarkerSize;
	/** outline weight of markers in pixels *
	* = 1 */
    float        MarkerWeight;
	/** alpha modifier applied to plot fills *
	* = 1 */
    float        FillAlpha;
	/** error bar whisker width in pixels *
	* = 5 */
    float        ErrorBarSize;
	/** error bar whisker weight in pixels *
	* = 1.5 */
    float        ErrorBarWeight;
	/** digital channels bit height (at y = 1.0f) in pixels *
	* = 8 */
    float        DigitalBitHeight;
	/** digital channels bit padding gap in pixels *
	* = 4 */
    float        DigitalBitGap;

    // plot styling variables

	/** line thickness of border around plot area
	* = 1 */
    float   PlotBorderSize;
	/** alpha multiplier applied to minor axis grid lines
	* = 0.25 */
    float   MinorAlpha;
	/** major tick lengths for X and Y axes
	* = 10,10 */
    ImVec2  MajorTickLen;
	/** minor tick lengths for X and Y axes
	* = 5,5 */
    ImVec2  MinorTickLen;
	/** line thickness of major ticks
	* = 1,1 */
    ImVec2  MajorTickSize;
	/** line thickness of minor ticks
	* = 1,1 */
    ImVec2  MinorTickSize;
	/** line thickness of major grid lines
	* = 1,1 */
    ImVec2  MajorGridSize;
	/** line thickness of minor grid lines
	* = 1,1 */
    ImVec2  MinorGridSize;
	/** padding between widget frame and plot area, labels, or outside legends (i.e. main padding)
	* = 10,10 */
    ImVec2  PlotPadding;
	/** padding between axes labels, tick labels, and plot edge
	* = 5,5 */
    ImVec2  LabelPadding;
	/** legend padding from plot edges
	* = 10,10 */
    ImVec2  LegendPadding;
	/** legend inner padding from legend edges
	* = 5,5 */
    ImVec2  LegendInnerPadding;
	/** spacing between legend entries
	* = 5,0 */
    ImVec2  LegendSpacing;
	/** padding between plot edge and interior mouse location text
	* = 10,10 */
    ImVec2  MousePosPadding;
	/** text padding around annotation labels
	* = 2,2 */
    ImVec2  AnnotationPadding;
    /** additional fit padding as a percentage of the fit extents (e.g. ImVec2(0.1f,0.1f) adds 10% to the fit extents of X and Y)
	* = 0,0 */
    ImVec2  FitPadding;
	/** default size used when ImVec2(0,0) is passed to BeginPlot
	* = 400,300 */
    ImVec2  PlotDefaultSize;
	/** minimum size plot frame can be when shrunk
	* = 300,225 */
    ImVec2  PlotMinSize;

    // style colors

	/** array of plot specific colors */
    //ImVec4       Colors[ImPlotCol_COUNT];

    // colormap

	/** The current colormap. Set this to either an ImPlotColormap_ enum or an index returned by AddColormap. */
    ImPlotColormap Colormap;

    // settings/flags

	/** enable global anti-aliasing on plot lines (overrides ImPlotFlags_AntiAliased)
	* = false */
    bool    AntiAliasedLines;
	/** axis labels will be formatted for your timezone when ImPlotAxisFlag_Time is enabled
	* = false */
    bool    UseLocalTime;
	/** dates will be formatted according to ISO 8601 where applicable (e.g. YYYY-MM-DD, YYYY-MM, --MM-DD, etc.)
	* = false */
    bool    UseISO8601;
	/** times will be formatted using a 24 hour clock
	* = false */
    bool    Use24HourClock;

    ImPlotStyle();
};

//-----------------------------------------------------------------------------
/** Begin/End Plot */
//-----------------------------------------------------------------------------

class ImPlot
{
public:

// Starts a 2D plotting context. If this function returns true, EndPlot() MUST
// be called! You are encouraged to use the following convention:
//
// if (BeginPlot(...)) {
//     ImPlot::PlotLine(...);
//     ...
//     EndPlot();
// }
//
// Important notes:
//
// - #title_id must be unique to the current ImGui ID scope. If you need to avoid ID
//   collisions or don't want to display a title in the plot, use double hashes
//   (e.g. "MyPlot##HiddenIdText" or "##NoTitle").
// - If #x_label and/or #y_label are provided, axes labels will be displayed.
// - #size is the **frame** size of the plot widget, not the plot area. The default
//   size of plots (i.e. when ImVec2(0,0)) can be modified in your ImPlotStyle
//   (default is 400x300 px).
// - Auxiliary y-axes must be enabled with ImPlotFlags_YAxis2/3 to be displayed.
// - See ImPlotFlags and ImPlotAxisFlags for more available options.
/*
@lua manual
IMPLOT_API bool BeginPlot(const char* title_id,
                          const char* x_label      = NULL,
                          const char* y_label      = NULL,
                          const ImVec2& size       = ImVec2(-1,0),
                          ImPlotFlags flags        = ImPlotFlags_None,
                          ImPlotAxisFlags x_flags  = ImPlotAxisFlags_None,
                          ImPlotAxisFlags y_flags  = ImPlotAxisFlags_None,
                          ImPlotAxisFlags y2_flags = ImPlotAxisFlags_NoGridLines,
                          ImPlotAxisFlags y3_flags = ImPlotAxisFlags_NoGridLines);
*/

/** Only call EndPlot() if BeginPlot() returns true! Typically called at the end
* of an if statement conditioned on BeginPlot(). See example above. */
static void EndPlot();

//-----------------------------------------------------------------------------
/** Plot Items */
//-----------------------------------------------------------------------------

/** Plots a centered text label at point x,y with an optional pixel offset. Text color can be changed with ImPlot::PushStyleColor(ImPlotCol_InlayText, ...). */
static void PlotText(const char* text, double x, double y, bool vertical = false, const ImVec2& pixel_offset = ImVec2(0,0));

/** Plots a dummy item (i.e. adds a legend entry colored by ImPlotCol_Line) */
static void PlotDummy(const char* label_id);


//-----------------------------------------------------------------------------
// Plot Utils
//-----------------------------------------------------------------------------

// The following functions MUST be called before BeginPlot!

/** Set the axes range limits of the next plot. Call right before BeginPlot(). If ImGuiCond_Always is used, the axes limits will be locked. */
IMPLOT_API void SetNextPlotLimits(double xmin, double xmax, double ymin, double ymax, ImGuiCond cond = ImGuiCond_Once);
/** Set the X axis range limits of the next plot. Call right before BeginPlot(). If ImGuiCond_Always is used, the X axis limits will be locked. */
IMPLOT_API void SetNextPlotLimitsX(double xmin, double xmax, ImGuiCond cond = ImGuiCond_Once);
/** Set the Y axis range limits of the next plot. Call right before BeginPlot(). If ImGuiCond_Always is used, the Y axis limits will be locked. */
IMPLOT_API void SetNextPlotLimitsY(double ymin, double ymax, ImGuiCond cond = ImGuiCond_Once, ImPlotYAxis y_axis = ImPlotYAxis_1);
/** Links the next plot limits to external values. Set to NULL for no linkage. The pointer data must remain valid until the matching call EndPlot. */
//@lua N/A
//IMPLOT_API void LinkNextPlotLimits(double* xmin, double* xmax, double* ymin, double* ymax, double* ymin2 = NULL, double* ymax2 = NULL, double* ymin3 = NULL, double* ymax3 = NULL);
/** Fits the next plot axes to all plotted data if they are unlocked (equivalent to double-clicks). */
IMPLOT_API void FitNextPlotAxes(bool x = true, bool y = true, bool y2 = true, bool y3 = true);

/** Set the X axis ticks and optionally the labels for the next plot. */
//@lua manual
//IMPLOT_API void SetNextPlotTicksX(const double* values, int n_ticks, const char* const labels[] = NULL, bool show_default = false);
//IMPLOT_API void SetNextPlotTicksX(double x_min, double x_max, int n_ticks, const char* const labels[] = NULL, bool show_default = false);

/** Set the Y axis ticks and optionally the labels for the next plot. */
//@lua manual
//IMPLOT_API void SetNextPlotTicksY(const double* values, int n_ticks, const char* const labels[] = NULL, bool show_default = false, ImPlotYAxis y_axis = 0);
//IMPLOT_API void SetNextPlotTicksY(double y_min, double y_max, int n_ticks, const char* const labels[] = NULL, bool show_default = false, ImPlotYAxis y_axis = 0);

/** Set the format for numeric X axis labels (default="%g"). Formated values will be doubles (i.e. don't supply %d, %i, etc.). Not applicable if ImPlotAxisFlags_Time enabled. */
IMPLOT_API void SetNextPlotFormatX(const char* fmt);
/** Set the format for numeric Y axis labels (default="%g"). Formated values will be doubles (i.e. don't supply %d, %i, etc.). */
IMPLOT_API void SetNextPlotFormatY(const char* fmt, ImPlotYAxis y_axis=ImPlotYAxis_1);

// The following functions MUST be called BETWEEN Begin/EndPlot!

/** Select which Y axis will be used for subsequent plot elements. The default is ImPlotYAxis_1, or the first (left) Y axis. Enable 2nd and 3rd axes with ImPlotFlags_YAxisX. */
IMPLOT_API void SetPlotYAxis(ImPlotYAxis y_axis);
/** Hides or shows the next plot item (i.e. as if it were toggled from the legend). Use ImGuiCond_Always if you need to forcefully set this every frame. */
IMPLOT_API void HideNextItem(bool hidden = true, ImGuiCond cond = ImGuiCond_Once);

/** Convert pixels to a position in the current plot's coordinate system. A negative y_axis uses the current value of SetPlotYAxis (ImPlotYAxis_1 initially). */
//IMPLOT_API ImPlotPoint PixelsToPlot(const ImVec2& pix, ImPlotYAxis y_axis = IMPLOT_AUTO);
IMPLOT_API ImPlotPoint PixelsToPlot(float x, float y, ImPlotYAxis y_axis = IMPLOT_AUTO);
/** Convert a position in the current plot's coordinate system to pixels. A negative y_axis uses the current value of SetPlotYAxis (ImPlotYAxis_1 initially). */
//IMPLOT_API ImVec2 PlotToPixels(const ImPlotPoint& plt, ImPlotYAxis y_axis = IMPLOT_AUTO);
IMPLOT_API ImVec2 PlotToPixels(double x, double y, ImPlotYAxis y_axis = IMPLOT_AUTO);
/** Get the current Plot position (top-left) in pixels. */
IMPLOT_API ImVec2 GetPlotPos();
/** Get the curent Plot size in pixels. */
IMPLOT_API ImVec2 GetPlotSize();
/** Returns true if the plot area in the current plot is hovered. */
IMPLOT_API bool IsPlotHovered();
/** Returns true if the XAxis plot area in the current plot is hovered. */
IMPLOT_API bool IsPlotXAxisHovered();
/** Returns true if the YAxis[n] plot area in the current plot is hovered. */
IMPLOT_API bool IsPlotYAxisHovered(ImPlotYAxis y_axis = 0);
/** Returns the mouse position in x,y coordinates of the current plot. A negative y_axis uses the current value of SetPlotYAxis (ImPlotYAxis_1 initially). */
IMPLOT_API ImPlotPoint GetPlotMousePos(ImPlotYAxis y_axis = IMPLOT_AUTO);
/** Returns the current plot axis range. A negative y_axis uses the current value of SetPlotYAxis (ImPlotYAxis_1 initially). */
IMPLOT_API ImPlotLimits GetPlotLimits(ImPlotYAxis y_axis = IMPLOT_AUTO);

/** Returns true if the current plot is being box selected. */
IMPLOT_API bool IsPlotSelected();
/** Returns the current plot box selection bounds. */
IMPLOT_API ImPlotLimits GetPlotSelection(ImPlotYAxis y_axis = IMPLOT_AUTO);

/** Returns true if the current plot is being queried or has an active query. Query must be enabled with ImPlotFlags_Query. */
IMPLOT_API bool IsPlotQueried();
/** Returns the current plot query bounds. Query must be enabled with ImPlotFlags_Query. */
IMPLOT_API ImPlotLimits GetPlotQuery(ImPlotYAxis y_axis = IMPLOT_AUTO);
/** Set the current plot query bounds. Query must be enabled with ImPlotFlags_Query. */
IMPLOT_API void SetPlotQuery(const ImPlotLimits& query, ImPlotYAxis y_axis = IMPLOT_AUTO);

//-----------------------------------------------------------------------------
// Plot Tools
//-----------------------------------------------------------------------------

// The following functions MUST be called BETWEEN Begin/EndPlot!

//@lua fix
/** Shows an annotation callout at a chosen point. */
IMPLOT_API void Annotate(double x, double y, const ImVec2& pix_offset, const char* fmt);
IMPLOT_API void Annotate(double x, double y, const ImVec2& pix_offset, const ImVec4& color, const char* fmt);
//IMPLOT_API void AnnotateV(double x, double y, const ImVec2& pix_offset, const char* fmt, va_list args);
//IMPLOT_API void AnnotateV(double x, double y, const ImVec2& pix_offset, const ImVec4& color, const char* fmt, va_list args);

//@lua fix
/** Same as above, but the annotation will always be clamped to stay inside the plot area. */
IMPLOT_API void AnnotateClamped(double x, double y, const ImVec2& pix_offset, const char* fmt);
IMPLOT_API void AnnotateClamped(double x, double y, const ImVec2& pix_offset, const ImVec4& color, const char* fmt);
//IMPLOT_API void AnnotateClampedV(double x, double y, const ImVec2& pix_offset, const char* fmt, va_list args);
//IMPLOT_API void AnnotateClampedV(double x, double y, const ImVec2& pix_offset, const ImVec4& color, const char* fmt, va_list args);

//@lua manual
/** Shows a draggable vertical guide line at an x-value. #col defaults to ImGuiCol_Text. */
//IMPLOT_API bool DragLineX(const char* id, double* x_value, bool show_label = true, const ImVec4& col = IMPLOT_AUTO_COL, float thickness = 1);
/** Shows a draggable horizontal guide line at a y-value. #col defaults to ImGuiCol_Text. */
//IMPLOT_API bool DragLineY(const char* id, double* y_value, bool show_label = true, const ImVec4& col = IMPLOT_AUTO_COL, float thickness = 1);
/** Shows a draggable point at x,y. #col defaults to ImGuiCol_Text. */
//IMPLOT_API bool DragPoint(const char* id, double* x, double* y, bool show_label = true, const ImVec4& col = IMPLOT_AUTO_COL, float radius = 4);

//-----------------------------------------------------------------------------
// Legend Utils and Tools
//-----------------------------------------------------------------------------

// The following functions MUST be called between Begin/EndPlot!

/** Set the location of the current plot's legend (default = North|West). */
IMPLOT_API void SetLegendLocation(ImPlotLocation location, ImPlotOrientation orientation = ImPlotOrientation_Vertical, bool outside = false);
/** Set the location of the current plot's mouse position text (default = South|East). */
IMPLOT_API void SetMousePosLocation(ImPlotLocation location);
/** Returns true if a plot item legend entry is hovered. */
IMPLOT_API bool IsLegendEntryHovered(const char* label_id);
/** Begin a popup for a legend entry. */
IMPLOT_API bool BeginLegendPopup(const char* label_id, ImGuiMouseButton mouse_button = 1);
/** End a popup for a legend entry. */
IMPLOT_API void EndLegendPopup();

//-----------------------------------------------------------------------------
// Drag and Drop Utils
//-----------------------------------------------------------------------------

// The following functions MUST be called BETWEEN Begin/EndPlot!

/** Turns the current plot's plotting area into a drag and drop target. Don't forget to call EndDragDropTarget! */
IMPLOT_API bool BeginDragDropTarget();
/** Turns the current plot's X-axis into a drag and drop target. Don't forget to call EndDragDropTarget! */
IMPLOT_API bool BeginDragDropTargetX();
/** Turns the current plot's Y-Axis into a drag and drop target. Don't forget to call EndDragDropTarget! */
IMPLOT_API bool BeginDragDropTargetY(ImPlotYAxis axis = ImPlotYAxis_1);
/** Turns the current plot's legend into a drag and drop target. Don't forget to call EndDragDropTarget! */
IMPLOT_API bool BeginDragDropTargetLegend();
/** Ends a drag and drop target (currently just an alias for ImGui::EndDragDropTarget). */
IMPLOT_API void EndDragDropTarget();

// NB: By default, plot and axes drag and drop *sources* require holding the Ctrl modifier to initiate the drag.
// You can change the modifier if desired. If ImGuiKeyModFlags_None is provided, the axes will be locked from panning.

/** Turns the current plot's plotting area into a drag and drop source. Don't forget to call EndDragDropSource! */
IMPLOT_API bool BeginDragDropSource(ImGuiKeyModFlags key_mods = ImGuiKeyModFlags_Ctrl, ImGuiDragDropFlags flags = 0);
/** Turns the current plot's X-axis into a drag and drop source. Don't forget to call EndDragDropSource! */
IMPLOT_API bool BeginDragDropSourceX(ImGuiKeyModFlags key_mods = ImGuiKeyModFlags_Ctrl, ImGuiDragDropFlags flags = 0);
/** Turns the current plot's Y-axis into a drag and drop source. Don't forget to call EndDragDropSource! */
IMPLOT_API bool BeginDragDropSourceY(ImPlotYAxis axis = ImPlotYAxis_1, ImGuiKeyModFlags key_mods = ImGuiKeyModFlags_Ctrl, ImGuiDragDropFlags flags = 0);
/** Turns an item in the current plot's legend into drag and drop source. Don't forget to call EndDragDropSource! */
IMPLOT_API bool BeginDragDropSourceItem(const char* label_id, ImGuiDragDropFlags flags = 0);
/** Ends a drag and drop source (currently just an alias for ImGui::EndDragDropSource). */
IMPLOT_API void EndDragDropSource();

//-----------------------------------------------------------------------------
// Plot and Item Styling
//-----------------------------------------------------------------------------

//@lua fix
/** Provides access to plot style structure for permanant modifications to colors, sizes, etc. */
IMPLOT_API ImPlotStyle& GetStyle();

/** Style plot colors for current ImGui style (default). */
IMPLOT_API void StyleColorsAuto(/*ImPlotStyle* dst = NULL*/);
/** Style plot colors for ImGui "Classic". */
IMPLOT_API void StyleColorsClassic(/*ImPlotStyle* dst = NULL*/);
/** Style plot colors for ImGui "Dark". */
IMPLOT_API void StyleColorsDark(/*ImPlotStyle* dst = NULL*/);
/** Style plot colors for ImGui "Light". */
IMPLOT_API void StyleColorsLight(/*ImPlotStyle* dst = NULL*/);

// Use PushStyleX to temporarily modify your ImPlotStyle. The modification
// will last until the matching call to PopStyleX. You MUST call a pop for
// every push, otherwise you will leak memory! This behaves just like ImGui.

// Temporarily modify a style color. Don't forget to call PopStyleColor!
//static void PushStyleColor(ImPlotCol idx, ImU32 col);
// Temporarily modify a style color. Don't forget to call PopStyleColor!
//static void PushStyleColor(ImPlotCol idx, const ImVec4& col);
/** Undo temporary style color modification(s). Undo multiple pushes at once by increasing count. */
static void PopStyleColor(int count = 1);

/** Temporarily modify a style variable of float type. Don't forget to call PopStyleVar! */
//static void PushStyleVar(int idx, float val);
/** Temporarily modify a style variable of int type. Don't forget to call PopStyleVar! */
//static void PushStyleVar(int idx, int val);
/** Undo temporary style variable modification(s). Undo multiple pushes at once by increasing count. */
static void PopStyleVar(int count = 1);

// The following can be used to modify the style of the next plot item ONLY. They do
// NOT require calls to PopStyleX. Leave style attributes you don't want modified to
// IMPLOT_AUTO or IMPLOT_AUTO_COL. Automatic styles will be deduced from the current
// values in your ImPlotStyle or from Colormap data.

/** Set the line color and weight for the next item only. */
IMPLOT_API void SetNextLineStyle(const ImVec4& col = IMPLOT_AUTO_COL, float weight = IMPLOT_AUTO);
/** Set the fill color for the next item only. */
IMPLOT_API void SetNextFillStyle(const ImVec4& col = IMPLOT_AUTO_COL, float alpha_mod = IMPLOT_AUTO);
/** Set the marker style for the next item only. */
IMPLOT_API void SetNextMarkerStyle(ImPlotMarker marker = IMPLOT_AUTO, float size = IMPLOT_AUTO, const ImVec4& fill = IMPLOT_AUTO_COL, float weight = IMPLOT_AUTO, const ImVec4& outline = IMPLOT_AUTO_COL);
/** Set the error bar style for the next item only. */
IMPLOT_API void SetNextErrorBarStyle(const ImVec4& col = IMPLOT_AUTO_COL, float size = IMPLOT_AUTO, float weight = IMPLOT_AUTO);

/** Gets the last item primary color (i.e. its legend icon color) */
IMPLOT_API ImVec4 GetLastItemColor();

/** Returns the null terminated string name for an ImPlotCol. */
IMPLOT_API const char* GetStyleColorName(ImPlotCol idx);
/** Returns the null terminated string name for an ImPlotMarker. */
IMPLOT_API const char* GetMarkerName(ImPlotMarker idx);

//-----------------------------------------------------------------------------
// Colormaps
//-----------------------------------------------------------------------------

// Item styling is based on colormaps when the relevant ImPlotCol_XXX is set to
// IMPLOT_AUTO_COL (default). Several built-in colormaps are available. You can
// add and then push/pop your own colormaps as well. To permanently set a colormap,
// modify the Colormap index member of your ImPlotStyle.

// Colormap data will be ignored and a custom color will be used if you have done one of the following:
//     1) Modified an item style color in your ImPlotStyle to anything other than IMPLOT_AUTO_COL.
//     2) Pushed an item style color using PushStyleColor().
//     3) Set the next item style with a SetNextXXXStyle function.

//@lua manual
// Add a new colormap. The color data will be copied. The colormap can be used by pushing either the returned index or the
// string name with PushColormap. The colormap name must be unique and the size must be greater than 1. You will receive
// an assert otherwise! By default colormaps are considered to be qualitative (i.e. discrete). If you want to create a
// continuous colormap, set #qual=false. This will treat the colors you provide as keys, and ImPlot will build a linearly
// interpolated lookup table. The memory footprint of this table will be exactly ((size-1)*255+1)*4 bytes.
// IMPLOT_API ImPlotColormap AddColormap(const char* name, const ImVec4* cols, int size, bool qual=true);
// IMPLOT_API ImPlotColormap AddColormap(const char* name, const ImU32*  cols, int size, bool qual=true);

/** Returns the number of available colormaps (i.e. the built-in + user-added count). */
IMPLOT_API int GetColormapCount();
/** Returns a null terminated string name for a colormap given an index. Returns NULL if index is invalid. */
IMPLOT_API const char* GetColormapName(ImPlotColormap cmap);
/** Returns an index number for a colormap given a valid string name. Returns -1 if name is invalid. */
IMPLOT_API ImPlotColormap GetColormapIndex(const char* name);

// Temporarily switch to one of the built-in (i.e. ImPlotColormap_XXX) or user-added colormaps (i.e. a return value of AddColormap). Don't forget to call PopColormap!
// IMPLOT_API void PushColormap(ImPlotColormap cmap);
// Push a colormap by string name. Use built-in names such as "Default", "Deep", "Jet", etc. or a string you provided to AddColormap. Don't forget to call PopColormap!
// IMPLOT_API void PushColormap(const char* name);
/** Undo temporary colormap modification(s). Undo multiple pushes at once by increasing count. */
IMPLOT_API void PopColormap(int count = 1);

/** Returns the next color from the current colormap and advances the colormap for the current plot.
* Can also be used with no return value to skip colors if desired. You need to call this between Begin/EndPlot! */
IMPLOT_API ImVec4 NextColormapColor();

// Colormap utils. If cmap = IMPLOT_AUTO (default), the current colormap is assumed.
// Pass an explicit colormap index (built-in or user-added) to specify otherwise.

/** Returns the size of a colormap. */
IMPLOT_API int GetColormapSize(ImPlotColormap cmap = IMPLOT_AUTO);
/** Returns a color from a colormap given an index >= 0 (modulo will be performed). */
IMPLOT_API ImVec4 GetColormapColor(int idx, ImPlotColormap cmap = IMPLOT_AUTO);
/** Sample a color from the current colormap given t between 0 and 1. */
IMPLOT_API ImVec4 SampleColormap(float t, ImPlotColormap cmap = IMPLOT_AUTO);

/** Shows a vertical color scale with linear spaced ticks using the specified color map. Use double hashes to hide label (e.g. "##NoLabel"). */
IMPLOT_API void ColormapScale(const char* label, double scale_min, double scale_max, const ImVec2& size = ImVec2(0,0), ImPlotColormap cmap = IMPLOT_AUTO, const char* fmt = "%g");
/** Shows a horizontal slider with a colormap gradient background. Optionally returns the color sampled at t in [0 1]. */
// IMPLOT_API bool ColormapSlider(const char* label, float* t, ImVec4* out = NULL, const char* format = "", ImPlotColormap cmap = IMPLOT_AUTO);
/** Shows a button with a colormap gradient brackground. */
IMPLOT_API bool ColormapButton(const char* label, const ImVec2& size = ImVec2(0,0), ImPlotColormap cmap = IMPLOT_AUTO);

// When items in a plot sample their color from a colormap, the color is cached and does not change
// unless explicitly overriden. Therefore, if you change the colormap after the item has already been plotted,
// item colors will NOT update. If you need item colors to resample the new colormap, then use this
// function to bust the cached colors. If #plot_title_id is NULL, then every item in EVERY existing plot
// will be cache busted. Otherwise only the plot specified by #plot_title_id will be busted. For the
// latter, this function must be called in the same ImGui ID scope that the plot is in. You should rarely if ever
// need this function, but it is available for applications that require runtime colormap swaps (e.g. Heatmaps demo).
// IMPLOT_API void BustColorCache(const char* plot_title_id = NULL);

//-----------------------------------------------------------------------------
// Miscellaneous
//-----------------------------------------------------------------------------

// Render icons similar to those that appear in legends (nifty for data lists).
// IMPLOT_API void ItemIcon(const ImVec4& col);
// IMPLOT_API void ItemIcon(ImU32 col);

IMPLOT_API void ColormapIcon(ImPlotColormap cmap);

/** Get the plot draw list for custom rendering to the current plot area. Call between Begin/EndPlot. */
IMPLOT_API ImDrawList* GetPlotDrawList();
/** Push clip rect for rendering to current plot area. The rect can be expanded or contracted by #expand pixels. Call between Begin/EndPlot.
IMPLOT_API void PushPlotClipRect(float expand=0); */
IMPLOT_API void PushPlotClipRect(float expand=0);
/** Pop plot clip rect. Call between Begin/EndPlot. */
IMPLOT_API void PopPlotClipRect();

/** Shows ImPlot style selector dropdown menu. */
IMPLOT_API bool ShowStyleSelector(const char* label);
/** Shows ImPlot colormap selector dropdown menu. */
IMPLOT_API bool ShowColormapSelector(const char* label);
/** Add basic help/info block for end users (not a window). */
IMPLOT_API void ShowStyleEditor(/*ImPlotStyle* ref = NULL*/);
/** Add basic help/info block (not a window): how to manipulate ImPlot as an end-user. */
IMPLOT_API void ShowUserGuide();
/** Shows ImPlot metrics/debug information window. */
//@lua manual
//IMPLOT_API void ShowMetricsWindow(bool* p_popen = NULL);

//-----------------------------------------------------------------------------
// Demo (add implot_demo.cpp to your sources!)
//-----------------------------------------------------------------------------

/** Shows the ImPlot demo window. */
//@lua manual
//IMPLOT_API void ShowDemoWindow(bool* p_open = NULL);

};

}  // namespace ImPlot
