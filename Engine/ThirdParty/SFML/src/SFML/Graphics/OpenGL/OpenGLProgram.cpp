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

#include <SFML/Graphics/GLCheck.hpp>
#include <SFML/Graphics/GLExtensions.hpp>
#include <SFML/Graphics/OpenGL/OpenGLBuffer.hpp>
#include <SFML/Graphics/OpenGL/OpenGLProgram.hpp>
#include <SFML/Graphics/OpenGL/OpenGLResource.hpp>
#include <SFML/Graphics/OpenGL/OpenGLVertexArray.hpp>
#include <SFML/Graphics/ShaderUniformType.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <iomanip>
#include <limits>
#include <ostream>
namespace sf::priv
{
namespace
{
namespace OpenGLProgramImpl
{
std::size_t getMaxTextureUnits()
{
    static const GLint maxUnits = []
    {
        GLint value = 0;
        glCheck(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &value));

        return value;
    }();

    return static_cast<std::size_t>(maxUnits);
}
constexpr std::string_view defaultVertex = R"(
in vec4 sf_Vertex;
in vec4 sf_Color;
in vec4 sf_MultiTexCoord0;
uniform mat4 sf_ModelViewProjectionMatrix;
uniform mat4 sf_TextureMatrix;
out vec4 sf_FrontColor;
out vec4 sf_TexCoord0;

void main()
{
    gl_Position = sf_ModelViewProjectionMatrix * sf_Vertex;
    gl_PointSize = 1.0;
    sf_FrontColor = sf_Color;
    sf_TexCoord0 = sf_TextureMatrix * sf_MultiTexCoord0;
}
)";

constexpr std::string_view defaultFragment = R"(
uniform sampler2D sf_Texture;
uniform bool sf_TextureEnabled;
in vec4 sf_FrontColor;
in vec4 sf_TexCoord0;
out vec4 sf_FragColor;

void main()
{
    sf_FragColor = sf_TextureEnabled ? texture(sf_Texture, sf_TexCoord0.xy) * sf_FrontColor : sf_FrontColor;
}
)";

std::string builtIn(std::string_view body)
{
#ifdef SFML_OPENGL_ES
    std::string source = "#version 300 es\nprecision highp float;\n";
#else
    std::string source = "#version 150\n";
#endif
    source += body;
    return source;
}

} // namespace OpenGLProgramImpl
} // namespace
using namespace OpenGLProgramImpl;
using TransientContextLock = OpenGLResource::TransientContextLock;
struct OpenGLProgram::UniformBinder
{
    UniformBinder(OpenGLProgram& shader, const std::string& name) : program(shader.m_shaderProgram)
    {
        if (program)
        {
            glCheck(glGetIntegerv(GL_CURRENT_PROGRAM, &savedProgram));
            if (program != static_cast<GLuint>(savedProgram))
                glCheck(glUseProgram(program));
            location = shader.getUniformLocation(name);
        }
    }

    ~UniformBinder()
    {
        const auto previous = static_cast<GLuint>(savedProgram);
        if (program && program != previous)
        {
            // Switching programs can complete deletion of the previously bound program.
            glCheck(glUseProgram(previous && glIsProgram(previous) ? previous : 0));
        }
    }

    UniformBinder(const UniformBinder&)            = delete;
    UniformBinder& operator=(const UniformBinder&) = delete;

    TransientContextLock lock;
    GLint                savedProgram{};
    GLuint               program;
    GLint                location{-1};
};

OpenGLProgram::~OpenGLProgram()
{
    if (m_shaderProgram)
    {
        const TransientContextLock lock;
        glCheck(glDeleteProgram(m_shaderProgram));
    }
}

