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
#include <SFML/Window/EglContext.hpp>
#include <SFML/Window/WindowImpl.hpp>
#ifdef SFML_SYSTEM_HARMONY
#include <SFML/Window/Harmony/NativeAppImpl.hpp>

#include <native_window/external_window.h>
#endif

#include <SFML/System/Err.hpp>
#include <SFML/System/Sleep.hpp>

#include <algorithm>
#include <array>
#include <memory>
#include <mutex>
#include <ostream>

#include <cstdint>
#ifdef SFML_SYSTEM_ANDROID
#include <android/native_window.h>
#endif
#if (defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD) || defined(SFML_SYSTEM_OPENBSD) || \
     defined(SFML_SYSTEM_NETBSD)) &&                                                               \
    !defined(SFML_USE_DRM)
#include <SFML/Window/Unix/Display.hpp>
#include <SFML/Window/Unix/Utils.hpp>

#include <X11/Xlib.h>
#endif

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
namespace EglContextImpl
{
#if defined(SFML_SYSTEM_ANDROID) || defined(SFML_SYSTEM_HARMONY)
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
#endif

EGLDisplay getInitializedDisplay()
{
    static const EGLDisplay display = []
    {
#if defined(SFML_SYSTEM_ANDROID) || defined(SFML_SYSTEM_HARMONY)
        if (!loadEglBootstrap())
#else
        if (!gladLoaderLoadEGL(EGL_NO_DISPLAY))
#endif
            return EGL_NO_DISPLAY;

#if (defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD) || defined(SFML_SYSTEM_OPENBSD) || \
     defined(SFML_SYSTEM_NETBSD)) &&                                                               \
    !defined(SFML_USE_DRM)
        static const auto xDisplay = sf::priv::openDisplay();
        const EGLDisplay  result   = eglGetDisplay(xDisplay.get());
#else
        const EGLDisplay result = eglGetDisplay(EGL_DEFAULT_DISPLAY);
#endif
        if (result == EGL_NO_DISPLAY || !eglInitialize(result, nullptr, nullptr) || !gladLoaderLoadEGL(result))
            return EGL_NO_DISPLAY;

        return result;
    }();
    return display;
}
} // namespace EglContextImpl
} // namespace


