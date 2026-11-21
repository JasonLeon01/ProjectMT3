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
#include <SFML/Graphics/GLExtensions.hpp>
#include <SFML/Graphics/RHI/Device.hpp>
namespace sf::priv
{
class OpenGLTimer final : public rhi::Timer
{
public:
    static bool                         isAvailable();
    static std::shared_ptr<OpenGLTimer> create(std::shared_ptr<void> context);
    ~OpenGLTimer() override;
    void                  end() override;
    std::optional<double> elapsedMilliseconds() override;

private:
    struct Functions
    {
        decltype(glGenQueries)          generate{};
        decltype(glDeleteQueries)       erase{};
        decltype(glBeginQuery)          begin{};
        decltype(glEndQuery)            end{};
        decltype(glGetQueryiv)          current{};
        decltype(glGetQueryObjectuiv)   ready{};
        decltype(glGetQueryObjectui64v) result{};
    };
    static Functions      functions();
    Functions             m_functions;
    std::shared_ptr<void> m_context;
    GLuint                m_query{};
    std::uint64_t         m_contextId{};
    bool                  m_ended{};
};
} // namespace sf::priv
