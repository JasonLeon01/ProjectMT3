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

#include <SFML/Graphics/DirectX/ComPtr.hpp>
#include <SFML/Graphics/DirectX/DirectXTest.hpp>

#include <SFML/Window/ContextSettings.hpp>

#include <SFML/System/Vector2.hpp>
#include <SFML/System/Win32/WindowsHeader.hpp>

#include <array>
#include <d3d11.h>
#include <memory>
#include <mutex>

#include <cstddef>
#include <cstdint>

namespace sf
{
class Image;
}

namespace sf::priv
{
class DirectXProgram;

struct DirectXImage
{
    ComPtr<ID3D11Texture2D>          texture;
    ComPtr<ID3D11ShaderResourceView> view;
    ComPtr<ID3D11RenderTargetView>   attachment;
    Vector2u                         size;
    DXGI_FORMAT                      format{DXGI_FORMAT_R8G8B8A8_UNORM};
};

struct DirectXBuffer
{
    ComPtr<ID3D11Buffer> buffer;
    std::size_t          capacity{};
};

class DirectXDevice
{
public:
    [[nodiscard]] static std::shared_ptr<DirectXDevice> create(const DirectXSettings& requested);
    ~DirectXDevice();
    DirectXDevice(const DirectXDevice&)            = delete;
    DirectXDevice& operator=(const DirectXDevice&) = delete;

    [[nodiscard]] std::unique_lock<std::recursive_mutex> lock() const;
    [[nodiscard]] ID3D11Device*                          device() const;
    [[nodiscard]] ID3D11DeviceContext*                   context() const;
    [[nodiscard]] DirectXSettings                        settings() const;
    [[nodiscard]] bool                                   check(HRESULT result, const char* operation);
    [[nodiscard]] bool                                   isLost() const;
    [[nodiscard]] unsigned int                           maximumTextureSize() const;
    [[nodiscard]] unsigned int samples(DXGI_FORMAT color, DXGI_FORMAT depth, unsigned int requested) const;

    [[nodiscard]] std::shared_ptr<DirectXImage> makeTexture(Vector2u size, bool sRgb);
    [[nodiscard]] std::shared_ptr<DirectXImage> makeFloatTexture(Vector2u size, const float* pixels);
    [[nodiscard]] bool uploadFloats(const DirectXImage& image, const float* pixels, Vector2u size, Vector2u offset);
    [[nodiscard]] std::shared_ptr<DirectXBuffer> makeBuffer(std::size_t bytes);
    [[nodiscard]] bool  upload(const DirectXImage& image, const std::uint8_t* pixels, Vector2u size, Vector2u offset);
    [[nodiscard]] bool  copy(const DirectXImage& source, const DirectXImage& destination, Vector2u offset);
    [[nodiscard]] Image readImage(const DirectXImage& image);
    [[nodiscard]] bool  generateMipmap(const DirectXImage& image);
    [[nodiscard]] ID3D11SamplerState*                    sampler(bool smooth, bool repeated, bool mipmapped);
    [[nodiscard]] const std::shared_ptr<DirectXImage>&   whiteTexture();
    [[nodiscard]] const std::shared_ptr<DirectXProgram>& defaultProgram();
    void                                                 unbindTextures();

private:
    DirectXDevice() = default;
    mutable std::recursive_mutex              m_mutex;
    ComPtr<ID3D11Device>                      m_device;
    ComPtr<ID3D11DeviceContext>               m_context;
    D3D_FEATURE_LEVEL                         m_level{};
    bool                                      m_lost{};
    std::array<ComPtr<ID3D11SamplerState>, 8> m_samplers;
    std::shared_ptr<DirectXImage>             m_white;
    std::shared_ptr<DirectXProgram>           m_program;
};

} // namespace sf::priv
