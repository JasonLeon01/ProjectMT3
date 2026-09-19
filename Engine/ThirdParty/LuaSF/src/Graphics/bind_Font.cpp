#include "Graphics/bind_Font.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_Font(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__Font = sf.new_usertype<sf::Font>("Font", sol::no_constructor);
    sol::table table_sf__Font = sf["Font"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Font>(lua);
    LUASF_STUB_DOC("\\brief Class for loading and manipulating character fonts");
    LUASF_STUB_CLASS("sf.Font");
    LUASF_STUB_DOC("\\brief Construct the font from a file\n\nThe supported font formats are: TrueType, Type 1, CFF,\nOpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.\nNote that this function knows nothing about the standard\nfonts installed on the user's system, thus you can't\nload them directly.\n\n\\warning SFML cannot preload all the font data in this\nfunction, so the file has to remain accessible until\nthe `sf::Font` object opens a new font or is destroyed.\n\n\\param filename Path of the font file to open\n\n\\throws sf::Exception if opening was unsuccessful\n\n\\see `openFromFile`, `openFromMemory`, `openFromStream`");
    LUASF_STUB_FUNCTION("sf.Font", "new", "fun(filename: string): sf.Font");
    LUASF_STUB_OVERLOAD("sf.Font", "new", "fun(stream: sf.InputStream): sf.Font");
    LUASF_STUB_OVERLOAD("sf.Font", "new", "fun(): sf.Font");
    LUASF_STUB_OVERLOAD("sf.Font", "new", "fun(data: any): sf.Font");
    type_sf__Font.set_function("new", sol::factories(
        [](std::string filename) {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::Font>(std::filesystem::path(filename)));
        },
        [](sol::object stream) {
            auto& stream_ref = stream.as<sf::InputStream&>();
            auto object = lua_sf::makeLongLivedMemoryObject<sf::Font>(stream_ref);
            lua_sf::rememberLongLivedStream(*object, std::move(stream));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        },
        []() {
            return lua_sf::wrapLuaSharedObject(lua_sf::makeLongLivedMemoryObject<sf::Font>());
        },
        [](sol::object data) {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            auto object = lua_sf::makeLongLivedMemoryObject<sf::Font>();
            if (!object->openFromMemory(data_buffer->data(), static_cast<std::size_t>(data_buffer->size())))
                throw std::runtime_error("Failed to open sf.Font from memory");
            lua_sf::rememberLongLivedMemory(*object, std::move(data_buffer));
            return lua_sf::wrapLuaSharedObject(std::move(object));
        }
    ));
    LUASF_STUB_DOC("\\brief Open the font from a file\n\nThe supported font formats are: TrueType, Type 1, CFF,\nOpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.\nNote that this function knows nothing about the standard\nfonts installed on the user's system, thus you can't\nload them directly.\n\n\\warning SFML cannot preload all the font data in this\nfunction, so the file has to remain accessible until\nthe `sf::Font` object opens a new font or is destroyed.\n\n\\param filename Path of the font file to load\n\n\\return `true` if opening succeeded, `false` if it failed\n\n\\see `openFromMemory`, `openFromStream`");
    LUASF_STUB_FUNCTION("sf.Font", "openFromFile", "fun(self: sf.Font, filename: string): boolean");
    type_sf__Font.set_function("openFromFile",
        [](sf::Font& self, std::string filename) -> bool {
            auto result = self.openFromFile(std::filesystem::path(filename));
            if (result)
                lua_sf::releaseLongLivedResources(self);
            return result;
        }
    );
    LUASF_STUB_DOC("\\brief Open the font from a file in memory\n\nThe supported font formats are: TrueType, Type 1, CFF,\nOpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.\n\n\\warning SFML cannot preload all the font data in this\nfunction, so the buffer pointed by `data` has to remain\nvalid until the `sf::Font` object opens a new font or\nis destroyed.\n\n\\param data        Pointer to the file data in memory\n\\param sizeInBytes Size of the data to load, in bytes\n\n\\return `true` if opening succeeded, `false` if it failed\n\n\\see `openFromFile`, `openFromStream`");
    LUASF_STUB_FUNCTION("sf.Font", "openFromMemory", "fun(self: sf.Font, data: any): boolean");
    type_sf__Font.set_function("openFromMemory",
        [](sf::Font& self, sol::object data) -> bool {
            auto data_buffer = lua_sf::makeLongLivedMemoryBuffer(data);
            const bool result = self.openFromMemory(data_buffer->data(), static_cast<std::size_t>(data_buffer->size()));
            if (result)
            {
                lua_sf::releaseLongLivedStream(self);
                lua_sf::rememberLongLivedMemory(self, std::move(data_buffer));
            }
            return result;
        }
    );
    LUASF_STUB_DOC("\\brief Open the font from a custom stream\n\nThe supported font formats are: TrueType, Type 1, CFF,\nOpenType, SFNT, X11 PCF, Windows FNT, BDF, PFR and Type 42.\n\n\\warning SFML cannot preload all the font data in this\nfunction, so the stream has to remain accessible until\nthe `sf::Font` object opens a new font or is destroyed.\n\n\\param stream Source stream to read from\n\n\\return `true` if opening succeeded, `false` if it failed\n\n\\see `openFromFile`, `openFromMemory`");
    LUASF_STUB_FUNCTION("sf.Font", "openFromStream", "fun(self: sf.Font, stream: sf.InputStream): boolean");
    type_sf__Font.set_function("openFromStream",
        [](sf::Font& self, sol::object stream) -> bool {
            auto& stream_ref = stream.as<sf::InputStream&>();
            const bool result = self.openFromStream(stream_ref);
            if (result)
            {
                lua_sf::releaseLongLivedMemory(self);
                lua_sf::rememberLongLivedStream(self, std::move(stream));
            }
            return result;
        }
    );
    LUASF_STUB_DOC("\\brief Get the font information\n\n\\return A structure that holds the font information");
    LUASF_STUB_FUNCTION("sf.Font", "getInfo", "fun(self: sf.Font): sf.Font.Info");
    type_sf__Font.set_function("getInfo",
        sol::policies(
            [](sf::Font& self) {
                return std::cref(self.getInfo());
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Retrieve a glyph of the font by glyph ID\n\nIf the font is a bitmap font, not all character sizes\nmight be available. If the glyph is not available at the\nrequested size, an empty glyph is returned.\n\nThis function is only useful for getting the glyphs\nreturned in the data from calling `shape`.\n\nBe aware that using a negative value for the outline\nthickness will cause distorted rendering.\n\n\\param id               ID of the glyph to get\n\\param characterSize    Reference character size\n\\param bold             Retrieve the bold version or the regular one?\n\\param outlineThickness Thickness of outline (when != 0 the glyph will not be filled)\n\n\\return The glyph corresponding to `id` and `characterSize`");
    LUASF_STUB_FUNCTION("sf.Font", "getGlyphById", "fun(self: sf.Font, id: integer, characterSize: integer, bold: boolean, outlineThickness: number): sf.Glyph");
    LUASF_STUB_OVERLOAD("sf.Font", "getGlyphById", "fun(self: sf.Font, id: integer, characterSize: integer, bold: boolean): sf.Glyph");
    type_sf__Font.set_function("getGlyphById",
        sol::policies(
            sol::overload(
                [](sf::Font& self, lua_sf::LuaIntegral<std::uint32_t> id, lua_sf::LuaIntegral<unsigned int> characterSize, bool bold, float outlineThickness) {
                    return std::cref(self.getGlyphById(id.value(), characterSize.value(), bold, outlineThickness));
                },
                [](sf::Font& self, lua_sf::LuaIntegral<std::uint32_t> id, lua_sf::LuaIntegral<unsigned int> characterSize, bool bold) {
                    return std::cref(self.getGlyphById(id.value(), characterSize.value(), bold));
                }
            ),
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Retrieve a glyph of the font\n\nIf the font is a bitmap font, not all character sizes\nmight be available. If the glyph is not available at the\nrequested size, an empty glyph is returned.\n\nYou may want to use `hasGlyph` to determine if the\nglyph exists before requesting it. If the glyph does not\nexist, a font specific default is returned.\n\nBe aware that using a negative value for the outline\nthickness will cause distorted rendering.\n\n\\param codePoint        Unicode code point of the character to get\n\\param characterSize    Reference character size\n\\param bold             Retrieve the bold version or the regular one?\n\\param outlineThickness Thickness of outline (when != 0 the glyph will not be filled)\n\n\\return The glyph corresponding to `codePoint` and `characterSize`");
    LUASF_STUB_FUNCTION("sf.Font", "getGlyph", "fun(self: sf.Font, codePoint: integer, characterSize: integer, bold: boolean, outlineThickness: number): sf.Glyph");
    LUASF_STUB_OVERLOAD("sf.Font", "getGlyph", "fun(self: sf.Font, codePoint: integer, characterSize: integer, bold: boolean): sf.Glyph");
    type_sf__Font.set_function("getGlyph",
        sol::policies(
            sol::overload(
                [](sf::Font& self, lua_sf::LuaIntegral<char32_t> codePoint, lua_sf::LuaIntegral<unsigned int> characterSize, bool bold, float outlineThickness) {
                    return std::cref(self.getGlyph(codePoint.value(), characterSize.value(), bold, outlineThickness));
                },
                [](sf::Font& self, lua_sf::LuaIntegral<char32_t> codePoint, lua_sf::LuaIntegral<unsigned int> characterSize, bool bold) {
                    return std::cref(self.getGlyph(codePoint.value(), characterSize.value(), bold));
                }
            ),
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Determine if this font has a glyph representing the requested code point\n\nMost fonts only include a very limited selection of glyphs from\nspecific Unicode subsets, like Latin, Cyrillic, or Asian characters.\n\nWhile code points without representation will return a font specific\ndefault character, it might be useful to verify whether specific\ncode points are included to determine whether a font is suited\nto display text in a specific language.\n\n\\param codePoint Unicode code point to check\n\n\\return `true` if the codepoint has a glyph representation, `false` otherwise");
    LUASF_STUB_FUNCTION("sf.Font", "hasGlyph", "fun(self: sf.Font, codePoint: integer): boolean");
    type_sf__Font.set_function("hasGlyph",
        [](sf::Font& self, lua_sf::LuaIntegral<char32_t> codePoint) -> bool {
            return self.hasGlyph(codePoint.value());
        }
    );
    LUASF_STUB_DOC("\\brief Get the kerning offset of two glyphs\n\n\\deprecated Use the `getKerning(char32_t, char32_t, unsigned int, bool)` overload instead.\n\nThe kerning is an extra offset (negative) to apply between two\nglyphs when rendering them, to make the pair look more \"natural\".\nFor example, the pair \"AV\" have a special kerning to make them\ncloser than other characters. Most of the glyphs pairs have a\nkerning offset of zero, though.\n\n\\param first         Unicode code point of the first character\n\\param second        Unicode code point of the second character\n\\param characterSize Reference character size\n\\param bold          Retrieve the bold version or the regular one?\n\n\\return Kerning value for `first` and `second`, in pixels");
    LUASF_STUB_FUNCTION("sf.Font", "getKerning", "fun(self: sf.Font, first: integer, second: integer, characterSize: integer, bold: boolean): number");
    LUASF_STUB_OVERLOAD("sf.Font", "getKerning", "fun(self: sf.Font, first: integer, second: integer, characterSize: integer, bold: boolean): number");
    LUASF_STUB_OVERLOAD("sf.Font", "getKerning", "fun(self: sf.Font, first: integer, second: integer, characterSize: integer): number");
    LUASF_STUB_OVERLOAD("sf.Font", "getKerning", "fun(self: sf.Font, first: integer, second: integer, characterSize: integer): number");
    type_sf__Font.set_function("getKerning",
        sol::overload(
            [](sf::Font& self, lua_sf::LuaIntegral<std::uint32_t> first, lua_sf::LuaIntegral<std::uint32_t> second, lua_sf::LuaIntegral<unsigned int> characterSize, bool bold) -> float {
                return self.getKerning(first.value(), second.value(), characterSize.value(), bold);
            },
            [](sf::Font& self, lua_sf::LuaIntegral<char32_t> first, lua_sf::LuaIntegral<char32_t> second, lua_sf::LuaIntegral<unsigned int> characterSize, bool bold) -> float {
                return self.getKerning(first.value(), second.value(), characterSize.value(), bold);
            },
            [](sf::Font& self, lua_sf::LuaIntegral<std::uint32_t> first, lua_sf::LuaIntegral<std::uint32_t> second, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
                return self.getKerning(first.value(), second.value(), characterSize.value());
            },
            [](sf::Font& self, lua_sf::LuaIntegral<char32_t> first, lua_sf::LuaIntegral<char32_t> second, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
                return self.getKerning(first.value(), second.value(), characterSize.value());
            }
        )
    );
    LUASF_STUB_DOC("\\brief Get the ascent\n\nThe ascent is the largest distance between the baseline and\nthe top of all glyphs in the font.\n\nBe aware that there is no uniform definition of how the\nascent is calculated. It can vary from font to font.\n\n\\param characterSize Reference character size\n\n\\return Ascent, in pixels");
    LUASF_STUB_FUNCTION("sf.Font", "getAscent", "fun(self: sf.Font, characterSize: integer): number");
    type_sf__Font.set_function("getAscent",
        [](sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
            return self.getAscent(characterSize.value());
        }
    );
    LUASF_STUB_DOC("\\brief Get the descent\n\nThe descent is the largest distance between the baseline and\nthe bottom of all glyphs in the font.\n\nBe aware that there is no uniform definition of how the\ndescent is calculated. It can vary from font to font.\n\nThe descent shares the same coordinate system as the\nascent. This means that it will be negative for distances\nbelow the baseline.\n\n\\param characterSize Reference character size\n\n\\return Descent, in pixels");
    LUASF_STUB_FUNCTION("sf.Font", "getDescent", "fun(self: sf.Font, characterSize: integer): number");
    type_sf__Font.set_function("getDescent",
        [](sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
            return self.getDescent(characterSize.value());
        }
    );
    LUASF_STUB_DOC("\\brief Get the line spacing\n\nLine spacing is the vertical offset to apply between two\nconsecutive lines of text.\n\n\\param characterSize Reference character size\n\n\\return Line spacing, in pixels");
    LUASF_STUB_FUNCTION("sf.Font", "getLineSpacing", "fun(self: sf.Font, characterSize: integer): number");
    type_sf__Font.set_function("getLineSpacing",
        [](sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
            return self.getLineSpacing(characterSize.value());
        }
    );
    LUASF_STUB_DOC("\\brief Get the position of the underline\n\nUnderline position is the vertical offset to apply between the\nbaseline and the underline.\n\n\\param characterSize Reference character size\n\n\\return Underline position, in pixels\n\n\\see `getUnderlineThickness`");
    LUASF_STUB_FUNCTION("sf.Font", "getUnderlinePosition", "fun(self: sf.Font, characterSize: integer): number");
    type_sf__Font.set_function("getUnderlinePosition",
        [](sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
            return self.getUnderlinePosition(characterSize.value());
        }
    );
    LUASF_STUB_DOC("\\brief Get the thickness of the underline\n\nUnderline thickness is the vertical size of the underline.\n\n\\param characterSize Reference character size\n\n\\return Underline thickness, in pixels\n\n\\see `getUnderlinePosition`");
    LUASF_STUB_FUNCTION("sf.Font", "getUnderlineThickness", "fun(self: sf.Font, characterSize: integer): number");
    type_sf__Font.set_function("getUnderlineThickness",
        [](sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) -> float {
            return self.getUnderlineThickness(characterSize.value());
        }
    );
    LUASF_STUB_DOC("\\brief Retrieve the texture containing the loaded glyphs of a certain size\n\nThe contents of the returned texture changes as more glyphs\nare requested, thus it is not very relevant. It is mainly\nused internally by `sf::Text`.\n\n\\param characterSize Reference character size\n\n\\return Texture containing the glyphs of the requested size");
    LUASF_STUB_FUNCTION("sf.Font", "getTexture", "fun(self: sf.Font, characterSize: integer): sf.Texture");
    type_sf__Font.set_function("getTexture",
        sol::policies(
            [](sf::Font& self, lua_sf::LuaIntegral<unsigned int> characterSize) {
                return std::cref(self.getTexture(characterSize.value()));
            },
            sol::self_dependency{}
        )
    );
    LUASF_STUB_DOC("\\brief Enable or disable the smooth filter\n\nWhen the filter is activated, the font appears smoother\nso that pixels are less noticeable. However if you want\nthe font to look exactly the same as its source file,\nyou should disable it.\nThe smooth filter is enabled by default.\n\n\\param smooth `true` to enable smoothing, `false` to disable it\n\n\\see `isSmooth`");
    LUASF_STUB_FUNCTION("sf.Font", "setSmooth", "fun(self: sf.Font, smooth: boolean)");
    type_sf__Font.set_function("setSmooth",
        [](sf::Font& self, bool smooth) {
            self.setSmooth(smooth);
        }
    );
    LUASF_STUB_DOC("\\brief Tell whether the smooth filter is enabled or not\n\n\\return `true` if smoothing is enabled, `false` if it is disabled\n\n\\see `setSmooth`");
    LUASF_STUB_FUNCTION("sf.Font", "isSmooth", "fun(self: sf.Font): boolean");
    type_sf__Font.set_function("isSmooth",
        [](sf::Font& self) -> bool {
            return self.isSmooth();
        }
    );
    auto type_sf__Font__Info = table_sf__Font.new_usertype<sf::Font::Info>("Info", sol::no_constructor);
    sol::table table_sf__Font__Info = table_sf__Font["Info"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::Font::Info>(lua);
    LUASF_STUB_DOC("\\brief Holds various information about a font");
    LUASF_STUB_CLASS("sf.Font.Info");
    LUASF_STUB_DOC("A unique ID that identifies the font");
    LUASF_STUB_FIELD("id", "integer");
    LUASF_STUB_DOC("The font family");
    LUASF_STUB_FIELD("family", "string");
    LUASF_STUB_DOC("Has kerning information");
    LUASF_STUB_FIELD("hasKerning", "boolean");
    LUASF_STUB_DOC("Has native vertical metrics");
    LUASF_STUB_FIELD("hasVerticalMetrics", "boolean");
    LUASF_STUB_FUNCTION("sf.Font.Info", "new", "fun(): sf.Font.Info");
    type_sf__Font__Info.set_function("new", sol::factories(
        []() {
            return lua_sf::makeLuaSharedObject<sf::Font::Info>();
        }
    ));
    type_sf__Font__Info.set("id", sol::property(
        [](sf::Font::Info& self) {
            return self.id;
        },
        [](sf::Font::Info& self, lua_sf::LuaIntegral<std::uint64_t> value) {
            self.id = value.value();
        }
    ));
    type_sf__Font__Info.set("family", sol::property(
        [](sf::Font::Info& self) {
            return std::string(self.family);
        },
        [](sf::Font::Info& self, std::string value) {
            self.family = value;
        }
    ));
    type_sf__Font__Info["hasKerning"] = sol::policies(&sf::Font::Info::hasKerning, sol::self_dependency{});
    type_sf__Font__Info["hasVerticalMetrics"] = sol::policies(&sf::Font::Info::hasVerticalMetrics, sol::self_dependency{});
}
