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
#include <SFML/Window/Context.hpp>
#include <SFML/Window/ContextSettings.hpp>
#include <SFML/Window/GlContext.hpp>

#include <SFML/System/Err.hpp>

#include <glad/gl.h>

#include <algorithm>
#include <atomic>
#include <iomanip>
#include <memory>
#include <mutex>
#include <optional>
#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <cassert>
#include <cctype>
#include <cstdlib>
#include <cstring>


#ifdef SFML_SYSTEM_EMSCRIPTEN
#include <SFML/Window/Emscripten/WebContext.hpp>
using ContextType = sf::priv::WebContext;
#elif defined(SFML_SYSTEM_WINDOWS)

#if defined(SFML_OPENGL_ES)

#include <SFML/Window/EglContext.hpp>
using ContextType = sf::priv::EglContext;

#else

#include <SFML/Window/Win32/WglContext.hpp>
using ContextType = sf::priv::WglContext;

#endif

#elif defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD) || defined(SFML_SYSTEM_OPENBSD) || \
    defined(SFML_SYSTEM_NETBSD)

#if defined(SFML_USE_DRM)

#include <SFML/Window/DRM/DRMContext.hpp>
using ContextType = sf::priv::DRMContext;

#else

#include <SFML/Window/Unix/Display.hpp>

#ifdef SFML_OPENGL_ES

#include <SFML/Window/EglContext.hpp>
using ContextType = sf::priv::EglContext;

#else

#include <SFML/Window/Unix/GlxContext.hpp>
using ContextType = sf::priv::GlxContext;

#endif
#endif

#elif defined(SFML_SYSTEM_MACOS)

#include <SFML/Window/macOS/SFContext.hpp>
using ContextType = sf::priv::SFContext;

#elif defined(SFML_SYSTEM_IOS)

#include <SFML/Window/iOS/EaglContext.hpp>
using ContextType = sf::priv::EaglContext;

#elif defined(SFML_SYSTEM_ANDROID) || defined(SFML_SYSTEM_HARMONY)

#include <SFML/Window/EglContext.hpp>
using ContextType = sf::priv::EglContext;

#endif

#ifdef SFML_SYSTEM_WINDOWS

using glEnableFuncType      = void(APIENTRY*)(GLenum);
using glGetErrorFuncType    = GLenum(APIENTRY*)();
using glGetIntegervFuncType = void(APIENTRY*)(GLenum, GLint*);
using glGetStringFuncType   = const GLubyte*(APIENTRY*)(GLenum);
using glGetStringiFuncType  = const GLubyte*(APIENTRY*)(GLenum, GLuint);
using glIsEnabledFuncType   = GLboolean(APIENTRY*)(GLenum);

#else

using glEnableFuncType      = void (*)(GLenum);
using glGetErrorFuncType    = GLenum (*)();
using glGetIntegervFuncType = void (*)(GLenum, GLint*);
using glGetStringFuncType   = const GLubyte* (*)(GLenum);
using glGetStringiFuncType  = const GLubyte* (*)(GLenum, GLuint);
using glIsEnabledFuncType   = GLboolean (*)(GLenum);

#endif

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE 0x809D
#endif

#ifndef GL_MAJOR_VERSION
#define GL_MAJOR_VERSION 0x821B
#endif

#ifndef GL_MINOR_VERSION
#define GL_MINOR_VERSION 0x821C
#endif

#ifndef GL_NUM_EXTENSIONS
#define GL_NUM_EXTENSIONS 0x821D
#endif

#ifndef GL_CONTEXT_FLAGS
#define GL_CONTEXT_FLAGS 0x821E
#endif

#ifndef GL_FRAMEBUFFER_SRGB
#define GL_FRAMEBUFFER_SRGB 0x8DB9
#endif

#ifndef GL_CONTEXT_FLAG_DEBUG_BIT
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#endif

#ifndef GL_CONTEXT_PROFILE_MASK
#define GL_CONTEXT_PROFILE_MASK 0x9126
#endif

#ifndef GL_CONTEXT_CORE_PROFILE_BIT
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#endif

#ifndef GL_CONTEXT_COMPATIBILITY_PROFILE_BIT
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#endif


