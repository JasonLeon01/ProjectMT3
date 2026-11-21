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
#include <SFML/Graphics/GraphicsDevice.hpp>
#include <SFML/Graphics/OpenGL/OpenGLBuffer.hpp>
#include <SFML/Graphics/OpenGL/OpenGLProgram.hpp>
#include <SFML/Graphics/OpenGL/OpenGLRenderTarget.hpp>
#include <SFML/Graphics/OpenGL/OpenGLResource.hpp>
#include <SFML/Graphics/OpenGL/OpenGLTexture.hpp>
#include <SFML/Graphics/RenderTextureImplDefault.hpp>
#include <SFML/Graphics/RenderTextureImplFBO.hpp>

#include <SFML/Window/Context.hpp>

#include <SFML/System/EnumArray.hpp>
#include <SFML/System/Err.hpp>

#include <algorithm>
#include <iterator>
#include <limits>
#include <mutex>
#include <ostream>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cassert>
#include <cmath>
#include <cstddef>


namespace
{
// A nested named namespace is used here to allow unity builds of SFML.
namespace OpenGLRenderTargetImpl
{
// Mutex to protect ID generation and our context-RenderTarget-map
std::recursive_mutex& getMutex()
{
    static std::recursive_mutex mutex;
    return mutex;
}

// Unique identifier, used for identifying RenderTargets when
// tracking the currently active RenderTarget within a given context
std::uint64_t getUniqueId()
{
    const std::scoped_lock lock(getMutex());
    static std::uint64_t   id = 1; // start at 1, zero is "no RenderTarget"
    return id++;
}

// Map to help us detect whether a different RenderTarget
// has been activated within a single context
using ContextRenderTargetMap = std::unordered_map<std::uint64_t, std::uint64_t>;
ContextRenderTargetMap& getContextRenderTargetMap()
{
    static ContextRenderTargetMap contextRenderTargetMap;
    return contextRenderTargetMap;
}

// Check if a RenderTarget with the given ID is active in the current context
bool isActive(std::uint64_t id)
{
    const std::scoped_lock lock(getMutex());
    const auto             it = getContextRenderTargetMap().find(sf::Context::getActiveContextId());
    return (it != getContextRenderTargetMap().end()) && (it->second == id);
}

// Convert an sf::BlendMode::Factor constant to the corresponding OpenGL constant.
std::uint32_t factorToGlConstant(sf::BlendMode::Factor blendFactor)
{
    // clang-format off
    switch (blendFactor)
    {
        case sf::BlendMode::Factor::Zero:             return GL_ZERO;
        case sf::BlendMode::Factor::One:              return GL_ONE;
        case sf::BlendMode::Factor::SrcColor:         return GL_SRC_COLOR;
        case sf::BlendMode::Factor::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
        case sf::BlendMode::Factor::DstColor:         return GL_DST_COLOR;
        case sf::BlendMode::Factor::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
        case sf::BlendMode::Factor::SrcAlpha:         return GL_SRC_ALPHA;
        case sf::BlendMode::Factor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
        case sf::BlendMode::Factor::DstAlpha:         return GL_DST_ALPHA;
        case sf::BlendMode::Factor::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
    }
    // clang-format on

    sf::err() << "Invalid value for sf::BlendMode::Factor! Fallback to sf::BlendMode::Factor::Zero." << std::endl;
    assert(false);
    return GL_ZERO;
}


// Convert an sf::BlendMode::Equation constant to the corresponding OpenGL constant.
std::uint32_t equationToGlConstant(sf::BlendMode::Equation blendEquation)
{
    switch (blendEquation)
    {
        case sf::BlendMode::Equation::Add:
            return GLEXT_GL_FUNC_ADD;
        case sf::BlendMode::Equation::Subtract:
            if (GLEXT_blend_subtract)
                return GLEXT_GL_FUNC_SUBTRACT;
            break;
        case sf::BlendMode::Equation::ReverseSubtract:
            if (GLEXT_blend_subtract)
                return GLEXT_GL_FUNC_REVERSE_SUBTRACT;
            break;
        case sf::BlendMode::Equation::Min:
            if (GLEXT_blend_minmax)
                return GLEXT_GL_MIN;
            break;
        case sf::BlendMode::Equation::Max:
            if (GLEXT_blend_minmax)
                return GLEXT_GL_MAX;
            break;
    }

    static bool warned = false;
    if (!warned)
    {
        sf::err() << "OpenGL extension EXT_blend_minmax or EXT_blend_subtract unavailable" << '\n'
                  << "Some blending equations will fallback to sf::BlendMode::Equation::Add" << '\n'
                  << "Ensure that hardware acceleration is enabled if available" << std::endl;

        warned = true;
    }

    return GLEXT_GL_FUNC_ADD;
}


// Convert an UpdateOperation constant to the corresponding OpenGL constant.
std::uint32_t stencilOperationToGlConstant(sf::StencilUpdateOperation operation)
{
    // clang-format off
    switch (operation)
    {
        case sf::StencilUpdateOperation::Keep:      return GL_KEEP;
        case sf::StencilUpdateOperation::Zero:      return GL_ZERO;
        case sf::StencilUpdateOperation::Replace:   return GL_REPLACE;
        case sf::StencilUpdateOperation::Increment: return GL_INCR;
        case sf::StencilUpdateOperation::Decrement: return GL_DECR;
        case sf::StencilUpdateOperation::Invert:    return GL_INVERT;
    }
    // clang-format on

    sf::err() << "Invalid value for sf::StencilUpdateOperation! Fallback to sf::StencilMode::Keep." << std::endl;
    assert(false);
    return GL_KEEP;
}


// Convert a Comparison constant to the corresponding OpenGL constant.
std::uint32_t stencilFunctionToGlConstant(sf::StencilComparison comparison)
{
    // clang-format off
    switch (comparison)
    {
        case sf::StencilComparison::Never:        return GL_NEVER;
        case sf::StencilComparison::Less:         return GL_LESS;
        case sf::StencilComparison::LessEqual:    return GL_LEQUAL;
        case sf::StencilComparison::Greater:      return GL_GREATER;
        case sf::StencilComparison::GreaterEqual: return GL_GEQUAL;
        case sf::StencilComparison::Equal:        return GL_EQUAL;
        case sf::StencilComparison::NotEqual:     return GL_NOTEQUAL;
        case sf::StencilComparison::Always:       return GL_ALWAYS;
    }
    // clang-format on

    sf::err() << "Invalid value for sf::StencilComparison! Fallback to sf::StencilMode::Always." << std::endl;
    assert(false);
    return GL_ALWAYS;
}
} // namespace OpenGLRenderTargetImpl
} // namespace


