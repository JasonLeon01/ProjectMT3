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

#include <SFML/Graphics/DirectX/DirectXTimer.hpp>

#include <array>
namespace sf::priv
{
std::shared_ptr<DirectXTimer> DirectXTimer::create(const std::shared_ptr<DirectXDevice>& device)
{
    const auto guard = device->lock();
    auto       timer = std::make_shared<DirectXTimer>();
    timer->m_device  = device;
    const D3D11_QUERY_DESC range{D3D11_QUERY_TIMESTAMP_DISJOINT, 0};
    const D3D11_QUERY_DESC stamp{D3D11_QUERY_TIMESTAMP, 0};
    if (!device->check(device->device()->CreateQuery(&range, timer->m_disjoint.put()), "create timing range") ||
        !device->check(device->device()->CreateQuery(&stamp, timer->m_begin.put()), "create start timestamp") ||
        !device->check(device->device()->CreateQuery(&stamp, timer->m_end.put()), "create end timestamp"))
        return {};
    device->context()->Begin(timer->m_disjoint.get());
    device->context()->End(timer->m_begin.get());
    return timer;
}
void DirectXTimer::end()
{
    if (m_ended)
        return;
    const auto guard = m_device->lock();
    m_device->context()->End(m_end.get());
    m_device->context()->End(m_disjoint.get());
    m_ended = true;
}
std::optional<double> DirectXTimer::elapsedMilliseconds()
{
    if (!m_ended)
        return {};
    const auto                          guard = m_device->lock();
    D3D11_QUERY_DATA_TIMESTAMP_DISJOINT range{};
    UINT64                              begin{};
    UINT64                              end{};
    const auto                          query = [&](ID3D11Query* object, void* data, UINT size)
    { return m_device->context()->GetData(object, data, size, D3D11_ASYNC_GETDATA_DONOTFLUSH); };
    const std::array results{query(m_disjoint.get(), &range, sizeof(range)),
                             query(m_begin.get(), &begin, sizeof(begin)),
                             query(m_end.get(), &end, sizeof(end))};
    for (const auto result : results)
        if (FAILED(result))
            return -1.0;
    for (const auto result : results)
        if (result == S_FALSE)
            return {};
    if (range.Disjoint || !range.Frequency)
        return -1.0;
    return static_cast<double>(end - begin) * 1000.0 / static_cast<double>(range.Frequency);
}
} // namespace sf::priv
