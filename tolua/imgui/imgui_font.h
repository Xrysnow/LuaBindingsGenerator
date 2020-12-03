#include "imgui.h"

#define IMGUI_API

namespace imgui
{
class ImFont;

class ImFontConfig
{
public:
	/** 
	* TTF/OTF data size
	*/
    int             FontDataSize;
	/** 
	* TTF/OTF data ownership taken by the container ImFontAtlas (will delete memory itself).
	* true
	*/
    bool            FontDataOwnedByAtlas;
	/** 
	* Index of font within TTF/OTF file
	* 0
	*/
    int             FontNo;
	/** 
	* Size in pixels for rasterizer (more or less maps to the resulting font height).
	*/
    float           SizePixels;
	/** 
	* Rasterize at higher quality for sub-pixel positioning. Read https://github.com/nothings/stb/blob/master/tests/oversample/README.md for details.
	* 3
	*/
    int             OversampleH;
	/** 
	* Rasterize at higher quality for sub-pixel positioning. We don't use sub-pixel positions on the Y axis.
	* 1
	*/
    int             OversampleV;
	/** 
	* Align every glyph to pixel boundary. Useful e.g. if you are merging a non-pixel aligned font with the default font. If enabled, you can set OversampleH/V to 1.
	* false
	*/
    bool            PixelSnapH;
	/** 
	*  Extra spacing (in pixels) between glyphs. Only X axis is supported for now.
	*   0, 0     
	*/
    ImVec2          GlyphExtraSpacing;
	/** 
	*  Offset all glyphs from this font input.
	*   0, 0     
	*/
    ImVec2          GlyphOffset;
	/** 
	* Minimum AdvanceX for glyphs, set Min to align font icons, set both Min/Max to enforce mono-space font
	* 0
	*/
    float           GlyphMinAdvanceX;
	/** 
	* Maximum AdvanceX for glyphs
	* FLT_MAX
	*/
    float           GlyphMaxAdvanceX;
	/** 
	* Merge into previous ImFont, so you can combine multiple inputs font into one ImFont (e.g. ASCII font + icons + Japanese glyphs). You may want to use GlyphOffset.y when merge font of different heights.
	* false
	*/
    bool            MergeMode;
	/** 
	* Settings for custom font rasterizer (e.g. ImGuiFreeType). Leave as zero if you aren't using one.
	* 0x00
	*/
    unsigned int    RasterizerFlags;
	/** 
	*  Brighten (>1.0f) or darken (<1.0f) font output. Brightening small fonts may be a good workaround to make them more readable.
	*   1.0f     
	*/
    float           RasterizerMultiply;
};

/**
* Hold rendering data for one glyph.
* (Note: some language parsers may fail to convert the 31+1 bitfield members, in this case maybe drop store a single u32 or we can rework this)
*/
class ImFontGlyph
{
public:
	/** 
	*  0x0000..0xFFFF
	*/
    unsigned int         Codepoint;
	/** 
	*  Flag to allow early out when rendering
	*/
    unsigned int         Visible;
	/** 
	*  Distance to next character (= data from font + ImFontConfig::GlyphExtraSpacing.x baked in)
	*/
    float           AdvanceX;
	/** 
	*  Glyph corners
	*/
    float           X0, Y0, X1, Y1;
	/** 
	*  Texture coordinates
	*/
    float           U0, V0, U1, V1;
};


/**
* See ImFontAtlas::AddCustomRectXXX functions.
*/
class ImFontAtlasCustomRect
{
public:
	/** 
	* Desired rectangle dimension
	* Input
	*/
	unsigned short  Width, Height;
	/** 
	* Packed position in Atlas
	* Output
	*/
	unsigned short  X, Y;
	/** 
	* For custom font glyphs only (ID < 0x110000)
	* Input
	*/
	unsigned int    GlyphID;
	/** 
	* For custom font glyphs only (ID<0x10000): glyph xadvance
	* Input
	*/
	float           GlyphAdvanceX;
	/** 
	* For custom font glyphs only (ID<0x10000): glyph display offset
	* Input
	*/
	ImVec2          GlyphOffset;
	/** 
	* For custom font glyphs only (ID<0x10000): target font
	* Input
	*/
	ImFont*         Font;
	bool IsPacked() const;
};


/**
* Load and rasterize multiple TTF/OTF fonts into a same texture. The font atlas will build a single texture holding:
*  - One or more fonts.
*  - Custom graphics data needed to render the shapes needed by Dear ImGui.
*  - Mouse cursor shapes for software cursor rendering (unless setting 'Flags |= ImFontAtlasFlags_NoMouseCursors' in the font atlas).
* It is the user-code responsibility to setup/build the atlas, then upload the pixel data into a texture accessible by your graphics api.
*  - Optionally, call any of the AddFont*** functions. If you don't call any, the default font embedded in the code will be loaded for you.
*  - Call GetTexDataAsAlpha8() or GetTexDataAsRGBA32() to build and retrieve pixels data.
*  - Upload the pixels data into a texture within your graphics system (see imgui_impl_xxxx.cpp examples)
*  - Call SetTexID(my_tex_id); and pass the pointer/identifier to your texture in a format natural to your graphics API.
*    This value will be passed back to you during rendering to identify the texture. Read FAQ entry about ImTextureID for more details.
* Common pitfalls:
* - If you pass a 'glyph_ranges' array to AddFont*** functions, you need to make sure that your array persist up until the
*   atlas is build (when calling GetTexData*** or Build()). We only copy the pointer, not the data.
* - Important: By default, AddFontFromMemoryTTF() takes ownership of the data. Even though we are not writing to it, we will free the pointer on destruction.
*   You can set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed,
* - Even though many functions are suffixed with "TTF", OTF data is supported just as well.
* - This is an old API and it is currently awkward for those and and various other reasons! We will address them in the future!
*/
class ImFontAtlas
{
public:
	// needs manual binding