namespace sf::priv
{
struct OpenGLRenderTarget::ProgrammableState
{
    struct State
    {
        GLint vertexArray{};

        GLint                    program{};
        GLint                    activeTexture{};
        std::vector<GLint>       textureBindings;
        GLint                    arrayBuffer{};
        std::array<GLint, 4>     viewport{};
        std::array<GLint, 4>     scissorBox{};
        GLboolean                blendEnabled{};
        GLboolean                stencilEnabled{};
        GLboolean                scissorEnabled{};
        GLboolean                cullEnabled{};
        GLboolean                depthEnabled{};
        GLint                    blendSrcRgb{};
        GLint                    blendDstRgb{};
        GLint                    blendSrcAlpha{};
        GLint                    blendDstAlpha{};
        GLint                    blendEquationRgb{};
        GLint                    blendEquationAlpha{};
        GLint                    stencilFunc{};
        GLint                    stencilRef{};
        GLint                    stencilValueMask{};
        GLint                    stencilFail{};
        GLint                    stencilDepthFail{};
        GLint                    stencilDepthPass{};
        GLint                    stencilWriteMask{};
        GLint                    stencilBackFunc{};
        GLint                    stencilBackRef{};
        GLint                    stencilBackValueMask{};
        GLint                    stencilBackFail{};
        GLint                    stencilBackDepthFail{};
        GLint                    stencilBackDepthPass{};
        GLint                    stencilBackWriteMask{};
        std::array<GLboolean, 4> colorMask{};
        std::array<GLfloat, 4>   clearColor{};
        GLint                    clearStencil{};
#ifndef SFML_OPENGL_ES
        bool      framebufferSrgbSupported{};
        GLboolean framebufferSrgbEnabled{};
#endif
    };

    std::vector<State>             states;
    std::unique_ptr<OpenGLProgram> defaultShader;
};

OpenGLRenderTarget::OpenGLRenderTarget(bool programmable) :
    m_modern(programmable ? std::make_unique<ProgrammableState>() : nullptr),
    m_id(OpenGLRenderTargetImpl::getUniqueId())
{
}

OpenGLRenderTarget::~OpenGLRenderTarget() = default;

void OpenGLRenderTarget::viewChanged()
{
    m_cache.viewChanged = true;
}

void OpenGLRenderTarget::clear(const rhi::TargetState& target, std::optional<Color> color, std::optional<StencilValue> stencil)
{
    if (!isActive() && !setActive(true))
        return;

    ensureExtensionsInit();
    applyTexture(nullptr);
    applyCurrentView(target);

#ifndef SFML_OPENGL_ES
    if (GLEXT_framebuffer_sRGB)
    {
        if (target.sRgb)
            glCheck(glEnable(GL_FRAMEBUFFER_SRGB));
        else
            glCheck(glDisable(GL_FRAMEBUFFER_SRGB));
    }
#endif

    GLbitfield mask{};
    if (color)
    {
        glCheck(glClearColor(color->r / 255.f, color->g / 255.f, color->b / 255.f, color->a / 255.f));
        mask |= GL_COLOR_BUFFER_BIT;
    }
    if (stencil)
    {
        glCheck(glClearStencil(static_cast<int>(stencil->value)));
        mask |= GL_STENCIL_BUFFER_BIT;
    }
    glCheck(glClear(mask));
}

void OpenGLRenderTarget::draw(const rhi::TargetState&  target,
                              const Vertex*            vertices,
                              std::size_t              vertexCount,
                              PrimitiveType            type,
                              const rhi::DrawSettings& states)
{
    if (!vertices || !vertexCount)
        return;
    if (isActive() || setActive(true))
    {
        if (m_modern)
        {
            drawProgrammable(target, vertices, 0, 0, vertexCount, type, states);
            return;
        }

        // Check if the vertex count is low enough so that we can pre-transform them
        const bool useVertexCache = (vertexCount <= m_cache.vertexCache.size());

        if (useVertexCache)
        {
            // Pre-transform the vertices and store them into the vertex cache
            for (std::size_t i = 0; i < vertexCount; ++i)
            {
                Vertex& vertex   = m_cache.vertexCache[i];
                vertex.position  = states.transform * vertices[i].position;
                vertex.color     = vertices[i].color;
                vertex.texCoords = vertices[i].texCoords;
            }
        }

        setupDraw(target, useVertexCache, states);

        // Check if texture coordinates array is needed, and update client state accordingly
        const bool enableTexCoordsArray = (states.texture || states.shader);
        if (!m_cache.enable || (enableTexCoordsArray != m_cache.texCoordsArrayEnabled))
        {
            if (enableTexCoordsArray)
                glCheck(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
            else
                glCheck(glDisableClientState(GL_TEXTURE_COORD_ARRAY));
        }

        // If we switch between non-cache and cache mode or enable texture
        // coordinates we need to set up the pointers to the vertices' components
        if (!m_cache.enable || !useVertexCache || !m_cache.useVertexCache)
        {
            const auto* data = reinterpret_cast<const std::byte*>(vertices);

            // If we pre-transform the vertices, we must use our internal vertex cache
            if (useVertexCache)
                data = reinterpret_cast<const std::byte*>(m_cache.vertexCache.data());

            glCheck(glVertexPointer(2, GL_FLOAT, sizeof(Vertex), data + 0));
            glCheck(glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), data + 8));
            if (enableTexCoordsArray)
                glCheck(glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), data + 12));
        }
        else if (enableTexCoordsArray && !m_cache.texCoordsArrayEnabled)
        {
            // If we enter this block, we are already using our internal vertex cache
            const auto* data = reinterpret_cast<const std::byte*>(m_cache.vertexCache.data());

            glCheck(glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), data + 12));
        }

        drawPrimitives(type, 0, vertexCount);
        cleanupDraw(states);

        // Update the cache
        m_cache.useVertexCache        = useVertexCache;
        m_cache.texCoordsArrayEnabled = enableTexCoordsArray;
    }
}


