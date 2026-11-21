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


////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Graphics/RHI/Access.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/ShaderUniformType.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <SFML/System/Err.hpp>
#include <SFML/System/Exception.hpp>
#include <SFML/System/InputStream.hpp>
#include <SFML/System/Utils.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/System/Vector3.hpp>

#include <algorithm>
#include <array>
#include <fstream>
#include <iomanip>
#include <limits>
#include <ostream>
#include <utility>
#include <vector>

#include <cstdint>

namespace
{
namespace ShaderImpl
{

// Read the contents of a file into an array of char
bool getFileContents(const std::filesystem::path& filename, std::vector<char>& buffer)
{
    if (auto file = std::ifstream(filename, std::ios_base::binary))
    {
        file.seekg(0, std::ios_base::end);
        const std::ifstream::pos_type size = file.tellg();
        if (size > 0)
        {
            file.seekg(0, std::ios_base::beg);
            buffer.resize(static_cast<std::size_t>(size));
            return static_cast<bool>(file.read(buffer.data(), static_cast<std::streamsize>(size)));
        }
    }

    return false;
}

// Read the contents of a stream into an array of char
bool getStreamContents(sf::InputStream& stream, std::vector<char>& buffer)
{
    bool                success = false;
    const std::optional size    = stream.getSize();
    if (size > std::size_t{0})
    {
        buffer.resize(*size);

        if (!stream.seek(0).has_value())
        {
            sf::err() << "Failed to seek shader stream" << std::endl;
            return false;
        }

        const std::optional read = stream.read(buffer.data(), *size);
        success                  = (read == size);
    }
    return success;
}

// Transforms an array of 2D vectors into a contiguous array of scalars
std::vector<float> flatten(const sf::Vector2f* vectorArray, std::size_t length)
{
    const std::size_t vectorSize = 2;

    std::vector<float> contiguous(vectorSize * length);
    for (std::size_t i = 0; i < length; ++i)
    {
        contiguous[vectorSize * i]     = vectorArray[i].x;
        contiguous[vectorSize * i + 1] = vectorArray[i].y;
    }

    return contiguous;
}

// Transforms an array of 3D vectors into a contiguous array of scalars
std::vector<float> flatten(const sf::Vector3f* vectorArray, std::size_t length)
{
    const std::size_t vectorSize = 3;

    std::vector<float> contiguous(vectorSize * length);
    for (std::size_t i = 0; i < length; ++i)
    {
        contiguous[vectorSize * i]     = vectorArray[i].x;
        contiguous[vectorSize * i + 1] = vectorArray[i].y;
        contiguous[vectorSize * i + 2] = vectorArray[i].z;
    }

    return contiguous;
}

// Transforms an array of 4D vectors into a contiguous array of scalars
std::vector<float> flatten(const sf::Glsl::Vec4* vectorArray, std::size_t length)
{
    const std::size_t vectorSize = 4;

    std::vector<float> contiguous(vectorSize * length);
    for (std::size_t i = 0; i < length; ++i)
    {
        contiguous[vectorSize * i]     = vectorArray[i].x;
        contiguous[vectorSize * i + 1] = vectorArray[i].y;
        contiguous[vectorSize * i + 2] = vectorArray[i].z;
        contiguous[vectorSize * i + 3] = vectorArray[i].w;
    }

    return contiguous;
}
} // namespace ShaderImpl
} // namespace


