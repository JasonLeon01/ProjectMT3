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

#include <SFML/Graphics/RHI/Access.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>

#include <SFML/System/Err.hpp>

#include <limits>
#include <ostream>
#include <utility>
namespace sf
{
VertexBuffer::VertexBuffer(PrimitiveType type) : m_primitiveType(type)
{
}


////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(Usage usage) : m_usage(usage)
{
}


////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(PrimitiveType type, Usage usage) : m_primitiveType(type), m_usage(usage)
{
}


////////////////////////////////////////////////////////////
VertexBuffer::VertexBuffer(const VertexBuffer& copy) : m_primitiveType(copy.m_primitiveType), m_usage(copy.m_usage)
{
    if (copy.m_size)
    {
        if (!create(copy.m_size))
        {
            err() << "Could not create vertex buffer for copying" << std::endl;
            return;
        }

        if (!update(copy))
            err() << "Could not copy vertex buffer" << std::endl;
    }
}


////////////////////////////////////////////////////////////
VertexBuffer::~VertexBuffer() = default;


////////////////////////////////////////////////////////////
bool VertexBuffer::create(std::size_t vertexCount)
{
    if (vertexCount > std::numeric_limits<std::ptrdiff_t>::max() / sizeof(Vertex))
        return false;
    const auto device = rhi::getDevice();
    if (!device)
        return false;
    auto resource = device->createBuffer(vertexCount * sizeof(Vertex), static_cast<rhi::BufferUsage>(m_usage));
    if (!resource)
        return false;
    m_resource = std::move(resource);
    m_size     = vertexCount;
    return true;
}


////////////////////////////////////////////////////////////
std::size_t VertexBuffer::getVertexCount() const
{
    return m_size;
}


////////////////////////////////////////////////////////////
bool VertexBuffer::update(const Vertex* vertices)
{
    return update(vertices, m_size, 0);
}


////////////////////////////////////////////////////////////
bool VertexBuffer::update(const Vertex* vertices, std::size_t vertexCount, unsigned int offset)
{
    if (!vertices || !m_resource || vertexCount > std::numeric_limits<std::ptrdiff_t>::max() / sizeof(Vertex) ||
        (offset && (offset > m_size || vertexCount > m_size - offset)))
        return false;
    if (!offset && vertexCount >= m_size)
    {
        const auto device = rhi::getDevice();
        auto replacement  = device->createBuffer(vertexCount * sizeof(Vertex), static_cast<rhi::BufferUsage>(m_usage));
        if (!replacement || !replacement->upload(vertices, vertexCount * sizeof(Vertex)))
            return false;
        m_resource = std::move(replacement);
        m_size     = vertexCount;
        return true;
    }
    return m_resource->upload(vertices, vertexCount * sizeof(Vertex), offset * sizeof(Vertex));
}


////////////////////////////////////////////////////////////
bool VertexBuffer::update(const VertexBuffer& vertexBuffer)
{
    if (!m_resource || !vertexBuffer.m_resource)
        return false;
    if (&vertexBuffer == this)
        return true;
    const auto device = rhi::getDevice();
    auto replacement = device->createBuffer(vertexBuffer.m_size * sizeof(Vertex), static_cast<rhi::BufferUsage>(m_usage));
    if (!replacement || !replacement->copy(*vertexBuffer.m_resource))
        return false;
    m_resource = std::move(replacement);
    m_size     = vertexBuffer.m_size;
    return true;
}


////////////////////////////////////////////////////////////
VertexBuffer& VertexBuffer::operator=(const VertexBuffer& right)
{
    VertexBuffer temp(right);

    swap(temp);

    return *this;
}


////////////////////////////////////////////////////////////
void VertexBuffer::swap(VertexBuffer& right) noexcept
{
    std::swap(m_resource, right.m_resource);
    std::swap(m_size, right.m_size);
    std::swap(m_primitiveType, right.m_primitiveType);
    std::swap(m_usage, right.m_usage);
}


////////////////////////////////////////////////////////////
unsigned int VertexBuffer::getNativeHandle() const
{
    return m_resource ? static_cast<const priv::BufferResource&>(*m_resource).nativeHandle() : 0;
}


////////////////////////////////////////////////////////////
void VertexBuffer::bind(const VertexBuffer* vertexBuffer)
{
    if (const auto device = rhi::getDevice())
        static_cast<priv::DeviceResource&>(*device).bindBuffer(vertexBuffer ? vertexBuffer->m_resource.get() : nullptr);
}


////////////////////////////////////////////////////////////
void VertexBuffer::setPrimitiveType(PrimitiveType type)
{
    m_primitiveType = type;
}


////////////////////////////////////////////////////////////
PrimitiveType VertexBuffer::getPrimitiveType() const
{
    return m_primitiveType;
}


////////////////////////////////////////////////////////////
void VertexBuffer::setUsage(Usage usage)
{
    m_usage = usage;
}


////////////////////////////////////////////////////////////
VertexBuffer::Usage VertexBuffer::getUsage() const
{
    return m_usage;
}


////////////////////////////////////////////////////////////
bool VertexBuffer::isAvailable()
{
    return rhi::getDevice() != nullptr;
}


////////////////////////////////////////////////////////////
void VertexBuffer::draw(RenderTarget& target, RenderStates states) const
{
    if (m_size)
        target.draw(*this, 0, m_size, states);
}


////////////////////////////////////////////////////////////
void swap(VertexBuffer& left, VertexBuffer& right) noexcept
{
    left.swap(right);
}

} // namespace sf