namespace
{
// A nested named namespace is used here to allow unity builds of SFML.
namespace GlContextImpl
{
// This structure contains all the state necessary to
// track current context information for each thread
struct CurrentContext
{
    std::uint64_t        id{};
    sf::priv::GlContext* ptr{};
    unsigned int         transientCount{};

    // This per-thread variable holds the current context information for each thread
    static CurrentContext& get()
    {
        thread_local CurrentContext currentContext;
        return currentContext;
    }

private:
    // Private constructor to prevent CurrentContext from being constructed outside of get()
    CurrentContext() = default;
};
} // namespace GlContextImpl
} // namespace


namespace sf::priv
{
// This structure contains all the state necessary to
// track SharedContext usage
struct GlContext::SharedContext
{
    ////////////////////////////////////////////////////////////
    /// \brief Constructor
    ///
    ////////////////////////////////////////////////////////////
    explicit SharedContext(const ContextSettings& settings)
    {
        const std::scoped_lock lock(mutex);

        context.emplace(nullptr, settings, Vector2u{1, 1});
        if (context->initialize(GlSettings(settings)))
        {
            loadExtensions();
            context->setActive(false);
        }
        else
        {
            context.reset();
        }
    }

    ////////////////////////////////////////////////////////////
    /// \brief Get weak_ptr
    ///
    /// \return weak_ptr to the SharedContext
    ///
    ////////////////////////////////////////////////////////////
    struct Registry
    {
        std::recursive_mutex         mutex;
        std::weak_ptr<SharedContext> shared;
    };

    static Registry& registry()
    {
        // A graphics resource with static storage duration can release the last
        // context after the graphics device registry has been destroyed.
        static auto* const state = new Registry;
        return *state;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Get shared_ptr to the shared context
    ///
    /// Create new object if one doesn't already exist.
    ///
    /// \return shared_ptr to the shared context
    ///
    ////////////////////////////////////////////////////////////
    static std::shared_ptr<SharedContext> current()
    {
        const std::scoped_lock lock(registry().mutex);
        return registry().shared.lock();
    }

    static std::shared_ptr<SharedContext> get(const ContextSettings& settings = {})
    {
        const std::scoped_lock lock(registry().mutex);
        auto&                  weakSharedContext = registry().shared;
        auto                   sharedContext     = weakSharedContext.lock();

        if (!sharedContext)
        {
            sharedContext = std::make_shared<GlContext::SharedContext>(settings);
            if (!sharedContext->context)
                return {};
            weakSharedContext = sharedContext;
        }

        return sharedContext;
    }

    ////////////////////////////////////////////////////////////
    /// \brief Load our extensions vector with the supported extensions
    ///
    ////////////////////////////////////////////////////////////
    void loadExtensions()
    {
        auto glGetErrorFunc    = reinterpret_cast<glGetErrorFuncType>(getFunction("glGetError"));
        auto glGetIntegervFunc = reinterpret_cast<glGetIntegervFuncType>(getFunction("glGetIntegerv"));
        auto glGetStringFunc   = reinterpret_cast<glGetStringFuncType>(getFunction("glGetString"));

        if (!glGetErrorFunc || !glGetIntegervFunc || !glGetStringFunc)
            return;

        // Check whether a >= 3.0 context is available
        int majorVersion = 0;
        glGetIntegervFunc(GL_MAJOR_VERSION, &majorVersion);

        auto glGetStringiFunc = reinterpret_cast<glGetStringiFuncType>(getFunction("glGetStringi"));

        if (glGetErrorFunc() == GL_INVALID_ENUM || !majorVersion || !glGetStringiFunc)
        {
            // Try to load the < 3.0 way
            if (const char* extensionString = reinterpret_cast<const char*>(glGetStringFunc(GL_EXTENSIONS)))
            {
                extensions.clear();

                do
                {
                    const char* extension = extensionString;

                    while (*extensionString && (*extensionString != ' '))
                        ++extensionString;

                    extensions.emplace_back(extension, extensionString);
                } while (*extensionString++);
            }
        }
        else
        {
            // Try to load the >= 3.0 way
            int numExtensions = 0;
            glGetIntegervFunc(GL_NUM_EXTENSIONS, &numExtensions);

            if (numExtensions)
            {
                extensions.clear();

                for (unsigned int i = 0; i < static_cast<unsigned int>(numExtensions); ++i)
                    if (const char* extensionString = reinterpret_cast<const char*>(glGetStringiFunc(GL_EXTENSIONS, i)))
                        extensions.emplace_back(extensionString);
            }
        }
    }

    // AMD drivers have issues with internal synchronization
    // We need to make sure that no operating system context
    // or pixel format operations are performed simultaneously
    // This mutex is also used to protect the shared context
    // from being locked on multiple threads
    std::recursive_mutex mutex;

    // Supported OpenGL extensions
    std::vector<std::string> extensions;

    // The hidden, inactive context that will be shared with all other contexts
    std::optional<ContextType> context;
};


// This structure contains all the state necessary to
// track TransientContext usage
struct GlContext::TransientContext
{
    ////////////////////////////////////////////////////////////
    /// \brief Constructor
    ///
    ////////////////////////////////////////////////////////////
    TransientContext()
    {
        // TransientContext should never be created if there is
        // already a context active on the current thread
        assert(!GlContextImpl::CurrentContext::get().id && "Another context is active on the current thread");

        sharedContext = SharedContext::get();
        if (sharedContext)
        {
            sharedContextLock = std::unique_lock(sharedContext->mutex);
            sharedContext->context->setActive(true);
        }
    }

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ////////////////////////////////////////////////////////////
    ~TransientContext()
    {
        if (sharedContextLock)
            sharedContext->context->setActive(false);
    }