void OpenGLRenderTarget::draw(const rhi::TargetState&  target,
                              const rhi::Buffer&       vertexBuffer,
                              std::size_t              firstVertex,
                              std::size_t              vertexCount,
                              PrimitiveType            type,
                              const rhi::DrawSettings& states)
{
    if (firstVertex > vertexBuffer.getSize() / sizeof(Vertex) ||
        vertexCount > vertexBuffer.getSize() / sizeof(Vertex) - firstVertex)
        return;
    if (isActive() || setActive(true))
    {
        if (m_modern)
        {
            drawProgrammable(target,
                             nullptr,
                             static_cast<const OpenGLBuffer&>(vertexBuffer).nativeHandle(),
                             firstVertex,
                             vertexCount,
                             type,
                             states);
            return;
        }

        setupDraw(target, false, states);

        // Bind vertex buffer
        OpenGLBuffer::bind(&vertexBuffer);

        // Always enable texture coordinates
        if (!m_cache.enable || !m_cache.texCoordsArrayEnabled)
            glCheck(glEnableClientState(GL_TEXTURE_COORD_ARRAY));

        glCheck(glVertexPointer(2, GL_FLOAT, sizeof(Vertex), reinterpret_cast<const void*>(0)));
        glCheck(glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), reinterpret_cast<const void*>(8)));
        glCheck(glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), reinterpret_cast<const void*>(12)));

        drawPrimitives(type, firstVertex, vertexCount);

        // Unbind vertex buffer
        OpenGLBuffer::bind(nullptr);

        cleanupDraw(states);

        // Update the cache
        m_cache.useVertexCache        = false;
        m_cache.texCoordsArrayEnabled = true;
    }
}


bool OpenGLRenderTarget::isActive() const
{
    return !m_windowResized && OpenGLRenderTargetImpl::isActive(m_id);
}

bool OpenGLRenderTarget::setActive(bool active)
{
    if (m_attachment && !m_attachment->activate(active))
        return false;
    // EGL may resize the drawable only when it is rebound, even if this context
    // was already current. Defer that rebind until the window is next used.
    if (active && m_windowResized && m_window && !m_window->setActive(false))
        return false;
    if (m_window && !m_window->setActive(active))
        return false;
    if (active)
        m_windowResized = false;
    if (active && m_window && RenderTextureImplFBO::isAvailable())
        glCheck(GLEXT_glBindFramebuffer(GLEXT_GL_FRAMEBUFFER, m_framebuffer));
    // Mark this RenderTarget as active or no longer active in the tracking map
    const std::scoped_lock lock(OpenGLRenderTargetImpl::getMutex());

    const std::uint64_t contextId = Context::getActiveContextId();
    if (!contextId)
    {
        m_cache.enable = false;
        return !active;
    }

    using OpenGLRenderTargetImpl::getContextRenderTargetMap;
    auto&      contextRenderTargetMap = getContextRenderTargetMap();
    const auto it                     = contextRenderTargetMap.find(contextId);

    if (active)
    {
        if (it == contextRenderTargetMap.end())
        {
            contextRenderTargetMap[contextId] = m_id;

            m_cache.glStatesSet = false;
            m_cache.enable      = false;
        }
        else if (it->second != m_id)
        {
            it->second = m_id;

            m_cache.enable = false;
        }
    }
    else
    {
        if (it != contextRenderTargetMap.end())
            contextRenderTargetMap.erase(it);

        m_cache.enable = false;
    }

    return true;
}