void OpenGLProgram::setUniform(const std::string& name, ShaderUniformType type, const void* values, std::size_t count)
{
    if (!values || !count)
        return;

    if (!m_shaderProgram || count > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()))
        return;

    const UniformBinder binder(*this, name);
    const auto          location = binder.location;
    if (location < 0)
        return;

    const auto  size = static_cast<GLsizei>(count);
    const auto* f    = static_cast<const GLfloat*>(values);
    const auto* i    = static_cast<const GLint*>(values);
    switch (type)
    {
        case ShaderUniformType::Float:
            glCheck(glUniform1fv(location, size, f));
            break;
        case ShaderUniformType::Vec2:
            glCheck(glUniform2fv(location, size, f));
            break;
        case ShaderUniformType::Vec3:
            glCheck(glUniform3fv(location, size, f));
            break;
        case ShaderUniformType::Vec4:
            glCheck(glUniform4fv(location, size, f));
            break;
        case ShaderUniformType::Int:
            glCheck(glUniform1iv(location, size, i));
            break;
        case ShaderUniformType::IVec2:
            glCheck(glUniform2iv(location, size, i));
            break;
        case ShaderUniformType::IVec3:
            glCheck(glUniform3iv(location, size, i));
            break;
        case ShaderUniformType::IVec4:
            glCheck(glUniform4iv(location, size, i));
            break;
        case ShaderUniformType::Mat3:
            glCheck(glUniformMatrix3fv(location, size, GL_FALSE, f));
            break;
        case ShaderUniformType::Mat4:
            glCheck(glUniformMatrix4fv(location, size, GL_FALSE, f));
            break;
    }
}

bool OpenGLProgram::compile(std::string_view          vertex,
                            std::string_view          geometry,
                            std::string_view          fragment,
                            const rhi::ShaderOptions& options)
{
    if (!validateShaderOptions(options) || (vertex.empty() && geometry.empty() && fragment.empty()))
        return false;

    if (!geometry.empty() && !geometryAvailable())
    {
        err() << "Geometry shaders are unavailable on the selected graphics device" << std::endl;
        return false;
    }

    const TransientContextLock lock;
    priv::ensureExtensionsInit();

    const bool programmable     = m_programmable;
    const auto fallbackVertex   = programmable && vertex.empty() ? builtIn(defaultVertex) : std::string{};
    const auto fallbackFragment = programmable && fragment.empty()
                                      ? builtIn(options.transformOutputs.empty()
                                                    ? defaultFragment
                                                    : std::string_view("out vec4 "
                                                                       "sf_FragColor; "
                                                                       "void main() { "
                                                                       "sf_FragColor = "
                                                                       "vec4(0.0); }"))
                                      : std::string{};
    if (!fallbackVertex.empty())
        vertex = fallbackVertex;
    if (!fallbackFragment.empty())
        fragment = fallbackFragment;

    const auto program = glCheck(glCreateProgram());
    if (!program)
        return false;

    const auto attach = [program](GLenum stage, std::string_view source)
    {
        if (source.empty())
            return true;
        if (source.size() > static_cast<std::size_t>(std::numeric_limits<GLint>::max()))
            return false;

        const auto shader = glCheck(glCreateShader(stage));
        if (!shader)
            return false;
        const auto* data = source.data();
        const auto  size = static_cast<GLint>(source.size());
        glCheck(glShaderSource(shader, 1, &data, &size));
        glCheck(glCompileShader(shader));

        GLint compiled{};
        glCheck(glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled));
        if (compiled)
        {
            glCheck(glAttachShader(program, shader));
        }
        else
        {
            GLint length{};
            glCheck(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length));
            std::vector<char> log(static_cast<std::size_t>(std::max(length, 1)));
            glCheck(glGetShaderInfoLog(shader, length, nullptr, log.data()));
            err() << "Failed to compile OpenGL shader\n" << log.data() << std::endl;
        }
        glCheck(glDeleteShader(shader));
        return compiled != GL_FALSE;
    };

    if (!attach(GL_VERTEX_SHADER, vertex) || !attach(GL_GEOMETRY_SHADER, geometry) || !attach(GL_FRAGMENT_SHADER, fragment))
    {
        glCheck(glDeleteProgram(program));
        return false;
    }

    if (programmable && options.vertexBindings.empty())
    {
        glCheck(glBindAttribLocation(program, 0, "sf_Vertex"));
        glCheck(glBindAttribLocation(program, 1, "sf_Color"));
        glCheck(glBindAttribLocation(program, 2, "sf_MultiTexCoord0"));
    }
    for (const auto& attribute : options.vertexAttributes)
        glCheck(glBindAttribLocation(program, attribute.location, attribute.name.c_str()));
    if (!options.transformOutputs.empty())
    {
        if (!glTransformFeedbackVaryings)
        {
            glDeleteProgram(program);
            return false;
        }
        std::vector<const char*> outputs;
        std::size_t              bytes = 0;
        for (const auto& output : options.transformOutputs)
        {
            outputs.push_back(output.name.c_str());
            bytes += output.components * sizeof(float);
        }
        if (bytes != options.transformStride)
        {
            glDeleteProgram(program);
            return false;
        }
        glCheck(
            glTransformFeedbackVaryings(program, static_cast<GLsizei>(outputs.size()), outputs.data(), GL_INTERLEAVED_ATTRIBS));
    }
    glCheck(glLinkProgram(program));
    GLint linked{};
    glCheck(glGetProgramiv(program, GL_LINK_STATUS, &linked));
    if (!linked)
    {
        GLint length{};
        glCheck(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length));
        std::vector<char> log(static_cast<std::size_t>(std::max(length, 1)));
        glCheck(glGetProgramInfoLog(program, length, nullptr, log.data()));
        err() << "Failed to link OpenGL shader\n" << log.data() << std::endl;
        glCheck(glDeleteProgram(program));
        return false;
    }

    if (m_shaderProgram)
        glCheck(glDeleteProgram(m_shaderProgram));

    m_options        = options;
    m_shaderProgram  = program;
    m_currentTexture = -1;
    m_textures.clear();
    m_uniforms.clear();
    glCheck(glFlush());
    return true;
}

