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

#include <SFML/System/Vector2.hpp>

#include <memory>
#include <mutex>
#include <optional>
#include <vector>

#include <cstddef>
#include <cstdint>

namespace sf
{
class Image;
}

namespace sf::priv
{
class MetalProgram;
struct MetalImage;
struct MetalBuffer;

class MetalDevice
{
public:
    struct Impl;

    [[nodiscard]] static std::shared_ptr<MetalDevice> create();
    ~MetalDevice();
    MetalDevice(const MetalDevice&)            = delete;
    MetalDevice& operator=(const MetalDevice&) = delete;

    [[nodiscard]] std::unique_lock<std::recursive_mutex> lock() const;
    [[nodiscard]] Impl&                                  impl() const;
    [[nodiscard]] unsigned int                           maximumTextureSize() const;
    [[nodiscard]] unsigned int                           maximumSamples() const;

    [[nodiscard]] std::shared_ptr<MetalImage> makeTexture(Vector2u size, bool sRgb, bool mipmapped = true, bool floating = false);
    [[nodiscard]] std::shared_ptr<MetalBuffer> makeBuffer(std::size_t bytes);
    [[nodiscard]] bool  upload(const MetalImage& image, const std::uint8_t* pixels, Vector2u size, Vector2u offset);
    [[nodiscard]] bool  uploadFloats(const MetalImage& image, const float* pixels, Vector2u size, Vector2u offset);
    [[nodiscard]] bool  copy(const MetalImage& source, const MetalImage& destination, Vector2u offset);
    [[nodiscard]] Image readImage(const MetalImage& image);
    [[nodiscard]] bool  generateMipmap(const MetalImage& image);
    [[nodiscard]] bool  write(const MetalBuffer& buffer, const void* data, std::size_t offset, std::size_t bytes);
    [[nodiscard]] bool  copy(const MetalBuffer& source, const MetalBuffer& destination, std::size_t bytes);
    [[nodiscard]] const std::shared_ptr<MetalImage>&   whiteTexture();
    [[nodiscard]] const std::shared_ptr<MetalProgram>& defaultProgram();
    [[nodiscard]] bool                                 submit(bool wait);

    [[nodiscard]] bool                                     loadPipelineCache(const void* data, std::size_t size);
    [[nodiscard]] std::optional<std::vector<std::uint8_t>> getPipelineCacheData();

private:
    MetalDevice();
    [[nodiscard]] bool uploadPixels(const MetalImage& image, const void* pixels, Vector2u size, Vector2u offset, std::size_t pixelSize);
    mutable std::recursive_mutex m_mutex;
    std::unique_ptr<Impl>        m_impl;
};

} // namespace sf::priv
