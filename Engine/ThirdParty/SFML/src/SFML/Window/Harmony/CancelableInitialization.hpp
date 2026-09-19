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

#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <cstdint>


namespace sf::priv::Harmony
{
////////////////////////////////////////////////////////////
/// \brief Externally synchronized, irreversibly cancelable init transaction
///
/// Platform work is performed between authorize() and commit() without
/// holding the mutex that protects this object. shutdown() invalidates every
/// outstanding token without waiting for that work to return.
///
////////////////////////////////////////////////////////////
class CancelableInitialization
{
public:
    using Token = std::uint64_t;

    [[nodiscard]] Token begin()
    {
        if (m_shutdown || m_phase != Phase::Idle)
            return {};

        advanceGeneration();
        m_activeToken = m_generation;
        m_phase       = Phase::Preparing;
        return m_activeToken;
    }

    [[nodiscard]] bool authorize(Token token)
    {
        if (!matches(token, Phase::Preparing))
            return false;

        m_phase = Phase::Authorized;
        return true;
    }

    [[nodiscard]] bool commit(Token token)
    {
        if (!matches(token, Phase::Authorized))
            return false;

        m_activeToken = {};
        m_phase       = Phase::Idle;
        return true;
    }

    void abort(Token token)
    {
        if (token && token == m_activeToken)
        {
            m_activeToken = {};
            m_phase       = Phase::Idle;
        }
    }

    void shutdown()
    {
        advanceGeneration();
        m_activeToken = {};
        m_phase       = Phase::Idle;
        m_shutdown    = true;
    }

    [[nodiscard]] bool isActive() const
    {
        return m_phase != Phase::Idle;
    }

private:
    enum class Phase
    {
        Idle,
        Preparing,
        Authorized
    };

    [[nodiscard]] bool matches(Token token, Phase phase) const
    {
        return token && !m_shutdown && token == m_activeToken && m_phase == phase;
    }

    void advanceGeneration()
    {
        do
        {
            ++m_generation;
        } while (!m_generation);
    }

    Token m_generation{};
    Token m_activeToken{};
    Phase m_phase{Phase::Idle};
    bool  m_shutdown{};
};

} // namespace sf::priv::Harmony