	//IMGUI_API ImFont*           AddFontDefault(const ImFontConfig* font_cfg = NULL);
    //IMGUI_API ImFont*           AddFont(const ImFontConfig* font_cfg);
	//IMGUI_API ImFont*           AddFontFromMemoryTTF(void* font_data, int font_size, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL); // Note: Transfer ownership of 'ttf_data' to ImFontAtlas! Will be deleted after destruction of the atlas. Set font_cfg->FontDataOwnedByAtlas=false to keep ownership of your data and it won't be freed.
    //IMGUI_API ImFont*           AddFontFromFileTTF(const char* filename, float size_pixels, const ImFontConfig* font_cfg = NULL, const ImWchar* glyph_ranges = NULL);

	/** 
	*  Clear input data (all ImFontConfig structures including sizes, TTF data, glyph ranges, etc.) = all the data used to build the texture and fonts.
	*/
    IMGUI_API void              ClearInputData();
	/** 
	*  Clear output texture data (CPU side). Saves RAM once the texture has been copied to graphics memory.
	*/
    IMGUI_API void              ClearTexData();
	/** 
	*  Clear output font data (glyphs storage, UV coordinates).
	*/
    IMGUI_API void              ClearFonts();
	/** 
	*  Clear all input and output.
	*/
    IMGUI_API void              Clear();

    // Build atlas, retrieve pixel data.
    // User is in charge of copying the pixels into graphics memory (e.g. create a texture with your engine). Then store your texture handle with SetTexID().
    // The pitch is always = Width * BytesPerPixels (1 or 4)
    // Building in RGBA32 format is provided for convenience and compatibility, but note that unless you manually manipulate or copy color data into
    // the texture (e.g. when using the AddCustomRect*** api), then the RGB pixels emitted will always be white (~75% of memory/bandwidth waste.
	
	/** 
	*  Build pixels data. This is called automatically for you by the GetTexData*** functions.
	*/
    IMGUI_API bool              Build();
    bool                        IsBuilt();
    //void                        SetTexID(ImTextureID id);
	
    //-------------------------------------------
    // Glyph Ranges
    //-------------------------------------------
	
	// needs manual binding

    // Helpers to retrieve list of common Unicode ranges (2 value per range, values are inclusive, zero-terminated list)
    // NB: Make sure that your string are UTF-8 and NOT in your local code page. In C++11, you can create UTF-8 string literal using the u8"Hello world" syntax. See FAQ for details.
    // NB: Consider using ImFontGlyphRangesBuilder to build glyph ranges from textual data.
	/** 
	*  Basic Latin, Extended Latin
	*/
    // IMGUI_API const ImWchar*    GetGlyphRangesDefault();
	/** 
	*  Default + Korean characters
	*/
    // IMGUI_API const ImWchar*    GetGlyphRangesKorean();
	/** 
	*  Default + Hiragana, Katakana, Half-Width, Selection of 1946 Ideographs
	*/
    // IMGUI_API const ImWchar*    GetGlyphRangesJapanese();
	/** 
	*  Default + Half-Width + Japanese Hiragana/Katakana + full set of about 21000 CJK Unified Ideographs
	*/
    // IMGUI_API const ImWchar*    GetGlyphRangesChineseFull();
    // IMGUI_API const ImWchar*    GetGlyphRangesChineseSimplifiedCommon();// Default + Half-Width + Japanese Hiragana/Katakana + set of 2500 CJK Unified Ideographs for common simplified Chinese
	/** 
	*  Default + about 400 Cyrillic characters
	*/
    // IMGUI_API const ImWchar*    GetGlyphRangesCyrillic();
	/** 
	*  Default + Thai characters
	*/
    // IMGUI_API const ImWchar*    GetGlyphRangesThai();
	/** 
	*  Default + Vietname characters
	*/
    // IMGUI_API const ImWchar*    GetGlyphRangesVietnamese();

    //-------------------------------------------
    // [BETA] Custom Rectangles/Glyphs API
    //-------------------------------------------

	// You can request arbitrary rectangles to be packed into the atlas, for your own purposes.
    // After calling Build(), you can query the rectangle position and render your pixels.
    // You can also request your rectangles to be mapped as font glyph (given a font + Unicode point),
    // so you can render e.g. custom colorful icons and use them as regular glyphs.
    // Read docs/FONTS.md for more details about using colorful icons.
    // Note: this API may be redesigned later in order to support multi-monitor varying DPI settings.

