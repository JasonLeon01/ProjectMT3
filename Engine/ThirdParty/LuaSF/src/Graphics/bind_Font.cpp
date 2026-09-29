#include "Graphics/bind_Font.hpp"

#include <algorithm>
#include <array>
#include <string_view>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace { constexpr std::array<std::string_view, 27> docs = {
    "\\brief Class for loading and manipulating character fonts",
    "\\brief Default constructor\n\nConstruct an empty font that does not contain any glyphs.",
    "\\brief Construct the font from a file\n\nThe supported font formats are: TrueType, Type 1, CFF,\nOpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.\nNote that this function knows nothing about the standard\nfonts installed on the user's system, thus you can't\nload them directly.\n\n\\warning SFML cannot preload all the font data in this\nfunction, so the file has to remain accessible until\nthe `sf::Font` object opens a new font or is destroyed.\n\n\\param filename Path of the font file to open\n\n\\throws sf::Exception if opening was unsuccessful\n\n\\see `openFromFile`, `openFromMemory`, `openFromStream`",
    "\\brief Construct the font from a file in memory\n\nThe supported font formats are: TrueType, Type 1, CFF,\nOpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.\n\n\\warning SFML cannot preload all the font data in this\nfunction, so the buffer pointed by `data` has to remain\nvalid until the `sf::Font` object opens a new font or\nis destroyed.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `openFromFile`, `openFromMemory`, `openFromStream`",
    "\\brief Construct the font from a custom stream\n\nThe supported font formats are: TrueType, Type 1, CFF,\nOpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.\nWarning: SFML cannot preload all the font data in this\nfunction, so the contents of `stream` have to remain\nvalid as long as the font is used.\n\n\\warning SFML cannot preload all the font data in this\nfunction, so the stream has to remain accessible until\nthe `sf::Font` object opens a new font or is destroyed.\n\n\\param stream Source stream to read from\n\n\\throws sf::Exception if loading was unsuccessful\n\n\\see `openFromFile`, `openFromMemory`, `openFromStream`",
    "\\brief Open the font from a file\n\nThe supported font formats are: TrueType, Type 1, CFF,\nOpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.\nNote that this function knows nothing about the standard\nfonts installed on the user's system, thus you can't\nload them directly.\n\n\\warning SFML cannot preload all the font data in this\nfunction, so the file has to remain accessible until\nthe `sf::Font` object opens a new font or is destroyed.\n\n\\param filename Path of the font file to load\n\n\\return `true` if opening succeeded, `false` if it failed\n\n\\see `openFromMemory`, `openFromStream`",
    "\\brief Open the font from a file in memory\n\nThe supported font formats are: TrueType, Type 1, CFF,\nOpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.\n\n\\warning SFML cannot preload all the font data in this\nfunction, so the buffer pointed by `data` has to remain\nvalid until the `sf::Font` object opens a new font or\nis destroyed.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\return `true` if opening succeeded, `false` if it failed\n\n\\see `openFromFile`, `openFromStream`",
    "\\brief Open the font from a custom stream\n\nThe supported font formats are: TrueType, Type 1, CFF,\nOpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.\n\n\\warning SFML cannot preload all the font data in this\nfunction, so the stream has to remain accessible until\nthe `sf::Font` object opens a new font or is destroyed.\n\n\\param stream Source stream to read from\n\n\\return `true` if opening succeeded, `false` if it failed\n\n\\see `openFromFile`, `openFromMemory`",
    "\\brief Get the font information\n\n\\return A structure that holds the font information",
    "\\brief Retrieve a glyph of the font by glyph ID\n\nIf the font is a bitmap font, not all character sizes\nmight be available. If the glyph is not available at the\nrequested size, an empty glyph is returned.\n\nThis function is only useful for getting the glyphs\nreturned in the data from calling `shape`.\n\nBe aware that using a negative value for the outline\nthickness will cause distorted rendering.\n\n\\param id               ID of the glyph to get\n\\param characterSize    Reference character size\n\\param bold             Retrieve the bold version or the regular one?\n\\param outlineThickness Thickness of outline (when != 0 the glyph will not be filled)\n\n\\return The glyph corresponding to `id` and `characterSize`",
    "\\brief Retrieve a glyph of the font\n\nIf the font is a bitmap font, not all character sizes\nmight be available. If the glyph is not available at the\nrequested size, an empty glyph is returned.\n\nYou may want to use `hasGlyph` to determine if the\nglyph exists before requesting it. If the glyph does not\nexist, a font specific default is returned.\n\nBe aware that using a negative value for the outline\nthickness will cause distorted rendering.\n\n\\param codePoint        Unicode code point of the character to get\n\\param characterSize    Reference character size\n\\param bold             Retrieve the bold version or the regular one?\n\\param outlineThickness Thickness of outline (when != 0 the glyph will not be filled)\n\n\\return The glyph corresponding to `codePoint` and `characterSize`",
    "\\brief Determine if this font has a glyph representing the requested code point\n\nMost fonts only include a very limited selection of glyphs from\nspecific Unicode subsets, like Latin, Cyrillic, or Asian characters.\n\nWhile code points without representation will return a font specific\ndefault character, it might be useful to verify whether specific\ncode points are included to determine whether a font is suited\nto display text in a specific language.\n\n\\param codePoint Unicode code point to check\n\n\\return `true` if the codepoint has a glyph representation, `false` otherwise",
    "\\brief Get the kerning offset of two glyphs\n\n\\deprecated Use the `getKerning(char32_t, char32_t, unsigned int, bool)` overload instead.\n\nThe kerning is an extra offset (negative) to apply between two\nglyphs when rendering them, to make the pair look more \"natural\".\nFor example, the pair \"AV\" have a special kerning to make them\ncloser than other characters. Most of the glyphs pairs have a\nkerning offset of zero, though.\n\n\\param first         Unicode code point of the first character\n\\param second        Unicode code point of the second character\n\\param characterSize Reference character size\n\\param bold          Retrieve the bold version or the regular one?\n\n\\return Kerning value for `first` and `second`, in pixels",
    "\\brief Get the kerning offset of two glyphs\n\nThe kerning is an extra offset (negative) to apply between two\nglyphs when rendering them, to make the pair look more \"natural\".\nFor example, the pair \"AV\" have a special kerning to make them\ncloser than other characters. Most of the glyphs pairs have a\nkerning offset of zero, though.\n\n\\param first         Unicode code point of the first character\n\\param second        Unicode code point of the second character\n\\param characterSize Reference character size\n\\param bold          Retrieve the bold version or the regular one?\n\n\\return Kerning value for `first` and `second`, in pixels",
    "\\brief Get the ascent\n\nThe ascent is the largest distance between the baseline and\nthe top of all glyphs in the font.\n\nBe aware that there is no uniform definition of how the\nascent is calculated. It can vary from font to font.\n\n\\param characterSize Reference character size\n\n\\return Ascent, in pixels",
    "\\brief Get the descent\n\nThe descent is the largest distance between the baseline and\nthe bottom of all glyphs in the font.\n\nBe aware that there is no uniform definition of how the\ndescent is calculated. It can vary from font to font.\n\nThe descent shares the same coordinate system as the\nascent. This means that it will be negative for distances\nbelow the baseline.\n\n\\param characterSize Reference character size\n\n\\return Descent, in pixels",
    "\\brief Get the line spacing\n\nLine spacing is the vertical offset to apply between two\nconsecutive lines of text.\n\n\\param characterSize Reference character size\n\n\\return Line spacing, in pixels",
    "\\brief Get the position of the underline\n\nUnderline position is the vertical offset to apply between the\nbaseline and the underline.\n\n\\param characterSize Reference character size\n\n\\return Underline position, in pixels\n\n\\see `getUnderlineThickness`",
    "\\brief Get the thickness of the underline\n\nUnderline thickness is the vertical size of the underline.\n\n\\param characterSize Reference character size\n\n\\return Underline thickness, in pixels\n\n\\see `getUnderlinePosition`",
    "\\brief Retrieve the texture containing the loaded glyphs of a certain size\n\nThe contents of the returned texture changes as more glyphs\nare requested, thus it is not very relevant. It is mainly\nused internally by `sf::Text`.\n\n\\param characterSize Reference character size\n\n\\return Texture containing the glyphs of the requested size",
    "\\brief Enable or disable the smooth filter\n\nWhen the filter is activated, the font appears smoother\nso that pixels are less noticeable. However if you want\nthe font to look exactly the same as its source file,\nyou should disable it.\nThe smooth filter is enabled by default.\n\n\\param smooth `true` to enable smoothing, `false` to disable it\n\n\\see `isSmooth`",
    "\\brief Tell whether the smooth filter is enabled or not\n\n\\return `true` if smoothing is enabled, `false` if it is disabled\n\n\\see `setSmooth`",
    "\\brief Holds various information about a font",
    "A unique ID that identifies the font",
    "The font family",
    "Has kerning information",
    "Has native vertical metrics",
}; }

