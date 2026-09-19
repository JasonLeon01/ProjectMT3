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
#include <SFML/System/Harmony/ResourceManagerImpl.hpp>

#include <memory>
#include <mutex>
#include <new>
#include <rawfile/raw_file_manager.h>
#include <utility>


namespace sf::priv::Harmony
{
class ResourceManagerLease
{
public:
    ResourceManagerLease(void* resourceManager, ResourceManagerRegistration registration) :
        m_resourceManager(resourceManager),
        m_registration(registration)
    {
    }

    ~ResourceManagerLease()
    {
        OH_ResourceManager_ReleaseNativeResourceManager(static_cast<NativeResourceManager*>(m_resourceManager));
    }

    [[nodiscard]] void* get() const
    {
        return m_resourceManager;
    }

    [[nodiscard]] ResourceManagerRegistration getRegistration() const
    {
        return m_registration;
    }

private:
    void*                       m_resourceManager{};
    ResourceManagerRegistration m_registration{};
};

} // namespace sf::priv::Harmony


namespace
{
class ResourceManagerRegistry
{
public:
    sf::priv::Harmony::ResourceManagerRegistration set(void* resourceManager)
    {
        sf::priv::Harmony::ResourceManagerLeasePtr     retired;
        sf::priv::Harmony::ResourceManagerRegistration registration{};

        {
            const std::lock_guard lock(m_mutex);

            if (m_current && sf::priv::Harmony::getResourceManager(m_current) == resourceManager)
                return {};

            sf::priv::Harmony::ResourceManagerLeasePtr next;
            if (resourceManager)
            {
                registration = nextRegistration();
                next         = std::make_shared<sf::priv::Harmony::ResourceManagerLease>(resourceManager, registration);
            }

            retired = std::exchange(m_current, std::move(next));
        }

        // Native manager destruction may re-enter this API. Never destroy the
        // retired lease while holding the registry mutex.
        retired.reset();
        return registration;
    }

    bool unset(sf::priv::Harmony::ResourceManagerRegistration registration)
    {
        sf::priv::Harmony::ResourceManagerLeasePtr retired;

        {
            const std::lock_guard lock(m_mutex);
            if (!m_current || m_current->getRegistration() != registration)
                return false;

            retired = std::move(m_current);
        }

        retired.reset();
        return true;
    }

    [[nodiscard]] sf::priv::Harmony::ResourceManagerLeasePtr acquire() const
    {
        const std::lock_guard lock(m_mutex);
        return m_current;
    }

private:
    [[nodiscard]] sf::priv::Harmony::ResourceManagerRegistration nextRegistration()
    {
        // Zero is reserved for an invalid registration token.
        do
        {
            ++m_nextRegistration;
        } while (!m_nextRegistration);

        return m_nextRegistration;
    }

    mutable std::mutex                             m_mutex;
    sf::priv::Harmony::ResourceManagerLeasePtr     m_current;
    sf::priv::Harmony::ResourceManagerRegistration m_nextRegistration{};
};


ResourceManagerRegistry& getRegistry()
{
    static ResourceManagerRegistry registry;
    return registry;
}
} // namespace


namespace sf::priv::Harmony
{
ResourceManagerRegistration registerOwnedResourceManager(void* resourceManager)
{
    if (!resourceManager)
        return {};

    try
    {
        return getRegistry().set(resourceManager);
    } catch (const std::bad_alloc&)
    {
        return {};
    }
}


bool unregisterResourceManager(ResourceManagerRegistration registration)
{
    return registration && getRegistry().unset(registration);
}


ResourceManagerLeasePtr acquireResourceManagerLease()
{
    return getRegistry().acquire();
}


void* getResourceManager(const ResourceManagerLeasePtr& lease)
{
    return lease ? lease->get() : nullptr;
}

} // namespace sf::priv::Harmony