std::vector<rhi::UniformInfo> OpenGLProgram::getActiveUniforms() const
{
    const TransientContextLock    lock;
    std::vector<rhi::UniformInfo> result;
    GLint                         count{};
    GLint                         length{};
    glGetProgramiv(m_shaderProgram, GL_ACTIVE_UNIFORMS, &count);
    glGetProgramiv(m_shaderProgram, GL_ACTIVE_UNIFORM_MAX_LENGTH, &length);
    std::vector<char> name(static_cast<std::size_t>(std::max(length, 1)));
    for (GLint index = 0; index < count; ++index)
    {
        GLenum  type{};
        GLint   size{};
        GLsizei used{};
        glGetActiveUniform(m_shaderProgram, static_cast<GLuint>(index), length, &used, &size, &type, name.data());
        rhi::UniformInfo info{std::string(name.data(), static_cast<std::size_t>(used)),
                              rhi::UniformType::Float,
                              static_cast<std::size_t>(size),
                              false};
        if (info.name.size() > 3 && info.name.substr(info.name.size() - 3) == "[0]")
            info.name.resize(info.name.size() - 3);
        switch (type)
        {
            case GL_FLOAT:
                break;
            case GL_FLOAT_VEC2:
                info.type = rhi::UniformType::Vec2;
                break;
            case GL_FLOAT_VEC3:
                info.type = rhi::UniformType::Vec3;
                break;
            case GL_FLOAT_VEC4:
                info.type = rhi::UniformType::Vec4;
                break;
            case GL_INT:
            case GL_BOOL:
                info.type = rhi::UniformType::Int;
                break;
            case GL_INT_VEC2:
            case GL_BOOL_VEC2:
                info.type = rhi::UniformType::IVec2;
                break;
            case GL_INT_VEC3:
            case GL_BOOL_VEC3:
                info.type = rhi::UniformType::IVec3;
                break;
            case GL_INT_VEC4:
            case GL_BOOL_VEC4:
                info.type = rhi::UniformType::IVec4;
                break;
            case GL_FLOAT_MAT3:
                info.type = rhi::UniformType::Mat3;
                break;
            case GL_FLOAT_MAT4:
                info.type = rhi::UniformType::Mat4;
                break;
            case GL_SAMPLER_2D:
                info.type    = rhi::UniformType::Int;
                info.sampler = true;
                break;
            default:
                continue;
        }
        for (const auto& alias : m_options.uniformAliases)
            if (alias.translatedName == info.name)
            {
                info.name = alias.name;
                break;
            }
        result.push_back(std::move(info));
    }
    return result;
}
bool OpenGLProgram::bindVertexBuffers(const std::vector<rhi::VertexBufferBinding>& buffers,
                                      std::size_t                                  first,
                                      std::size_t                                  count,
                                      std::size_t                                  instances) const
{
    const auto divisor = glVertexAttribDivisor ? glVertexAttribDivisor : glVertexAttribDivisorARB;
    if (!validateVertexBindings(m_options, buffers, first, count, instances) || !divisor)
        return false;
    GLint maximum{};
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maximum);
    return std::all_of(m_options.vertexAttributes.begin(),
                       m_options.vertexAttributes.end(),
                       [&](const auto& attribute)
                       {
                           if (attribute.binding >= buffers.size() ||
                               attribute.location >= static_cast<unsigned int>(maximum) || !attribute.components ||
                               attribute.components > 4)
                               return false;
                           const auto& input  = buffers[attribute.binding];
                           const auto& layout = m_options.vertexBindings[attribute.binding];
                           const auto* buffer = input.buffer ? resourceCast<const OpenGLBuffer>(input.buffer.get()) : nullptr;
                           if (!buffer || !layout.stride ||
                               layout.stride > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()) ||
                               attribute.offset + attribute.components * sizeof(float) > layout.stride)
                               return false;
                           glBindBuffer(GL_ARRAY_BUFFER, buffer->nativeHandle());
                           glEnableVertexAttribArray(attribute.location);
                           glVertexAttribPointer(attribute.location,
                                                 static_cast<GLint>(attribute.components),
                                                 GL_FLOAT,
                                                 GL_FALSE,
                                                 static_cast<GLsizei>(layout.stride),
                                                 reinterpret_cast<const void*>(input.offset + attribute.offset));
                           divisor(attribute.location, layout.stepRate);
                           return true;
                       });
}
bool OpenGLProgram::transformVertices(const std::vector<rhi::VertexBufferBinding>& buffers,
                                      std::size_t                                  count,
                                      const rhi::Buffer&                           output)
{
    const TransientContextLock lock;
    const auto*                buffer = resourceCast<const OpenGLBuffer>(&output);
    if (!buffer || m_options.transformOutputs.empty() || !m_options.transformStride ||
        count > output.getSize() / m_options.transformStride ||
        count > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()) || !glBeginTransformFeedback ||
        !glGenVertexArrays)
        return false;
    for (const auto& input : buffers)
        if (input.buffer.get() == &output)
            return false;
    GLint oldArray{};
    GLint oldBuffer{};
    GLint oldProgram{};
    GLint oldFeedback{};
    glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldArray);
    glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
    glGetIntegerv(GL_CURRENT_PROGRAM, &oldProgram);
    glGetIntegeri_v(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING, 0, &oldFeedback);
    const bool discard = glIsEnabled(GL_RASTERIZER_DISCARD) != GL_FALSE;
    const auto array   = OpenGLVertexArray::acquire();
    glBindVertexArray(array->handle);
    const bool valid = bindVertexBuffers(buffers, 0, count, 1);
    if (valid)
    {
        bind(this);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, buffer->nativeHandle());
        glEnable(GL_RASTERIZER_DISCARD);
        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(count));
        glEndTransformFeedback();
    }
    if (!discard)
        glDisable(GL_RASTERIZER_DISCARD);
    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, static_cast<GLuint>(oldFeedback));
    const auto divisor = glVertexAttribDivisor ? glVertexAttribDivisor : glVertexAttribDivisorARB;
    for (const auto& attribute : m_options.vertexAttributes)
    {
        glDisableVertexAttribArray(attribute.location);
        if (divisor)
            divisor(attribute.location, 0);
    }
    glBindVertexArray(static_cast<GLuint>(oldArray));
    glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(oldBuffer));
    glUseProgram(static_cast<GLuint>(oldProgram));
    return valid;
}