namespace sf::priv
{
////////////////////////////////////////////////////////////
EglContext::EglContext(EglContext* shared) :
    EglContext(shared, shared ? shared->getSettings() : ContextSettings{}, {1, 1})
{
}


////////////////////////////////////////////////////////////
EglContext::EglContext(EglContext* shared, const ContextSettings& settings, const WindowImpl& owner, unsigned int bitsPerPixel)
{
    m_display = EglContextImpl::getInitializedDisplay();
    if (m_display == EGL_NO_DISPLAY)
        return;

    m_settings = GlSettings(settings);
    m_config   = getBestConfig(m_display, bitsPerPixel, settings, EGL_WINDOW_BIT);
    if (!m_config)
        return;

    updateSettings();
    createContext(shared, m_settings);
#ifdef SFML_SYSTEM_HARMONY
    (void)owner;
    m_tracksNativeWindow = true;
    if (m_context != EGL_NO_CONTEXT)
        refreshNativeWindow();
#else
    if (m_context != EGL_NO_CONTEXT)
        createSurface(static_cast<EGLNativeWindowType>(owner.getNativeHandle()));
#endif
}


////////////////////////////////////////////////////////////
EglContext::EglContext(EglContext* shared, const ContextSettings& settings, Vector2u size)
{
    m_display = EglContextImpl::getInitializedDisplay();
    if (m_display == EGL_NO_DISPLAY)
        return;

    m_settings = GlSettings(settings);
    m_config   = getBestConfig(m_display, VideoMode::getDesktopMode().bitsPerPixel, settings, EGL_PBUFFER_BIT);
    if (!m_config)
        return;

    updateSettings();
    createContext(shared, m_settings);
    if (m_context == EGL_NO_CONTEXT)
        return;

    const std::array attributes{EGL_WIDTH, static_cast<EGLint>(size.x), EGL_HEIGHT, static_cast<EGLint>(size.y), EGL_NONE};
    m_surface = eglCheck(eglCreatePbufferSurface(m_display, m_config, attributes.data()));
}


////////////////////////////////////////////////////////////
EglContext::~EglContext()
{
    if (m_display == EGL_NO_DISPLAY || m_context == EGL_NO_CONTEXT)
        return;

    // Notify unshared OpenGL resources of context destruction
    cleanupUnsharedResources();

    // Deactivate the current context
    const EGLContext currentContext = eglCheck(eglGetCurrentContext());

    if (currentContext == m_context)
    {
        eglCheck(eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
    }

    // Destroy context
    if (m_context != EGL_NO_CONTEXT)
    {
        eglCheck(eglDestroyContext(m_display, m_context));
    }

    // Destroy surface
    if (m_surface != EGL_NO_SURFACE)
    {
        eglCheck(eglDestroySurface(m_display, m_surface));
    }
}


////////////////////////////////////////////////////////////
GlFunctionPointer EglContext::getFunction(const char* name)
{

    return getEglGlFunction(name);
}


////////////////////////////////////////////////////////////
bool EglContext::makeCurrent(bool current)
{
    if (m_surface == EGL_NO_SURFACE)
        return false;

    if (current)
        return EGL_FALSE != eglCheck(eglMakeCurrent(m_display, m_surface, m_surface, m_context));

    return EGL_FALSE != eglCheck(eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));
}


////////////////////////////////////////////////////////////
void EglContext::display()
{
    if (m_surface != EGL_NO_SURFACE)
        eglCheck(eglSwapBuffers(m_display, m_surface));
}


////////////////////////////////////////////////////////////
void EglContext::setVerticalSyncEnabled(bool enabled)
{
    eglCheck(eglSwapInterval(m_display, enabled));
}


////////////////////////////////////////////////////////////
void EglContext::createContext(EglContext* shared, const GlSettings& settings)
{
    if (!eglCheck(eglBindAPI(EGL_OPENGL_ES_API)))
        return;

    const EGLContext share = shared ? shared->m_context : EGL_NO_CONTEXT;
    if (share != EGL_NO_CONTEXT)
        eglCheck(eglMakeCurrent(m_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT));

    const std::array attributes{EGL_CONTEXT_MAJOR_VERSION,
                                static_cast<EGLint>(settings.majorVersion),
                                EGL_CONTEXT_MINOR_VERSION,
                                static_cast<EGLint>(settings.minorVersion),
                                EGL_NONE};
    const std::array majorOnly{EGL_CONTEXT_CLIENT_VERSION, static_cast<EGLint>(settings.majorVersion), EGL_NONE};

    m_context = eglCheck(
        eglCreateContext(m_display,
                         m_config,
                         share,
                         (SF_GLAD_EGL_VERSION_1_5 || SF_GLAD_EGL_KHR_create_context) ? attributes.data() : majorOnly.data()));
}


////////////////////////////////////////////////////////////
void EglContext::createSurface(EGLNativeWindowType window)
{
    if (!window || m_context == EGL_NO_CONTEXT)
        return;

#ifdef SFML_SYSTEM_ANDROID
    EGLint visual{};
    if (eglGetConfigAttrib(m_display, m_config, EGL_NATIVE_VISUAL_ID, &visual))
        ANativeWindow_setBuffersGeometry(window, 0, 0, visual);
#endif

    if (m_settings.sRgbCapable && SF_GLAD_EGL_KHR_gl_colorspace)
    {
        const std::array attributes{EGL_GL_COLORSPACE_KHR, EGL_GL_COLORSPACE_SRGB_KHR, EGL_NONE};
        m_surface = eglCreateWindowSurface(m_display, m_config, window, attributes.data());
    }

    if (m_surface == EGL_NO_SURFACE)
    {
        m_settings.sRgbCapable = false;
        m_surface              = eglCheck(eglCreateWindowSurface(m_display, m_config, window, nullptr));
    }
}


#ifdef SFML_SYSTEM_HARMONY
void EglContext::refreshNativeWindow()
{
    if (!m_tracksNativeWindow)
        return;
    auto&         host = Harmony::getHostState();
    NativeWindow* window{};
    {
        const std::lock_guard lock(host.mutex);
        if (m_surfaceGeneration == host.surfaceGeneration && (m_surface != EGL_NO_SURFACE || !host.window))
            return;
        m_surfaceGeneration = host.surfaceGeneration;
        if (host.window && OH_NativeWindow_NativeObjectReference(host.window) == 0)
            window = host.window;
    }
    destroySurface();
    if (window)
    {
        createSurface(window);
        (void)OH_NativeWindow_NativeObjectUnreference(window);
    }
}
#endif

#ifdef SFML_SYSTEM_ANDROID
void EglContext::setNativeWindow(WindowHandle handle)
{
    destroySurface();
    createSurface(static_cast<EGLNativeWindowType>(handle));
}
#endif


////////////////////////////////////////////////////////////
void EglContext::destroySurface()
{
    if (m_surface == EGL_NO_SURFACE)
        return;

    // Ensure that this context is no longer active since our surface is going to be destroyed
    setActive(false);

    eglCheck(eglDestroySurface(m_display, m_surface));
    m_surface = EGL_NO_SURFACE;
}


////////////////////////////////////////////////////////////
EGLConfig EglContext::getBestConfig(EGLDisplay display, unsigned int bitsPerPixel, const ContextSettings& settings, EGLint requestedSurfaceType)
{

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
        constexpr int requiredRenderableType = EGL_OPENGL_ES3_BIT_KHR;
        if (!(surfaceType & requestedSurfaceType) || !(renderableType & requiredRenderableType))
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
void EglContext::updateSettings()
{
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


#if (defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD) || defined(SFML_SYSTEM_OPENBSD) || \
     defined(SFML_SYSTEM_NETBSD)) &&                                                               \
    !defined(SFML_USE_DRM)
////////////////////////////////////////////////////////////
XVisualInfo EglContext::selectBestVisual(::Display* xDisplay, unsigned int bitsPerPixel, const ContextSettings& settings)
{

    // Get the initialized EGL display
    EGLDisplay display = EglContextImpl::getInitializedDisplay();

    // Get the best EGL config matching the default video settings
    if (display == EGL_NO_DISPLAY)
        return {};

    const EGLConfig config = getBestConfig(display, bitsPerPixel, settings, EGL_WINDOW_BIT);
    if (!config)
        return {};

    // Retrieve the visual id associated with this EGL config
    EGLint nativeVisualId = 0;

    eglCheck(eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &nativeVisualId));

    if (nativeVisualId == 0)
    {
        // Should never happen...
        err() << "No EGL visual found. You should check your graphics driver" << std::endl;

        return {};
    }

    XVisualInfo vTemplate;
    vTemplate.visualid = static_cast<VisualID>(nativeVisualId);

    // Get X11 visuals compatible with this EGL config
    int visualCount = 0;
    // NOLINTNEXTLINE(cppcoreguidelines-avoid-c-arrays,modernize-avoid-c-arrays)
    const auto availableVisuals = X11Ptr<XVisualInfo[]>(XGetVisualInfo(xDisplay, VisualIDMask, &vTemplate, &visualCount));

    if (visualCount == 0)
    {
        // Can't happen...
        err() << "No X11 visual found. Bug in your EGL implementation ?" << std::endl;

        return {};
    }

    // Pick up the best one
    return availableVisuals[0];
}
#endif

} // namespace sf::priv
