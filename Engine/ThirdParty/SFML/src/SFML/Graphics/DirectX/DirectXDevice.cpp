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

#include <SFML/Graphics/DirectX/DirectXDevice.hpp>
#include <SFML/Graphics/DirectX/DirectXProgram.hpp>
#include <SFML/Graphics/Image.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <limits>
#include <ostream>
#include <vector>

#ifdef SFML_DIRECTX_TESTS
#include <d3d11sdklayers.h>
#endif

#include <cstring>

namespace sf::priv
{
namespace
{
namespace DirectXDeviceImpl
{
#ifdef SFML_DIRECTX_TESTS
struct TestConfiguration
{
    bool                         warp{};
    bool                         debug{};
    unsigned int                 maximumLevel{};
    bool                         fail{};
    std::weak_ptr<DirectXDevice> device;
};
TestConfiguration testConfiguration;
#endif
} // namespace DirectXDeviceImpl
} // namespace

std::shared_ptr<DirectXDevice> DirectXDevice::create(const DirectXSettings& requested)
{
    using namespace DirectXDeviceImpl;
    auto                           result = std::shared_ptr<DirectXDevice>(new DirectXDevice);
    constexpr std::array           levels{D3D_FEATURE_LEVEL_12_1,
                                D3D_FEATURE_LEVEL_12_0,
                                D3D_FEATURE_LEVEL_11_1,
                                D3D_FEATURE_LEVEL_11_0,
                                D3D_FEATURE_LEVEL_10_1,
                                D3D_FEATURE_LEVEL_10_0};
    const auto                     minimum = (requested.majorVersion << 12) | (requested.minorVersion << 8);
    std::vector<D3D_FEATURE_LEVEL> candidates;
    auto                           driver = D3D_DRIVER_TYPE_HARDWARE;
    UINT                           flags  = 0;
    for (auto level : levels)
    {
        if (static_cast<unsigned int>(level) < minimum)
            continue;
#ifdef SFML_DIRECTX_TESTS
        if (testConfiguration.maximumLevel && static_cast<unsigned int>(level) > testConfiguration.maximumLevel)
            continue;
#endif
        candidates.push_back(level);
    }
#ifdef SFML_DIRECTX_TESTS
    if (testConfiguration.fail)
        return {};
    if (testConfiguration.warp)
        driver = D3D_DRIVER_TYPE_WARP;
    if (testConfiguration.debug)
        flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    HRESULT status = E_INVALIDARG;
    while (!candidates.empty())
    {
        status = D3D11CreateDevice(nullptr,
                                   driver,
                                   nullptr,
                                   flags,
                                   candidates.data(),
                                   static_cast<UINT>(candidates.size()),
                                   D3D11_SDK_VERSION,
                                   result->m_device.put(),
                                   &result->m_level,
                                   result->m_context.put());
        if (status != E_INVALIDARG)
            break;
        // Old runtimes reject unknown feature levels, rather than skipping them.
        candidates.erase(candidates.begin());
    }
    if (!result->check(status, "create device"))
        return {};
#ifdef SFML_DIRECTX_TESTS
    testConfiguration.device = result;
#endif
    return result;
}

DirectXDevice::~DirectXDevice()
{
    if (m_context)
    {
        m_context->ClearState();
        m_context->Flush();
    }
}

std::unique_lock<std::recursive_mutex> DirectXDevice::lock() const
{
    return std::unique_lock(m_mutex);
}
ID3D11Device* DirectXDevice::device() const
{
    return m_device.get();
}
ID3D11DeviceContext* DirectXDevice::context() const
{
    return m_context.get();
}
DirectXSettings DirectXDevice::settings() const
{
    return {static_cast<unsigned int>(m_level) >> 12, (static_cast<unsigned int>(m_level) >> 8) & 0xF};
}
bool DirectXDevice::isLost() const
{
    return m_lost;
}
unsigned int DirectXDevice::maximumTextureSize() const
{
    return m_level >= D3D_FEATURE_LEVEL_11_0 ? D3D11_REQ_TEXTURE2D_U_OR_V_DIMENSION : D3D10_REQ_TEXTURE2D_U_OR_V_DIMENSION;
}

bool DirectXDevice::check(HRESULT result, const char* operation)
{
    if (SUCCEEDED(result))
        return !m_lost;
    err() << "DirectX could not " << operation << " (HRESULT 0x" << std::hex << static_cast<unsigned long>(result)
          << std::dec << ')' << std::endl;
    if (result == DXGI_ERROR_DEVICE_REMOVED || result == DXGI_ERROR_DEVICE_RESET || result == DXGI_ERROR_DEVICE_HUNG)
    {
        m_lost = true;
        err() << "The DirectX graphics device was lost" << std::endl;
    }
    return false;
}

unsigned int DirectXDevice::samples(DXGI_FORMAT color, DXGI_FORMAT depth, unsigned int requested) const
{
    for (unsigned int count = 32; count > 1; count /= 2)
    {
        UINT colorQuality = 0;
        UINT depthQuality = 1;
        if (count > requested || FAILED(m_device->CheckMultisampleQualityLevels(color, count, &colorQuality)))
            continue;
        if (depth != DXGI_FORMAT_UNKNOWN && FAILED(m_device->CheckMultisampleQualityLevels(depth, count, &depthQuality)))
            continue;
        if (colorQuality && depthQuality)
            return count;
    }
    return 1;
}

std::shared_ptr<DirectXImage> DirectXDevice::makeTexture(Vector2u size, bool sRgb)
{
    const auto guard = lock();
    if (m_lost || !size.x || !size.y || size.x > maximumTextureSize() || size.y > maximumTextureSize())
        return {};
    auto image    = std::make_shared<DirectXImage>();
    image->size   = size;
    image->format = sRgb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
    D3D11_TEXTURE2D_DESC description{};
    description.Width            = size.x;
    description.Height           = size.y;
    description.ArraySize        = 1;
    description.Format           = DXGI_FORMAT_R8G8B8A8_TYPELESS;
    description.SampleDesc.Count = 1;
    description.BindFlags        = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    description.MiscFlags        = D3D11_RESOURCE_MISC_GENERATE_MIPS;
    if (!check(m_device->CreateTexture2D(&description, nullptr, image->texture.put()), "create texture"))
        return {};
    D3D11_SHADER_RESOURCE_VIEW_DESC view{};
    view.Format              = image->format;
    view.ViewDimension       = D3D11_SRV_DIMENSION_TEXTURE2D;
    view.Texture2D.MipLevels = UINT(-1);
    D3D11_RENDER_TARGET_VIEW_DESC attachment{};
    attachment.Format        = image->format;
    attachment.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    if (!check(m_device->CreateShaderResourceView(image->texture.get(), &view, image->view.put()),
               "create texture view") ||
        !check(m_device->CreateRenderTargetView(image->texture.get(), &attachment, image->attachment.put()),
               "create color attachment"))
        return {};
    return image;
}

std::shared_ptr<DirectXImage> DirectXDevice::makeFloatTexture(Vector2u size, const float* pixels)
{
    const auto guard = lock();
    if (m_lost || !size.x || !size.y || size.x > maximumTextureSize() || size.y > maximumTextureSize())
        return {};
    auto image    = std::make_shared<DirectXImage>();
    image->size   = size;
    image->format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    D3D11_TEXTURE2D_DESC desc{};
    desc.Width            = size.x;
    desc.Height           = size.y;
    desc.MipLevels        = 1;
    desc.ArraySize        = 1;
    desc.Format           = image->format;
    desc.SampleDesc.Count = 1;
    desc.BindFlags        = D3D11_BIND_SHADER_RESOURCE;
    const D3D11_SUBRESOURCE_DATA data{pixels, static_cast<UINT>(size.x * 4u * sizeof(float)), 0};
    if (!check(m_device->CreateTexture2D(&desc, pixels ? &data : nullptr, image->texture.put()),
               "create float texture") ||
        !check(m_device->CreateShaderResourceView(image->texture.get(), nullptr, image->view.put()),
               "create float texture view"))
        return {};
    return image;
}
bool DirectXDevice::uploadFloats(const DirectXImage& image, const float* pixels, Vector2u size, Vector2u offset)
{
    const auto guard = lock();
    if (m_lost || image.format != DXGI_FORMAT_R32G32B32A32_FLOAT || !pixels || !size.x || !size.y ||
        offset.x > image.size.x || offset.y > image.size.y || size.x > image.size.x - offset.x ||
        size.y > image.size.y - offset.y)
        return false;
    unbindTextures();
    const D3D11_BOX box{offset.x, offset.y, 0, offset.x + size.x, offset.y + size.y, 1};
    m_context->UpdateSubresource(image.texture.get(), 0, &box, pixels, static_cast<UINT>(size.x * 4u * sizeof(float)), 0);
    return check(m_device->GetDeviceRemovedReason(), "upload float texture");
}

std::shared_ptr<DirectXBuffer> DirectXDevice::makeBuffer(std::size_t bytes)
{
    const auto guard = lock();
    if (m_lost || bytes > std::numeric_limits<UINT>::max())
        return {};
    auto buffer      = std::make_shared<DirectXBuffer>();
    buffer->capacity = bytes;
    D3D11_BUFFER_DESC description{};
    description.ByteWidth = static_cast<UINT>(std::max(bytes, std::size_t{1}));
    description.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
    if (!check(m_device->CreateBuffer(&description, nullptr, buffer->buffer.put()), "create vertex buffer"))
        return {};
    return buffer;
}

void DirectXDevice::unbindTextures()
{
    const std::array<ID3D11ShaderResourceView*, D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT> empty{};
    m_context->VSSetShaderResources(0, static_cast<UINT>(empty.size()), empty.data());
    m_context->GSSetShaderResources(0, static_cast<UINT>(empty.size()), empty.data());
    m_context->PSSetShaderResources(0, static_cast<UINT>(empty.size()), empty.data());
    m_context->OMSetRenderTargets(0, nullptr, nullptr);
}

bool DirectXDevice::upload(const DirectXImage& image, const std::uint8_t* pixels, Vector2u size, Vector2u offset)
{
    const auto guard = lock();
    if (m_lost || !pixels || !size.x || !size.y || offset.x > image.size.x || offset.y > image.size.y ||
        size.x > image.size.x - offset.x || size.y > image.size.y - offset.y)
        return false;
    unbindTextures();
    const D3D11_BOX box{offset.x, offset.y, 0, offset.x + size.x, offset.y + size.y, 1};
    m_context->UpdateSubresource(image.texture.get(), 0, &box, pixels, size.x * 4, 0);
    return check(m_device->GetDeviceRemovedReason(), "upload texture");
}

bool DirectXDevice::copy(const DirectXImage& source, const DirectXImage& destination, Vector2u offset)
{
    const auto guard = lock();
    if (m_lost || offset.x > destination.size.x || offset.y > destination.size.y ||
        source.size.x > destination.size.x - offset.x || source.size.y > destination.size.y - offset.y)
        return false;
    if (source.texture.get() == destination.texture.get())
        return true;
    unbindTextures();
    const D3D11_BOX box{0, 0, 0, source.size.x, source.size.y, 1};
    m_context->CopySubresourceRegion(destination.texture.get(), 0, offset.x, offset.y, 0, source.texture.get(), 0, &box);
    return check(m_device->GetDeviceRemovedReason(), "copy texture");
}

Image DirectXDevice::readImage(const DirectXImage& image)
{
    const auto guard = lock();
    if (m_lost)
        return {};
    D3D11_TEXTURE2D_DESC description{};
    image.texture->GetDesc(&description);
    description.MipLevels      = 1;
    description.BindFlags      = 0;
    description.MiscFlags      = 0;
    description.Usage          = D3D11_USAGE_STAGING;
    description.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    ComPtr<ID3D11Texture2D> staging;
    if (!check(m_device->CreateTexture2D(&description, nullptr, staging.put()), "create readback texture"))
        return {};
    unbindTextures();
    const D3D11_BOX box{0, 0, 0, image.size.x, image.size.y, 1};
    m_context->CopySubresourceRegion(staging.get(), 0, 0, 0, 0, image.texture.get(), 0, &box);
    std::vector<std::uint8_t> pixels(static_cast<std::size_t>(image.size.x) * image.size.y * 4);
    D3D11_MAPPED_SUBRESOURCE  mapped{};
    if (!check(m_context->Map(staging.get(), 0, D3D11_MAP_READ, 0, &mapped), "read texture"))
        return {};
    for (unsigned int row = 0; row < image.size.y; ++row)
        std::memcpy(pixels.data() + static_cast<std::size_t>(row) * image.size.x * 4,
                    static_cast<const std::uint8_t*>(mapped.pData) + static_cast<std::size_t>(row) * mapped.RowPitch,
                    static_cast<std::size_t>(image.size.x) * 4);
    m_context->Unmap(staging.get(), 0);
    return {image.size, pixels.data()};
}

bool DirectXDevice::generateMipmap(const DirectXImage& image)
{
    const auto guard = lock();
    if (m_lost)
        return false;
    unbindTextures();
    m_context->GenerateMips(image.view.get());
    return check(m_device->GetDeviceRemovedReason(), "generate mipmaps");
}

ID3D11SamplerState* DirectXDevice::sampler(bool smooth, bool repeated, bool mipmapped)
{
    const auto index = (smooth ? 1u : 0u) | (repeated ? 2u : 0u) | (mipmapped ? 4u : 0u);
    auto&      value = m_samplers[index];
    if (!value)
    {
        D3D11_SAMPLER_DESC description{};
        description.Filter   = smooth      ? D3D11_FILTER_MIN_MAG_MIP_LINEAR
                               : mipmapped ? D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR
                                           : D3D11_FILTER_MIN_MAG_MIP_POINT;
        description.AddressU = description.AddressV = description.AddressW = repeated ? D3D11_TEXTURE_ADDRESS_WRAP
                                                                                      : D3D11_TEXTURE_ADDRESS_CLAMP;
        description.ComparisonFunc                                         = D3D11_COMPARISON_NEVER;
        description.MaxLOD                                                 = mipmapped ? D3D11_FLOAT32_MAX : 0.f;
        description.MaxAnisotropy                                          = 1;
        if (!check(m_device->CreateSamplerState(&description, value.put()), "create sampler"))
            return nullptr;
    }
    return value.get();
}

const std::shared_ptr<DirectXImage>& DirectXDevice::whiteTexture()
{
    if (!m_white)
    {
        m_white = makeTexture({1, 1}, false);
        constexpr std::array<std::uint8_t, 4> white{255, 255, 255, 255};
        if (m_white && !upload(*m_white, white.data(), {1, 1}, {}))
            m_white.reset();
    }
    return m_white;
}

const std::shared_ptr<DirectXProgram>& DirectXDevice::defaultProgram()
{
    if (!m_program)
        m_program = DirectXProgram::create(*this);
    return m_program;
}

#ifdef SFML_DIRECTX_TESTS
void configureDirectXTestDevice(bool warp, bool debug, unsigned int maximumLevel, bool fail)
{
    DirectXDeviceImpl::testConfiguration = {warp, debug, maximumLevel, fail, {}};
}

bool directXTestDebugLayerClean()
{
    const auto device = DirectXDeviceImpl::testConfiguration.device.lock();
    if (!device || !DirectXDeviceImpl::testConfiguration.debug)
        return true;
    ComPtr<ID3D11InfoQueue> queue;
    if (FAILED(device->device()->QueryInterface(IID_ID3D11InfoQueue, reinterpret_cast<void**>(queue.put()))))
        return false;
    bool clean = true;
    for (UINT64 index = 0; index < queue->GetNumStoredMessages(); ++index)
    {
        SIZE_T length = 0;
        if (FAILED(queue->GetMessage(index, nullptr, &length)))
            return false;
        std::vector<std::byte> storage(length);
        auto*                  message = reinterpret_cast<D3D11_MESSAGE*>(storage.data());
        if (FAILED(queue->GetMessage(index, message, &length)))
            return false;
        if (message->Severity <= D3D11_MESSAGE_SEVERITY_WARNING)
        {
            err() << "DirectX validation: " << message->pDescription << std::endl;
            clean = false;
        }
    }
    queue->ClearStoredMessages();
    return clean;
}
#endif
} // namespace sf::priv