void OpenGLProgram::bindTextures() const
{
    auto it = m_textures.begin();
    for (std::size_t i = 0; i < m_textures.size(); ++i)
    {
        const auto index = static_cast<GLsizei>(i + 1);
        glCheck(glUniform1i(it->first, index));
        glCheck(GLEXT_glActiveTexture(GLEXT_GL_TEXTURE0 + static_cast<GLenum>(index)));
        bindOpenGLTexture(it->second.get(), CoordinateType::Normalized, m_programmable);
        ++it;
    }

    // Make sure that the texture unit which is left active is the number 0
    glCheck(GLEXT_glActiveTexture(GLEXT_GL_TEXTURE0));
}

int OpenGLProgram::getUniformLocation(const std::string& name)
{
    std::string lookup = name;
    for (const auto& alias : m_options.uniformAliases)
        if (alias.name == name)
        {
            lookup = alias.translatedName;
            break;
        }
    // Check the cache
    if (const auto it = m_uniforms.find(name); it != m_uniforms.end())
    {
        // Already in cache, return it
        return it->second;
    }

    // Not in cache, request the location from OpenGL
    const int location = glGetUniformLocation(m_shaderProgram, lookup.c_str());
    m_uniforms.try_emplace(name, location);

    if (location == -1)
        err() << "Uniform " << std::quoted(name) << " not found in shader" << std::endl;

    return location;
}