namespace sf
{
////////////////////////////////////////////////////////////
using namespace ShaderImpl;


////////////////////////////////////////////////////////////
Shader::Shader(const std::filesystem::path& filename, Type type)
{
    if (!loadFromFile(filename, type))
        throw Exception("Failed to load shader from file");
}


////////////////////////////////////////////////////////////
Shader::Shader(const std::filesystem::path& vertexShaderFilename, const std::filesystem::path& fragmentShaderFilename)
{
    if (!loadFromFile(vertexShaderFilename, fragmentShaderFilename))
        throw Exception("Failed to load shader from files");
}


////////////////////////////////////////////////////////////
Shader::Shader(const std::filesystem::path& vertexShaderFilename,
               const std::filesystem::path& geometryShaderFilename,
               const std::filesystem::path& fragmentShaderFilename)
{
    if (!loadFromFile(vertexShaderFilename, geometryShaderFilename, fragmentShaderFilename))
        throw Exception("Failed to load shader from files");
}


////////////////////////////////////////////////////////////
Shader::Shader(std::string_view shader, Type type)
{
    if (!loadFromMemory(shader, type))
        throw Exception("Failed to load shader from memory");
}


////////////////////////////////////////////////////////////
Shader::Shader(std::string_view vertexShader, std::string_view fragmentShader)
{
    if (!loadFromMemory(vertexShader, fragmentShader))
        throw Exception("Failed to load shader from memory");
}


////////////////////////////////////////////////////////////
Shader::Shader(std::string_view vertexShader, std::string_view geometryShader, std::string_view fragmentShader)
{
    if (!loadFromMemory(vertexShader, geometryShader, fragmentShader))
        throw Exception("Failed to load shader from memory");
}


////////////////////////////////////////////////////////////
Shader::Shader(InputStream& stream, Type type)
{
    if (!loadFromStream(stream, type))
        throw Exception("Failed to load shader from stream");
}


////////////////////////////////////////////////////////////
Shader::Shader(InputStream& vertexShaderStream, InputStream& fragmentShaderStream)
{
    if (!loadFromStream(vertexShaderStream, fragmentShaderStream))
        throw Exception("Failed to load shader from streams");
}


////////////////////////////////////////////////////////////
Shader::Shader(InputStream& vertexShaderStream, InputStream& geometryShaderStream, InputStream& fragmentShaderStream)
{
    if (!loadFromStream(vertexShaderStream, geometryShaderStream, fragmentShaderStream))
        throw Exception("Failed to load shader from streams");
}


////////////////////////////////////////////////////////////
Shader::~Shader() = default;

Shader::Shader(Shader&& source) noexcept = default;

Shader& Shader::operator=(Shader&& right) noexcept
{
    if (this != &right)
    {
        m_resource = std::move(right.m_resource);
        m_samplers = std::move(right.m_samplers);
    }
    return *this;
}

////////////////////////////////////////////////////////////
bool Shader::loadFromFile(const std::filesystem::path& filename, Type type)
{
    // Read the file
    std::vector<char> shader;
    if (!getFileContents(filename, shader))
    {
        err() << "Failed to open shader file\n" << formatDebugPathInfo(filename) << std::endl;
        return false;
    }

    // Compile the shader program
    if (type == Type::Vertex)
        return compile({shader.data(), shader.size()}, {}, {});

    if (type == Type::Geometry)
        return compile({}, {shader.data(), shader.size()}, {});

    return compile({}, {}, {shader.data(), shader.size()});
}


////////////////////////////////////////////////////////////
bool Shader::loadFromFile(const std::filesystem::path& vertexShaderFilename,
                          const std::filesystem::path& fragmentShaderFilename)
{
    // Read the vertex shader file
    std::vector<char> vertexShader;
    if (!getFileContents(vertexShaderFilename, vertexShader))
    {
        err() << "Failed to open vertex shader file\n" << formatDebugPathInfo(vertexShaderFilename) << std::endl;
        return false;
    }

    // Read the fragment shader file
    std::vector<char> fragmentShader;
    if (!getFileContents(fragmentShaderFilename, fragmentShader))
    {
        err() << "Failed to open fragment shader file\n" << formatDebugPathInfo(fragmentShaderFilename) << std::endl;
        return false;
    }

    // Compile the shader program
    return compile({vertexShader.data(), vertexShader.size()}, {}, {fragmentShader.data(), fragmentShader.size()});
}


////////////////////////////////////////////////////////////
bool Shader::loadFromFile(const std::filesystem::path& vertexShaderFilename,
                          const std::filesystem::path& geometryShaderFilename,
                          const std::filesystem::path& fragmentShaderFilename)
{
    // Read the vertex shader file
    std::vector<char> vertexShader;
    if (!getFileContents(vertexShaderFilename, vertexShader))
    {
        err() << "Failed to open vertex shader file\n" << formatDebugPathInfo(vertexShaderFilename) << std::endl;
        return false;
    }

    // Read the geometry shader file
    std::vector<char> geometryShader;
    if (!getFileContents(geometryShaderFilename, geometryShader))
    {
        err() << "Failed to open geometry shader file\n" << formatDebugPathInfo(geometryShaderFilename) << std::endl;
        return false;
    }

    // Read the fragment shader file
    std::vector<char> fragmentShader;
    if (!getFileContents(fragmentShaderFilename, fragmentShader))
    {
        err() << "Failed to open fragment shader file\n" << formatDebugPathInfo(fragmentShaderFilename) << std::endl;
        return false;
    }

    // Compile the shader program
    return compile({vertexShader.data(), vertexShader.size()},
                   {geometryShader.data(), geometryShader.size()},
                   {fragmentShader.data(), fragmentShader.size()});
}


////////////////////////////////////////////////////////////
bool Shader::loadFromMemory(std::string_view shader, Type type)
{
    // Compile the shader program
    if (type == Type::Vertex)
        return compile(shader, {}, {});

    if (type == Type::Geometry)
        return compile({}, shader, {});

    return compile({}, {}, shader);
}


////////////////////////////////////////////////////////////
bool Shader::loadFromMemory(std::string_view vertexShader, std::string_view fragmentShader)
{
    // Compile the shader program
    return compile(vertexShader, {}, fragmentShader);
}


////////////////////////////////////////////////////////////
bool Shader::loadFromMemory(std::string_view vertexShader, std::string_view geometryShader, std::string_view fragmentShader)
{
    // Compile the shader program
    return compile(vertexShader, geometryShader, fragmentShader);
}


////////////////////////////////////////////////////////////
bool Shader::loadFromStream(InputStream& stream, Type type)
{
    // Read the shader code from the stream
    std::vector<char> shader;
    if (!getStreamContents(stream, shader))
    {
        err() << "Failed to read shader from stream" << std::endl;
        return false;
    }

    // Compile the shader program
    if (type == Type::Vertex)
        return compile({shader.data(), shader.size()}, {}, {});

    if (type == Type::Geometry)
        return compile({}, {shader.data(), shader.size()}, {});

    return compile({}, {}, {shader.data(), shader.size()});
}


////////////////////////////////////////////////////////////
bool Shader::loadFromStream(InputStream& vertexShaderStream, InputStream& fragmentShaderStream)
{
    // Read the vertex shader code from the stream
    std::vector<char> vertexShader;
    if (!getStreamContents(vertexShaderStream, vertexShader))
    {
        err() << "Failed to read vertex shader from stream" << std::endl;
        return false;
    }

    // Read the fragment shader code from the stream
    std::vector<char> fragmentShader;
    if (!getStreamContents(fragmentShaderStream, fragmentShader))
    {
        err() << "Failed to read fragment shader from stream" << std::endl;
        return false;
    }

    // Compile the shader program
    return compile({vertexShader.data(), vertexShader.size()}, {}, {fragmentShader.data(), fragmentShader.size()});
}


////////////////////////////////////////////////////////////
bool Shader::loadFromStream(InputStream& vertexShaderStream, InputStream& geometryShaderStream, InputStream& fragmentShaderStream)
{
    // Read the vertex shader code from the stream
    std::vector<char> vertexShader;
    if (!getStreamContents(vertexShaderStream, vertexShader))
    {
        err() << "Failed to read vertex shader from stream" << std::endl;
        return false;
    }

    // Read the geometry shader code from the stream
    std::vector<char> geometryShader;
    if (!getStreamContents(geometryShaderStream, geometryShader))
    {
        err() << "Failed to read geometry shader from stream" << std::endl;
        return false;
    }

    // Read the fragment shader code from the stream
    std::vector<char> fragmentShader;
    if (!getStreamContents(fragmentShaderStream, fragmentShader))
    {
        err() << "Failed to read fragment shader from stream" << std::endl;
        return false;
    }

    // Compile the shader program
    return compile({vertexShader.data(), vertexShader.size()},
                   {geometryShader.data(), geometryShader.size()},
                   {fragmentShader.data(), fragmentShader.size()});
}


////////////////////////////////////////////////////////////
using priv::ShaderUniformType;

void Shader::setUniform(const std::string& name, float value)
{
    setUniformData(name, ShaderUniformType::Float, &value, 1);
}

void Shader::setUniform(const std::string& name, Glsl::Vec2 value)
{
    const std::array values{value.x, value.y};
    setUniformData(name, ShaderUniformType::Vec2, values.data(), 1);
}

void Shader::setUniform(const std::string& name, const Glsl::Vec3& value)
{
    const std::array values{value.x, value.y, value.z};
    setUniformData(name, ShaderUniformType::Vec3, values.data(), 1);
}

void Shader::setUniform(const std::string& name, const Glsl::Vec4& value)
{
    const std::array values{value.x, value.y, value.z, value.w};
    setUniformData(name, ShaderUniformType::Vec4, values.data(), 1);
}

void Shader::setUniform(const std::string& name, int value)
{
    setUniformData(name, ShaderUniformType::Int, &value, 1);
}

void Shader::setUniform(const std::string& name, Glsl::Ivec2 value)
{
    const std::array values{value.x, value.y};
    setUniformData(name, ShaderUniformType::IVec2, values.data(), 1);
}

void Shader::setUniform(const std::string& name, const Glsl::Ivec3& value)
{
    const std::array values{value.x, value.y, value.z};
    setUniformData(name, ShaderUniformType::IVec3, values.data(), 1);
}

void Shader::setUniform(const std::string& name, const Glsl::Ivec4& value)
{
    const std::array values{value.x, value.y, value.z, value.w};
    setUniformData(name, ShaderUniformType::IVec4, values.data(), 1);
}

void Shader::setUniform(const std::string& name, bool value)
{
    setUniform(name, int(value));
}

void Shader::setUniform(const std::string& name, Glsl::Bvec2 value)
{
    setUniform(name, Glsl::Ivec2(value));
}

void Shader::setUniform(const std::string& name, const Glsl::Bvec3& value)
{
    setUniform(name, Glsl::Ivec3(value));
}

void Shader::setUniform(const std::string& name, const Glsl::Bvec4& value)
{
    setUniform(name, Glsl::Ivec4(value));
}

void Shader::setUniform(const std::string& name, const Glsl::Mat3& value)
{
    setUniformData(name, ShaderUniformType::Mat3, value.array.data(), 1);
}

void Shader::setUniform(const std::string& name, const Glsl::Mat4& value)
{
    setUniformData(name, ShaderUniformType::Mat4, value.array.data(), 1);
}

void Shader::setUniform(const std::string& name, const Texture& texture)
{
    if (!m_resource)
        return;
    m_samplers[name] = &texture;
    m_resource->setSampler(name, priv::RhiAccess::texture(texture), false);
}


////////////////////////////////////////////////////////////
void Shader::setUniform(const std::string& name, CurrentTextureType)
{
    if (!m_resource)
        return;
    m_samplers.erase(name);
    m_resource->setSampler(name, {}, true);
}


////////////////////////////////////////////////////////////
void Shader::setUniformArray(const std::string& name, const float* values, std::size_t length)
{
    setUniformData(name, ShaderUniformType::Float, values, length);
}

void Shader::setUniformArray(const std::string& name, const Glsl::Vec2* values, std::size_t length)
{
    const auto packed = flatten(values, length);
    setUniformData(name, ShaderUniformType::Vec2, packed.data(), length);
}

void Shader::setUniformArray(const std::string& name, const Glsl::Vec3* values, std::size_t length)
{
    const auto packed = flatten(values, length);
    setUniformData(name, ShaderUniformType::Vec3, packed.data(), length);
}

void Shader::setUniformArray(const std::string& name, const Glsl::Vec4* values, std::size_t length)
{
    const auto packed = flatten(values, length);
    setUniformData(name, ShaderUniformType::Vec4, packed.data(), length);
}

void Shader::setUniformArray(const std::string& name, const Glsl::Mat3* values, std::size_t length)
{
    std::vector<float> packed;
    packed.reserve(9 * length);
    for (std::size_t index = 0; index < length; ++index)
        packed.insert(packed.end(), values[index].array.begin(), values[index].array.end());
    setUniformData(name, ShaderUniformType::Mat3, packed.data(), length);
}

void Shader::setUniformArray(const std::string& name, const Glsl::Mat4* values, std::size_t length)
{
    std::vector<float> packed;
    packed.reserve(16 * length);
    for (std::size_t index = 0; index < length; ++index)
        packed.insert(packed.end(), values[index].array.begin(), values[index].array.end());
    setUniformData(name, ShaderUniformType::Mat4, packed.data(), length);
}

void Shader::setUniformData(const std::string& name, ShaderUniformType type, const void* values, std::size_t count)
{
    if (m_resource && values && count)
        m_resource->setUniform(name, type, values, count);
}

unsigned int Shader::getNativeHandle() const
{
    return m_resource ? static_cast<const priv::ProgramResource&>(*m_resource).nativeHandle() : 0;
}


////////////////////////////////////////////////////////////
void Shader::bind(const Shader* shader)
{
    if (const auto device = rhi::getDevice())
    {
        if (shader)
            shader->refreshSamplers();
        static_cast<priv::DeviceResource&>(*device).bindShader(shader ? shader->m_resource.get() : nullptr);
    }
}

bool Shader::isAvailable()
{
    const auto device = rhi::getDevice();
    return device && device->capabilities().shaders;
}

bool Shader::isGeometryAvailable()
{
    const auto device = rhi::getDevice();
    return device && device->capabilities().geometryShaders;
}


bool Shader::compile(std::string_view vertex, std::string_view geometry, std::string_view fragment)
{
    const auto device = rhi::getDevice();
    if (!device || (vertex.empty() && geometry.empty() && fragment.empty()))
        return false;
    if (!geometry.empty() && !device->capabilities().geometryShaders)
        return false;
    auto resource = device->createShader(vertex, geometry, fragment);
    if (!resource)
        return false;
    m_resource = std::move(resource);
    m_samplers.clear();
    return true;
}

void Shader::refreshSamplers() const
{
    if (m_resource)
        for (const auto& [name, texture] : m_samplers)
            m_resource->setSampler(name, priv::RhiAccess::texture(*texture), false);
}
} // namespace sf
