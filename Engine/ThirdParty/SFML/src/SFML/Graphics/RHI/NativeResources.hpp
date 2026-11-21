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
// Native devices differ only in whether their image operations accept a reference
// or a retaining shared_ptr. Keep that adaptation out of the SFML resource classes.
#define SFML_IMAGE_OPERATION(name)                                                                      \
    template <typename DeviceType, typename ImageType, typename... Args>                                \
    auto name##Image(DeviceType& device, const std::shared_ptr<ImageType>& image, int, Args&&... args)  \
        -> decltype(device.name(*image, std::forward<Args>(args)...))                                   \
    {                                                                                                   \
        return device.name(*image, std::forward<Args>(args)...);                                        \
    }                                                                                                   \
    template <typename DeviceType, typename ImageType, typename... Args>                                \
    auto name##Image(DeviceType& device, const std::shared_ptr<ImageType>& image, long, Args&&... args) \
        -> decltype(device.name(image, std::forward<Args>(args)...))                                    \
    {                                                                                                   \
        return device.name(image, std::forward<Args>(args)...);                                         \
    }
// Some MSVC clang-tidy versions miss std::forward in these generated parameter-pack wrappers.
// NOLINTBEGIN(cppcoreguidelines-missing-std-forward)
SFML_IMAGE_OPERATION(upload)
SFML_IMAGE_OPERATION(readImage)
SFML_IMAGE_OPERATION(generateMipmap)
// NOLINTEND(cppcoreguidelines-missing-std-forward)
#undef SFML_IMAGE_OPERATION

template <typename DeviceType, typename ImageType>
auto copyImage(DeviceType&                       device,
               const std::shared_ptr<ImageType>& source,
               const std::shared_ptr<ImageType>& destination,
               Vector2u                          offset,
               int) -> decltype(device.copy(*source, *destination, offset))
{
    return device.copy(*source, *destination, offset);
}
template <typename DeviceType, typename ImageType>
auto copyImage(DeviceType&                       device,
               const std::shared_ptr<ImageType>& source,
               const std::shared_ptr<ImageType>& destination,
               Vector2u                          offset,
               long) -> decltype(device.copy(source, destination, offset))
{
    return device.copy(source, destination, offset);
}

template <typename DeviceType, typename ImageType>
auto uploadFloatImage(DeviceType&                       device,
                      const std::shared_ptr<ImageType>& image,
                      const float*                      pixels,
                      Vector2u                          size,
                      Vector2u                          offset,
                      int) -> decltype(device.uploadFloats(*image, pixels, size, offset))
{
    return device.uploadFloats(*image, pixels, size, offset);
}
template <typename DeviceType, typename ImageType>
auto uploadFloatImage(DeviceType&                       device,
                      const std::shared_ptr<ImageType>& image,
                      const float*                      pixels,
                      Vector2u                          size,
                      Vector2u                          offset,
                      long) -> decltype(device.uploadFloats(image, pixels, size, offset))
{
    return device.uploadFloats(image, pixels, size, offset);
}
template <typename DeviceType, typename ImageType>
bool uploadFloatImage(DeviceType&, const std::shared_ptr<ImageType>&, const float*, Vector2u, Vector2u, ...)
{
    return false;
}

template <typename DeviceType, typename ImageType>
class NativeTexture final : public TextureResource
{
public:
    [[nodiscard]] const void* implementationType() const override
    {
        return resourceType<NativeTexture>();
    }
    NativeTexture(std::shared_ptr<DeviceType> owner, std::shared_ptr<ImageType> value, Vector2u size, bool srgb) :
        device(std::move(owner)),
        image(std::move(value))
    {
        m_size = m_actualSize = size;
        m_sRgb                = srgb;
    }
    bool upload(const std::uint8_t* pixels, Vector2u size, Vector2u offset) override
    {
        if (!pixels || !size.x || !size.y || !fits(size, offset))
            return false;
        const auto lock = device->lock();
        if (!uploadImage(*device, image, 0, pixels, size, offset))
            return false;
        setGlslFlipped(false);
        changed();
        return true;
    }
    bool uploadFloats(const float* pixels, Vector2u size, Vector2u offset) override
    {
        if (!pixels || !fits(size, offset))
            return false;
        const auto lock = device->lock();
        if (!uploadFloatImage(*device, image, pixels, size, offset, 0))
            return false;
        setGlslFlipped(false);
        changed();
        return true;
    }
    bool copy(const rhi::Texture& source, Vector2u offset) override
    {
        const auto* other = resourceCast<const NativeTexture>(&source);
        if (!other || other->device != device || !fits(other->getSize(), offset))
            return false;
        const auto lock = device->lock();
        if (!copyImage(*device, other->image, image, offset, 0))
            return false;
        setGlslFlipped(false);
        changed();
        return true;
    }
    Image read() const override
    {
        const auto lock = device->lock();
        return readImageImage(*device, image, 0);
    }
    bool generateMipmap() override
    {
        const auto lock = device->lock();
        m_hasMipmap     = generateMipmapImage(*device, image, 0);
        return m_hasMipmap;
    }
    std::shared_ptr<DeviceType> device;
    std::shared_ptr<ImageType>  image;

private:
    [[nodiscard]] bool fits(Vector2u size, Vector2u offset) const
    {
        return offset.x <= m_size.x && offset.y <= m_size.y && size.x <= m_size.x - offset.x &&
               size.y <= m_size.y - offset.y;
    }
};

