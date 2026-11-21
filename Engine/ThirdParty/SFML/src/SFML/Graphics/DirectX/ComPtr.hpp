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

#include <utility>

namespace sf::priv
{
// A small COM owner shared by the MSVC and MinGW implementations.
template <typename T>
class ComPtr
{
public:
    ComPtr() = default;
    ~ComPtr()
    {
        reset();
    }

    ComPtr(const ComPtr&)            = delete;
    ComPtr& operator=(const ComPtr&) = delete;
    ComPtr(ComPtr&& other) noexcept : m_pointer(std::exchange(other.m_pointer, nullptr))
    {
    }
    ComPtr& operator=(ComPtr&& other) noexcept
    {
        if (this != &other)
        {
            reset();
            m_pointer = std::exchange(other.m_pointer, nullptr);
        }
        return *this;
    }

    [[nodiscard]] T* get() const
    {
        return m_pointer;
    }
    [[nodiscard]] T* operator->() const
    {
        return m_pointer;
    }
    explicit operator bool() const
    {
        return m_pointer != nullptr;
    }
    [[nodiscard]] T** put()
    {
        reset();
        return &m_pointer;
    }
    void reset()
    {
        if (auto* pointer = std::exchange(m_pointer, nullptr))
            pointer->Release();
    }

private:
    T* m_pointer{};
};
} // namespace sf::priv
