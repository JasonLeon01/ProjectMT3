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

#include <SFML/Graphics/DirectX/DirectXRenderTarget.hpp>

#include <SFML/Window/WindowSurface.hpp>

namespace sf::priv
{
class DirectXWindowSurface final : public WindowSurface
{
public:
    [[nodiscard]] static std::unique_ptr<DirectXWindowSurface> create(
        const std::shared_ptr<DirectXDevice>& device,
        WindowHandle                          handle,
        Vector2u                              size,
        const ContextSettings&                settings);
    ~DirectXWindowSurface() override;
    [[nodiscard]] const ContextSettings&                      getSettings() const override;
    [[nodiscard]] bool                                        setActive(bool active) override;
    void                                                      display() override;
    void                                                      setVerticalSyncEnabled(bool enabled) override;
    void                                                      resize(Vector2u size);
    [[nodiscard]] const std::shared_ptr<DirectXRenderTarget>& renderTarget() const;
    [[nodiscard]] bool copyTo(const std::shared_ptr<DirectXImage>& destination, Vector2u offset) const;

private:
    DirectXWindowSurface() = default;
    std::shared_ptr<DirectXDevice>       m_device;
    std::shared_ptr<DirectXRenderTarget> m_target;
    ComPtr<IDXGISwapChain>               m_swapchain;
    Vector2u                             m_size;
    bool                                 m_vsync{};
    bool                                 m_dirty{};
};
} // namespace sf::priv