    ////////////////////////////////////////////////////////////
    /// \brief Deleted copy constructor
    ///
    ////////////////////////////////////////////////////////////
    TransientContext(const TransientContext&) = delete;

    ////////////////////////////////////////////////////////////
    /// \brief Deleted copy assignment
    ///
    ////////////////////////////////////////////////////////////
    TransientContext& operator=(const TransientContext&) = delete;

    ////////////////////////////////////////////////////////////
    /// \brief Get the thread local TransientContext
    ///
    /// This per-thread variable tracks if and how a transient
    /// context is currently being used on the current thread
    ///
    /// \return The thread local TransientContext
    ///
    ////////////////////////////////////////////////////////////
    static std::optional<TransientContext>& get()
    {
        static thread_local std::optional<TransientContext> transientContext;
        return transientContext;
    }

    ///////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    std::unique_lock<std::recursive_mutex> sharedContextLock;
    std::shared_ptr<SharedContext>         sharedContext;
};


// This structure contains all the implementation data we
// don't want to expose through the visible interface
struct GlContext::Impl
{
    ////////////////////////////////////////////////////////////
    /// \brief Constructor
    ///
    ////////////////////////////////////////////////////////////
    Impl()
    {
        const std::scoped_lock lock(registry().mutex);
        auto&                  weakUnsharedGlObjects = registry().objects;
        unsharedGlObjects                            = weakUnsharedGlObjects.lock();

        if (!unsharedGlObjects)
        {
            unsharedGlObjects = std::make_shared<UnsharedGlObjects>();

            weakUnsharedGlObjects = unsharedGlObjects;
        }
    }

    // Structure to track which unshared object belongs to which context
    struct UnsharedGlObject
    {
        std::uint64_t         contextId{};
        std::shared_ptr<void> object;
    };

    using UnsharedGlObjects = std::vector<UnsharedGlObject>;

    struct Registry
    {
        std::mutex                       mutex;
        std::weak_ptr<UnsharedGlObjects> objects;
    };

    static Registry& registry()
    {
        // The graphics device can release its final context during static destruction.
        static auto* const state = new Registry;
        return *state;
    }

