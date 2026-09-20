#include "Graphics/bind_Text.hpp"

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

namespace { constexpr std::array<std::string_view, 77> docs = {
    "\\brief Graphical text that can be drawn to a render target",
    "\\brief Construct the text from a string, font and size\n\nNote that if the used font is a bitmap font, it is not\nscalable, thus not all requested sizes will be available\nto use. This needs to be taken into consideration when\nsetting the character size. If you need to display text\nof a certain size, make sure the corresponding bitmap\nfont that supports that size is used.\n\n\\param string         Text assigned to the string\n\\param font           Font used to draw the string\n\\param characterSize  Base size of characters, in pixels",
    "\\brief set the position of the object\n\nThis function completely overwrites the previous position.\nSee the move function to apply an offset based on the previous position instead.\nThe default position of a transformable object is (0, 0).\n\nNote that `sf::Text` may appear offset when positioned.\nThis is because its local bounds are influenced by font metrics (e.g. tallest characters)\nto consistently align with the text's baseline. As such the `getGlobalBounds()`\nposition may not match the position you set.\n\nTo account for this offset, the local bounds need to be considered.\nEither by including it in the position calculation:\n\\code\ntext.setPosition(position - text.getLocalBounds().position);\n\\endcode\nOr by adjusting the text's origin:\n\\code\ntext.setOrigin(text.getLocalBounds().position);\ntext.setPosition(position);\n\\endcode\n\n\\param position New position\n\n\\see `move`, `getPosition`",
    "\\brief set the orientation of the object\n\nThis function completely overwrites the previous rotation.\nSee the rotate function to add an angle based on the previous rotation instead.\nThe default rotation of a transformable object is 0.\n\n\\param angle New rotation\n\n\\see `rotate`, `getRotation`",
    "\\brief set the scale factors of the object\n\nThis function completely overwrites the previous scale.\nSee the scale function to add a factor based on the previous scale instead.\nThe default scale of a transformable object is (1, 1).\n\n\\param factors New scale factors\n\n\\see `scale`, `getScale`",
    "\\brief set the local origin of the object\n\nThe origin of an object defines the center point for\nall transformations (position, scale, rotation).\nThe coordinates of this point must be relative to the\ntop-left corner of the object, and ignore all\ntransformations (position, scale, rotation).\nThe default origin of a transformable object is (0, 0).\n\n\\param origin New origin\n\n\\see `getOrigin`",
    "\\brief get the position of the object\n\n\\return Current position\n\n\\see `setPosition`",
    "\\brief get the orientation of the object\n\nThe rotation is always in the range [0, 360].\n\n\\return Current rotation\n\n\\see `setRotation`",
    "\\brief get the current scale of the object\n\n\\return Current scale factors\n\n\\see `setScale`",
    "\\brief get the local origin of the object\n\n\\return Current origin\n\n\\see `setOrigin`",
    "\\brief Move the object by a given offset\n\nThis function adds to the current position of the object,\nunlike `setPosition` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nobject.setPosition(object.getPosition() + offset);\n\\endcode\n\n\\param offset Offset\n\n\\see `setPosition`",
    "\\brief Rotate the object\n\nThis function adds to the current rotation of the object,\nunlike `setRotation` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nobject.setRotation(object.getRotation() + angle);\n\\endcode\n\n\\param angle Angle of rotation",
    "\\brief Scale the object\n\nThis function multiplies the current scale of the object,\nunlike `setScale` which overwrites it.\nThus, it is equivalent to the following code:\n\\code\nsf::Vector2f scale = object.getScale();\nobject.setScale(scale.x * factor.x, scale.y * factor.y);\n\\endcode\n\n\\param factor Scale factors\n\n\\see `setScale`",
    "\\brief get the combined transform of the object\n\n\\return Transform combining the position/rotation/scale/origin of the object\n\n\\see `getInverseTransform`",
    "\\brief get the inverse of the combined transform of the object\n\n\\return Inverse of the combined transformations applied to the object\n\n\\see `getTransform`",
    "\\brief Set the text's string\n\nThe `string` argument is a `sf::String`, which can\nautomatically be constructed from standard string types.\nSo, the following calls are all valid:\n\\code\ntext.setString(\"hello\");\ntext.setString(L\"hello\");\ntext.setString(std::string(\"hello\"));\ntext.setString(std::wstring(L\"hello\"));\n\\endcode\nA text's string is empty by default.\n\n\\param string New string\n\n\\see `getString`",
    "\\brief Set the text's font\n\nThe `font` argument refers to a font that must\nexist as long as the text uses it. Indeed, the text\ndoesn't store its own copy of the font, but rather keeps\na pointer to the one that you passed to this function.\nIf the font is destroyed and the text tries to\nuse it, the behavior is undefined.\n\n\\param font New font\n\n\\see `getFont`",
    "\\brief Set the character size\n\nThe default size is 30.\n\nNote that if the used font is a bitmap font, it is not\nscalable, thus not all requested sizes will be available\nto use. This needs to be taken into consideration when\nsetting the character size. If you need to display text\nof a certain size, make sure the corresponding bitmap\nfont that supports that size is used.\n\n\\param size New character size, in pixels\n\n\\see `getCharacterSize`",
    "\\brief Set the line spacing factor\n\nThe default spacing between lines is defined by the font.\nThis method enables you to set a factor for the spacing\nbetween lines. By default the line spacing factor is 1.\n\n\\param spacingFactor New line spacing factor\n\n\\see `getLineSpacing`",
    "\\brief Set the letter spacing factor\n\nThe default spacing between letters is defined by the font.\nThis factor doesn't directly apply to the existing\nspacing between each character, it rather adds a fixed\nspace between them which is calculated from the font\nmetrics and the character size.\nNote that factors below 1 (including negative numbers) bring\ncharacters closer to each other.\nBy default the letter spacing factor is 1.\n\n\\param spacingFactor New letter spacing factor\n\n\\see `getLetterSpacing`",
    "\\brief Set the text's style\n\nYou can pass a combination of one or more styles, for\nexample `sf::Text::Bold | sf::Text::Italic`.\nThe default style is `sf::Text::Regular`.\n\n\\param style New style\n\n\\see `getStyle`",
    "\\brief Set the fill color of the text\n\nBy default, the text's fill color is opaque white.\nSetting the fill color to a transparent color with an outline\nwill cause the outline to be displayed in the fill area of the text.\n\n\\param color New fill color of the text\n\n\\see `getFillColor`",
    "\\brief Set the outline color of the text\n\nBy default, the text's outline color is opaque black.\n\n\\param color New outline color of the text\n\n\\see `getOutlineColor`",
    "\\brief Set the thickness of the text's outline\n\nBy default, the outline thickness is 0.\n\nBe aware that using a negative value for the outline\nthickness will cause distorted rendering.\n\n\\param thickness New outline thickness, in pixels\n\n\\see `getOutlineThickness`",
    "\\brief Set the line alignment for a multi-line text\n\nBy default, the lines will be aligned according to the\ndirection of the line's script. Left-to-right scripts\nwill be aligned to the left and right-to-left scripts\nwill be aligned to the right.\n\nForcing alignment will ignore script direction and always\nalign according to the requested line alignment.\n\n\\param lineAlignment New line alignment\n\n\\see `getLineAlignment`",
    "\\brief Set the text orientation\n\nBy default, the lines will have horizontal orientation.\n\nBe aware that most fonts don't natively support vertical\norientations. Fonts that are the most likely to natively\nsupport vertical orientations are those whose scripts\nalso support vertical orientations e.g. east asian scripts.\n\nIf a font does not natively support vertical orientation,\nvertical metrics might still be provided for shaping.\nIn this case, they are very likely to be emulated and might\nnot result in good visual output.\n\nSome metrics such as advance and baseline position will\nbe rotated so they match the vertical axis.\n\n\\param textOrientation New text orientation\n\n\\see `getTextOrientation`",
    "\\brief Get the text's string\n\nThe returned string is a `sf::String`, which can automatically\nbe converted to standard string types. So, the following\nlines of code are all valid:\n\\code\nsf::String   s1 = text.getString();\nstd::string  s2 = text.getString();\nstd::wstring s3 = text.getString();\n\\endcode\n\n\\return Text's string\n\n\\see `setString`",
    "\\brief Get the text's font\n\nThe returned reference is const, which means that you\ncannot modify the font when you get it from this function.\n\n\\return Reference to the text's font\n\n\\see `setFont`",
    "\\brief Get the character size\n\n\\return Size of the characters, in pixels\n\n\\see `setCharacterSize`",
    "\\brief Get the size of the letter spacing factor\n\n\\return Size of the letter spacing factor\n\n\\see `setLetterSpacing`",
    "\\brief Get the size of the line spacing factor\n\n\\return Size of the line spacing factor\n\n\\see `setLineSpacing`",
    "\\brief Get the text's style\n\n\\return Text's style\n\n\\see `setStyle`",
    "\\brief Get the fill color of the text\n\n\\return Fill color of the text\n\n\\see `setFillColor`",
    "\\brief Get the outline color of the text\n\n\\return Outline color of the text\n\n\\see `setOutlineColor`",
    "\\brief Get the outline thickness of the text\n\n\\return Outline thickness of the text, in pixels\n\n\\see `setOutlineThickness`",
    "\\brief Get the line alignment for a multi-line text\n\n\\return Line alignment\n\n\\see `setLineAlignment`",
    "\\brief Get the text orientation\n\n\\return Text orientation\n\n\\see `setTextOrientation`",
    "\\brief Return the position of the `index`-th character\n\n\\deprecated Use `getShapedGlyphs()` instead.\n\nThis function computes the visual position of a character\nfrom its index in the string. The returned position is\nin global coordinates (translation, rotation, scale and\norigin are applied).\nIf `index` is out of range, the position of the end of\nthe string is returned.\n\n\\param index Index of the character\n\n\\return Position of the character",
    "\\brief Return a list of shaped glyphs that make up the text\n\nThe result of shaping i.e. positioning individual glyphs\nbased on the properties of the font and the input text\nis a sequence of shaped glyphs that each have a collection\nof properties.\n\nIn addition to the glyph information that is available\nby looking up a glyph from a font, the glyph position,\nglyph cluster ID and direction of the text represented\nby the glyph is provided.\n\nWhen specifying unicode text, multiple unicode codepoints\nmight combine to form e.g. a ligature such as \u00e6 or\nbase-and-mark sequence such as \u00e9 which are composed of\nmultiple individual glyphs. These combinations are known\nas grapheme clusters. When segmenting text into grapheme\nclusters, each cluster identifies a complete unit of text\nthat will be drawn. There are other methods of segmenting\ntext into clusters e.g. without combining marks.\nCharacter cluster segmentation is used as the default.\nA single grapheme can be represented by an individual\ncodepoint or by a composition of codepoints e.g. an e as\nthe base and an accent as the mark which together compose\nthe grapheme \u00e9. The cluster groups that result from shaping\ndepend on whether the input text provides composed\ncodepoints or decomposed codepoints. This is an advanced\ntopic known as unicode normalisation.\n\nWhen positioning e.g. a cursor within the text, grapheme\nclusters can be treated as the basic units of which the\ntext is composed and not subdivided into their individual\ncomponents or glyphs. If positioning of the cursor within\na single grapheme e.g. a ligature is required, a more\nfine-grained cluster segmentation algorithm should be used.\n\nThe returned glyph positions are in local coordinates\n(translation, rotation, scale and origin are not applied).\n\n\\return List of shaped glyphs that make up the text\n\n\\see `setClusterGrouping`",
    "\\brief Return the cluster grouping algorithm in use\n\n\\return The cluster grouping algorithm in use",
    "\\brief Set the cluster grouping algorithm to use\n\nBy default, character cluster grouping is used.\n\nCharacter cluster grouping is good enough to be able to\nposition cursors in most scenarios. If more coarse-grained\ngrouping is required, grapheme grouping can be selected.\n\nCluster grouping can also be disabled if necessary.\n\n\\param clusterGrouping The cluster grouping algorithm to use",
    "\\brief Set the glyph pre-processor to be called per glyph\n\nThe glyph pre-processor is a callable that will be called\nwith glyph data to be pre-processed.\n\n\\param glyphPreProcessor The glyph pre-processor to be called per glyph, pass an empty pre-processor to disable pre-processing",
    "\\brief Get a reference to the vertex data of this text\n\nThe vertex data is regenerated by the text whenever it is\nnecessary. Any changes made to the vertex data will be\ndiscarded whenever this happens.\n\n\\return Reference to the vertex data of this text",
    "\\brief Get a reference to the outline vertex data of this text\n\nThe outline vertex data is regenerated by the text whenever\nit is necessary. Any changes made to the outline vertex data\nwill be discarded whenever this happens.\n\n\\return Reference to the vertex data of this text",
    "\\brief Get the local bounding rectangle of the entity\n\nThe returned rectangle is in local coordinates, which means\nthat it ignores the transformations (translation, rotation,\nscale, ...) that are applied to the entity.\nIn other words, this function returns the bounds of the\nentity in the entity's coordinate system.\n\n\\return Local bounding rectangle of the entity",
    "\\brief Get the global bounding rectangle of the entity\n\nThe returned rectangle is in global coordinates, which means\nthat it takes into account the transformations (translation,\nrotation, scale, ...) that are applied to the entity.\nIn other words, this function returns the bounds of the\ntext in the global 2D world's coordinate system.\n\n\\return Global bounding rectangle of the entity",
    "\\brief Enumeration of the string drawing styles",
    "Regular characters, no style",
    "Bold characters",
    "Italic characters",
    "Underlined characters",
    "Strike through characters",
    "\\brief Enumeration of the text alignment options",
    "Automatically align lines by script direction, left-align left-to-right text and right-align right-to-left text",
    "Force align all lines to the left, regardless of script direction",
    "Force align all lines centrally",
    "Force align lines to the right, regardless of script direction",
    "\\brief Cluster Grouping",
    "Group clusters by grapheme",
    "Group clusters by character",
    "Do not group clusters",
    "\\brief Text Direction",
    "Unspecified",
    "Left-to-right",
    "Right-to-left",
    "Top-to-bottom",
    "Bottom-to-top",
    "\\brief Text Orientation",
    "Default (left-to-right or right-to-left depending on detected script)",
    "\\brief Structure describing a glyph after shaping",
    "Position of the glyph within a text",
    "Cluster ID",
    "Text direction",
    "The baseline position of the line this glyph is a part of",
    "Starting offset of the vertex data belonging to this glyph",
    "Count of vertices belonging to this glyph",
    "\\brief Callable that is provided with glyph data for pre-processing\n\nWhen re-generating the text geometry, shaping will be\nperformed on the input string using the set font. The\nresult of shaping is a set of shaped glyphs. Shaped\nglyphs are glyphs that have been positioned by the shaper\nand whose script direction has also been determined.\n\nBecause multiple input codepoints can be merged into a\nsingle glyph and single codepoints decomposed into multiple\nglyphs, the shaper provides a way to map the shaping output\nback to the input. When the input string is provided to\nthe shaper, a monotonically increasing character index is\nattached to each input codepoint. If the input string\nconsists of 10 codepoints, the indices will be 0 to 9.\n\nAfter shaping each shaped glyph will be assigned a\ncluster value. These cluster values are derived from the\ninput indices that were provided to the shaper. Because\nof the merging and decomposing that happens during shaping,\nthere isn't a 1 to 1 mapping between input indices and\noutput cluster values.\n\nIn order to set the glyph properties reliably, they have\nto be set based on text segmentation boundaries such as\ngraphemes, words and sentences. See the corresponding\nmethods in `sf::String` that can check for these boundaries.\n\nOnce the input text segments to be pre-processed have\nbeen determined, they have to be applied to the shaped\nglyphs. When using character or grapheme cluster grouping\nit is guaranteed that the resulting cluster values are\nmonotonic. This means that cluster values will not be\nreordered beyond the bounds of the indices that were\nprovided with the input text.\n\nWhat this means is that given a segment of text that\nshould e.g. be colored differently, if a beginning and\nend index can be determined from the input codepoints,\nthese index boundaries can be used to select the clusters\nof the shaped glyphs that correspond to the input segment\nand thus whose color needs to be set.\n\nHere is an example string with codepoint indices:\n\\code\nI   l i k e   f l o w e r s ,   m u f f i n s   a n d   w a f f l e s .\n0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 2 2 3 3 3 3 3 3\n0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5\n\\endcode\nAfter shaping, due to ligature merging of fl, ffi and ffl,\nthe output clusters might look like:\n\\code\nI   l i k e   fl o w e r s ,   m u ffi n s   a n d   w a ffl e s .\n0 0 0 0 0 0 0 0  0 1 1 1 1 1 1 1 1 1   2 2 2 2 2 2 2 2 2 3   3 3 3\n0 1 2 3 4 5 6 7  9 0 1 2 3 4 5 6 7 8   1 2 3 4 5 6 7 8 9 0   3 4 5\n\\endcode\n\nIn order to e.g. color the word \"muffins\", the beginning\nand end codepoint indices of the word have to be determined,\nin this case 16 and 22. After shaping, any glyphs belonging\nto the word \"muffins\" will have cluster values between and\nincluding 16 and 22. In the example above the clusters\n16, 17, 18, 21 and 22 belong to the word \"muffins\".\nColoring the glyphs with those indices will result in the\nword \"muffins\" being colored.\n\nThe same applies to \"flowers\" and \"waffles\" in the example\nabove.\n\nBecause merging and decomposition of codepoints cannot\nhappen beyond word boundaries, applying properties to\nglyphs using the above method is safe when segmenting\nbased on words. As can be seen above it would not work\nwhen attempting to apply a different property to the\nsingle graphemes 'f', 'l' or 'i' since they can be\nmerged with neighbouring graphemes into a single glyph.\n\nThe opposite, decomposition, of the following input:\n\\code\nI   f i n d   c l i c h \u00e9 s   f u n n y .\n0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1\n0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0\n\\endcode\ninto glyphs would look like:\n\\code\nI   f i n d   c l i c h e + s   f u n n y .\n0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1\n0 1 2 3 4 5 6 7 8 9 0 1 2 2 3 4 5 6 7 8 9 0\n\\endcode\nThe + at cluster 12 is a placeholder for the acute accent.\nThis can occur if the font provides the glyph for the accent\nseperate from the base glyph e which also has the cluster\nvalue 12. The codepoint \u00e9 is thus decomposed into a base glyph\nand a mark glyph. The cluster value of the mark in such a\ndecomposition will be identical to the base. Because of this,\nthe same procedure as demonstrated in the fist example can be\napplied here as well.\n\nThe above examples are just simple examples of how to map\ninput codepoint indices to output cluster values. While\nmerging and decomposition are an exception in latin script,\nthey can occur very frequently in other scripts. The mapping\nprocedure described above will work for all scripts.\n\nOnce the boundaries of the cluster values whose properties to\nmodify have been determined, they can be used from within\nthe callable to set said properties on a glyph by glyph basis.\n\nThe callable will be called in the order in which glyph\ngeometry is generated. This does not always happen in\nascending cluster order such as in right-to-left text where\nit happens in descending cluster order.\n\nBe aware that while changing the character size per glyph\nis not possible, changing its style or outline thickness\nis. Doing this, however, might lead to slight inconsistencies\nwhen the text bounds are computed at the end of the geometry\nupdate process. The same applies to the italic style.\n\nIn contrast, changing the fill or outline color is safe\nsince they don't have any effect on the pixel coverage of\nthe glyph.\n\nSetting the underlined and strikethrough styles per glyph\nis technically possible but not yet implemented.\n\nNote: Because text bounds are computed based on the\ngeometry, it is not safe or reliable to query the text bounds\nfrom within this callable. If it is absolutely necessary\nto make decisions within this callable based on text bounds,\nmultiple geometry updates will be necessary. The first\ngeometry update is run with the pre-processor set to\npass through data. Based on the first update the text bounds\ncan be queried and stored. The stored text bounds can then\nbe used in the second geometry update.",
}; }

