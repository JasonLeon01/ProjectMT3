#include "Graphics/bind_BlendMode.hpp"

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

namespace { constexpr std::array<std::string_view, 33> docs = {
    "\\brief Blending modes for drawing",
    "Source blending factor for the color channels",
    "Destination blending factor for the color channels",
    "Blending equation for the color channels",
    "Source blending factor for the alpha channel",
    "Destination blending factor for the alpha channel",
    "Blending equation for the alpha channel",
    "\\brief Default constructor\n\nConstructs a blending mode that does alpha blending.",
    "\\brief Construct the blend mode given the factors and equation.\n\nThis constructor uses the same factors and equation for both\ncolor and alpha components. It also defaults to the Add equation.\n\n\\param sourceFactor      Specifies how to compute the source factor for the color and alpha channels.\n\\param destinationFactor Specifies how to compute the destination factor for the color and alpha channels.\n\\param blendEquation     Specifies how to combine the source and destination colors and alpha.",
    "\\brief Construct the blend mode given the factors and equation.\n\n\\param colorSourceFactor      Specifies how to compute the source factor for the color channels.\n\\param colorDestinationFactor Specifies how to compute the destination factor for the color channels.\n\\param colorBlendEquation     Specifies how to combine the source and destination colors.\n\\param alphaSourceFactor      Specifies how to compute the source factor.\n\\param alphaDestinationFactor Specifies how to compute the destination factor.\n\\param alphaBlendEquation     Specifies how to combine the source and destination alphas.",
    "\\brief Enumeration of the blending factors\n\nThe factors are mapped directly to their OpenGL equivalents,\nspecified by glBlendFunc() or glBlendFuncSeparate().",
    "(0, 0, 0, 0)",
    "(1, 1, 1, 1)",
    "(src.r, src.g, src.b, src.a)",
    "(1, 1, 1, 1) - (src.r, src.g, src.b, src.a)",
    "(dst.r, dst.g, dst.b, dst.a)",
    "(1, 1, 1, 1) - (dst.r, dst.g, dst.b, dst.a)",
    "(src.a, src.a, src.a, src.a)",
    "(1, 1, 1, 1) - (src.a, src.a, src.a, src.a)",
    "(dst.a, dst.a, dst.a, dst.a)",
    "(1, 1, 1, 1) - (dst.a, dst.a, dst.a, dst.a)",
    "\\brief Enumeration of the blending equations\n\nThe equations are mapped directly to their OpenGL equivalents,\nspecified by glBlendEquation() or glBlendEquationSeparate().",
    "Pixel = Src * SrcFactor + Dst * DstFactor",
    "Pixel = Src * SrcFactor - Dst * DstFactor",
    "Pixel = Dst * DstFactor - Src * SrcFactor",
    "Pixel = min(Dst, Src)",
    "Pixel = max(Dst, Src)",
    "Blend source and dest according to dest alpha",
    "Add source to dest",
    "Multiply source and dest",
    "Take minimum between source and dest",
    "Take maximum between source and dest",
    "Overwrite dest with source",
}; }