void bind_Font(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Font = lua_glue::BindClass<sf::Font>(sf, "Font");
    lua_glue::Table table_sf__Font = sf["Font"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Font>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Font");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Font", "new", "fun(filename: string): sf.Font");
    LUASF_STUB_OVERLOAD("sf.Font", "new", "fun(stream: sf.InputStream): sf.Font");
    LUASF_STUB_OVERLOAD("sf.Font", "new", "fun(): sf.Font");
    LUASF_STUB_OVERLOAD("sf.Font", "new", "fun(data: any): sf.Font");
    lua_glue::BindCallable(type_sf__Font, "new",
        [](std::string filename) {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::Font>(std::filesystem::path(filename)));
        },
        docs[2]
    );
    lua_glue::BindCallable(type_sf__Font, "new",
        [](lua_glue::Object stream) {
            auto& stream_ref = stream.as<sf::InputStream&>();
            auto object = lua_sf::makeLongLivedMemoryObject<sf::Font>(stream_ref);
            lua_sf::rememberLongLivedStream(*object, std::move(stream));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        },
        docs[4]
    );
    lua_glue::BindCallable(type_sf__Font, "new",
        []() {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::Font>());
        },
        docs[1]
    );
    lua_glue::BindCallable(type_sf__Font, "new",
        [](lua_glue::Object data) {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            auto object = lua_sf::makeLongLivedMemoryObject<sf::Font>();
            if (!object->openFromMemory(data_buffer->data(), static_cast<std::size_t>(data_buffer->size())))
                throw std::runtime_error("Failed to open sf.Font from memory");
            lua_sf::rememberLongLivedMemory(*object, std::move(data_buffer));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Font", "openFromFile", "fun(self: sf.Font, filename: string): boolean");
    lua_glue::BindCallable(type_sf__Font, "openFromFile",
        [](sf::Font& self, std::string filename) -> bool {
            auto result = self.openFromFile(std::filesystem::path(filename));
            if (result)
                lua_sf::releaseLongLivedResources(self);
            return result;
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Font", "openFromMemory", "fun(self: sf.Font, data: any): boolean");
    lua_glue::BindCallable(type_sf__Font, "openFromMemory",
        [](sf::Font& self, lua_glue::Object data) -> bool {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            const bool result = self.openFromMemory(data_buffer->data(), static_cast<std::size_t>(data_buffer->size()));
            if (result)
            {
                lua_sf::releaseLongLivedStream(self);
                lua_sf::rememberLongLivedMemory(self, std::move(data_buffer));
            }
            return result;
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Font", "openFromStream", "fun(self: sf.Font, stream: sf.InputStream): boolean");
    lua_glue::BindCallable(type_sf__Font, "openFromStream",
        [](sf::Font& self, lua_glue::Object stream) -> bool {
            auto& stream_ref = stream.as<sf::InputStream&>();
            const bool result = self.openFromStream(stream_ref);
            if (result)
            {
                lua_sf::releaseLongLivedMemory(self);
                lua_sf::rememberLongLivedStream(self, std::move(stream));
            }
            return result;
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Font", "getInfo", "fun(self: sf.Font): sf.Font.Info");
    lua_glue::BindCallable(type_sf__Font, "getInfo",
        [](const sf::Font& self) {
            return std::cref(self.getInfo());
        },
        docs[8],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Font", "getGlyphById", "fun(self: sf.Font, id: integer, characterSize: integer, bold: boolean, outlineThickness?: number): sf.Glyph");
    lua_glue::BindCallable(type_sf__Font, "getGlyphById",
        [](const sf::Font& self, lua_sf::LuaIntegral<std::uint32_t> id, lua_sf::LuaIntegral<unsigned int> characterSize, bool bold, float outlineThickness) {
            return std::cref(self.getGlyphById(id.value(), characterSize.value(), bold, outlineThickness));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<float>(0);
        }}},
        docs[9],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Font", "getGlyph", "fun(self: sf.Font, codePoint: integer, characterSize: integer, bold: boolean, outlineThickness?: number): sf.Glyph");
    lua_glue::BindCallable(type_sf__Font, "getGlyph",
        [](const sf::Font& self, lua_sf::LuaIntegral<char32_t> codePoint, lua_sf::LuaIntegral<unsigned int> characterSize, bool bold, float outlineThickness) {
            return std::cref(self.getGlyph(codePoint.value(), characterSize.value(), bold, outlineThickness));
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<float>(0);
        }}},
        docs[10],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Font", "hasGlyph", "fun(self: sf.Font, codePoint: integer): boolean");
    lua_glue::BindCallable(type_sf__Font, "hasGlyph",
        [](const sf::Font& self, lua_sf::LuaIntegral<char32_t> codePoint) -> bool {
            return self.hasGlyph(codePoint.value());
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Font", "getKerning", "fun(self: sf.Font, first: integer, second: integer, characterSize: integer, bold?: boolean): number");
    LUASF_STUB_OVERLOAD("sf.Font", "getKerning", "fun(self: sf.Font, first: integer, second: integer, characterSize: integer, bold?: boolean): number");
    lua_glue::BindCallable(type_sf__Font, "getKerning",
        [](const sf::Font& self, lua_sf::LuaIntegral<std::uint32_t> first, lua_sf::LuaIntegral<std::uint32_t> second, lua_sf::LuaIntegral<unsigned int> characterSize, bool bold) -> float {
            return self.getKerning(first.value(), second.value(), characterSize.value(), bold);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[12]
    );
    lua_glue::BindCallable(type_sf__Font, "getKerning",
        [](const sf::Font& self, lua_sf::LuaIntegral<char32_t> first, lua_sf::LuaIntegral<char32_t> second, lua_sf::LuaIntegral<unsigned int> characterSize, bool bold) -> float {
            return self.getKerning(first.value(), second.value(), characterSize.value(), bold);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<bool>(false);
        }}},
        docs[13]
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Font", "getAscent", "fun(self: sf.Font, characterSize: integer): number");
    lua_glue::BindCallable(type_sf__Font, "getAscent",
        [](const sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
            return self.getAscent(characterSize.value());
        },
        docs[14]
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.Font", "getDescent", "fun(self: sf.Font, characterSize: integer): number");
    lua_glue::BindCallable(type_sf__Font, "getDescent",
        [](const sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
            return self.getDescent(characterSize.value());
        },
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Font", "getLineSpacing", "fun(self: sf.Font, characterSize: integer): number");
    lua_glue::BindCallable(type_sf__Font, "getLineSpacing",
        [](const sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
            return self.getLineSpacing(characterSize.value());
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.Font", "getUnderlinePosition", "fun(self: sf.Font, characterSize: integer): number");
    lua_glue::BindCallable(type_sf__Font, "getUnderlinePosition",
        [](const sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
            return self.getUnderlinePosition(characterSize.value());
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.Font", "getUnderlineThickness", "fun(self: sf.Font, characterSize: integer): number");
    lua_glue::BindCallable(type_sf__Font, "getUnderlineThickness",
        [](const sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
            return self.getUnderlineThickness(characterSize.value());
        },
        docs[18]
    );
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.Font", "getTexture", "fun(self: sf.Font, characterSize: integer): sf.Texture");
    lua_glue::BindCallable(type_sf__Font, "getTexture",
        [](const sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) {
            return std::cref(self.getTexture(characterSize.value()));
        },
        docs[19],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.Font", "setSmooth", "fun(self: sf.Font, smooth: boolean)");
    lua_glue::BindCallable(type_sf__Font, "setSmooth",
        [](sf::Font& self, bool smooth) {
            self.setSmooth(smooth);
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Font", "isSmooth", "fun(self: sf.Font): boolean");
    lua_glue::BindCallable(type_sf__Font, "isSmooth",
        [](const sf::Font& self) -> bool {
            return self.isSmooth();
        },
        docs[21]
    );
    auto type_sf__Font__Info = lua_glue::BindClass<sf::Font::Info>(table_sf__Font, "Info");
    lua_glue::Table table_sf__Font__Info = table_sf__Font["Info"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Font::Info>(lua);
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_CLASS("sf.Font.Info");
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FIELD("id", "integer");
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FIELD("family", "string");
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FIELD("hasKerning", "boolean");
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FIELD("hasVerticalMetrics", "boolean");
    LUASF_STUB_FUNCTION("sf.Font.Info", "new", "fun(): sf.Font.Info");
    lua_glue::BindCallable(type_sf__Font__Info, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Font::Info>();
        }
    );
    lua_glue::BindProperty(type_sf__Font__Info, "id",
        [](const sf::Font::Info& self) {
            return self.id;
        },
        [](sf::Font::Info& self, lua_sf::LuaIntegral<std::uint64_t> value) {
            self.id = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Font__Info, "family",
        [](const sf::Font::Info& self) {
            return std::string(self.family);
        },
        [](sf::Font::Info& self, std::string value) {
            self.family = value;
        }
    );
    lua_glue::BindAttr<bool>(type_sf__Font__Info, "hasKerning", &sf::Font::Info::hasKerning);
    lua_glue::BindAttr<bool>(type_sf__Font__Info, "hasVerticalMetrics", &sf::Font::Info::hasVerticalMetrics);
}
