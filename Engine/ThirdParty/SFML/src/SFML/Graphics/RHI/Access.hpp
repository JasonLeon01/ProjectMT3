////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2026 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

#pragma once

#include <SFML/Graphics/RHI/Resource.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTargetBackend.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>

#include <SFML/Window/Window.hpp>

namespace sf::priv
{
class RhiAccess
{
public:
    static const std::shared_ptr<rhi::Texture>& texture(const Texture& value)
    {
        return value.m_resource;
    }
    static const std::shared_ptr<rhi::Buffer>& buffer(const VertexBuffer& value)
    {
        return value.m_resource;
    }
    static std::shared_ptr<rhi::ShaderProgram> shader(const Shader& value)
    {
        value.refreshSamplers();
        return value.m_resource;
    }
    static void shader(Shader& value, std::shared_ptr<rhi::ShaderProgram> program)
    {
        value.m_samplers.clear();
        value.m_resource = std::move(program);
    }
    static std::shared_ptr<rhi::RenderTarget> target(RenderTarget& value)
    {
        return value.m_backend;
    }
    static const WindowImpl* window(const WindowBase& value)
    {
        return value.m_impl.get();
    }
    static bool copyWindow(const Window& window, rhi::Texture& texture, Vector2u offset);
};

[[nodiscard]] rhi::DrawSettings prepareState(const RenderStates& states);
[[nodiscard]] rhi::TargetState  prepareTarget(const RenderTarget& target);
} // namespace sf::priv