template <typename DeviceType, typename BufferType>
class NativeBuffer final : public BufferResource
{
public:
    [[nodiscard]] const void* implementationType() const override
    {
        return resourceType<NativeBuffer>();
    }
    NativeBuffer(std::shared_ptr<DeviceType> owner, std::shared_ptr<BufferType> value, std::size_t size) :
        device(std::move(owner)),
        buffer(std::move(value)),
        bytes(size)
    {
    }
    [[nodiscard]] std::size_t getSize() const override
    {
        return bytes;
    }
    bool               upload(const void* data, std::size_t size, std::size_t offset) override;
    bool               copy(const rhi::Buffer& source) override;
    bool               read(void* data, std::size_t size, std::size_t offset) const override;
    [[nodiscard]] bool fits(std::size_t size, std::size_t offset) const
    {
        return offset <= bytes && size <= bytes - offset;
    }
    std::shared_ptr<DeviceType> device;
    std::shared_ptr<BufferType> buffer;
    std::size_t                 bytes;
};

template <typename Texture, typename Surface>
class NativeSurface final : public SurfaceResource
{
public:
    explicit NativeSurface(std::unique_ptr<Surface> value) : surface(std::move(value))
    {
    }
    [[nodiscard]] std::shared_ptr<rhi::RenderTarget> getRenderTarget() const override
    {
        return surface->renderTarget();
    }
    bool setActive(bool active) override
    {
        return surface->setActive(active);
    }
    void resize(Vector2u size) override
    {
        surface->resize(size);
    }
    void display() override
    {
        surface->display();
    }
    void setVerticalSyncEnabled(bool value) override
    {
        surface->setVerticalSyncEnabled(value);
    }
    bool copyTo(rhi::Texture& target, Vector2u offset) const override
    {
        auto* texture = resourceCast<Texture>(&target);
        if (!texture || !surface->copyTo(texture->image, offset))
            return false;
        texture->setGlslFlipped(true);
        texture->changed();
        return true;
    }
#ifdef SFML_SYSTEM_ANDROID
    void setNativeWindow(WindowHandle handle) override
    {
        surface->setNativeWindow(handle);
    }
#endif
    std::unique_ptr<Surface> surface;
};

template <typename DeviceType, typename ImageType, typename BufferType, typename ProgramType, typename TargetType>
class NativeDeviceResource : public DeviceResource
{
public:
    using DeviceResource::createShader;

    NativeDeviceResource(std::shared_ptr<DeviceType> value, GraphicsBackend backend, ContextSettings::BackendSettings settings) :
        m_native(std::move(value)),
        m_backend(backend),
        m_settings(settings)
    {
    }
    [[nodiscard]] GraphicsBackend backend() const override
    {
        return m_backend;
    }
    [[nodiscard]] ContextSettings::BackendSettings settings() const override
    {
        return m_settings;
    }
    std::shared_ptr<rhi::Texture> createTexture(Vector2u size, bool srgb) override
    {
        const auto limit = capabilities().maximumTextureSize;
        if (!size.x || !size.y || size.x > limit || size.y > limit)
            return {};
        const auto lock  = m_native->lock();
        auto       image = m_native->makeTexture(size, srgb);
        return image ? std::make_shared<NativeTexture<DeviceType, ImageType>>(m_native, std::move(image), size, srgb)
                     : nullptr;
    }
    std::shared_ptr<rhi::Buffer> createBuffer(std::size_t bytes, rhi::BufferUsage) override
    {
        const auto lock   = m_native->lock();
        auto       buffer = m_native->makeBuffer(bytes);
        return buffer ? std::make_shared<NativeBuffer<DeviceType, BufferType>>(m_native, std::move(buffer), bytes) : nullptr;
    }
    std::shared_ptr<rhi::ShaderProgram> createShader(std::string_view vertex,
                                                     std::string_view geometry,
                                                     std::string_view fragment) override
    {
        if ((!geometry.empty() && !capabilities().geometryShaders) ||
            (vertex.empty() && geometry.empty() && fragment.empty()))
            return {};
        const auto lock = m_native->lock();
        return ProgramType::create(*m_native, vertex, geometry, fragment);
    }
    std::shared_ptr<rhi::RenderTarget> createRenderTarget(std::shared_ptr<rhi::Texture> color,
                                                          const ContextSettings&        settings) override
    {
        const auto texture = resourcePointerCast<NativeTexture<DeviceType, ImageType>>(color);
        if (!texture || texture->device != m_native || !acceptsSettings(settings))
            return {};
        auto target = TargetType::create(m_native, texture->image, settings);
        if (target)
            target->setColorTexture(std::move(color));
        return target;
    }

protected:
    std::shared_ptr<DeviceType> m_native;

private:
    GraphicsBackend                  m_backend;
    ContextSettings::BackendSettings m_settings;
};
} // namespace sf::priv