    ///////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    std::shared_ptr<UnsharedGlObjects> unsharedGlObjects; //!< The current object's handle to unshared objects
    const std::uint64_t                id{
        []
        {
            static std::atomic<std::uint64_t> atomicId(1); // start at 1, zero is "no context"
            return atomicId.fetch_add(1);
        }()}; //!< Unique identifier, used for identifying contexts when managing unshareable OpenGL resources
};


////////////////////////////////////////////////////////////
std::shared_ptr<void> GlContext::getSharedContext()
{
    ContextSettings settings;
    return prepareSharedContext(settings);
}


////////////////////////////////////////////////////////////
std::shared_ptr<void> GlContext::prepareSharedContext(ContextSettings& settings)
{
    if (!validGlSettings(settings))
    {
        err() << "Invalid OpenGL backend or API version" << std::endl;
        return {};
    }

#if (defined(SFML_SYSTEM_LINUX) || defined(SFML_SYSTEM_FREEBSD) || defined(SFML_SYSTEM_OPENBSD) || \
     defined(SFML_SYSTEM_NETBSD)) &&                                                               \
    !defined(SFML_USE_DRM)
    const auto display = tryOpenDisplay();
    if (!display)
    {
        err() << "OpenGL requires an X11 display on this platform" << std::endl;
        return {};
    }
#endif

    const auto shared = SharedContext::get(settings);
    if (!shared)
        return {};

    const GlSettings requested(settings);
    if (!shared->context->m_settings.supports(requested))
    {
        shared->context->checkSettings(requested);
        return {};
    }

