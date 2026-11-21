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
namespace sf::priv
{
class OpenGLRenderTarget;
class OpenGLTexture final : public TextureResource
{
public:
    [[nodiscard]] const void* implementationType() const override
    {
        return resourceType<OpenGLTexture>();
    }
    explicit OpenGLTexture(std::shared_ptr<void> context) : m_context(std::move(context))
    {
    }
    ~OpenGLTexture() override;
    bool                       resizeFloat(Vector2u size, const float* pixels);
    bool                       uploadFloats(const float* pixels, Vector2u size, Vector2u offset) override;
    bool                       resize(Vector2u size, bool srgb);
    bool                       upload(const std::uint8_t* pixels, Vector2u size, Vector2u dest) override;
    bool                       copy(const rhi::Texture& source, Vector2u dest) override;
    Image                      read() const override;
    void                       setSmooth(bool smooth) override;
    void                       setRepeated(bool repeated) override;
    bool                       generateMipmap() override;
    void                       invalidateMipmap() override;
    [[nodiscard]] unsigned int nativeHandle() const override
    {
        return m_texture;
    }
    bool                copyWindow(Vector2u size, Vector2u dest);
    static unsigned int maximumSize();

private:
    friend class OpenGLRenderTarget;
    static unsigned int   getValidSize(unsigned int size);
    std::shared_ptr<void> m_context;
    unsigned int          m_texture{};
    bool                  m_float{};
};
} // namespace sf::priv
