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

#include <memory>

#include <cstdint>

namespace sf::priv
{
// The current context owns this state, including its non-shareable vertex arrays.
class OpenGLContextState
{
public:
    OpenGLContextState();
    ~OpenGLContextState();
    OpenGLContextState(const OpenGLContextState&)            = delete;
    OpenGLContextState& operator=(const OpenGLContextState&) = delete;

    static OpenGLContextState& get();
    void                       bindStream();
    void                       bindVertexBuffer(unsigned int buffer);
    void                       bindCustomLayout();
    [[nodiscard]] unsigned int maximumVertexAttributes();

    std::uint64_t activeTarget{};

private:
    const std::shared_ptr<void>* m_dispatchSlot;
    unsigned int                 m_stream{};
    unsigned int                 m_streamArray{};
    unsigned int                 m_bufferArray{};
    unsigned int                 m_customArray{};
    int                          m_maximumVertexAttributes{};
};
} // namespace sf::priv
