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

#include <SFML/Graphics/RHI/Device.hpp>

#include <functional>
#include <memory>

namespace sf::priv
{
struct GraphicsDevice
{
    GraphicsBackend                  backend;
    ContextSettings::BackendSettings settings;
    std::shared_ptr<rhi::Device>     device;
};
[[nodiscard]] std::shared_ptr<rhi::Device> createOpenGLRhi(const ContextSettings& requested);
[[nodiscard]] std::shared_ptr<rhi::Device> createVulkanRhi(const VulkanSettings& settings);
[[nodiscard]] std::shared_ptr<rhi::Device> createDirectXRhi(const DirectXSettings& settings);
[[nodiscard]] std::shared_ptr<rhi::Device> createMetalRhi();

// If the first operation needs a window, wait until the window and its surface
// have been created before committing to the candidate.
[[nodiscard]] bool withGraphicsDevice(const ContextSettings& settings,
                                      const std::function<bool(const GraphicsDevice&, const ContextSettings&)>& operation);

[[nodiscard]] const GraphicsDevice* getGraphicsDevice();
} // namespace sf::priv