////////////////////////////////////////////////////////////
void OpenGLRenderTarget::pushGLStates(const rhi::TargetState& target)
{
    if (isActive() || setActive(true))
    {
#ifdef SFML_DEBUG
        ensureExtensionsInit();
        // make sure that the user didn't leave an unchecked OpenGL error
        const GLenum error = glGetError();
        if (error != GL_NO_ERROR)
        {
            err() << "OpenGL error (" << error << ") detected in user code, "
                  << "you should check for errors with glGetError()" << std::endl;
        }
#endif

        ensureExtensionsInit();
        if (m_modern)
        {
            ProgrammableState::State state;

            glCheck(glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &state.vertexArray));

            glCheck(glGetIntegerv(GL_CURRENT_PROGRAM, &state.program));
            glCheck(glGetIntegerv(GL_ACTIVE_TEXTURE, &state.activeTexture));

            GLint textureUnitCount = 0;
            glCheck(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &textureUnitCount));
            state.textureBindings.resize(static_cast<std::size_t>(std::max(textureUnitCount, 1)));
            for (std::size_t i = 0; i < state.textureBindings.size(); ++i)
            {
                glCheck(glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i)));
                glCheck(glGetIntegerv(GL_TEXTURE_BINDING_2D, &state.textureBindings[i]));
            }
            glCheck(glActiveTexture(static_cast<GLenum>(state.activeTexture)));

            glCheck(glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &state.arrayBuffer));
            glCheck(glGetIntegerv(GL_VIEWPORT, state.viewport.data()));
            glCheck(glGetIntegerv(GL_SCISSOR_BOX, state.scissorBox.data()));
            state.blendEnabled   = glIsEnabled(GL_BLEND);
            state.stencilEnabled = glIsEnabled(GL_STENCIL_TEST);
            state.scissorEnabled = glIsEnabled(GL_SCISSOR_TEST);
            state.cullEnabled    = glIsEnabled(GL_CULL_FACE);
            state.depthEnabled   = glIsEnabled(GL_DEPTH_TEST);

            glCheck(glGetIntegerv(GL_BLEND_SRC_RGB, &state.blendSrcRgb));
            glCheck(glGetIntegerv(GL_BLEND_DST_RGB, &state.blendDstRgb));
            glCheck(glGetIntegerv(GL_BLEND_SRC_ALPHA, &state.blendSrcAlpha));
            glCheck(glGetIntegerv(GL_BLEND_DST_ALPHA, &state.blendDstAlpha));
            glCheck(glGetIntegerv(GL_BLEND_EQUATION_RGB, &state.blendEquationRgb));
            glCheck(glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &state.blendEquationAlpha));

            glCheck(glGetIntegerv(GL_STENCIL_FUNC, &state.stencilFunc));
            glCheck(glGetIntegerv(GL_STENCIL_REF, &state.stencilRef));
            glCheck(glGetIntegerv(GL_STENCIL_VALUE_MASK, &state.stencilValueMask));
            glCheck(glGetIntegerv(GL_STENCIL_FAIL, &state.stencilFail));
            glCheck(glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &state.stencilDepthFail));
            glCheck(glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &state.stencilDepthPass));
            glCheck(glGetIntegerv(GL_STENCIL_WRITEMASK, &state.stencilWriteMask));
            glCheck(glGetIntegerv(GL_STENCIL_BACK_FUNC, &state.stencilBackFunc));
            glCheck(glGetIntegerv(GL_STENCIL_BACK_REF, &state.stencilBackRef));
            glCheck(glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &state.stencilBackValueMask));
            glCheck(glGetIntegerv(GL_STENCIL_BACK_FAIL, &state.stencilBackFail));
            glCheck(glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &state.stencilBackDepthFail));
            glCheck(glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &state.stencilBackDepthPass));
            glCheck(glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &state.stencilBackWriteMask));

            glCheck(glGetBooleanv(GL_COLOR_WRITEMASK, state.colorMask.data()));
            glCheck(glGetFloatv(GL_COLOR_CLEAR_VALUE, state.clearColor.data()));
            glCheck(glGetIntegerv(GL_STENCIL_CLEAR_VALUE, &state.clearStencil));

#ifndef SFML_OPENGL_ES
            state.framebufferSrgbSupported = GLEXT_framebuffer_sRGB;
            if (state.framebufferSrgbSupported)
                state.framebufferSrgbEnabled = glIsEnabled(GL_FRAMEBUFFER_SRGB);
#endif

            m_modern->states.push_back(std::move(state));
        }
#ifndef SFML_OPENGL_ES
        else
        {
            glCheck(glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS));
            glCheck(glPushAttrib(GL_ALL_ATTRIB_BITS));
            glCheck(glMatrixMode(GL_MODELVIEW));
            glCheck(glPushMatrix());
            glCheck(glMatrixMode(GL_PROJECTION));
            glCheck(glPushMatrix());
            glCheck(glMatrixMode(GL_TEXTURE));
            glCheck(glPushMatrix());
        }
#endif
    }

    resetGLStates(target);
}


////////////////////////////////////////////////////////////
void OpenGLRenderTarget::popGLStates(const rhi::TargetState&)
{
    if (!isActive() && !setActive(true))
        return;

    if (m_modern)
    {
        if (m_modern->states.empty())
            return;
        ProgrammableState::State state = std::move(m_modern->states.back());
        m_modern->states.pop_back();

        glCheck(glUseProgram(static_cast<GLuint>(state.program)));

        for (std::size_t i = 0; i < state.textureBindings.size(); ++i)
        {
            glCheck(glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(i)));
            glCheck(glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(state.textureBindings[i])));
        }
        glCheck(glActiveTexture(static_cast<GLenum>(state.activeTexture)));

        glCheck(glBindVertexArray(static_cast<GLuint>(state.vertexArray)));
        glCheck(glBindBuffer(GL_ARRAY_BUFFER, static_cast<GLuint>(state.arrayBuffer)));

        glCheck(glViewport(state.viewport[0], state.viewport[1], state.viewport[2], state.viewport[3]));
        glCheck(glScissor(state.scissorBox[0], state.scissorBox[1], state.scissorBox[2], state.scissorBox[3]));
        glCheck(glBlendFuncSeparate(static_cast<GLenum>(state.blendSrcRgb),
                                    static_cast<GLenum>(state.blendDstRgb),
                                    static_cast<GLenum>(state.blendSrcAlpha),
                                    static_cast<GLenum>(state.blendDstAlpha)));
        glCheck(glBlendEquationSeparate(static_cast<GLenum>(state.blendEquationRgb),
                                        static_cast<GLenum>(state.blendEquationAlpha)));

        glCheck(glStencilFuncSeparate(GL_FRONT,
                                      static_cast<GLenum>(state.stencilFunc),
                                      state.stencilRef,
                                      static_cast<GLuint>(state.stencilValueMask)));
        glCheck(glStencilOpSeparate(GL_FRONT,
                                    static_cast<GLenum>(state.stencilFail),
                                    static_cast<GLenum>(state.stencilDepthFail),
                                    static_cast<GLenum>(state.stencilDepthPass)));
        glCheck(glStencilMaskSeparate(GL_FRONT, static_cast<GLuint>(state.stencilWriteMask)));
        glCheck(glStencilFuncSeparate(GL_BACK,
                                      static_cast<GLenum>(state.stencilBackFunc),
                                      state.stencilBackRef,
                                      static_cast<GLuint>(state.stencilBackValueMask)));
        glCheck(glStencilOpSeparate(GL_BACK,
                                    static_cast<GLenum>(state.stencilBackFail),
                                    static_cast<GLenum>(state.stencilBackDepthFail),
                                    static_cast<GLenum>(state.stencilBackDepthPass)));
        glCheck(glStencilMaskSeparate(GL_BACK, static_cast<GLuint>(state.stencilBackWriteMask)));

        glCheck(glColorMask(state.colorMask[0], state.colorMask[1], state.colorMask[2], state.colorMask[3]));
        glCheck(glClearColor(state.clearColor[0], state.clearColor[1], state.clearColor[2], state.clearColor[3]));
        glCheck(glClearStencil(state.clearStencil));

        const auto restoreEnable = [](GLenum capability, GLboolean enabled)
        {
            if (enabled)
                glCheck(glEnable(capability));
            else
                glCheck(glDisable(capability));
        };

        restoreEnable(GL_BLEND, state.blendEnabled);
        restoreEnable(GL_STENCIL_TEST, state.stencilEnabled);
        restoreEnable(GL_SCISSOR_TEST, state.scissorEnabled);
        restoreEnable(GL_CULL_FACE, state.cullEnabled);
        restoreEnable(GL_DEPTH_TEST, state.depthEnabled);

