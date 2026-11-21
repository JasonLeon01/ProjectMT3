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

#include <SFML/Graphics/RHI/Access.hpp>

namespace sf::rhi
{
std::shared_ptr<Texture> getTexture(const sf::Texture& texture)
{
    return priv::RhiAccess::texture(texture);
}
std::shared_ptr<ShaderProgram> getShader(const sf::Shader& shader)
{
    return priv::RhiAccess::shader(shader);
}
void setShader(sf::Shader& shader, std::shared_ptr<ShaderProgram> program)
{
    priv::RhiAccess::shader(shader, std::move(program));
}
std::shared_ptr<RenderTarget> getRenderTarget(sf::RenderTarget& target)
{
    return priv::RhiAccess::target(target);
}
TargetState getTargetState(const sf::RenderTarget& target)
{
    return priv::prepareTarget(target);
}
} // namespace sf::rhi