void bind_Text(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__Text = lua_glue::BindClass<sf::Text>(sf, "Text");
    lua_glue::BindBase<sf::Text, sf::Drawable>(type_sf__Text);
    lua_glue::BindBase<sf::Text, sf::Transformable>(type_sf__Text);
    lua_glue::Table table_sf__Text = sf["Text"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Text>(lua);
    lua_glue::Table native_bases_sf__Text = lua.create_table();
    native_bases_sf__Text.add(lua["sf"]["Drawable"].get<lua_glue::Table>());
    native_bases_sf__Text.add(lua["sf"]["Transformable"].get<lua_glue::Table>());
    table_sf__Text.raw_set("__nativeBases", native_bases_sf__Text);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.Text", "sf.Drawable, sf.Transformable");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FUNCTION("sf.Text", "new", "fun(font: sf.Font, string?: string, characterSize?: integer): sf.Text");
    lua_glue::BindCallable(type_sf__Text, "new",
        [](const sf::Font& font, std::string string, lua_sf::LuaIntegral<unsigned int> characterSize) {
            return lua_sf::makeLuaSharedObject<sf::Text>(font, lua_sf::to_sf_string(string), characterSize.value());
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return lua_sf::to_utf8_string(static_cast<sf::String>(""));
        }},
        lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<unsigned int>(30);
        }}},
        docs[1]
    );
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FUNCTION("sf.Text", "setPosition", "fun(self: sf.Text, position: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Text, "setPosition",
        [](sf::Text& self, sf::Vector2f position) {
            static_cast<sf::Transformable&>(self).setPosition(position);
        },
        docs[2]
    );
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FUNCTION("sf.Text", "setRotation", "fun(self: sf.Text, angle: sf.Angle)");
    lua_glue::BindCallable(type_sf__Text, "setRotation",
        [](sf::Text& self, sf::Angle angle) {
            static_cast<sf::Transformable&>(self).setRotation(angle);
        },
        docs[3]
    );
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FUNCTION("sf.Text", "setScale", "fun(self: sf.Text, factors: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Text, "setScale",
        [](sf::Text& self, sf::Vector2f factors) {
            static_cast<sf::Transformable&>(self).setScale(factors);
        },
        docs[4]
    );
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FUNCTION("sf.Text", "setOrigin", "fun(self: sf.Text, origin: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Text, "setOrigin",
        [](sf::Text& self, sf::Vector2f origin) {
            static_cast<sf::Transformable&>(self).setOrigin(origin);
        },
        docs[5]
    );
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FUNCTION("sf.Text", "getPosition", "fun(self: sf.Text): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Text, "getPosition",
        [](const sf::Text& self) -> sf::Vector2f {
            return static_cast<const sf::Transformable&>(self).getPosition();
        },
        docs[6]
    );
    LUASF_STUB_DOC(docs[7]);
    LUASF_STUB_FUNCTION("sf.Text", "getRotation", "fun(self: sf.Text): sf.Angle");
    lua_glue::BindCallable(type_sf__Text, "getRotation",
        [](const sf::Text& self) -> sf::Angle {
            return static_cast<const sf::Transformable&>(self).getRotation();
        },
        docs[7]
    );
    LUASF_STUB_DOC(docs[8]);
    LUASF_STUB_FUNCTION("sf.Text", "getScale", "fun(self: sf.Text): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Text, "getScale",
        [](const sf::Text& self) -> sf::Vector2f {
            return static_cast<const sf::Transformable&>(self).getScale();
        },
        docs[8]
    );
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.Text", "getOrigin", "fun(self: sf.Text): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Text, "getOrigin",
        [](const sf::Text& self) -> sf::Vector2f {
            return static_cast<const sf::Transformable&>(self).getOrigin();
        },
        docs[9]
    );
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_FUNCTION("sf.Text", "move", "fun(self: sf.Text, offset: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Text, "move",
        [](sf::Text& self, sf::Vector2f offset) {
            static_cast<sf::Transformable&>(self).move(offset);
        },
        docs[10]
    );
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FUNCTION("sf.Text", "rotate", "fun(self: sf.Text, angle: sf.Angle)");
    lua_glue::BindCallable(type_sf__Text, "rotate",
        [](sf::Text& self, sf::Angle angle) {
            static_cast<sf::Transformable&>(self).rotate(angle);
        },
        docs[11]
    );
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FUNCTION("sf.Text", "scale", "fun(self: sf.Text, factor: sf.Vector2f)");
    lua_glue::BindCallable(type_sf__Text, "scale",
        [](sf::Text& self, sf::Vector2f factor) {
            static_cast<sf::Transformable&>(self).scale(factor);
        },
        docs[12]
    );
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.Text", "getTransform", "fun(self: sf.Text): sf.Transform");
    lua_glue::BindCallable(type_sf__Text, "getTransform",
        [](const sf::Text& self) {
            return std::cref(static_cast<const sf::Transformable&>(self).getTransform());
        },
        docs[13],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FUNCTION("sf.Text", "getInverseTransform", "fun(self: sf.Text): sf.Transform");
    lua_glue::BindCallable(type_sf__Text, "getInverseTransform",
        [](const sf::Text& self) {
            return std::cref(static_cast<const sf::Transformable&>(self).getInverseTransform());
        },
        docs[14],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FUNCTION("sf.Text", "setString", "fun(self: sf.Text, string: string)");
    lua_glue::BindCallable(type_sf__Text, "setString",
        [](sf::Text& self, std::string string) {
            self.setString(lua_sf::to_sf_string(string));
        },
        docs[15]
    );
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FUNCTION("sf.Text", "setFont", "fun(self: sf.Text, font: sf.Font)");
    lua_glue::BindCallable(type_sf__Text, "setFont",
        [](sf::Text& self, const sf::Font& font) {
            self.setFont(font);
        },
        docs[16]
    );
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FUNCTION("sf.Text", "setCharacterSize", "fun(self: sf.Text, size: integer)");
    lua_glue::BindCallable(type_sf__Text, "setCharacterSize",
        [](sf::Text& self, lua_sf::LuaIntegral<unsigned int> size) {
            self.setCharacterSize(size.value());
        },
        docs[17]
    );
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FUNCTION("sf.Text", "setLineSpacing", "fun(self: sf.Text, spacingFactor: number)");
    lua_glue::BindCallable(type_sf__Text, "setLineSpacing",
        [](sf::Text& self, float spacingFactor) {
            self.setLineSpacing(spacingFactor);
        },
        docs[18]
    );
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FUNCTION("sf.Text", "setLetterSpacing", "fun(self: sf.Text, spacingFactor: number)");
    lua_glue::BindCallable(type_sf__Text, "setLetterSpacing",
        [](sf::Text& self, float spacingFactor) {
            self.setLetterSpacing(spacingFactor);
        },
        docs[19]
    );
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FUNCTION("sf.Text", "setStyle", "fun(self: sf.Text, style: integer)");
    lua_glue::BindCallable(type_sf__Text, "setStyle",
        [](sf::Text& self, lua_sf::LuaIntegral<std::uint32_t> style) {
            self.setStyle(style.value());
        },
        docs[20]
    );
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_FUNCTION("sf.Text", "setFillColor", "fun(self: sf.Text, color: sf.Color)");
    lua_glue::BindCallable(type_sf__Text, "setFillColor",
        [](sf::Text& self, sf::Color color) {
            self.setFillColor(color);
        },
        docs[21]
    );
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FUNCTION("sf.Text", "setOutlineColor", "fun(self: sf.Text, color: sf.Color)");
    lua_glue::BindCallable(type_sf__Text, "setOutlineColor",
        [](sf::Text& self, sf::Color color) {
            self.setOutlineColor(color);
        },
        docs[22]
    );
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FUNCTION("sf.Text", "setOutlineThickness", "fun(self: sf.Text, thickness: number)");
    lua_glue::BindCallable(type_sf__Text, "setOutlineThickness",
        [](sf::Text& self, float thickness) {
            self.setOutlineThickness(thickness);
        },
        docs[23]
    );
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FUNCTION("sf.Text", "setLineAlignment", "fun(self: sf.Text, lineAlignment: sf.Text.LineAlignment)");
    lua_glue::BindCallable(type_sf__Text, "setLineAlignment",
        [](sf::Text& self, sf::Text::LineAlignment lineAlignment) {
            self.setLineAlignment(lineAlignment);
        },
        docs[24]
    );
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FUNCTION("sf.Text", "setTextOrientation", "fun(self: sf.Text, textOrientation: sf.Text.TextOrientation)");
    lua_glue::BindCallable(type_sf__Text, "setTextOrientation",
        [](sf::Text& self, sf::Text::TextOrientation textOrientation) {
            self.setTextOrientation(textOrientation);
        },
        docs[25]
    );
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FUNCTION("sf.Text", "getString", "fun(self: sf.Text): string");
    lua_glue::BindCallable(type_sf__Text, "getString",
        [](const sf::Text& self) -> std::string {
            return lua_sf::to_utf8_string(self.getString());
        },
        docs[26],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_FUNCTION("sf.Text", "getFont", "fun(self: sf.Text): sf.Font");
    lua_glue::BindCallable(type_sf__Text, "getFont",
        [](const sf::Text& self) {
            return std::cref(self.getFont());
        },
        docs[27],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_FUNCTION("sf.Text", "getCharacterSize", "fun(self: sf.Text): integer");
    lua_glue::BindCallable(type_sf__Text, "getCharacterSize",
        [](const sf::Text& self) -> unsigned int {
            return self.getCharacterSize();
        },
        docs[28]
    );
    LUASF_STUB_DOC(docs[29]);
    LUASF_STUB_FUNCTION("sf.Text", "getLetterSpacing", "fun(self: sf.Text): number");
    lua_glue::BindCallable(type_sf__Text, "getLetterSpacing",
        [](const sf::Text& self) -> float {
            return self.getLetterSpacing();
        },
        docs[29]
    );
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_FUNCTION("sf.Text", "getLineSpacing", "fun(self: sf.Text): number");
    lua_glue::BindCallable(type_sf__Text, "getLineSpacing",
        [](const sf::Text& self) -> float {
            return self.getLineSpacing();
        },
        docs[30]
    );
    LUASF_STUB_DOC(docs[31]);
    LUASF_STUB_FUNCTION("sf.Text", "getStyle", "fun(self: sf.Text): integer");
    lua_glue::BindCallable(type_sf__Text, "getStyle",
        [](const sf::Text& self) -> std::uint32_t {
            return self.getStyle();
        },
        docs[31]
    );
    LUASF_STUB_DOC(docs[32]);
    LUASF_STUB_FUNCTION("sf.Text", "getFillColor", "fun(self: sf.Text): sf.Color");
    lua_glue::BindCallable(type_sf__Text, "getFillColor",
        [](const sf::Text& self) -> sf::Color {
            return self.getFillColor();
        },
        docs[32]
    );
    LUASF_STUB_DOC(docs[33]);
    LUASF_STUB_FUNCTION("sf.Text", "getOutlineColor", "fun(self: sf.Text): sf.Color");
    lua_glue::BindCallable(type_sf__Text, "getOutlineColor",
        [](const sf::Text& self) -> sf::Color {
            return self.getOutlineColor();
        },
        docs[33]
    );
    LUASF_STUB_DOC(docs[34]);
    LUASF_STUB_FUNCTION("sf.Text", "getOutlineThickness", "fun(self: sf.Text): number");
    lua_glue::BindCallable(type_sf__Text, "getOutlineThickness",
        [](const sf::Text& self) -> float {
            return self.getOutlineThickness();
        },
        docs[34]
    );
    LUASF_STUB_DOC(docs[35]);
    LUASF_STUB_FUNCTION("sf.Text", "getLineAlignment", "fun(self: sf.Text): sf.Text.LineAlignment");
    lua_glue::BindCallable(type_sf__Text, "getLineAlignment",
        [](const sf::Text& self) -> sf::Text::LineAlignment {
            return self.getLineAlignment();
        },
        docs[35]
    );
    LUASF_STUB_DOC(docs[36]);
    LUASF_STUB_FUNCTION("sf.Text", "getTextOrientation", "fun(self: sf.Text): sf.Text.TextOrientation");
    lua_glue::BindCallable(type_sf__Text, "getTextOrientation",
        [](const sf::Text& self) -> sf::Text::TextOrientation {
            return self.getTextOrientation();
        },
        docs[36]
    );
    LUASF_STUB_DOC(docs[37]);
    LUASF_STUB_FUNCTION("sf.Text", "findCharacterPos", "fun(self: sf.Text, index: integer): sf.Vector2f");
    lua_glue::BindCallable(type_sf__Text, "findCharacterPos",
        [](const sf::Text& self, lua_sf::LuaIntegral<std::size_t> index) -> sf::Vector2f {
            return self.findCharacterPos(index.value());
        },
        docs[37]
    );
    LUASF_STUB_DOC(docs[38]);
    LUASF_STUB_FUNCTION("sf.Text", "getShapedGlyphs", "fun(self: sf.Text): sf.Text.ShapedGlyph[]");
    lua_glue::BindCallable(type_sf__Text, "getShapedGlyphs",
        [lua](const sf::Text& self) -> lua_glue::Object {
            return lua_sf::vector_to_object(lua, self.getShapedGlyphs());
        },
        docs[38],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[39]);
    LUASF_STUB_FUNCTION("sf.Text", "getClusterGrouping", "fun(self: sf.Text): sf.Text.ClusterGrouping");
    lua_glue::BindCallable(type_sf__Text, "getClusterGrouping",
        [](const sf::Text& self) -> sf::Text::ClusterGrouping {
            return self.getClusterGrouping();
        },
        docs[39]
    );
    LUASF_STUB_DOC(docs[40]);
    LUASF_STUB_FUNCTION("sf.Text", "setClusterGrouping", "fun(self: sf.Text, clusterGrouping: sf.Text.ClusterGrouping)");
    lua_glue::BindCallable(type_sf__Text, "setClusterGrouping",
        [](sf::Text& self, sf::Text::ClusterGrouping clusterGrouping) {
            self.setClusterGrouping(clusterGrouping);
        },
        docs[40]
    );
    LUASF_STUB_DOC(docs[41]);
    LUASF_STUB_FUNCTION("sf.Text", "setGlyphPreProcessor", "fun(self: sf.Text, glyphPreProcessor: sf.Text.GlyphPreProcessor|nil)");
    lua_glue::BindCallable(type_sf__Text, "setGlyphPreProcessor",
        [](sf::Text& self, lua_glue::Object glyphPreProcessor) {
            self.setGlyphPreProcessor(lua_sf::callback::from_object<sf::Text::GlyphPreProcessor, lua_sf::callback::GlyphPreProcessorCodec>(glyphPreProcessor, lua_sf::callback::CallbackOptions{"sf::Text::setGlyphPreProcessor.glyphPreProcessor", true}));
        },
        docs[41]
    );
    LUASF_STUB_DOC(docs[42]);
    LUASF_STUB_FUNCTION("sf.Text", "getVertexData", "fun(self: sf.Text): sf.VertexArray");
    lua_glue::BindCallable(type_sf__Text, "getVertexData",
        [](const sf::Text& self) {
            return std::ref(self.getVertexData());
        },
        docs[42],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[43]);
    LUASF_STUB_FUNCTION("sf.Text", "getOutlineVertexData", "fun(self: sf.Text): sf.VertexArray");
    lua_glue::BindCallable(type_sf__Text, "getOutlineVertexData",
        [](const sf::Text& self) {
            return std::ref(self.getOutlineVertexData());
        },
        docs[43],
        lua_glue::ReturnPolicy::ReferenceInternal
    );
    LUASF_STUB_DOC(docs[44]);
    LUASF_STUB_FUNCTION("sf.Text", "getLocalBounds", "fun(self: sf.Text): sf.FloatRect");
    lua_glue::BindCallable(type_sf__Text, "getLocalBounds",
        [](const sf::Text& self) -> sf::FloatRect {
            return self.getLocalBounds();
        },
        docs[44]
    );
    LUASF_STUB_DOC(docs[45]);
    LUASF_STUB_FUNCTION("sf.Text", "getGlobalBounds", "fun(self: sf.Text): sf.FloatRect");
    lua_glue::BindCallable(type_sf__Text, "getGlobalBounds",
        [](const sf::Text& self) -> sf::FloatRect {
            return self.getGlobalBounds();
        },
        docs[45]
    );
    LUASF_STUB_DOC(docs[46]);
    LUASF_STUB_CLASS("sf.Text.Style");
    LUASF_STUB_DOC(docs[47]);
    LUASF_STUB_FIELD("Regular", "integer");
    LUASF_STUB_DOC(docs[48]);
    LUASF_STUB_FIELD("Bold", "integer");
    LUASF_STUB_DOC(docs[49]);
    LUASF_STUB_FIELD("Italic", "integer");
    LUASF_STUB_DOC(docs[50]);
    LUASF_STUB_FIELD("Underlined", "integer");
    LUASF_STUB_DOC(docs[51]);
    LUASF_STUB_FIELD("StrikeThrough", "integer");
    lua_glue::BindEnum<sf::Text::Style>(table_sf__Text, "Style", {
        {"Regular", sf::Text::Style::Regular},
        {"Bold", sf::Text::Style::Bold},
        {"Italic", sf::Text::Style::Italic},
        {"Underlined", sf::Text::Style::Underlined},
        {"StrikeThrough", sf::Text::Style::StrikeThrough}
    });
    LUASF_STUB_DOC(docs[52]);
    LUASF_STUB_CLASS("sf.Text.LineAlignment");
    LUASF_STUB_DOC(docs[53]);
    LUASF_STUB_FIELD("Default", "sf.Text.LineAlignment");
    LUASF_STUB_DOC(docs[54]);
    LUASF_STUB_FIELD("Left", "sf.Text.LineAlignment");
    LUASF_STUB_DOC(docs[55]);
    LUASF_STUB_FIELD("Center", "sf.Text.LineAlignment");
    LUASF_STUB_DOC(docs[56]);
    LUASF_STUB_FIELD("Right", "sf.Text.LineAlignment");
    lua_glue::BindEnum<sf::Text::LineAlignment>(table_sf__Text, "LineAlignment", {
        {"Default", sf::Text::LineAlignment::Default},
        {"Left", sf::Text::LineAlignment::Left},
        {"Center", sf::Text::LineAlignment::Center},
        {"Right", sf::Text::LineAlignment::Right}
    });
    LUASF_STUB_DOC(docs[57]);
    LUASF_STUB_CLASS("sf.Text.ClusterGrouping");
    LUASF_STUB_DOC(docs[58]);
    LUASF_STUB_FIELD("Grapheme", "sf.Text.ClusterGrouping");
    LUASF_STUB_DOC(docs[59]);
    LUASF_STUB_FIELD("Character", "sf.Text.ClusterGrouping");
    LUASF_STUB_DOC(docs[60]);
    LUASF_STUB_FIELD("None", "sf.Text.ClusterGrouping");
    lua_glue::BindEnum<sf::Text::ClusterGrouping>(table_sf__Text, "ClusterGrouping", {
        {"Grapheme", sf::Text::ClusterGrouping::Grapheme},
        {"Character", sf::Text::ClusterGrouping::Character},
        {"None", sf::Text::ClusterGrouping::None}
    });
    LUASF_STUB_DOC(docs[61]);
    LUASF_STUB_CLASS("sf.Text.TextDirection");
    LUASF_STUB_DOC(docs[62]);
    LUASF_STUB_FIELD("Unspecified", "sf.Text.TextDirection");
    LUASF_STUB_DOC(docs[63]);
    LUASF_STUB_FIELD("LeftToRight", "sf.Text.TextDirection");
    LUASF_STUB_DOC(docs[64]);
    LUASF_STUB_FIELD("RightToLeft", "sf.Text.TextDirection");
    LUASF_STUB_DOC(docs[65]);
    LUASF_STUB_FIELD("TopToBottom", "sf.Text.TextDirection");
    LUASF_STUB_DOC(docs[66]);
    LUASF_STUB_FIELD("BottomToTop", "sf.Text.TextDirection");
    lua_glue::BindEnum<sf::Text::TextDirection>(table_sf__Text, "TextDirection", {
        {"Unspecified", sf::Text::TextDirection::Unspecified},
        {"LeftToRight", sf::Text::TextDirection::LeftToRight},
        {"RightToLeft", sf::Text::TextDirection::RightToLeft},
        {"TopToBottom", sf::Text::TextDirection::TopToBottom},
        {"BottomToTop", sf::Text::TextDirection::BottomToTop}
    });
    LUASF_STUB_DOC(docs[67]);
    LUASF_STUB_CLASS("sf.Text.TextOrientation");
    LUASF_STUB_DOC(docs[68]);
    LUASF_STUB_FIELD("Default", "sf.Text.TextOrientation");
    LUASF_STUB_DOC(docs[65]);
    LUASF_STUB_FIELD("TopToBottom", "sf.Text.TextOrientation");
    LUASF_STUB_DOC(docs[66]);
    LUASF_STUB_FIELD("BottomToTop", "sf.Text.TextOrientation");
    lua_glue::BindEnum<sf::Text::TextOrientation>(table_sf__Text, "TextOrientation", {
        {"Default", sf::Text::TextOrientation::Default},
        {"TopToBottom", sf::Text::TextOrientation::TopToBottom},
        {"BottomToTop", sf::Text::TextOrientation::BottomToTop}
    });
    auto type_sf__Text__ShapedGlyph = lua_glue::BindClass<sf::Text::ShapedGlyph>(table_sf__Text, "ShapedGlyph");
    lua_glue::Table table_sf__Text__ShapedGlyph = table_sf__Text["ShapedGlyph"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::Text::ShapedGlyph>(lua);
    LUASF_STUB_DOC(docs[69]);
    LUASF_STUB_CLASS("sf.Text.ShapedGlyph");
    LUASF_STUB_FIELD("glyph", "sf.Glyph");
    LUASF_STUB_DOC(docs[70]);
    LUASF_STUB_FIELD("position", "sf.Vector2f");
    LUASF_STUB_DOC(docs[71]);
    LUASF_STUB_FIELD("cluster", "integer");
    LUASF_STUB_DOC(docs[72]);
    LUASF_STUB_FIELD("textDirection", "sf.Text.TextDirection");
    LUASF_STUB_DOC(docs[73]);
    LUASF_STUB_FIELD("baseline", "number");
    LUASF_STUB_DOC(docs[74]);
    LUASF_STUB_FIELD("vertexOffset", "integer");
    LUASF_STUB_DOC(docs[75]);
    LUASF_STUB_FIELD("vertexCount", "integer");
    LUASF_STUB_FUNCTION("sf.Text.ShapedGlyph", "new", "fun(): sf.Text.ShapedGlyph");
    lua_glue::BindCallable(type_sf__Text__ShapedGlyph, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::Text::ShapedGlyph>();
        }
    );
    lua_glue::BindAttr<sf::Glyph>(type_sf__Text__ShapedGlyph, "glyph", &sf::Text::ShapedGlyph::glyph);
    lua_glue::BindAttr<sf::Vector2f>(type_sf__Text__ShapedGlyph, "position", &sf::Text::ShapedGlyph::position);
    lua_glue::BindProperty(type_sf__Text__ShapedGlyph, "cluster",
        [](const sf::Text::ShapedGlyph& self) {
            return self.cluster;
        },
        [](sf::Text::ShapedGlyph& self, lua_sf::LuaIntegral<std::uint32_t> value) {
            self.cluster = value.value();
        }
    );
    lua_glue::BindAttr<sf::Text::TextDirection>(type_sf__Text__ShapedGlyph, "textDirection", &sf::Text::ShapedGlyph::textDirection);
    lua_glue::BindAttr<float>(type_sf__Text__ShapedGlyph, "baseline", &sf::Text::ShapedGlyph::baseline);
    lua_glue::BindProperty(type_sf__Text__ShapedGlyph, "vertexOffset",
        [](const sf::Text::ShapedGlyph& self) {
            return self.vertexOffset;
        },
        [](sf::Text::ShapedGlyph& self, lua_sf::LuaIntegral<std::size_t> value) {
            self.vertexOffset = value.value();
        }
    );
    lua_glue::BindProperty(type_sf__Text__ShapedGlyph, "vertexCount",
        [](const sf::Text::ShapedGlyph& self) {
            return self.vertexCount;
        },
        [](sf::Text::ShapedGlyph& self, lua_sf::LuaIntegral<std::size_t> value) {
            self.vertexCount = value.value();
        }
    );
    LUASF_STUB_DOC(docs[76]);
    LUASF_STUB_ALIAS("sf.Text.GlyphPreProcessor", "fun(shapedGlyph: sf.Text.ShapedGlyph, style: integer, fillColor: sf.Color, outlineColor: sf.Color, outlineThickness: number): {style: integer?, fillColor: sf.Color?, outlineColor: sf.Color?, outlineThickness: number?}|nil");
}
