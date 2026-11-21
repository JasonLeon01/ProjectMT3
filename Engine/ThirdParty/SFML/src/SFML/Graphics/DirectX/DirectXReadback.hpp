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
#include <SFML/Graphics/RHI/Device.hpp>
namespace sf::priv
{
class DirectXReadback final : public rhi::Readback
{
public:
    static std::shared_ptr<DirectXReadback>        create(const std::shared_ptr<DirectXDevice>& device,
                                                          const DirectXImage&                   image,
                                                          IntRect                               area);
    rhi::ReadbackStatus                            poll() override;
    [[nodiscard]] const std::vector<std::uint8_t>& data() const override;

private:
    std::shared_ptr<DirectXDevice> m_device;
    ComPtr<ID3D11Texture2D>        m_staging;
    ComPtr<ID3D11Query>            m_fence;
    Vector2u                       m_size;
    std::vector<std::uint8_t>      m_data;
    rhi::ReadbackStatus            m_status{rhi::ReadbackStatus::Pending};
};
} // namespace sf::priv