#ifndef SFML_OPENGL_ES
        if (state.framebufferSrgbSupported)
            restoreEnable(GL_FRAMEBUFFER_SRGB, state.framebufferSrgbEnabled);
#endif
    }
#ifndef SFML_OPENGL_ES
    else
    {
        glCheck(glMatrixMode(GL_PROJECTION));
        glCheck(glPopMatrix());
        glCheck(glMatrixMode(GL_MODELVIEW));
        glCheck(glPopMatrix());
        glCheck(glMatrixMode(GL_TEXTURE));
        glCheck(glPopMatrix());
        glCheck(glPopClientAttrib());
        glCheck(glPopAttrib());
    }
#endif
    m_cache.enable      = false;
    m_cache.glStatesSet = false;
}


////////////////////////////////////////////////////////////
void OpenGLRenderTarget::resetGLStates(const rhi::TargetState&)
{
    // Check here to make sure a context change does not happen after activate(true)
    const bool shaderAvailable       = true;
    const bool vertexBufferAvailable = true;

// Workaround for states not being properly reset on
// macOS unless a context switch really takes place
#ifdef SFML_SYSTEM_MACOS
    if (!setActive(false))
    {
        err() << "Failed to set render target inactive" << std::endl;
    }
#endif

    if (isActive() || setActive(true))
    {
        // Make sure that extensions are initialized
        ensureExtensionsInit();

        // Make sure that the texture unit which is active is the number 0
        if (GLEXT_multitexture)
        {
            if (!m_modern)
                glCheck(glClientActiveTexture(GL_TEXTURE0));
            glCheck(GLEXT_glActiveTexture(GLEXT_GL_TEXTURE0));
        }

        // Define the default OpenGL states
        glCheck(glDisable(GL_CULL_FACE));
        if (!m_modern)
            glCheck(glDisable(GL_LIGHTING));
        glCheck(glDisable(GL_STENCIL_TEST));
        glCheck(glDisable(GL_DEPTH_TEST));
        if (!m_modern)
            glCheck(glDisable(GL_ALPHA_TEST));
        glCheck(glDisable(GL_SCISSOR_TEST));
        if (!m_modern)
            glCheck(glEnable(GL_TEXTURE_2D));
        glCheck(glEnable(GL_BLEND));
        if (!m_modern)
            glCheck(glMatrixMode(GL_MODELVIEW));
        if (!m_modern)
            glCheck(glLoadIdentity());
        if (!m_modern)
            glCheck(glEnableClientState(GL_VERTEX_ARRAY));
        if (!m_modern)
            glCheck(glEnableClientState(GL_COLOR_ARRAY));
        if (!m_modern)
            glCheck(glEnableClientState(GL_TEXTURE_COORD_ARRAY));
        glCheck(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
        m_cache.scissorEnabled = false;
        m_cache.stencilEnabled = false;
        m_cache.glStatesSet    = true;

        // Apply the default SFML states
        applyBlendMode(BlendAlpha);
        applyStencilMode(StencilMode());
        applyTexture(nullptr);
        if (shaderAvailable)
            OpenGLProgram::bind(nullptr);

        if (vertexBufferAvailable)
            glCheck(OpenGLBuffer::bind(nullptr));

        m_cache.texCoordsArrayEnabled = true;

        m_cache.useVertexCache = false;

        // Set the default view
        m_cache.viewChanged = true;

        m_cache.enable = true;
    }
}


void OpenGLRenderTarget::applyCurrentView(const rhi::TargetState& target)
{
    // Set the viewport
    const IntRect viewport    = target.viewport;
    const int     viewportTop = static_cast<int>(target.size.y) - (viewport.position.y + viewport.size.y);
    glCheck(glViewport(viewport.position.x, viewportTop, viewport.size.x, viewport.size.y));

    // Set the scissor rectangle and enable/disable scissor testing
    if (target.scissor == IntRect({}, Vector2i(target.size)))
    {
        if (!m_cache.enable || m_cache.scissorEnabled)
        {
            glCheck(glDisable(GL_SCISSOR_TEST));
            m_cache.scissorEnabled = false;
        }
    }
    else
    {
        const IntRect pixelScissor = target.scissor;
        const int     scissorTop   = static_cast<int>(target.size.y) - (pixelScissor.position.y + pixelScissor.size.y);
        glCheck(glScissor(pixelScissor.position.x, scissorTop, pixelScissor.size.x, pixelScissor.size.y));

        if (!m_cache.enable || !m_cache.scissorEnabled)
        {
            glCheck(glEnable(GL_SCISSOR_TEST));
            m_cache.scissorEnabled = true;
        }
    }

    if (!m_modern)
    {
        glCheck(glMatrixMode(GL_PROJECTION));
        glCheck(glLoadMatrixf(target.projection.getMatrix()));
        glCheck(glMatrixMode(GL_MODELVIEW));
    }

    m_cache.viewChanged = false;
}


////////////////////////////////////////////////////////////
void OpenGLRenderTarget::applyBlendMode(const BlendMode& mode)
{
    using OpenGLRenderTargetImpl::equationToGlConstant;
    using OpenGLRenderTargetImpl::factorToGlConstant;

    // Apply the blend mode, falling back to the non-separate versions if necessary
    if (GLEXT_blend_func_separate)
    {
        glCheck(GLEXT_glBlendFuncSeparate(factorToGlConstant(mode.colorSrcFactor),
                                          factorToGlConstant(mode.colorDstFactor),
                                          factorToGlConstant(mode.alphaSrcFactor),
                                          factorToGlConstant(mode.alphaDstFactor)));
    }
    else
    {
        glCheck(glBlendFunc(factorToGlConstant(mode.colorSrcFactor), factorToGlConstant(mode.colorDstFactor)));
    }

    if (GLEXT_blend_equation)
    {
        if (GLEXT_blend_equation_separate)
        {
            glCheck(GLEXT_glBlendEquationSeparate(equationToGlConstant(mode.colorEquation),
                                                  equationToGlConstant(mode.alphaEquation)));
        }
        else
        {
            glCheck(GLEXT_glBlendEquation(equationToGlConstant(mode.colorEquation)));
        }
    }
    else if ((mode.colorEquation != BlendMode::Equation::Add) || (mode.alphaEquation != BlendMode::Equation::Add))
    {
        static bool warned = false;

        if (!warned)
        {
#ifdef SFML_OPENGL_ES
            err() << "OpenGL ES extension OES_blend_subtract unavailable" << std::endl;
#else
            err() << "OpenGL extension EXT_blend_minmax and EXT_blend_subtract unavailable" << std::endl;
#endif
            err() << "Selecting a blend equation not possible" << '\n'
                  << "Ensure that hardware acceleration is enabled if available" << std::endl;

            warned = true;
        }
    }

    m_cache.lastBlendMode = mode;
}


////////////////////////////////////////////////////////////
void OpenGLRenderTarget::applyStencilMode(const StencilMode& mode)
{
    using OpenGLRenderTargetImpl::stencilFunctionToGlConstant;
    using OpenGLRenderTargetImpl::stencilOperationToGlConstant;

    // Fast path if we have a default (disabled) stencil mode
    if (mode == StencilMode())
    {
        if (!m_cache.enable || m_cache.stencilEnabled)
        {
            glCheck(glDisable(GL_STENCIL_TEST));
            glCheck(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

            m_cache.stencilEnabled = false;
        }
    }
    else
    {
        // Apply the stencil mode
        if (!m_cache.enable || !m_cache.stencilEnabled)
            glCheck(glEnable(GL_STENCIL_TEST));

        glCheck(glStencilOp(GL_KEEP,
                            stencilOperationToGlConstant(mode.stencilUpdateOperation),
                            stencilOperationToGlConstant(mode.stencilUpdateOperation)));
        glCheck(glStencilFunc(stencilFunctionToGlConstant(mode.stencilComparison),
                              static_cast<int>(mode.stencilReference.value),
                              mode.stencilMask.value));

        m_cache.stencilEnabled = true;
    }

    m_cache.lastStencilMode = mode;
}


////////////////////////////////////////////////////////////
void OpenGLRenderTarget::applyTransform(const Transform& transform)
{
    // No need to call glMatrixMode(GL_MODELVIEW), it is always the
    // current mode (for optimization purpose, since it's the most used)
    if (transform == Transform::Identity)
        glCheck(glLoadIdentity());
    else
        glCheck(glLoadMatrixf(transform.getMatrix()));
}


////////////////////////////////////////////////////////////
void OpenGLRenderTarget::applyTexture(const rhi::Texture* texture, CoordinateType coordinateType)
{
    bindOpenGLTexture(texture, coordinateType, m_modern != nullptr);

    m_cache.lastTextureId      = texture ? static_cast<const TextureResource*>(texture)->revision() : 0;
    m_cache.lastCoordinateType = coordinateType;
}


////////////////////////////////////////////////////////////
void OpenGLRenderTarget::setupDraw(const rhi::TargetState& target, bool useVertexCache, const rhi::DrawSettings& states)
{
    // GL_FRAMEBUFFER_SRGB is not available on OpenGL ES
    // If a framebuffer supports sRGB, it will always be enabled on OpenGL ES
#ifndef SFML_OPENGL_ES
    // Enable or disable sRGB encoding
    // This is needed for drivers that do not check the format of the surface drawn to before applying sRGB conversion
    if (!m_cache.enable)
    {
        if (target.sRgb)
            glCheck(glEnable(GL_FRAMEBUFFER_SRGB));
        else if (GLEXT_framebuffer_sRGB)
            glCheck(glDisable(GL_FRAMEBUFFER_SRGB));
    }
#endif

    // First set the persistent OpenGL states if it's the very first call
    if (!m_cache.glStatesSet)
        resetGLStates(target);

    if (!m_modern && useVertexCache)
    {
        // Since vertices are transformed, we must use an identity transform to render them
        if (!m_cache.enable || !m_cache.useVertexCache)
            glCheck(glLoadIdentity());
    }
    else if (!m_modern)
    {
        applyTransform(states.transform);
    }

    // Apply the view
    applyCurrentView(target);

    // Apply the blend mode
    if (!m_cache.enable || (states.blendMode != m_cache.lastBlendMode))
        applyBlendMode(states.blendMode);

    // Apply the stencil mode
    if (!m_cache.enable || (states.stencilMode != m_cache.lastStencilMode))
        applyStencilMode(states.stencilMode);

    // Mask the color buffer off if necessary
    if (states.stencilMode.stencilOnly)
        glCheck(glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE));

    // Apply the texture
    if (!m_cache.enable || (states.texture && static_cast<const TextureResource*>(states.texture.get())->attachment()))
    {
        // If the texture is an FBO attachment, always rebind it
        // in order to inform the OpenGL driver that we want changes
        // made to it in other contexts to be visible here as well
        // This saves us from having to call glFlush() in
        // RenderTextureImplFBO which can be quite costly
        // See: https://www.khronos.org/opengl/wiki/Memory_Model
        applyTexture(states.texture.get(), states.coordinateType);
    }
    else
    {
        const std::uint64_t textureId = states.texture
                                            ? static_cast<const TextureResource*>(states.texture.get())->revision()
                                            : 0;
        if (textureId != m_cache.lastTextureId || states.coordinateType != m_cache.lastCoordinateType)
            applyTexture(states.texture.get(), states.coordinateType);
    }

    // Apply the shader
    if (!m_modern && states.shader)
        OpenGLProgram::bind(states.shader.get());
}


////////////////////////////////////////////////////////////
void OpenGLRenderTarget::drawPrimitives(PrimitiveType type, std::size_t firstVertex, std::size_t vertexCount)
{
    // Find the OpenGL primitive type
    static constexpr EnumArray<PrimitiveType, GLenum, 6> modes =
        {GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN};
    const GLenum mode = modes[type];

    // Draw the primitives
    glCheck(glDrawArrays(mode, static_cast<GLint>(firstVertex), static_cast<GLsizei>(vertexCount)));
}


////////////////////////////////////////////////////////////
void OpenGLRenderTarget::cleanupDraw(const rhi::DrawSettings& states)
{
    // Unbind the shader, if any
    if (m_modern || states.shader)
        OpenGLProgram::bind(nullptr);

    // If the texture we used to draw belonged to a RenderTexture, then forcibly unbind that texture.
    // This prevents a bug where some drivers do not clear RenderTextures properly.
    if (states.texture && static_cast<const TextureResource*>(states.texture.get())->attachment())
        applyTexture(nullptr);

    // Mask the color buffer back on if necessary
    if (states.stencilMode.stencilOnly)
        glCheck(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));

    // Re-enable the cache at the end of the draw if it was disabled
    m_cache.enable = true;
}


