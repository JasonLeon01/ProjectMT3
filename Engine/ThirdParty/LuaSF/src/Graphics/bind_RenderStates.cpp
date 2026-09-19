#include "Graphics/bind_RenderStates.hpp"

#include <algorithm>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

void bind_RenderStates(sol::state_view lua) {
    sol::table sf = lua_sf::sf_table(lua);
    auto type_sf__RenderStates = sf.new_usertype<sf::RenderStates>("RenderStates", sol::no_constructor);
    sol::table table_sf__RenderStates = sf["RenderStates"].get<sol::table>();
    lua_sf::mark_shared_usertype<sf::RenderStates>(lua);
    LUASF_STUB_DOC("\\brief Define the states used for drawing to a `RenderTarget`");
    LUASF_STUB_CLASS("sf.RenderStates");
    LUASF_STUB_DOC("Blending mode");
    LUASF_STUB_FIELD("blendMode", "sf.BlendMode");
    LUASF_STUB_DOC("Stencil mode");
    LUASF_STUB_FIELD("stencilMode", "sf.StencilMode");
    LUASF_STUB_DOC("Transform");
    LUASF_STUB_FIELD("transform", "sf.Transform");
    LUASF_STUB_DOC("Texture coordinate type");
    LUASF_STUB_FIELD("coordinateType", "sf.CoordinateType");
    LUASF_STUB_DOC("Texture");
    LUASF_STUB_FIELD("texture", "sf.Texture");
    LUASF_STUB_DOC("Shader");
    LUASF_STUB_FIELD("shader", "sf.Shader");
    LUASF_STUB_DOC("\\brief Construct a set of render states with all its attributes\n\n\\param theBlendMode      Blend mode to use\n\\param theStencilMode    Stencil mode to use\n\\param theTransform      Transform to use\n\\param theCoordinateType Texture coordinate type to use\n\\param theTexture        Texture to use\n\\param theShader         Shader to use");
    LUASF_STUB_FUNCTION("sf.RenderStates", "new", "fun(theBlendMode: sf.BlendMode, theStencilMode: sf.StencilMode, theTransform: sf.Transform, theCoordinateType: sf.CoordinateType, theTexture: sf.Texture, theShader: sf.Shader): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(theBlendMode: sf.BlendMode): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(theStencilMode: sf.StencilMode): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(theTransform: sf.Transform): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(theTexture: sf.Texture): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(theShader: sf.Shader): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(): sf.RenderStates");
    type_sf__RenderStates.set_function("new", sol::factories(
        [](const sf::BlendMode& theBlendMode, const sf::StencilMode& theStencilMode, const sf::Transform& theTransform, sf::CoordinateType theCoordinateType, const sf::Texture* theTexture, const sf::Shader* theShader) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theBlendMode, theStencilMode, theTransform, theCoordinateType, theTexture, theShader);
        },
        [](const sf::BlendMode& theBlendMode) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theBlendMode);
        },
        [](const sf::StencilMode& theStencilMode) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theStencilMode);
        },
        [](const sf::Transform& theTransform) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theTransform);
        },
        [](const sf::Texture* theTexture) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theTexture);
        },
        [](const sf::Shader* theShader) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theShader);
        },
        []() {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>();
        }
    ));
    type_sf__RenderStates["blendMode"] = sol::policies(&sf::RenderStates::blendMode, sol::self_dependency{});
    type_sf__RenderStates["stencilMode"] = sol::policies(&sf::RenderStates::stencilMode, sol::self_dependency{});
    type_sf__RenderStates["transform"] = sol::policies(&sf::RenderStates::transform, sol::self_dependency{});
    type_sf__RenderStates["coordinateType"] = sol::policies(&sf::RenderStates::coordinateType, sol::self_dependency{});
    type_sf__RenderStates["texture"] = sol::policies(&sf::RenderStates::texture, sol::self_dependency{});
    type_sf__RenderStates["shader"] = sol::policies(&sf::RenderStates::shader, sol::self_dependency{});
    LUASF_STUB_DOC("Special instance holding the default render states");
    LUASF_STUB_VALUE("sf.RenderStates", "Default", "sf.RenderStates");
    table_sf__RenderStates["Default"] = sf::RenderStates::Default;
}
