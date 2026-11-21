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

#include <SFML/Graphics/GLExtensions.hpp>
#include <SFML/Graphics/OpenGL/OpenGLResource.hpp>
#include <SFML/Graphics/OpenGL/OpenGLVertexArray.hpp>

#include <SFML/Window/Context.hpp>

#include <iterator>
#include <mutex>
#include <unordered_map>

#include <cstdint>
namespace sf::priv
{
OpenGLVertexArray::OpenGLVertexArray()
{
    glGenVertexArrays(1, &handle);
}
OpenGLVertexArray::~OpenGLVertexArray()
{
    glDeleteVertexArrays(1, &handle);
}
std::shared_ptr<OpenGLVertexArray> OpenGLVertexArray::acquire()
{
    static std::mutex                                                          mutex;
    static std::unordered_map<std::uint64_t, std::weak_ptr<OpenGLVertexArray>> arrays;
    const std::scoped_lock                                                     guard(mutex);
    for (auto it = arrays.begin(); it != arrays.end();)
        it = it->second.expired() ? arrays.erase(it) : std::next(it);
    auto& weak = arrays[Context::getActiveContextId()];
    if (auto array = weak.lock())
        return array;
    auto array = std::make_shared<OpenGLVertexArray>();
    OpenGLResource::registerUnsharedGlObject(array);
    weak = array;
    return array;
}
} // namespace sf::priv