bool OpenGLRenderTarget::drawVertices(
    const rhi::TargetState&                      target,
    const std::vector<rhi::VertexBufferBinding>& buffers,
    std::size_t                                  first,
    std::size_t                                  count,
    std::size_t                                  instances,
    PrimitiveType                                type,
    const rhi::DrawSettings&                     states)
{
    if (!m_modern || !states.shader || !instances || !setActive(true) || !glDrawArraysInstanced ||
        count > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()) ||
        first > static_cast<std::size_t>(std::numeric_limits<GLint>::max()) ||
        instances > static_cast<std::size_t>(std::numeric_limits<GLsizei>::max()))
        return false;
    const auto* program = resourceCast<const OpenGLProgram>(states.shader.get());
    if (!program || !validateVertexBindings(program->getOptions(), buffers, first, count, instances))
        return false;
    for (const auto& binding : buffers)
        if (!resourceCast<const OpenGLBuffer>(binding.buffer.get()))
            return false;
    drawProgrammable(target, nullptr, 0, first, count, type, states, &buffers, instances);
    colorWritten();
    return true;
}
bool OpenGLRenderTarget::prewarm(const rhi::TargetState& target, const rhi::DrawSettings& states)
{
    if (!m_modern || !setActive(true))
        return false;
    drawProgrammable(target, nullptr, 0, 0, 0, PrimitiveType::Triangles, states);
    return m_modern->defaultShader != nullptr;
}

