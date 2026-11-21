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
#include <SFML/Graphics/DirectX/DirectXDevice.hpp>
#include <SFML/Graphics/RHI/NativeResources.hpp>
namespace sf::priv
{
using DirectXTextureResource = NativeTexture<DirectXDevice, DirectXImage>;
using DirectXBufferResource  = NativeBuffer<DirectXDevice, DirectXBuffer>;
template <>
bool DirectXBufferResource::upload(const void*, std::size_t, std::size_t);
template <>
bool DirectXBufferResource::copy(const rhi::Buffer&);
template <>
bool                                 DirectXBufferResource::read(void*, std::size_t, std::size_t) const;
inline std::shared_ptr<DirectXImage> nativeDirectXImage(const rhi::Texture* texture)
{
    const auto* resource = resourceCast<const DirectXTextureResource>(texture);
    return resource ? resource->image : nullptr;
}
inline std::shared_ptr<DirectXBuffer> nativeDirectXBuffer(const rhi::Buffer& buffer)
{
    const auto* resource = resourceCast<const DirectXBufferResource>(&buffer);
    return resource ? resource->buffer : nullptr;
}
} // namespace sf::priv
