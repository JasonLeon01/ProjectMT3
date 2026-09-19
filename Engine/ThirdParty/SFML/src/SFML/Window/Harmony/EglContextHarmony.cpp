////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2013 Jonathan De Wachter (dewachter.jonathan@gmail.com)
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
#include <SFML/Window/Harmony/EglContextHarmony.hpp>
#include <SFML/Window/Harmony/NativeAppImpl.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window/WindowImpl.hpp>

#include <SFML/System/Err.hpp>

#include <algorithm>
#include <array>
#include <memory>
#include <mutex>
#include <ostream>

#include <cstdint>

// We check for this definition in order to avoid multiple definitions of GLAD
// entities during unity builds of SFML.
#ifndef SF_GLAD_EGL_IMPLEMENTATION_INCLUDED
#define SF_GLAD_EGL_IMPLEMENTATION_INCLUDED
#define SF_GLAD_EGL_IMPLEMENTATION
#include <glad/egl.h>
#undef SF_GLAD_EGL_IMPLEMENTATION
#endif

#include <SFML/Window/EglFunctionLoader.hpp>

namespace
{
// A nested named namespace is used here to allow unity builds of SFML.
namespace EglContextHarmonyImpl
{
sf::ContextSettings normalizeSettings(sf::ContextSettings settings)
{
#ifdef SFML_OPENGL_ES
    if (settings.majorVersion < 3)
    {
        settings.majorVersion = 2;
        settings.minorVersion = 0;
    }
    else if (settings.majorVersion > 3 || settings.minorVersion > 2)
    {
        settings.majorVersion = 3;
        settings.minorVersion = 2;
    }
    settings.attributeFlags = sf::ContextSettings::Default;
#else
    // Harmony desktop OpenGL starts at 3.0 and currently exposes at most 4.2.
    // Keep this guard at the EGL boundary as well as in GlContext so internal
    // callers cannot accidentally request an unsupported client version.
    if (settings.majorVersion < 3)
    {
        settings.majorVersion = 3;
        settings.minorVersion = 0;
    }
    else if (settings.majorVersion > 4 || (settings.majorVersion == 4 && settings.minorVersion > 2))
    {
        settings.majorVersion = 4;
        settings.minorVersion = 2;
    }

    settings.attributeFlags &= sf::ContextSettings::Core | sf::ContextSettings::Debug;
#endif
    return settings;
}

template <typename T>
T loadEglBootstrapSymbol(void* handle, const char* name)
{
    return reinterpret_cast<T>(reinterpret_cast<std::uintptr_t>(dlsym(handle, name)));
}

bool loadEglBootstrap()
{
    static void* const handle = dlopen("libEGL.so", RTLD_LAZY | RTLD_LOCAL);

    if (!handle)
        return false;

    eglGetDisplay = loadEglBootstrapSymbol<PFNEGLGETDISPLAYPROC>(handle, "eglGetDisplay");
    eglInitialize = loadEglBootstrapSymbol<PFNEGLINITIALIZEPROC>(handle, "eglInitialize");
    eglGetError   = loadEglBootstrapSymbol<PFNEGLGETERRORPROC>(handle, "eglGetError");
    return eglGetDisplay && eglInitialize && eglGetError;
}

EGLDisplay getInitializedDisplay()
{
    static EGLDisplay display = EGL_NO_DISPLAY;

    if (display == EGL_NO_DISPLAY)
    {
        display = eglCheck(eglGetDisplay(EGL_DEFAULT_DISPLAY));
        if (display == EGL_NO_DISPLAY || eglCheck(eglInitialize(display, nullptr, nullptr)) == EGL_FALSE)
        {
            display = EGL_NO_DISPLAY;
            return display;
        }
    }

    return display;
}

#ifdef SFML_OPENGL_ES
constexpr EGLenum     harmonyClientApi     = EGL_OPENGL_ES_API;
constexpr const char* harmonyClientApiName = "OpenGL ES";
#else
constexpr EGLenum     harmonyClientApi     = EGL_OPENGL_API;
constexpr const char* harmonyClientApiName = "desktop OpenGL";

bool queryDesktopGlSupport()
{
    if (!eglGetProcAddress)
    {
        sf::err() << "Cannot resolve OH_Graphics_QueryGL because eglGetProcAddress is unavailable; "
                     "rebuild SFML with SFML_OPENGL_ES enabled for this device"
                  << std::endl;
        return false;
    }

    using QueryGlFunction = EGLBoolean (*)();
    const auto address    = eglGetProcAddress("OH_Graphics_QueryGL");
    const auto queryGl    = reinterpret_cast<QueryGlFunction>(address);
    if (!queryGl)
    {
        sf::err() << "OH_Graphics_QueryGL is unavailable in this Harmony API 22+ desktop OpenGL build; "
                     "rebuild SFML with SFML_OPENGL_ES enabled for this device"
                  << std::endl;
        return false;
    }

    if (queryGl() != EGL_FALSE)
        return true;

    sf::err() << "OH_Graphics_QueryGL reports that desktop OpenGL is unavailable (verify NEED_OPENGL=1); "
                 "rebuild SFML with SFML_OPENGL_ES enabled for this device"
              << std::endl;
    return false;
}
#endif


////////////////////////////////////////////////////////////
bool ensureInit()
{
    static std::once_flag flag;
    static bool           initialized = false;

    std::call_once(flag,
                   []
                   {
                       // Harmony's EGL rejects version/extension queries until
                       // its display is initialized. Load only the bootstrap
                       // entry points first; the complete GLAD pass follows
                       // after eglInitialize succeeds.
                       if (!loadEglBootstrap())
                       {
                           // At this point, the failure is unrecoverable
                           // Dump a message to the console and let the application terminate
                           sf::err() << "Failed to load EGL entry points" << std::endl;

                           return;
                       }

                       // Continue loading with a display
                       const EGLDisplay display = getInitializedDisplay();
                       if (display == EGL_NO_DISPLAY || !gladLoaderLoadEGL(display))
                       {
                           sf::err() << "Failed to initialize EGL or load display entry points" << std::endl;
                           return;
                       }

#ifndef SFML_OPENGL_ES
                       // The 2-in-1 desktop backend requires API 22. Treat both
                       // a missing capability query and a negative result as
                       // authoritative; this binary must never fall back to GLES.
                       if (!queryDesktopGlSupport())
                           return;
#endif

                       // HarmonyOS only reports EGL 1.0 before a display is
                       // initialized, so bind after reloading against it.
                       if (!eglBindAPI || eglCheck(eglBindAPI(harmonyClientApi)) == EGL_FALSE)
                       {
                           sf::err() << "Failed to bind the Harmony EGL " << harmonyClientApiName << " client API"
                                     << std::endl;
                           return;
                       }

                       initialized = true;
                   });
    return initialized;
}
} // namespace EglContextHarmonyImpl
} // namespace