void OpenGLRenderTarget::drawProgrammable(
    const rhi::TargetState&                      target,
    const Vertex*                                vertices,
    unsigned int                                 buffer,
    std::size_t                                  first,
    std::size_t                                  count,
    PrimitiveType                                type,
    const rhi::DrawSettings&                     states,
    const std::vector<rhi::VertexBufferBinding>* bindings,
    std::size_t                                  instances)
{
    struct VertexObjects
    {
        GLuint array{};
        GLuint stream{};

        ~VertexObjects()
        {
            glCheck(glDeleteVertexArrays(1, &array));
            glCheck(glDeleteBuffers(1, &stream));
        }
    };

    // VAOs are context-local. Share the streaming objects between all targets
    // using a given context and delete them while that context is current.
    const auto objects = []
    {
        static std::mutex                                                      mutex;
        static std::unordered_map<std::uint64_t, std::weak_ptr<VertexObjects>> cache;
        const std::scoped_lock                                                 lock(mutex);
        const auto                                                             context = Context::getActiveContextId();
        if (const auto it = cache.find(context); it != cache.end())
            if (auto value = it->second.lock())
                return value;

        for (auto it = cache.begin(); it != cache.end();)
            it = it->second.expired() ? cache.erase(it) : std::next(it);

        auto value = std::make_shared<VertexObjects>();
        glCheck(glGenVertexArrays(1, &value->array));
        glCheck(glGenBuffers(1, &value->stream));
        OpenGLResource::registerUnsharedGlObject(value);
        cache[context] = value;
        return value;
    };

    setupDraw(target, false, states);
    if (!m_modern->defaultShader)
    {
        m_modern->defaultShader = std::make_unique<OpenGLProgram>(std::shared_ptr<void>{}, true);
        if (!m_modern->defaultShader->loadDefault())
        {
            m_modern->defaultShader.reset();
            cleanupDraw(states);
            return;
        }
    }

    const auto vertexObjects = objects();
    glCheck(glBindVertexArray(vertexObjects->array));
    glCheck(glBindBuffer(GL_ARRAY_BUFFER, vertices || !buffer ? vertexObjects->stream : buffer));
    if (vertices)
        glCheck(glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sizeof(Vertex) * count), vertices, GL_STREAM_DRAW));

    const auto* shader = states.shader ? resourceCast<const OpenGLProgram>(states.shader.get())
                                       : m_modern->defaultShader.get();
    if (bindings)
    {
        if (!shader || !shader->bindVertexBuffers(*bindings, first, count, instances))
        {
            cleanupDraw(states);
            return;
        }
    }
    else
    {
        glCheck(glVertexAttribPointer(0,
                                      2,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      sizeof(Vertex),
                                      reinterpret_cast<const void*>(offsetof(Vertex, position))));
        glCheck(glVertexAttribPointer(1,
                                      4,
                                      GL_UNSIGNED_BYTE,
                                      GL_TRUE,
                                      sizeof(Vertex),
                                      reinterpret_cast<const void*>(offsetof(Vertex, color))));
        glCheck(glVertexAttribPointer(2,
                                      2,
                                      GL_FLOAT,
                                      GL_FALSE,
                                      sizeof(Vertex),
                                      reinterpret_cast<const void*>(offsetof(Vertex, texCoords))));
        for (GLuint index = 0; index < 3; ++index)
            glCheck(glEnableVertexAttribArray(index));
    }
    OpenGLProgram::bind(shader);
    const auto program = shader ? shader->nativeHandle() : 0;
    if (program)
    {
        const auto parameters    = prepareDraw(target, states);
        const auto textureMatrix = states.texture ? states.texture->coordinateMatrix(states.coordinateType)
                                                  : std::array<float, 16>{1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1};
        glCheck(
            glUniformMatrix4fv(glGetUniformLocation(program, "sf_ModelViewMatrix"), 1, GL_FALSE, parameters.modelView.data()));
        glCheck(
            glUniformMatrix4fv(glGetUniformLocation(program, "sf_ProjectionMatrix"), 1, GL_FALSE, parameters.projection.data()));
        glCheck(glUniformMatrix4fv(glGetUniformLocation(program, "sf_ModelViewProjectionMatrix"),
                                   1,
                                   GL_FALSE,
                                   parameters.transform.data()));
        glCheck(glUniformMatrix4fv(glGetUniformLocation(program, "sf_TextureMatrix"), 1, GL_FALSE, textureMatrix.data()));
        glCheck(glUniform1i(glGetUniformLocation(program, "sf_TextureEnabled"), states.texture ? GL_TRUE : GL_FALSE));
        glCheck(glUniform1i(glGetUniformLocation(program, "sf_Texture"), 0));
        glUniform4fv(glGetUniformLocation(program, "sf_TargetSize"), 1, parameters.targetSize.data());
        if (count)
        {
            if (bindings)
            {
                static constexpr std::array<GLenum, 6>
                    modes{GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN};
                glDrawArraysInstanced(modes[static_cast<std::size_t>(type)],
                                      static_cast<GLint>(first),
                                      static_cast<GLsizei>(count),
                                      static_cast<GLsizei>(instances));
            }
            else
                drawPrimitives(type, first, count);
        }
    }

    if (bindings && shader)
    {
        const auto divisor = glVertexAttribDivisor ? glVertexAttribDivisor : glVertexAttribDivisorARB;
        for (const auto& attribute : shader->getOptions().vertexAttributes)
        {
            glDisableVertexAttribArray(attribute.location);
            divisor(attribute.location, 0);
        }
    }
    cleanupDraw(states);
}

