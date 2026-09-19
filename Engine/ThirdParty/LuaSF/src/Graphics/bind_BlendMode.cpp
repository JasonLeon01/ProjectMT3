#include "Graphics/bind_BlendMode.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_BlendMode(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__BlendMode = sf.new_usertype<sf::BlendMode>("BlendMode", sol::no_constructor);
    sol::table table_sf__BlendMode = sf["BlendMode"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::BlendMode>(lua);
    LUASF_STUB_DOC("\\brief Blending modes for drawing");
    LUASF_STUB_CLASS("sf.BlendMode");
    LUASF_STUB_DOC("Source blending factor for the color channels");
    LUASF_STUB_FIELD("colorSrcFactor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("Destination blending factor for the color channels");
    LUASF_STUB_FIELD("colorDstFactor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("Blending equation for the color channels");
    LUASF_STUB_FIELD("colorEquation", "sf.BlendMode.Equation");
    LUASF_STUB_DOC("Source blending factor for the alpha channel");
    LUASF_STUB_FIELD("alphaSrcFactor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("Destination blending factor for the alpha channel");
    LUASF_STUB_FIELD("alphaDstFactor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("Blending equation for the alpha channel");
    LUASF_STUB_FIELD("alphaEquation", "sf.BlendMode.Equation");
    LUASF_STUB_DOC("\\brief Construct the blend mode given the factors and equation.\n\n\\param colorSourceFactor      Specifies how to compute the source factor for the color channels.\n\\param colorDestinationFactor Specifies how to compute the destination factor for the color channels.\n\\param colorBlendEquation     Specifies how to combine the source and destination colors.\n\\param alphaSourceFactor      Specifies how to compute the source factor.\n\\param alphaDestinationFactor Specifies how to compute the destination factor.\n\\param alphaBlendEquation     Specifies how to combine the source and destination alphas.");
    LUASF_STUB_FUNCTION("sf.BlendMode", "new", "fun(colorSourceFactor: sf.BlendMode.Factor, colorDestinationFactor: sf.BlendMode.Factor, colorBlendEquation: sf.BlendMode.Equation, alphaSourceFactor: sf.BlendMode.Factor, alphaDestinationFactor: sf.BlendMode.Factor, alphaBlendEquation: sf.BlendMode.Equation): sf.BlendMode");
    LUASF_STUB_OVERLOAD("sf.BlendMode", "new", "fun(sourceFactor: sf.BlendMode.Factor, destinationFactor: sf.BlendMode.Factor, blendEquation: sf.BlendMode.Equation): sf.BlendMode");
    LUASF_STUB_OVERLOAD("sf.BlendMode", "new", "fun(sourceFactor: sf.BlendMode.Factor, destinationFactor: sf.BlendMode.Factor): sf.BlendMode");
    LUASF_STUB_OVERLOAD("sf.BlendMode", "new", "fun(): sf.BlendMode");
    type_sf__BlendMode.set_function("new", sol::factories(
        [](sf::BlendMode::Factor colorSourceFactor, sf::BlendMode::Factor colorDestinationFactor, sf::BlendMode::Equation colorBlendEquation, sf::BlendMode::Factor alphaSourceFactor, sf::BlendMode::Factor alphaDestinationFactor, sf::BlendMode::Equation alphaBlendEquation) {
            return lua_sf::makeLuaSharedObject<sf::BlendMode>(colorSourceFactor, colorDestinationFactor, colorBlendEquation, alphaSourceFactor, alphaDestinationFactor, alphaBlendEquation);
        },
        [](sf::BlendMode::Factor sourceFactor, sf::BlendMode::Factor destinationFactor, sf::BlendMode::Equation blendEquation) {
            return lua_sf::makeLuaSharedObject<sf::BlendMode>(sourceFactor, destinationFactor, blendEquation);
        },
        [](sf::BlendMode::Factor sourceFactor, sf::BlendMode::Factor destinationFactor) {
            return lua_sf::makeLuaSharedObject<sf::BlendMode>(sourceFactor, destinationFactor);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::BlendMode>();
        }
    ));
    type_sf__BlendMode["colorSrcFactor"] = sol::policies(&sf::BlendMode::colorSrcFactor, sol::self_dependency{});
    type_sf__BlendMode["colorDstFactor"] = sol::policies(&sf::BlendMode::colorDstFactor, sol::self_dependency{});
    type_sf__BlendMode["colorEquation"] = sol::policies(&sf::BlendMode::colorEquation, sol::self_dependency{});
    type_sf__BlendMode["alphaSrcFactor"] = sol::policies(&sf::BlendMode::alphaSrcFactor, sol::self_dependency{});
    type_sf__BlendMode["alphaDstFactor"] = sol::policies(&sf::BlendMode::alphaDstFactor, sol::self_dependency{});
    type_sf__BlendMode["alphaEquation"] = sol::policies(&sf::BlendMode::alphaEquation, sol::self_dependency{});
    LUASF_STUB_DOC("\\brief Enumeration of the blending factors\n\nThe factors are mapped directly to their OpenGL equivalents,\nspecified by glBlendFunc() or glBlendFuncSeparate().");
    LUASF_STUB_CLASS("sf.BlendMode.Factor");
    LUASF_STUB_DOC("(0, 0, 0, 0)");
    LUASF_STUB_FIELD("Zero", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("(1, 1, 1, 1)");
    LUASF_STUB_FIELD("One", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("(src.r, src.g, src.b, src.a)");
    LUASF_STUB_FIELD("SrcColor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("(1, 1, 1, 1) - (src.r, src.g, src.b, src.a)");
    LUASF_STUB_FIELD("OneMinusSrcColor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("(dst.r, dst.g, dst.b, dst.a)");
    LUASF_STUB_FIELD("DstColor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("(1, 1, 1, 1) - (dst.r, dst.g, dst.b, dst.a)");
    LUASF_STUB_FIELD("OneMinusDstColor", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("(src.a, src.a, src.a, src.a)");
    LUASF_STUB_FIELD("SrcAlpha", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("(1, 1, 1, 1) - (src.a, src.a, src.a, src.a)");
    LUASF_STUB_FIELD("OneMinusSrcAlpha", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("(dst.a, dst.a, dst.a, dst.a)");
    LUASF_STUB_FIELD("DstAlpha", "sf.BlendMode.Factor");
    LUASF_STUB_DOC("(1, 1, 1, 1) - (dst.a, dst.a, dst.a, dst.a)");
    LUASF_STUB_FIELD("OneMinusDstAlpha", "sf.BlendMode.Factor");
    table_sf__BlendMode.new_enum("Factor",
        "Zero", sf::BlendMode::Factor::Zero,
        "One", sf::BlendMode::Factor::One,
        "SrcColor", sf::BlendMode::Factor::SrcColor,
        "OneMinusSrcColor", sf::BlendMode::Factor::OneMinusSrcColor,
        "DstColor", sf::BlendMode::Factor::DstColor,
        "OneMinusDstColor", sf::BlendMode::Factor::OneMinusDstColor,
        "SrcAlpha", sf::BlendMode::Factor::SrcAlpha,
        "OneMinusSrcAlpha", sf::BlendMode::Factor::OneMinusSrcAlpha,
        "DstAlpha", sf::BlendMode::Factor::DstAlpha,
        "OneMinusDstAlpha", sf::BlendMode::Factor::OneMinusDstAlpha
    );
    LUASF_STUB_DOC("\\brief Enumeration of the blending equations\n\nThe equations are mapped directly to their OpenGL equivalents,\nspecified by glBlendEquation() or glBlendEquationSeparate().");
    LUASF_STUB_CLASS("sf.BlendMode.Equation");
    LUASF_STUB_DOC("Pixel = Src * SrcFactor + Dst * DstFactor");
    LUASF_STUB_FIELD("Add", "sf.BlendMode.Equation");
    LUASF_STUB_DOC("Pixel = Src * SrcFactor - Dst * DstFactor");
    LUASF_STUB_FIELD("Subtract", "sf.BlendMode.Equation");
    LUASF_STUB_DOC("Pixel = Dst * DstFactor - Src * SrcFactor");
    LUASF_STUB_FIELD("ReverseSubtract", "sf.BlendMode.Equation");
    LUASF_STUB_DOC("Pixel = min(Dst, Src)");
    LUASF_STUB_FIELD("Min", "sf.BlendMode.Equation");
    LUASF_STUB_DOC("Pixel = max(Dst, Src)");
    LUASF_STUB_FIELD("Max", "sf.BlendMode.Equation");
    table_sf__BlendMode.new_enum("Equation",
        "Add", sf::BlendMode::Equation::Add,
        "Subtract", sf::BlendMode::Equation::Subtract,
        "ReverseSubtract", sf::BlendMode::Equation::ReverseSubtract,
        "Min", sf::BlendMode::Equation::Min,
        "Max", sf::BlendMode::Equation::Max
    );
    LUASF_STUB_DOC("Blend source and dest according to dest alpha");
    LUASF_STUB_VALUE("sf", "BlendAlpha", "sf.BlendMode");
    sf["BlendAlpha"] = sf::BlendAlpha;
    LUASF_STUB_DOC("Add source to dest");
    LUASF_STUB_VALUE("sf", "BlendAdd", "sf.BlendMode");
    sf["BlendAdd"] = sf::BlendAdd;
    LUASF_STUB_DOC("Multiply source and dest");
    LUASF_STUB_VALUE("sf", "BlendMultiply", "sf.BlendMode");
    sf["BlendMultiply"] = sf::BlendMultiply;
    LUASF_STUB_DOC("Take minimum between source and dest");
    LUASF_STUB_VALUE("sf", "BlendMin", "sf.BlendMode");
    sf["BlendMin"] = sf::BlendMin;
    LUASF_STUB_DOC("Take maximum between source and dest");
    LUASF_STUB_VALUE("sf", "BlendMax", "sf.BlendMode");
    sf["BlendMax"] = sf::BlendMax;
    LUASF_STUB_DOC("Overwrite dest with source");
    LUASF_STUB_VALUE("sf", "BlendNone", "sf.BlendMode");
    sf["BlendNone"] = sf::BlendNone;
}