    settings.backend = shared->context->getSettings().backend;
    return shared;
}


////////////////////////////////////////////////////////////
void GlContext::registerUnsharedGlObject(std::shared_ptr<void> object)
{
    if (const std::lock_guard lock(Impl::registry().mutex); const auto unsharedGlObjects = Impl::registry().objects.lock())
        unsharedGlObjects->emplace_back(Impl::UnsharedGlObject{GlContextImpl::CurrentContext::get().id, std::move(object)});
}


////////////////////////////////////////////////////////////
void GlContext::unregisterUnsharedGlObject(std::shared_ptr<void> object)
{
    if (const std::lock_guard lock(Impl::registry().mutex); const auto unsharedGlObjects = Impl::registry().objects.lock())
    {
        // Find the object in unshared objects and remove it if its associated context is currently active
        // This will trigger the destructor of the object since shared_ptr
        // in unshared objects should be the only one existing
        const auto iter = std::find_if(unsharedGlObjects->begin(),
                                       unsharedGlObjects->end(),
                                       [&object](const Impl::UnsharedGlObject& obj) {
                                           return (obj.object == object) &&
                                                  (obj.contextId == GlContextImpl::CurrentContext::get().id);
                                       });

        if (iter != unsharedGlObjects->end())
            unsharedGlObjects->erase(iter);
    }
}


////////////////////////////////////////////////////////////
void GlContext::acquireTransientContext()
{
    auto& currentContext = GlContextImpl::CurrentContext::get();

#ifdef SFML_SYSTEM_HARMONY
    if (currentContext.ptr)
        static_cast<ContextType*>(currentContext.ptr)->refreshNativeWindow();
#endif

    if (!currentContext.id)
    {
        // A context borrowed by an outer lock may have been deactivated or destroyed.
        auto& fallback = TransientContext::get();
        if (!fallback)
            fallback.emplace();
        else if (fallback->sharedContext)
            fallback->sharedContext->context->setActive(true);
    }

    ++currentContext.transientCount;
}


////////////////////////////////////////////////////////////
void GlContext::releaseTransientContext()
{
    auto& currentContext = GlContextImpl::CurrentContext::get();
    assert(currentContext.transientCount && "Transient context release without a matching acquire");
    if (!currentContext.transientCount)
        return;

    --currentContext.transientCount;
    if (!currentContext.transientCount)
        TransientContext::get().reset();
}


////////////////////////////////////////////////////////////
std::unique_ptr<GlContext> GlContext::create()
{
    return create(ContextSettings{}, Vector2u{1, 1});
}


////////////////////////////////////////////////////////////
std::unique_ptr<GlContext> GlContext::create(const ContextSettings& settings, const WindowImpl& owner, unsigned int bitsPerPixel)
{
    ContextSettings effective = settings;
    const auto      shared    = std::static_pointer_cast<SharedContext>(prepareSharedContext(effective));
    if (!shared)
        return {};

    const std::scoped_lock lock(shared->mutex);
    if (!shared->context->setActive(true))
        return {};

    auto context = std::make_unique<ContextType>(&shared->context.value(), effective, owner, bitsPerPixel);
    shared->context->setActive(false);
    if (!context->initialize(GlSettings(effective)))
        return {};

    context->m_sharedContext = shared;
    context->checkSettings(GlSettings(settings));
    return context;
}


////////////////////////////////////////////////////////////
std::unique_ptr<GlContext> GlContext::create(const ContextSettings& settings, Vector2u size)
{
    ContextSettings effective = settings;
    const auto      shared    = std::static_pointer_cast<SharedContext>(prepareSharedContext(effective));
    if (!shared)
        return {};

    const std::scoped_lock lock(shared->mutex);
    if (!shared->context->setActive(true))
        return {};

    auto context = std::make_unique<ContextType>(&shared->context.value(), effective, size);
    shared->context->setActive(false);
    if (!context->initialize(GlSettings(effective)))
        return {};

    context->m_sharedContext = shared;
    context->checkSettings(GlSettings(settings));
    return context;
}


////////////////////////////////////////////////////////////
bool GlContext::isExtensionAvailable(std::string_view name)
{
    // If this function is called before any context is available,
    // the shared context will be created for the duration of this call
    const auto sharedContext = SharedContext::get();

    return sharedContext && std::find(sharedContext->extensions.begin(), sharedContext->extensions.end(), name) !=
                                sharedContext->extensions.end();
}


////////////////////////////////////////////////////////////
GlFunctionPointer GlContext::getFunction(const char* name)
{
    // Make sure we don't try to create the shared context here since
    // setActive can be called during construction and lead to infinite recursion
    const auto sharedContext = SharedContext::current();

    // We can't and don't need to lock when we are currently creating the shared context
    std::unique_lock<std::recursive_mutex> lock;

    if (sharedContext)
        lock = std::unique_lock(sharedContext->mutex);

    return ContextType::getFunction(name);
}


////////////////////////////////////////////////////////////
const GlContext* GlContext::getActiveContext()
{
    return GlContextImpl::CurrentContext::get().ptr;
}


////////////////////////////////////////////////////////////
std::uint64_t GlContext::getActiveContextId()
{
    return GlContextImpl::CurrentContext::get().id;
}


////////////////////////////////////////////////////////////
GlContext::~GlContext()
{
    auto& currentContext = GlContextImpl::CurrentContext::get();

    if (m_impl->id == currentContext.id)
    {
        currentContext.id  = 0;
        currentContext.ptr = nullptr;
    }
}


////////////////////////////////////////////////////////////
const ContextSettings& GlContext::getSettings() const
{
    return m_actualSettings;
}


////////////////////////////////////////////////////////////
bool GlContext::setActive(bool active)
{
#ifdef SFML_SYSTEM_HARMONY
    if (active)
        static_cast<ContextType&>(*this).refreshNativeWindow();
#endif
#ifdef SFML_SYSTEM_EMSCRIPTEN
    if (active && WebContext::isLost())
        return false;
#endif
    auto& currentContext = GlContextImpl::CurrentContext::get();

    // Make sure we don't try to create the shared context here since
    // setActive can be called during construction and lead to infinite recursion
    const auto sharedContext = SharedContext::current();

    if (active)
    {
        if (m_impl->id != currentContext.id)
        {
            // We can't and don't need to lock when we are currently creating the shared context
            std::unique_lock<std::recursive_mutex> lock;

            if (sharedContext)
                lock = std::unique_lock(sharedContext->mutex);

            // Activate the context
            if (makeCurrent(true))
            {
                // Set it as the new current context for this thread
                currentContext.id  = m_impl->id;
                currentContext.ptr = this;
                return true;
            }

            return false;
        }

        // This context is already the active one on this thread, don't do anything
        return true;
    }

    if (m_impl->id == currentContext.id)
    {
        // We can't and don't need to lock when we are currently creating the shared context
        std::unique_lock<std::recursive_mutex> lock;

        if (sharedContext)
            lock = std::unique_lock(sharedContext->mutex);

        // Deactivate the context
        if (makeCurrent(false))
        {
            currentContext.id  = 0;
            currentContext.ptr = nullptr;
            return true;
        }

        return false;
    }

    // This context is not the active one on this thread, don't do anything
    return true;
}


////////////////////////////////////////////////////////////
GlContext::GlContext() : m_impl(std::make_unique<Impl>())
{
    // Nothing to do
}


////////////////////////////////////////////////////////////
int GlContext::evaluateFormat(
    unsigned int           bitsPerPixel,
    const ContextSettings& settings,
    int                    colorBits,
    int                    depthBits,
    int                    stencilBits,
    int                    antiAliasing,
    bool                   accelerated,
    bool                   sRgb)
{
    int colorDiff        = static_cast<int>(bitsPerPixel) - colorBits;
    int depthDiff        = static_cast<int>(settings.depthBits) - depthBits;
    int stencilDiff      = static_cast<int>(settings.stencilBits) - stencilBits;
    int antiAliasingDiff = static_cast<int>(settings.antiAliasingLevel) - antiAliasing;

    // Weight sub-scores so that better settings don't score equally as bad as worse settings
    colorDiff *= ((colorDiff > 0) ? 100'000 : 1);
    depthDiff *= ((depthDiff > 0) ? 100'000 : 1);
    stencilDiff *= ((stencilDiff > 0) ? 100'000 : 1);
    antiAliasingDiff *= ((antiAliasingDiff > 0) ? 100'000 : 1);

    // Aggregate the scores
    int score = std::abs(colorDiff) + std::abs(depthDiff) + std::abs(stencilDiff) + std::abs(antiAliasingDiff);

    // If the user wants an sRGB capable format, try really hard to get one
    if (settings.sRgbCapable && !sRgb)
        score += 10'000'000;

    // Make sure we prefer hardware acceleration over features
    if (!accelerated)
        score += 100'000'000;

    return score;
}


////////////////////////////////////////////////////////////
void GlContext::cleanupUnsharedResources()
{
    const auto& currentContext = GlContextImpl::CurrentContext::get();

    // Save the current context so we can restore it later
    GlContext* contextToRestore = currentContext.ptr;

    // If this context is already active there is no need to save it
    if (m_impl->id == currentContext.id)
        contextToRestore = nullptr;

    // Make this context active so resources can be freed
    setActive(true);

    {
        const std::scoped_lock lock(Impl::registry().mutex);

        // Destroy the unshared objects contained in this context
        for (auto iter = m_impl->unsharedGlObjects->begin(); iter != m_impl->unsharedGlObjects->end();)
        {
            if (iter->contextId == m_impl->id)
            {
                iter = m_impl->unsharedGlObjects->erase(iter);
            }
            else
            {
                ++iter;
            }
        }
    }

    // Make the originally active context active again
    if (contextToRestore)
        contextToRestore->setActive(true);
}


////////////////////////////////////////////////////////////
bool GlContext::initialize(const GlSettings& requestedSettings)
{
    if (!setActive(true))
    {
        err() << "Unable to activate the requested OpenGL context" << std::endl;
        return false;
    }

    // Retrieve the context version number
    int majorVersion = 0;
    int minorVersion = 0;

    // Try the new way first
    auto glGetIntegervFunc = reinterpret_cast<glGetIntegervFuncType>(getFunction("glGetIntegerv"));
    auto glGetErrorFunc    = reinterpret_cast<glGetErrorFuncType>(getFunction("glGetError"));
    auto glGetStringFunc   = reinterpret_cast<glGetStringFuncType>(getFunction("glGetString"));
    auto glEnableFunc      = reinterpret_cast<glEnableFuncType>(getFunction("glEnable"));
    auto glIsEnabledFunc   = reinterpret_cast<glIsEnabledFuncType>(getFunction("glIsEnabled"));

    if (!glGetIntegervFunc || !glGetErrorFunc || !glGetStringFunc || !glEnableFunc || !glIsEnabledFunc)
    {
        err() << "Could not load necessary function to initialize OpenGL context" << std::endl;
        return false;
    }

    glGetIntegervFunc(GL_MAJOR_VERSION, &majorVersion);
    glGetIntegervFunc(GL_MINOR_VERSION, &minorVersion);

    if (glGetErrorFunc() != GL_INVALID_ENUM)
    {
        m_settings.majorVersion = static_cast<unsigned int>(majorVersion);
        m_settings.minorVersion = static_cast<unsigned int>(minorVersion);
    }
    else
    {
        // Try the old way

        m_settings.majorVersion = 0;
        m_settings.minorVersion = 0;

        if (const char* version = reinterpret_cast<const char*>(glGetStringFunc(GL_VERSION)))
        {
            // OpenGL ES Common Lite profile: The beginning of the returned string is "OpenGL ES-CL major.minor"
            // OpenGL ES Common profile:      The beginning of the returned string is "OpenGL ES-CM major.minor"
            // OpenGL ES Full profile:        The beginning of the returned string is "OpenGL ES major.minor"
            // Desktop OpenGL:                The beginning of the returned string is "major.minor"

            // Helper to parse OpenGL version strings
            static const auto parseVersionString =
                [](const char* versionString, const char* prefix, unsigned int& major, unsigned int& minor)
            {
                const std::size_t prefixLength = std::strlen(prefix);

                if ((std::strlen(versionString) >= (prefixLength + 3)) &&
                    (std::strncmp(versionString, prefix, prefixLength) == 0) && std::isdigit(versionString[prefixLength]) &&
                    (versionString[prefixLength + 1] == '.') && std::isdigit(versionString[prefixLength + 2]))
                {
                    major = static_cast<unsigned int>(versionString[prefixLength] - '0');
                    minor = static_cast<unsigned int>(versionString[prefixLength + 2] - '0');

                    return true;
                }

                return false;
            };

            if (!parseVersionString(version, "OpenGL ES-CL ", m_settings.majorVersion, m_settings.minorVersion) &&
                !parseVersionString(version, "OpenGL ES-CM ", m_settings.majorVersion, m_settings.minorVersion) &&
                !parseVersionString(version, "OpenGL ES ", m_settings.majorVersion, m_settings.minorVersion) &&
                !parseVersionString(version, "", m_settings.majorVersion, m_settings.minorVersion))
            {
                err() << "Unable to parse OpenGL version string: " << std::quoted(version) << "" << std::endl;
            }
        }
        else
        {
            err() << "Unable to retrieve OpenGL version string" << std::endl;
        }
    }

#ifndef SFML_OPENGL_ES
    // 3.0 contexts only deprecate features, but do not remove them yet
    // 3.1 contexts remove features if ARB_compatibility is not present
    // 3.2+ contexts remove features only if a core profile is requested

    // If the context was created with wglCreateContext, it is guaranteed to be compatibility.
    // If a 3.0 context was created with wglCreateContextAttribsARB, it is guaranteed to be compatibility.
    // If a 3.1 context was created with wglCreateContextAttribsARB, the compatibility flag
    // is set only if ARB_compatibility is present
    // If a 3.2+ context was created with wglCreateContextAttribsARB, the compatibility flag
    // would have been set correctly already depending on whether ARB_create_context_profile is supported.

    // If the user requests a 3.0 context, it will be a compatibility context regardless of the requested profile.
    // If the user requests a 3.1 context and its creation was successful, the specification
    // states that it will not be a compatibility profile context regardless of the requested
    // profile unless ARB_compatibility is present.

    m_settings.attributeFlags = GlSettings::Default;

    if (m_settings.majorVersion >= 3)
    {
        // Retrieve the context flags
        int flags = 0;
        glGetIntegervFunc(GL_CONTEXT_FLAGS, &flags);

        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
            m_settings.attributeFlags |= GlSettings::Debug;

        if ((m_settings.majorVersion == 3) && (m_settings.minorVersion == 1))
        {
            m_settings.attributeFlags |= GlSettings::Core;

            auto glGetStringiFunc = reinterpret_cast<glGetStringiFuncType>(getFunction("glGetStringi"));

            if (glGetStringiFunc)
            {
                int numExtensions = 0;
                glGetIntegervFunc(GL_NUM_EXTENSIONS, &numExtensions);

                for (unsigned int i = 0; i < static_cast<unsigned int>(numExtensions); ++i)
                {
                    const char* extensionString = reinterpret_cast<const char*>(glGetStringiFunc(GL_EXTENSIONS, i));

                    if (std::strstr(extensionString, "GL_ARB_compatibility"))
                    {
                        m_settings.attributeFlags &= ~static_cast<std::uint32_t>(GlSettings::Core);
                        break;
                    }
                }
            }
        }
        else if ((m_settings.majorVersion > 3) || (m_settings.minorVersion >= 2))
        {
            // Retrieve the context profile
            int profile = 0;
            glGetIntegervFunc(GL_CONTEXT_PROFILE_MASK, &profile);

            if (profile & GL_CONTEXT_CORE_PROFILE_BIT)
                m_settings.attributeFlags |= GlSettings::Core;
        }
    }

    // Enable anti-aliasing if requested by the user and supported
    if ((requestedSettings.antiAliasingLevel > 0) && (m_settings.antiAliasingLevel > 0))
    {
        glEnableFunc(GL_MULTISAMPLE);
    }
    else
    {
        m_settings.antiAliasingLevel = 0;
    }

    // Enable sRGB if requested by the user and supported
    if (requestedSettings.sRgbCapable && m_settings.sRgbCapable)
    {
        glEnableFunc(GL_FRAMEBUFFER_SRGB);

        // Check to see if the enable was successful
        if (glIsEnabledFunc(GL_FRAMEBUFFER_SRGB) == GL_FALSE)
        {
            err() << "Warning: Failed to enable GL_FRAMEBUFFER_SRGB" << std::endl;
            m_settings.sRgbCapable = false;
        }
    }
    else
    {
        m_settings.sRgbCapable = false;
    }
#else
    m_settings.sRgbCapable = requestedSettings.sRgbCapable && m_settings.sRgbCapable;
#endif

    m_actualSettings = m_settings.actual();
    if (!m_settings.supports(requestedSettings))
    {
        checkSettings(requestedSettings);
        return false;
    }
    return true;
}


////////////////////////////////////////////////////////////
void GlContext::checkSettings(const GlSettings& requestedSettings) const
{
    // Perform checks to inform the user if they are getting a context they might not have expected
    if (!m_settings.supports(requestedSettings) ||
        ((requestedSettings.attributeFlags & GlSettings::Debug) && !(m_settings.attributeFlags & GlSettings::Debug)) ||
        (m_settings.stencilBits < requestedSettings.stencilBits) ||
        (m_settings.antiAliasingLevel < requestedSettings.antiAliasingLevel) ||
        (m_settings.depthBits < requestedSettings.depthBits) || (!m_settings.sRgbCapable && requestedSettings.sRgbCapable))
    {
        err() << "Warning: The created OpenGL context does not fully meet the settings that were requested" << '\n'
              << "Requested: version = " << requestedSettings.majorVersion << "." << requestedSettings.minorVersion
              << " ; depth bits = " << requestedSettings.depthBits << " ; stencil bits = " << requestedSettings.stencilBits
              << " ; AA level = " << requestedSettings.antiAliasingLevel << std::boolalpha
              << " ; core = " << ((requestedSettings.attributeFlags & GlSettings::Core) != 0)
              << " ; debug = " << ((requestedSettings.attributeFlags & GlSettings::Debug) != 0)
              << " ; sRGB = " << requestedSettings.sRgbCapable << std::noboolalpha << '\n'
              << "Created: version = " << m_settings.majorVersion << "." << m_settings.minorVersion
              << " ; depth bits = " << m_settings.depthBits << " ; stencil bits = " << m_settings.stencilBits
              << " ; AA level = " << m_settings.antiAliasingLevel << std::boolalpha
              << " ; core = " << ((m_settings.attributeFlags & GlSettings::Core) != 0)
              << " ; debug = " << ((m_settings.attributeFlags & GlSettings::Debug) != 0)
              << " ; sRGB = " << m_settings.sRgbCapable << std::noboolalpha << std::endl;
    }
}

} // namespace sf::priv