    IMGUI_API int       AddCustomRectRegular(int width, int height);

    IMGUI_API int       AddCustomRectFontGlyph(ImFont* font, ImWchar id, int width, int height, float advance_x, const ImVec2& offset = ImVec2(0,0));

    ImFontAtlasCustomRect*   GetCustomRectByIndex(int index);
	
    //-------------------------------------------
    // Members
    //-------------------------------------------

	/** 
	*  Marked as Locked by ImGui::NewFrame() so attempt to modify the atlas will assert.
	*/
    bool                        Locked;
	/** 
	*  Build flags (see ImFontAtlasFlags_)
	*/
    ImFontAtlasFlags            Flags;
	/** 
	*  User data to refer to the texture once it has been uploaded to user's graphic systems. It is passed back to you during rendering via the ImDrawCmd structure.
	*/
    //ImTextureID                 TexID;
	/** 
	*  Texture width desired by user before Build(). Must be a power-of-two. If have many glyphs your graphics API have texture size restrictions you may want to increase texture width to decrease height.
	*/
    int                         TexDesiredWidth;
	/** 
	*  Padding between glyphs within texture in pixels. Defaults to 1. If your rendering method doesn't rely on bilinear filtering you may set this to 0.
	*/
    int                         TexGlyphPadding;
};

/**
* Font runtime data and rendering
* ImFontAtlas automatically loads a default embedded font for you when you call GetTexDataAsAlpha8() or GetTexDataAsRGBA32().
*/
class ImFont
{
public:

	// needs manual binding
	/** 
	*  Sparse. Glyphs->AdvanceX in a directly indexable way (cache-friendly for CalcTextSize functions which only this this info, and are often bottleneck in large UI).
	* out
	* 12-16
	*/
    //ImVector<float>             IndexAdvanceX;
	
	/** 
	* = FallbackGlyph->AdvanceX
	* out
	* 4
	*/
    float                       FallbackAdvanceX;
	/** 
	*  Height of characters/line, set during loading (don't change after loading)
	* in
	* 4
	*/
    float                       FontSize;
	// needs manual binding
	/** 
	*  Sparse. Index glyphs by Unicode code-point.
	* out
	* 12-16
	*/
    //ImVector<ImWchar>           IndexLookup;
	// needs manual binding
	/** 
	*  All glyphs.
	* out
	* 12-16
	*/
    //ImVector<ImFontGlyph>       Glyphs;
	/** 
	* = FindGlyph(FontFallbackChar)
	* out
	* 4-8
	*/
    const ImFontGlyph*          FallbackGlyph;
	
	/** 
	*  What we has been loaded into
	* out
	* 4-8
	*/
    ImFontAtlas*                ContainerAtlas;
	/** 
	*  Pointer within ContainerAtlas->ConfigData
	* in
	* 4-8
	*/
    const ImFontConfig*         ConfigData;
	/** 
	*  Number of ImFontConfig involved in creating this font. Bigger than 1 when merging multiple font sources into one ImFont.
	*  ~ 1        
	* in
	* 2
	*/
    short                       ConfigDataCount;
	/** 
	*  Replacement glyph if one isn't found. Only set via SetFallbackChar()
	*  = '?'      
	* in
	* 2
	*/
    ImWchar                     FallbackChar;
	/**
	*  Explicitly specify unicode codepoint of ellipsis character. When fonts are being merged first specified ellipsis will be used.
	* -1
	*/
	ImWchar                     EllipsisChar;
	/** 
	* out
	* 1
	*/
    bool                        DirtyLookupTables;
	/** 
	*  Base font scale, multiplied by the per-window font scale which you can adjust with SetWindowFontScale()
	*  = 1.f      
	* in
	* 4
	*/
    float                       Scale;
	/** 
	*  Ascent: distance from top to bottom of e.g. 'A' [0..FontSize]
	* out
	* 4+4
	*/
    float                       Ascent, Descent;
	/** 
	*  Total surface in pixels to get an idea of the font rasterization/texture cost (not exact, we approximate the cost of padding between glyphs)
	* out
	* 4
	*/
    int                         MetricsTotalSurface;

    // Methods
    const ImFontGlyph*FindGlyph(ImWchar c) const;
    const ImFontGlyph*FindGlyphNoFallback(ImWchar c) const;
    float                       GetCharAdvance(ImWchar c) const;
    bool                        IsLoaded() const;
	/** 
	*  'max_width' stops rendering after a certain width (could be turned into a 2d size). FLT_MAX to disable.
	*/
    const char*                 GetDebugName() const;
    // 'wrap_width' enable automatic word-wrapping across multiple lines to fit into given width. 0.0f to disable.
	
	// needs manual binding
	/** 
	*  utf8
	*/
    //IMGUI_API ImVec2            CalcTextSizeA(float size, float max_width, float wrap_width, const char* text_begin, const char* text_end = NULL, const char** remaining = NULL) const;
    //IMGUI_API const char*       CalcWordWrapPositionA(float scale, const char* text, const char* text_end, float wrap_width) const;

};

}
