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

#include <SFML/Graphics/Metal/MetalTimer.hpp>

namespace sf::priv
{
std::shared_ptr<MetalTimer> MetalTimer::create(std::shared_ptr<MetalDevice> device)
{
    const auto guard = device->lock();
    if (!device->submit(false))
        return {};
    auto result      = std::make_shared<MetalTimer>();
    result->m_device = std::move(device);
    result->m_range  = std::make_shared<MetalTimingRange>();
    result->m_device->impl().timingRanges.push_back(result->m_range);
    return result;
}

void MetalTimer::end()
{
    if (m_ended)
        return;
    const auto guard   = m_device->lock();
    m_valid            = m_device->submit(false);
    m_range->capturing = false;
    m_ended            = true;
}

std::optional<double> MetalTimer::elapsedMilliseconds()
{
    const auto guard = m_device->lock();
    if (!m_ended)
        return std::nullopt;
    if (m_elapsed)
        return m_elapsed;
    double elapsed = 0.;
    for (const id<MTLCommandBuffer>& buffer : m_range->buffers)
    {
        if (buffer.status == MTLCommandBufferStatusError)
            m_valid = false;
        if (m_valid && buffer.status != MTLCommandBufferStatusCompleted)
            return std::nullopt;
        const double begin  = buffer.GPUStartTime;
        const double finish = buffer.GPUEndTime;
        if (begin <= 0. || finish < begin)
            m_valid = false;
        elapsed += (finish - begin) * 1000.;
    }
    m_elapsed = m_valid ? elapsed : -1.;
    m_range->buffers.clear();
    return m_elapsed;
}
} // namespace sf::priv
