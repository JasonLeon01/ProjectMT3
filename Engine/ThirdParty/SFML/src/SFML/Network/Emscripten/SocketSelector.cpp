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
#include <SFML/Network/Emscripten/WebTransport.hpp>
#include <SFML/Network/Socket.hpp>
#include <SFML/Network/SocketSelector.hpp>

#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

#include <cstdint>


namespace sf
{
struct SocketSelector::SocketSelectorImpl
{
    struct Entry
    {
        const Socket*                                      socket{};
        SocketHandle                                       handle{};
        SocketSelector::ReadinessType                      mask{SocketSelector::Receive};
        std::function<void(SocketSelector::ReadinessType)> callback;
        bool                                               readyReceive{};
        bool                                               readySend{};
        std::uint64_t                                      registration{};
    };

    std::vector<Entry>                entries;
    std::unordered_set<std::uint64_t> registrations;
    std::uint64_t                     nextRegistration{};

    bool add(const Socket*                                      socket,
             SocketHandle                                       handle,
             SocketSelector::ReadinessType                      mask,
             std::function<void(SocketSelector::ReadinessType)> callback)
    {
        const auto iter = std::find_if(entries.begin(),
                                       entries.end(),
                                       [socket](const Entry& entry) { return entry.socket == socket; });
        if (iter != entries.end())
        {
            registrations.erase(iter->registration);
            *iter = Entry{socket, handle, mask, std::move(callback), false, false, ++nextRegistration};
            registrations.insert(iter->registration);
            return true;
        }

        entries.push_back(Entry{socket, handle, mask, std::move(callback), false, false, ++nextRegistration});
        registrations.insert(entries.back().registration);
        return true;
    }

    bool remove(const Socket* socket)
    {
        const auto iter = std::find_if(entries.begin(),
                                       entries.end(),
                                       [socket](const Entry& entry) { return entry.socket == socket; });
        if (iter == entries.end())
            return false;

        registrations.erase(iter->registration);
        entries.erase(iter);
        return true;
    }

    void clear()
    {
        entries.clear();
        registrations.clear();
    }

    bool wait(Time timeout)
    {
        std::vector<int> handles;
        std::vector<int> masks;
        handles.reserve(entries.size());
        masks.reserve(entries.size());
        for (auto& entry : entries)
        {
            handles.push_back(static_cast<int>(entry.handle));
            masks.push_back(static_cast<int>(entry.mask));
            entry.readyReceive = false;
            entry.readySend    = false;
        }

        int timeoutMs = -1;
        if (timeout != Time::Zero)
        {
            const auto milliseconds = timeout.asMilliseconds();
            timeoutMs               = milliseconds > 0 ? static_cast<int>(milliseconds) : 0;
        }

        const int* handlePtr = handles.empty() ? nullptr : handles.data();
        const int* maskPtr   = masks.empty() ? nullptr : masks.data();
        static_cast<void>(priv::webWait(handlePtr, maskPtr, static_cast<int>(handles.size()), timeoutMs));

        auto ready = false;
        for (auto& entry : entries)
        {
            entry.readyReceive = ((entry.mask & SocketSelector::Receive) != 0) && priv::webIsReadable(entry.handle);
            entry.readySend    = ((entry.mask & SocketSelector::Send) != 0) && priv::webIsWritable(entry.handle);
            ready              = ready || entry.readyReceive || entry.readySend;
        }
        return ready;
    }

    bool isReady(const Socket* socket, SocketSelector::ReadinessType readinessType) const
    {
        const auto iter = std::find_if(entries.begin(),
                                       entries.end(),
                                       [socket](const Entry& entry) { return entry.socket == socket; });
        if (iter == entries.end())
            return false;

        auto ready = false;
        if ((readinessType & SocketSelector::Receive) != 0)
            ready = ready || iter->readyReceive;
        if ((readinessType & SocketSelector::Send) != 0)
            ready = ready || iter->readySend;
        return ready;
    }

    void dispatchReadyCallbacks()
    {
        // Callbacks may add, remove, clear or replace registrations. Keep their
        // storage alive, but do not dispatch entries canceled during this batch.
        std::vector<Entry> pending;
        for (const auto& entry : entries)
        {
            if (entry.callback && (entry.readyReceive || entry.readySend))
                pending.push_back(entry);
        }

        for (const auto& entry : pending)
        {
            if (registrations.find(entry.registration) == registrations.end())
                continue;

            SocketSelector::ReadinessType readinessType{};
            if (entry.readyReceive)
                readinessType |= SocketSelector::Receive;
            if (entry.readySend)
                readinessType |= SocketSelector::Send;
            if (readinessType != 0)
                entry.callback(readinessType);
        }
    }
};


////////////////////////////////////////////////////////////
SocketSelector::SocketSelector() : m_impl(std::make_unique<SocketSelectorImpl>())
{
}


////////////////////////////////////////////////////////////
SocketSelector::~SocketSelector() = default;


////////////////////////////////////////////////////////////
SocketSelector::SocketSelector(const SocketSelector& copy) : m_impl(std::make_unique<SocketSelectorImpl>(*copy.m_impl))
{
}


////////////////////////////////////////////////////////////
SocketSelector& SocketSelector::operator=(const SocketSelector& right)
{
    SocketSelector temp(right);
    std::swap(m_impl, temp.m_impl);
    return *this;
}


////////////////////////////////////////////////////////////
SocketSelector::SocketSelector(SocketSelector&&) noexcept = default;


////////////////////////////////////////////////////////////
SocketSelector& SocketSelector::operator=(SocketSelector&&) noexcept = default;


////////////////////////////////////////////////////////////
bool SocketSelector::add(const Socket&                                    socket,
                         ReadinessType                                    readinessType,
                         std::function<void(ReadinessType readinessType)> readyCallback)
{
    if (const SocketHandle handle = socket.getNativeHandle(); handle >= 0)
        return m_impl->add(&socket, handle, readinessType, std::move(readyCallback));

    return false;
}


////////////////////////////////////////////////////////////
bool SocketSelector::remove(const Socket& socket)
{
    return m_impl->remove(&socket);
}


////////////////////////////////////////////////////////////
void SocketSelector::clear()
{
    m_impl->clear();
}


////////////////////////////////////////////////////////////
bool SocketSelector::wait(Time timeout)
{
    return m_impl->wait(timeout);
}


////////////////////////////////////////////////////////////
bool SocketSelector::isReady(const Socket& socket, ReadinessType readinessType) const
{
    return m_impl->isReady(&socket, readinessType);
}


////////////////////////////////////////////////////////////
void SocketSelector::dispatchReadyCallbacks()
{
    m_impl->dispatchReadyCallbacks();
}
} // namespace sf