std::shared_ptr<OpenGLRenderTarget> OpenGLRenderTarget::create(std::shared_ptr<OpenGLTexture> texture,
                                                               const ContextSettings&         settings,
                                                               bool                           programmable)
{
    auto target = std::make_shared<OpenGLRenderTarget>(programmable);
    if (RenderTextureImplFBO::isAvailable())
    {
        target->m_attachment     = std::make_unique<RenderTextureImplFBO>();
        texture->m_fboAttachment = true;
    }
    else
        target->m_attachment = std::make_unique<RenderTextureImplDefault>();
    ContextSettings actual = settings;
    actual.sRgbCapable     = settings.sRgbCapable && texture->isSrgb();
    if (!target->m_attachment->create(texture->m_actualSize, texture->m_texture, actual))
        return {};
    target->m_size                 = texture->getSize();
    target->m_settings             = settings;
    target->m_settings.sRgbCapable = target->m_attachment->isSrgb();
    target->m_color                = std::move(texture);
    return target;
}
void OpenGLRenderTarget::finish()
{
    if (!m_attachment || !setActive(true))
        return;
    m_attachment->updateTexture(m_color->m_texture);
    m_color->m_pixelsFlipped = true;
    m_color->invalidateMipmap();
}
void OpenGLRenderTarget::attachWindow(std::shared_ptr<WindowSurface> surface, Vector2u size)
{
    m_window   = std::move(surface);
    m_size     = size;
    m_settings = m_window->getSettings();
    if (m_window->setActive(true) && RenderTextureImplFBO::isAvailable())
        glCheck(glGetIntegerv(GLEXT_GL_FRAMEBUFFER_BINDING, reinterpret_cast<GLint*>(&m_framebuffer)));
}
} // namespace sf::priv