bool OpenGLProgram::loadDefault()
{
    return compile(builtIn(defaultVertex), {}, builtIn(defaultFragment));
}

bool OpenGLProgram::geometryAvailable()
{

    const TransientContextLock lock;
    priv::ensureExtensionsInit();
#ifdef SFML_OPENGL_ES
    return SF_GLAD_GL_ES_VERSION_3_2 || SF_GLAD_GL_EXT_geometry_shader;
#else
    return GLEXT_GL_VERSION_3_2 || GLEXT_geometry_shader4;
#endif
}

void OpenGLProgram::setSampler(const std::string& name, std::shared_ptr<rhi::Texture> texture, bool current)
{
    if (!m_shaderProgram)
        return;
    const TransientContextLock lock;
    const auto                 location = getUniformLocation(name);
    if (location < 0)
        return;
    if (current)
    {
        m_currentTexture = location;
        m_textures.erase(location);
        return;
    }
    if (m_currentTexture == location)
        m_currentTexture = -1;
    if (m_textures.find(location) == m_textures.end() && m_textures.size() + 1 >= getMaxTextureUnits())
        return;
    m_textures[location] = std::move(texture);
}
void OpenGLProgram::bind(const rhi::ShaderProgram* value)
{
    const TransientContextLock lock;
    ensureExtensionsInit();
    const auto* shader = resourceCast<const OpenGLProgram>(value);
    glCheck(glUseProgram(shader ? shader->m_shaderProgram : 0));
    if (shader)
    {
        shader->bindTextures();
        if (shader->m_currentTexture != -1)
            glCheck(glUniform1i(shader->m_currentTexture, 0));
    }
}

} // namespace sf::priv