namespace sf::priv
{
////////////////////////////////////////////////////////////
EglContextHarmony::EglContextHarmony(EglContextHarmony* shared)
{
    if (!EglContextHarmonyImpl::ensureInit())
        return;

    // Get the initialized EGL display
    m_display = EglContextHarmonyImpl::getInitializedDisplay();

    const ContextSettings settings = EglContextHarmonyImpl::normalizeSettings(
        shared ? shared->m_settings : ContextSettings{});

    // Prefer a real pbuffer. Desktop OpenGL can use a window-capable config
    // without a surface only when EGL_KHR_surfaceless_context is advertised.
    m_config = getBestConfig(m_display, VideoMode::getDesktopMode().bitsPerPixel, settings, EGL_PBUFFER_BIT);
#ifndef SFML_OPENGL_ES
    if (!m_config && SF_GLAD_EGL_KHR_surfaceless_context)
    {
        m_config = getBestConfig(m_display, VideoMode::getDesktopMode().bitsPerPixel, settings, EGL_WINDOW_BIT);
        m_surfacelessContext = m_config != nullptr;
    }
#endif
    if (!m_config)
    {
#ifdef SFML_OPENGL_ES
        err() << "Failed to find an EGL pbuffer configuration supporting OpenGL ES " << settings.majorVersion << "."
              << settings.minorVersion << std::endl;
#else
        err() << "Failed to find a desktop OpenGL EGL pbuffer configuration"
              << (SF_GLAD_EGL_KHR_surfaceless_context ? " or a window configuration for surfaceless use"
                                                      : "; EGL_KHR_surfaceless_context is unavailable")
              << std::endl;
#endif
        return;
    }
    updateSettings();

    // Create EGL context
    createContext(shared, settings, VideoMode::getDesktopMode().bitsPerPixel, EGL_PBUFFER_BIT);
    if (m_context == EGL_NO_CONTEXT)
        return;

    if (!m_surfacelessContext)
    {
        static constexpr std::array attribList = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
        m_surface = eglCheck(eglCreatePbufferSurface(m_display, m_config, attribList.data()));

        if (m_surface == EGL_NO_SURFACE)
        {
#ifndef SFML_OPENGL_ES
            if (SF_GLAD_EGL_KHR_surfaceless_context)
                m_surfacelessContext = true;
            else
#endif
            {
                err() << "Failed to create the Harmony shared EGL pbuffer" << std::endl;
                eglCheck(eglDestroyContext(m_display, m_context));
                m_context = EGL_NO_CONTEXT;
            }
        }
    }
}


////////////////////////////////////////////////////////////
EglContextHarmony::EglContextHarmony(EglContextHarmony*                 shared,
                                     const ContextSettings&             settings,
                                     [[maybe_unused]] const WindowImpl& owner,
                                     unsigned int                       bitsPerPixel)
{
    if (!EglContextHarmonyImpl::ensureInit())
        return;

    m_harmonyWindowContext = true;
    m_requestSrgb          = settings.sRgbCapable;

    // Get the initialized EGL display
    m_display = EglContextHarmonyImpl::getInitializedDisplay();

    // Get the best EGL config matching the requested video settings
    ContextSettings effectiveSettings = settings;
    effectiveSettings                 = EglContextHarmonyImpl::normalizeSettings(effectiveSettings);
#ifdef SFML_OPENGL_ES
    const EGLint requestedSurfaceType = EGL_WINDOW_BIT | EGL_PBUFFER_BIT;
#else
    EGLint requestedSurfaceType = EGL_WINDOW_BIT | EGL_PBUFFER_BIT;
#endif
    m_config = getBestConfig(m_display, bitsPerPixel, effectiveSettings, requestedSurfaceType);
#ifndef SFML_OPENGL_ES
    if (!m_config && SF_GLAD_EGL_KHR_surfaceless_context)
    {
        requestedSurfaceType = EGL_WINDOW_BIT;
        m_config             = getBestConfig(m_display, bitsPerPixel, effectiveSettings, requestedSurfaceType);
        m_surfacelessContext = m_config != nullptr;
    }
#endif

    if (!m_config)
    {
#ifdef SFML_OPENGL_ES
        if (shared)
            err() << "Failed to find an EGL window configuration matching the locked OpenGL ES share-group version"
                  << std::endl;
        else
            err() << "Failed to find an EGL configuration supporting OpenGL ES " << effectiveSettings.majorVersion
                  << "." << effectiveSettings.minorVersion << std::endl;
#else
        err() << "Failed to find a desktop OpenGL EGL window configuration with pbuffer support"
              << (SF_GLAD_EGL_KHR_surfaceless_context ? " or a window configuration for surfaceless fallback"
                                                      : "; EGL_KHR_surfaceless_context is unavailable")
              << std::endl;
#endif
        return;
    }

    updateSettings();

    // Create EGL context
    createContext(shared, effectiveSettings, bitsPerPixel, requestedSurfaceType);
    if (m_context == EGL_NO_CONTEXT)
        return;

    if (!m_surfacelessContext)
    {
        // XComponent surfaces are asynchronous. Keep the window context current
        // on a persistent pbuffer while the native surface is absent.
        static constexpr std::array fallbackAttributes = {EGL_WIDTH, 1, EGL_HEIGHT, 1, EGL_NONE};
        m_fallbackSurface = eglCheck(eglCreatePbufferSurface(m_display, m_config, fallbackAttributes.data()));
        if (m_fallbackSurface == EGL_NO_SURFACE)
        {
#ifndef SFML_OPENGL_ES
            if (SF_GLAD_EGL_KHR_surfaceless_context)
                m_surfacelessContext = true;
            else
#endif
            {
#ifdef SFML_OPENGL_ES
                err() << "Failed to create the Harmony fallback EGL pbuffer" << std::endl;
#else
                err() << "Failed to create the Harmony fallback EGL pbuffer and surfaceless contexts are unavailable"
                      << std::endl;
#endif
                eglCheck(eglDestroyContext(m_display, m_context));
                m_context = EGL_NO_CONTEXT;
                return;
            }
        }
    }

    // Keep the native window and generation from the same lifecycle state.
    // Holding the host lock also prevents a destroy callback from invalidating
    // the pointer while eglCreateWindowSurface consumes it.
    auto&                 host = Harmony::getHostState();
    const std::lock_guard hostLock(host.mutex);
    if (host.window && host.size.x && host.size.y)
        createSurface(host.window);
    m_surfaceGeneration = host.surfaceGeneration;
}


////////////////////////////////////////////////////////////
EglContextHarmony::EglContextHarmony(EglContextHarmony* shared, const ContextSettings& settings, Vector2u size)
{
    if (!EglContextHarmonyImpl::ensureInit())
        return;

    const ContextSettings effectiveSettings = EglContextHarmonyImpl::normalizeSettings(settings);
    m_display                               = EglContextHarmonyImpl::getInitializedDisplay();
    m_config = getBestConfig(m_display, VideoMode::getDesktopMode().bitsPerPixel, effectiveSettings, EGL_PBUFFER_BIT);
#ifndef SFML_OPENGL_ES
    if (!m_config && SF_GLAD_EGL_KHR_surfaceless_context)
    {
        m_config = getBestConfig(m_display, VideoMode::getDesktopMode().bitsPerPixel, effectiveSettings, EGL_WINDOW_BIT);
        m_surfacelessContext = m_config != nullptr;
    }
#endif

    if (!m_config)
    {
#ifdef SFML_OPENGL_ES
        if (shared)
            err() << "Failed to find an EGL pbuffer configuration matching the locked OpenGL ES share-group version"
                  << std::endl;
        else
            err() << "Failed to find an EGL configuration supporting OpenGL ES " << effectiveSettings.majorVersion
                  << "." << effectiveSettings.minorVersion << std::endl;
#else
        err() << "Failed to find a desktop OpenGL EGL pbuffer configuration"
              << (SF_GLAD_EGL_KHR_surfaceless_context ? " or a window configuration for surfaceless use"
                                                      : "; EGL_KHR_surfaceless_context is unavailable")
              << std::endl;
#endif
        return;
    }

    updateSettings();

    createContext(shared, effectiveSettings, VideoMode::getDesktopMode().bitsPerPixel, EGL_PBUFFER_BIT);
    if (m_context == EGL_NO_CONTEXT)
        return;

    if (!m_surfacelessContext)
    {
        const std::array attribList = {EGL_WIDTH,
                                       static_cast<EGLint>(std::max(size.x, 1u)),
                                       EGL_HEIGHT,
                                       static_cast<EGLint>(std::max(size.y, 1u)),
                                       EGL_NONE};
        m_surface                   = eglCheck(eglCreatePbufferSurface(m_display, m_config, attribList.data()));

        if (m_surface == EGL_NO_SURFACE)
        {
#ifndef SFML_OPENGL_ES
            if (SF_GLAD_EGL_KHR_surfaceless_context)
                m_surfacelessContext = true;
            else
#endif
            {
                err() << "Failed to create the Harmony offscreen EGL pbuffer" << std::endl;
                eglCheck(eglDestroyContext(m_display, m_context));
                m_context = EGL_NO_CONTEXT;
            }
        }
    }
}


////////////////////////////////////////////////////////////
EglContextHarmony::~EglContextHarmony()
{
    // Notify unshared OpenGL resources of context destruction
    cleanupUnsharedResources();

    // Deactivate the current context
    const EGLContext currentContext = eglGetCurrentContext ? eglCheck(eglGetCurrentContext()) : EGL_NO_CONTEXT;

    if (currentContext == m_context && eglMakeCurrent)
    {
        eglCheck(eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
    }

    // Destroy context
    if (m_context != EGL_NO_CONTEXT && eglDestroyContext)
    {
        eglCheck(eglDestroyContext(m_display, m_context));
    }

    // Destroy surface
    if (m_surface != EGL_NO_SURFACE && eglDestroySurface)
    {
        eglCheck(eglDestroySurface(m_display, m_surface));
    }

    if (m_fallbackSurface != EGL_NO_SURFACE && eglDestroySurface)
        eglCheck(eglDestroySurface(m_display, m_fallbackSurface));
}


////////////////////////////////////////////////////////////
GlFunctionPointer EglContextHarmony::getFunction(const char* name)
{
    if (!EglContextHarmonyImpl::ensureInit())
        return nullptr;

    return getEglGlFunction(name);
}


////////////////////////////////////////////////////////////
bool EglContextHarmony::makeCurrent(bool current)
{
    if (m_display == EGL_NO_DISPLAY || m_context == EGL_NO_CONTEXT || !eglMakeCurrent)
        return false;

    if (m_harmonyWindowContext)
    {
        if (!current)
        {
            // A raw EGL user can replace SFML's context without updating its
            // thread-local cache. In that case, clearing the SFML cache must
            // not unbind the unrelated context that is actually current.
            if (!eglGetCurrentContext)
                return false;

            if (eglCheck(eglGetCurrentContext()) != m_context)
                return true;

            return EGL_FALSE != eglCheck(eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
        }

        if (!synchronizeHarmonySurface())
            return false;

        return makeHarmonyCurrent(m_surface != EGL_NO_SURFACE ? m_surface : m_fallbackSurface);
    }
    if (m_surface == EGL_NO_SURFACE && !m_surfacelessContext)
        return false;

    if (current)
        return EGL_FALSE != eglCheck(eglMakeCurrent(m_display, m_surface, m_surface, m_context));

    return EGL_FALSE != eglCheck(eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
}


////////////////////////////////////////////////////////////
void EglContextHarmony::display()
{
    if (m_harmonyWindowContext)
    {
        // getActiveContext() verifies both SFML's thread-local owner and EGL's
        // actual context/DRAW/READ bindings. Never swap an inactive or stale
        // window surface merely because its generation still matches.
        if (getActiveContext() != this || m_surface == EGL_NO_SURFACE)
            return;
    }
    if (m_surface != EGL_NO_SURFACE)
    {
        if (eglCheck(eglSwapBuffers(m_display, m_surface)) == EGL_FALSE)
            err() << "Failed to present the Harmony EGL window surface" << std::endl;
    }
}


////////////////////////////////////////////////////////////
void EglContextHarmony::setVerticalSyncEnabled(bool enabled)
{
    if (m_harmonyWindowContext)
    {
        m_verticalSyncEnabled = enabled;
        m_verticalSyncDirty   = true;

        if (m_surface != EGL_NO_SURFACE && eglGetCurrentContext && eglGetCurrentSurface &&
            eglCheck(eglGetCurrentContext()) == m_context && eglCheck(eglGetCurrentSurface(EGL_DRAW)) == m_surface &&
            eglCheck(eglSwapInterval(m_display, enabled)) != EGL_FALSE)
            m_verticalSyncDirty = false;

        return;
    }

    eglCheck(eglSwapInterval(m_display, enabled));
}


////////////////////////////////////////////////////////////
void EglContextHarmony::createContext(EglContextHarmony*     shared,
                                      const ContextSettings& settings,
                                      unsigned int           bitsPerPixel,
                                      EGLint                 surfaceType)
{
    const EGLContext toShared = shared ? shared->m_context : EGL_NO_CONTEXT;
    if (toShared != EGL_NO_CONTEXT)
        eglCheck(eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));

    (void)bitsPerPixel;
    (void)surfaceType;

    if (shared && toShared == EGL_NO_CONTEXT)
    {
        err() << "Cannot create a Harmony EGL context from an invalid shared context" << std::endl;
        return;
    }

#ifdef SFML_OPENGL_ES
    m_settings.majorVersion = settings.majorVersion;
    m_settings.minorVersion = settings.minorVersion;
    if (settings.majorVersion >= 3 && (SF_GLAD_EGL_VERSION_1_5 || SF_GLAD_EGL_KHR_create_context))
    {
        const std::array contextAttributes{EGL_CONTEXT_MAJOR_VERSION,
                                           static_cast<EGLint>(settings.majorVersion),
                                           EGL_CONTEXT_MINOR_VERSION,
                                           static_cast<EGLint>(settings.minorVersion),
                                           EGL_NONE};
        m_context = eglCheck(eglCreateContext(m_display, m_config, toShared, contextAttributes.data()));
    }
    else
    {
        const std::array contextAttributes{EGL_CONTEXT_CLIENT_VERSION, static_cast<EGLint>(settings.majorVersion), EGL_NONE};
        m_context = eglCheck(eglCreateContext(m_display, m_config, toShared, contextAttributes.data()));
    }

    if (m_context == EGL_NO_CONTEXT)
        err() << "Failed to create a Harmony OpenGL ES " << settings.majorVersion << "." << settings.minorVersion
              << " context" << std::endl;
#else
    if (!SF_GLAD_EGL_VERSION_1_5 && !SF_GLAD_EGL_KHR_create_context)
    {
        err() << "Harmony desktop OpenGL requires EGL 1.5 or EGL_KHR_create_context" << std::endl;
        return;
    }

    std::array<EGLint, 9> contextAttributes{};
    std::size_t           attributeCount  = 0;
    const auto            appendAttribute = [&](EGLint attribute, EGLint value)
    {
        contextAttributes[attributeCount++] = attribute;
        contextAttributes[attributeCount++] = value;
    };

    appendAttribute(EGL_CONTEXT_MAJOR_VERSION, static_cast<EGLint>(settings.majorVersion));
    appendAttribute(EGL_CONTEXT_MINOR_VERSION, static_cast<EGLint>(settings.minorVersion));

    if (settings.majorVersion > 3 || (settings.majorVersion == 3 && settings.minorVersion >= 2))
    {
        appendAttribute(EGL_CONTEXT_OPENGL_PROFILE_MASK,
                        (settings.attributeFlags & ContextSettings::Core) ? EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT
                                                                          : EGL_CONTEXT_OPENGL_COMPATIBILITY_PROFILE_BIT);
    }

    if (settings.attributeFlags & ContextSettings::Debug)
    {
        if (SF_GLAD_EGL_VERSION_1_5)
            appendAttribute(EGL_CONTEXT_OPENGL_DEBUG, EGL_TRUE);
        else
            appendAttribute(EGL_CONTEXT_FLAGS_KHR, EGL_CONTEXT_OPENGL_DEBUG_BIT_KHR);
    }

    contextAttributes[attributeCount] = EGL_NONE;
    m_context = eglCheck(eglCreateContext(m_display, m_config, toShared, contextAttributes.data()));

    if (m_context == EGL_NO_CONTEXT)
    {
        err() << "Failed to create a Harmony desktop OpenGL " << settings.majorVersion << "." << settings.minorVersion
              << ((settings.attributeFlags & ContextSettings::Core) ? " core" : " compatibility")
              << ((settings.attributeFlags & ContextSettings::Debug) ? " debug" : "") << " context" << std::endl;
    }
#endif
}


////////////////////////////////////////////////////////////
void EglContextHarmony::createSurface(EGLNativeWindowType window)
{
    if (!window || m_display == EGL_NO_DISPLAY || !m_config)
        return;

    if (m_requestSrgb && SF_GLAD_EGL_KHR_gl_colorspace)
    {
        static constexpr std::array attributes{EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE};
        m_surface              = eglCheck(eglCreateWindowSurface(m_display, m_config, window, attributes.data()));
        m_settings.sRgbCapable = m_surface != EGL_NO_SURFACE;
    }

    if (m_surface == EGL_NO_SURFACE)
    {
        m_surface              = eglCheck(eglCreateWindowSurface(m_display, m_config, window, nullptr));
        m_settings.sRgbCapable = false;
    }

    if (m_harmonyWindowContext && m_surface != EGL_NO_SURFACE)
        m_verticalSyncDirty = true;
}


////////////////////////////////////////////////////////////
void EglContextHarmony::destroySurface()
{
    if (m_surface == EGL_NO_SURFACE)
        return;

    if (m_harmonyWindowContext && eglGetCurrentContext && eglCheck(eglGetCurrentContext()) == m_context)
    {
        // Move the persistent context to its pbuffer or its explicitly supported
        // surfaceless target before destroying the XComponent surface. This
        // bypasses GlContext::setActive() so its context ID remains unchanged.
        if (!makeHarmonyCurrent(m_fallbackSurface))
        {
            err() << "Failed to move the Harmony EGL context to its fallback target" << std::endl;
            // Keep the SFML cache intact until synchronization either restores
            // the context or reports failure to GlContext::setActive().
            if (eglMakeCurrent)
                eglCheck(eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
        }
    }

    eglCheck(eglDestroySurface(m_display, m_surface));
    m_surface = EGL_NO_SURFACE;
}


bool EglContextHarmony::validateCurrentContext()
{
    if (!m_harmonyWindowContext)
        return true;

    if (!synchronizeHarmonySurface())
        return false;

    const EGLSurface target = m_surface != EGL_NO_SURFACE ? m_surface : m_fallbackSurface;
    if (!eglGetCurrentContext || !eglGetCurrentSurface)
        return false;

    const EGLContext currentContext = eglCheck(eglGetCurrentContext());
    if (currentContext == m_context && eglCheck(eglGetCurrentSurface(EGL_DRAW)) == target &&
        eglCheck(eglGetCurrentSurface(EGL_READ)) == target)
        return true;

    // Repair a driver-side unbind or a stale surface binding, but never steal
    // the thread from a different non-null EGLContext. In the latter case the
    // SFML cache is stale and GlContext must invalidate it.
    if (currentContext != EGL_NO_CONTEXT && currentContext != m_context)
        return false;

    return makeHarmonyCurrent(target);
}


bool EglContextHarmony::synchronizeHarmonySurface()
{
    const bool restoreCurrent = eglGetCurrentContext && (eglCheck(eglGetCurrentContext()) == m_context);

    while (true)
    {
        const auto snapshot         = Harmony::getSurfaceSnapshot();
        const bool surfaceAvailable = snapshot.window && snapshot.size.x && snapshot.size.y;
        const bool surfaceCreated   = m_surface != EGL_NO_SURFACE;
        if (snapshot.generation == m_surfaceGeneration && surfaceAvailable == surfaceCreated)
            return m_surface != EGL_NO_SURFACE || m_fallbackSurface != EGL_NO_SURFACE || m_surfacelessContext;

        // Publish first so a failed fallback transition that has to deactivate
        // the context cannot recursively process the same generation.
        m_surfaceGeneration = snapshot.generation;
        if (m_surface != EGL_NO_SURFACE)
            destroySurface();

        auto&                 host = Harmony::getHostState();
        const std::lock_guard hostLock(host.mutex);
        if (host.surfaceGeneration != m_surfaceGeneration)
            continue;
        if (host.window && host.size.x && host.size.y)
            createSurface(host.window);
        break;
    }

    const EGLSurface target = m_surface != EGL_NO_SURFACE ? m_surface : m_fallbackSurface;
    if (restoreCurrent && !makeHarmonyCurrent(target))
        return false;

    return target != EGL_NO_SURFACE || m_surfacelessContext;
}


bool EglContextHarmony::makeHarmonyCurrent(EGLSurface surface)
{
    if ((surface == EGL_NO_SURFACE && !m_surfacelessContext) || m_context == EGL_NO_CONTEXT || !eglMakeCurrent)
        return false;

    if (eglCheck(eglMakeCurrent(m_display, surface, surface, m_context)) == EGL_FALSE)
        return false;

    if (surface == m_surface && m_verticalSyncDirty &&
        eglCheck(eglSwapInterval(m_display, m_verticalSyncEnabled)) != EGL_FALSE)
        m_verticalSyncDirty = false;

    return true;
}


////////////////////////////////////////////////////////////
EGLConfig EglContextHarmony::getBestConfig(EGLDisplay             display,
                                           unsigned int           bitsPerPixel,
                                           const ContextSettings& settings,
                                           EGLint                 requestedSurfaceType)
{
    if (!EglContextHarmonyImpl::ensureInit())
        return {};

    // Determine the number of available configs
    EGLint configCount = 0;
    eglCheck(eglGetConfigs(display, nullptr, 0, &configCount));

    // Retrieve the list of available configs
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    const auto configs = std::make_unique<EGLConfig[]>(static_cast<std::size_t>(configCount));

    eglCheck(eglGetConfigs(display, configs.get(), configCount, &configCount));

    // Evaluate all the returned configs, and pick the best one
    int       bestScore = 0x7FFFFFFF;
    EGLConfig bestConfig{};

    for (std::size_t i = 0; i < static_cast<std::size_t>(configCount); ++i)
    {
        // Check mandatory attributes
        int surfaceType    = 0;
        int renderableType = 0;
        eglCheck(eglGetConfigAttrib(display, configs[i], EGL_SURFACE_TYPE, &surfaceType));
        eglCheck(eglGetConfigAttrib(display, configs[i], EGL_RENDERABLE_TYPE, &renderableType));
#ifdef SFML_OPENGL_ES
        const int requiredRenderableType = settings.majorVersion >= 3 ? EGL_OPENGL_ES3_BIT_KHR : EGL_OPENGL_ES2_BIT;
#else
        constexpr int requiredRenderableType = EGL_OPENGL_BIT;
#endif
        if ((surfaceType & requestedSurfaceType) != requestedSurfaceType || !(renderableType & requiredRenderableType))
            continue;

        // Extract the components of the current config
        int red           = 0;
        int green         = 0;
        int blue          = 0;
        int alpha         = 0;
        int depth         = 0;
        int stencil       = 0;
        int multiSampling = 0;
        int samples       = 0;
        int caveat        = 0;
        eglCheck(eglGetConfigAttrib(display, configs[i], EGL_RED_SIZE, &red));
        eglCheck(eglGetConfigAttrib(display, configs[i], EGL_GREEN_SIZE, &green));
        eglCheck(eglGetConfigAttrib(display, configs[i], EGL_BLUE_SIZE, &blue));
        eglCheck(eglGetConfigAttrib(display, configs[i], EGL_ALPHA_SIZE, &alpha));
        eglCheck(eglGetConfigAttrib(display, configs[i], EGL_DEPTH_SIZE, &depth));
        eglCheck(eglGetConfigAttrib(display, configs[i], EGL_STENCIL_SIZE, &stencil));
        eglCheck(eglGetConfigAttrib(display, configs[i], EGL_SAMPLE_BUFFERS, &multiSampling));
        eglCheck(eglGetConfigAttrib(display, configs[i], EGL_SAMPLES, &samples));
        eglCheck(eglGetConfigAttrib(display, configs[i], EGL_CONFIG_CAVEAT, &caveat));

        // Evaluate the config
        const int color = red + green + blue + alpha;
        const int score = evaluateFormat(bitsPerPixel,
                                         settings,
                                         color,
                                         depth,
                                         stencil,
                                         multiSampling ? samples : 0,
                                         caveat == EGL_NONE,
                                         false);

        // If it's better than the current best, make it the new best
        if (score < bestScore)
        {
            bestScore  = score;
            bestConfig = configs[i];
        }
    }

    return bestConfig;
}


////////////////////////////////////////////////////////////
void EglContextHarmony::updateSettings()
{
#ifndef SFML_OPENGL_ES
    m_settings.majorVersion = 3;
    m_settings.minorVersion = 0;
#endif
    m_settings.attributeFlags    = ContextSettings::Default;
    m_settings.depthBits         = 0;
    m_settings.stencilBits       = 0;
    m_settings.antiAliasingLevel = 0;

    EGLint tmp = 0;

    // Update the internal context settings with the current config
    if (eglCheck(eglGetConfigAttrib(m_display, m_config, EGL_DEPTH_SIZE, &tmp)) != EGL_FALSE)
        m_settings.depthBits = static_cast<unsigned int>(tmp);

    if (eglCheck(eglGetConfigAttrib(m_display, m_config, EGL_STENCIL_SIZE, &tmp)) != EGL_FALSE)
        m_settings.stencilBits = static_cast<unsigned int>(tmp);

    if (eglCheck(eglGetConfigAttrib(m_display, m_config, EGL_SAMPLE_BUFFERS, &tmp)) != EGL_FALSE && tmp &&
        eglCheck(eglGetConfigAttrib(m_display, m_config, EGL_SAMPLES, &tmp)) != EGL_FALSE)
        m_settings.antiAliasingLevel = static_cast<unsigned int>(tmp);
}
} // namespace sf::priv