void bind_BlendMode(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__BlendMode = lua_glue::BindClass<sf::BlendMode>(sf, "BlendMode");
    lua_glue::Table table_sf__BlendMode = sf["BlendMode"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::BlendMode>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.BlendMode");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("colorSrcFactor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("colorDstFactor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("colorEquation", "sf.BlendMode.Equation");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("alphaSrcFactor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("alphaDstFactor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("alphaEquation", "sf.BlendMode.Equation");
    LUASF_STUB_DOC(docs[9]);
    LUASF_STUB_FUNCTION("sf.BlendMode", "new", "fun(colorSourceFactor: sf.BlendMode.Factor, colorDestinationFactor: sf.BlendMode.Factor, colorBlendEquation: sf.BlendMode.Equation, alphaSourceFactor: sf.BlendMode.Factor, alphaDestinationFactor: sf.BlendMode.Factor, alphaBlendEquation: sf.BlendMode.Equation): sf.BlendMode");
    LUASF_STUB_OVERLOAD("sf.BlendMode", "new", "fun(sourceFactor: sf.BlendMode.Factor, destinationFactor: sf.BlendMode.Factor, blendEquation?: sf.BlendMode.Equation): sf.BlendMode");
    LUASF_STUB_OVERLOAD("sf.BlendMode", "new", "fun(): sf.BlendMode");
    lua_glue::BindCallable(type_sf__BlendMode, "new",
        [](sf::BlendMode::Factor colorSourceFactor, sf::BlendMode::Factor colorDestinationFactor, sf::BlendMode::Equation colorBlendEquation, sf::BlendMode::Factor alphaSourceFactor, sf::BlendMode::Factor alphaDestinationFactor, sf::BlendMode::Equation alphaBlendEquation) {
            return lua_sf::makeLuaSharedObject<sf::BlendMode>(colorSourceFactor, colorDestinationFactor, colorBlendEquation, alphaSourceFactor, alphaDestinationFactor, alphaBlendEquation);
        },
        docs[9]
    );
    lua_glue::BindCallable(type_sf__BlendMode, "new",
        [](sf::BlendMode::Factor sourceFactor, sf::BlendMode::Factor destinationFactor, sf::BlendMode::Equation blendEquation) {
            return lua_sf::makeLuaSharedObject<sf::BlendMode>(sourceFactor, destinationFactor, blendEquation);
        },
        lua_glue::Defaults{lua_glue::DefaultFactory{[lua]() {
            using namespace sf;
            return static_cast<sf::BlendMode::Equation>(sf::BlendMode::Equation::Add);
        }}},
        docs[8]
    );
    lua_glue::BindCallable(type_sf__BlendMode, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::BlendMode>();
        },
        docs[7]
    );
    lua_glue::BindAttr<sf::BlendMode::Factor>(type_sf__BlendMode, "colorSrcFactor", &sf::BlendMode::colorSrcFactor);
    lua_glue::BindAttr<sf::BlendMode::Factor>(type_sf__BlendMode, "colorDstFactor", &sf::BlendMode::colorDstFactor);
    lua_glue::BindAttr<sf::BlendMode::Equation>(type_sf__BlendMode, "colorEquation", &sf::BlendMode::colorEquation);
    lua_glue::BindAttr<sf::BlendMode::Factor>(type_sf__BlendMode, "alphaSrcFactor", &sf::BlendMode::alphaSrcFactor);
    lua_glue::BindAttr<sf::BlendMode::Factor>(type_sf__BlendMode, "alphaDstFactor", &sf::BlendMode::alphaDstFactor);
    lua_glue::BindAttr<sf::BlendMode::Equation>(type_sf__BlendMode, "alphaEquation", &sf::BlendMode::alphaEquation);
    LUASF_STUB_DOC(docs[10]);
    LUASF_STUB_CLASS("sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[11]);
    LUASF_STUB_FIELD("Zero", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[12]);
    LUASF_STUB_FIELD("One", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FIELD("SrcColor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_FIELD("OneMinusSrcColor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[15]);
    LUASF_STUB_FIELD("DstColor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[16]);
    LUASF_STUB_FIELD("OneMinusDstColor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[17]);
    LUASF_STUB_FIELD("SrcAlpha", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[18]);
    LUASF_STUB_FIELD("OneMinusSrcAlpha", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[19]);
    LUASF_STUB_FIELD("DstAlpha", "sf.BlendMode.Factor");
    LUASF_STUB_DOC(docs[20]);
    LUASF_STUB_FIELD("OneMinusDstAlpha", "sf.BlendMode.Factor");
    lua_glue::BindEnum<sf::BlendMode::Factor>(table_sf__BlendMode, "Factor", {
        {"Zero", sf::BlendMode::Factor::Zero},
        {"One", sf::BlendMode::Factor::One},
        {"SrcColor", sf::BlendMode::Factor::SrcColor},
        {"OneMinusSrcColor", sf::BlendMode::Factor::OneMinusSrcColor},
        {"DstColor", sf::BlendMode::Factor::DstColor},
        {"OneMinusDstColor", sf::BlendMode::Factor::OneMinusDstColor},
        {"SrcAlpha", sf::BlendMode::Factor::SrcAlpha},
        {"OneMinusSrcAlpha", sf::BlendMode::Factor::OneMinusSrcAlpha},
        {"DstAlpha", sf::BlendMode::Factor::DstAlpha},
        {"OneMinusDstAlpha", sf::BlendMode::Factor::OneMinusDstAlpha}
    });
    LUASF_STUB_DOC(docs[21]);
    LUASF_STUB_CLASS("sf.BlendMode.Equation");
    LUASF_STUB_DOC(docs[22]);
    LUASF_STUB_FIELD("Add", "sf.BlendMode.Equation");
    LUASF_STUB_DOC(docs[23]);
    LUASF_STUB_FIELD("Subtract", "sf.BlendMode.Equation");
    LUASF_STUB_DOC(docs[24]);
    LUASF_STUB_FIELD("ReverseSubtract", "sf.BlendMode.Equation");
    LUASF_STUB_DOC(docs[25]);
    LUASF_STUB_FIELD("Min", "sf.BlendMode.Equation");
    LUASF_STUB_DOC(docs[26]);
    LUASF_STUB_FIELD("Max", "sf.BlendMode.Equation");
    lua_glue::BindEnum<sf::BlendMode::Equation>(table_sf__BlendMode, "Equation", {
        {"Add", sf::BlendMode::Equation::Add},
        {"Subtract", sf::BlendMode::Equation::Subtract},
        {"ReverseSubtract", sf::BlendMode::Equation::ReverseSubtract},
        {"Min", sf::BlendMode::Equation::Min},
        {"Max", sf::BlendMode::Equation::Max}
    });
    LUASF_STUB_DOC(docs[27]);
    LUASF_STUB_VALUE("sf", "BlendAlpha", "sf.BlendMode");
    lua_glue::BindStaticAttr<const sf::BlendMode>(sf, "BlendAlpha", &sf::BlendAlpha);
    LUASF_STUB_DOC(docs[28]);
    LUASF_STUB_VALUE("sf", "BlendAdd", "sf.BlendMode");
    lua_glue::BindStaticAttr<const sf::BlendMode>(sf, "BlendAdd", &sf::BlendAdd);
    LUASF_STUB_DOC(docs[29]);
    LUASF_STUB_VALUE("sf", "BlendMultiply", "sf.BlendMode");
    lua_glue::BindStaticAttr<const sf::BlendMode>(sf, "BlendMultiply", &sf::BlendMultiply);
    LUASF_STUB_DOC(docs[30]);
    LUASF_STUB_VALUE("sf", "BlendMin", "sf.BlendMode");
    lua_glue::BindStaticAttr<const sf::BlendMode>(sf, "BlendMin", &sf::BlendMin);
    LUASF_STUB_DOC(docs[31]);
    LUASF_STUB_VALUE("sf", "BlendMax", "sf.BlendMode");
    lua_glue::BindStaticAttr<const sf::BlendMode>(sf, "BlendMax", &sf::BlendMax);
    LUASF_STUB_DOC(docs[32]);
    LUASF_STUB_VALUE("sf", "BlendNone", "sf.BlendMode");
    lua_glue::BindStaticAttr<const sf::BlendMode>(sf, "BlendNone", &sf::BlendNone);
}
