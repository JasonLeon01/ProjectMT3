#include "Graphics/bind_RenderStates.hpp"

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

namespace { constexpr std::array<std::string_view, 15> docs = {
    "\\brief Define the states used for drawing to a `RenderTarget`",
    "Blending mode",
    "Stencil mode",
    "Transform",
    "Texture coordinate type",
    "Texture",
    "Shader",
    "\\brief Default constructor\n\nConstructing a default set of render states is equivalent\nto using `sf::RenderStates::Default`.\nThe default set defines:\n\\li the `BlendAlpha` blend mode\n\\li the default `StencilMode` (no stencil)\n\\li the identity transform\n\\li a `nullptr` texture\n\\li a `nullptr` shader",
    "\\brief Construct a default set of render states with a custom blend mode\n\n\\param theBlendMode Blend mode to use",
    "\\brief Construct a default set of render states with a custom stencil mode\n\n\\param theStencilMode Stencil mode to use",
    "\\brief Construct a default set of render states with a custom transform\n\n\\param theTransform Transform to use",
    "\\brief Construct a default set of render states with a custom texture\n\n\\param theTexture Texture to use",
    "\\brief Construct a default set of render states with a custom shader\n\n\\param theShader Shader to use",
    "\\brief Construct a set of render states with all its attributes\n\n\\param theBlendMode      Blend mode to use\n\\param theStencilMode    Stencil mode to use\n\\param theTransform      Transform to use\n\\param theCoordinateType Texture coordinate type to use\n\\param theTexture        Texture to use\n\\param theShader         Shader to use",
    "Special instance holding the default render states",
}; }

void bind_RenderStates(lua_glue::StateView lua) {
    lua_glue::Table sf = lua_sf::sf_table(lua);
    auto type_sf__RenderStates = lua_glue::BindClass<sf::RenderStates>(sf, "RenderStates");
    lua_glue::Table table_sf__RenderStates = sf["RenderStates"].get<lua_glue::Table>();
    lua_sf::mark_shared_usertype<sf::RenderStates>(lua);
    LUASF_STUB_DOC(docs[0]);
    LUASF_STUB_CLASS("sf.RenderStates");
    LUASF_STUB_DOC(docs[1]);
    LUASF_STUB_FIELD("blendMode", "sf.BlendMode");
    LUASF_STUB_DOC(docs[2]);
    LUASF_STUB_FIELD("stencilMode", "sf.StencilMode");
    LUASF_STUB_DOC(docs[3]);
    LUASF_STUB_FIELD("transform", "sf.Transform");
    LUASF_STUB_DOC(docs[4]);
    LUASF_STUB_FIELD("coordinateType", "sf.CoordinateType");
    LUASF_STUB_DOC(docs[5]);
    LUASF_STUB_FIELD("texture", "sf.Texture");
    LUASF_STUB_DOC(docs[6]);
    LUASF_STUB_FIELD("shader", "sf.Shader");
    LUASF_STUB_DOC(docs[13]);
    LUASF_STUB_FUNCTION("sf.RenderStates", "new", "fun(theBlendMode: sf.BlendMode, theStencilMode: sf.StencilMode, theTransform: sf.Transform, theCoordinateType: sf.CoordinateType, theTexture: sf.Texture, theShader: sf.Shader): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(theBlendMode: sf.BlendMode): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(theStencilMode: sf.StencilMode): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(theTransform: sf.Transform): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(theTexture: sf.Texture): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(theShader: sf.Shader): sf.RenderStates");
    LUASF_STUB_OVERLOAD("sf.RenderStates", "new", "fun(): sf.RenderStates");
    lua_glue::BindCallable(type_sf__RenderStates, "new",
        [](const sf::BlendMode& theBlendMode, const sf::StencilMode& theStencilMode, const sf::Transform& theTransform, sf::CoordinateType theCoordinateType, const sf::Texture* theTexture, const sf::Shader* theShader) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theBlendMode, theStencilMode, theTransform, theCoordinateType, theTexture, theShader);
        },
        docs[13]
    );
    lua_glue::BindCallable(type_sf__RenderStates, "new",
        [](const sf::BlendMode& theBlendMode) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theBlendMode);
        },
        docs[8]
    );
    lua_glue::BindCallable(type_sf__RenderStates, "new",
        [](const sf::StencilMode& theStencilMode) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theStencilMode);
        },
        docs[9]
    );
    lua_glue::BindCallable(type_sf__RenderStates, "new",
        [](const sf::Transform& theTransform) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theTransform);
        },
        docs[10]
    );
    lua_glue::BindCallable(type_sf__RenderStates, "new",
        [](const sf::Texture* theTexture) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theTexture);
        },
        docs[11]
    );
    lua_glue::BindCallable(type_sf__RenderStates, "new",
        [](const sf::Shader* theShader) {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>(theShader);
        },
        docs[12]
    );
    lua_glue::BindCallable(type_sf__RenderStates, "new",
        []() {
            return lua_sf::makeLuaSharedObject<sf::RenderStates>();
        },
        docs[7]
    );
    lua_glue::BindAttr<sf::BlendMode>(type_sf__RenderStates, "blendMode", &sf::RenderStates::blendMode);
    lua_glue::BindAttr<sf::StencilMode>(type_sf__RenderStates, "stencilMode", &sf::RenderStates::stencilMode);
    lua_glue::BindAttr<sf::Transform>(type_sf__RenderStates, "transform", &sf::RenderStates::transform);
    lua_glue::BindAttr<sf::CoordinateType>(type_sf__RenderStates, "coordinateType", &sf::RenderStates::coordinateType);
    lua_glue::BindAttr<const sf::Texture*>(type_sf__RenderStates, "texture", &sf::RenderStates::texture);
    lua_glue::BindAttr<const sf::Shader*>(type_sf__RenderStates, "shader", &sf::RenderStates::shader);
    LUASF_STUB_DOC(docs[14]);
    LUASF_STUB_VALUE("sf.RenderStates", "Default", "sf.RenderStates");
    lua_glue::BindStaticAttr<const sf::RenderStates>(table_sf__RenderStates, "Default", &sf::RenderStates::Default);
}
