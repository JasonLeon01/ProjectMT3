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

#include <SFML/Graphics/Metal/MetalObjects.hpp>
#include <SFML/Graphics/RHI/Device.hpp>

namespace sf::priv
{
class MetalTimer final : public rhi::Timer
{
public:
    [[nodiscard]] static std::shared_ptr<MetalTimer> create(std::shared_ptr<MetalDevice> device);
    void                                             end() override;
    [[nodiscard]] std::optional<double>              elapsedMilliseconds() override;

private:
    std::shared_ptr<MetalDevice>      m_device;
    std::shared_ptr<MetalTimingRange> m_range;
    std::optional<double>             m_elapsed;
    bool                              m_ended{};
    bool                              m_valid{true};
};
} // namespace sf::priv
