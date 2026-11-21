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
#include <SFML/Network/Dns.hpp>
#include <SFML/Network/Emscripten/WebTransport.hpp>

#include <SFML/System/String.hpp>

#include <algorithm>
#include <array>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>


namespace
{
struct Answer
{
    std::uint16_t type{};
    std::uint16_t qclass{};
    std::size_t   offset{};
    std::uint16_t length{};
};

struct Message
{
    std::vector<std::uint8_t> bytes;
    std::vector<Answer>       answers;
};

[[nodiscard]] const char* dohUrl()
{
    const char* override = std::getenv("SFML_DOH_URL");
    if (override && (override[0] != '\0'))
        return override;

    return "https://cloudflare-dns.com/dns-query";
}

[[nodiscard]] int timeoutMilliseconds(std::optional<sf::Time> timeout)
{
    if (!timeout.has_value() || (*timeout <= sf::Time::Zero))
        return 0;

    const auto milliseconds = timeout->asMilliseconds();
    if (milliseconds > std::numeric_limits<int>::max())
        return std::numeric_limits<int>::max();
    return static_cast<int>(milliseconds);
}

[[nodiscard]] bool callerSuppliedServer(const std::vector<sf::IpAddress>& servers)
{
    if (servers.empty())
        return false;

    sf::priv::webSkip("Dns query with a caller-supplied server");
    return true;
}

[[nodiscard]] std::string normalizedName(const sf::String& hostname)
{
    std::string name = hostname.toAnsiString();
    for (char& character : name)
        character = static_cast<char>(std::tolower(static_cast<unsigned char>(character)));
    if (!name.empty() && (name.back() == '.'))
        name.pop_back();
    return name;
}

[[nodiscard]] bool read16(const Message& message, std::size_t& offset, std::uint16_t& value)
{
    if (offset + 2 > message.bytes.size())
        return false;

    value = static_cast<std::uint16_t>((message.bytes[offset] << 8) | message.bytes[offset + 1]);
    offset += 2;
    return true;
}

[[nodiscard]] bool readName(const Message& message, std::size_t& offset, std::string& name, int depth)
{
    if (depth > 8)
        return false;

    name.clear();
    auto pos    = offset;
    auto jumped = false;
    while (pos < message.bytes.size())
    {
        const auto length = message.bytes[pos];
        if (length == 0)
        {
            if (!jumped)
                offset = pos + 1;
            return true;
        }

        if ((length & 0xC0) == 0xC0)
        {
            if (pos + 1 >= message.bytes.size())
                return false;

            const auto pointer = static_cast<std::size_t>(((length & 0x3F) << 8) | message.bytes[pos + 1]);
            if (!jumped)
                offset = pos + 2;

            std::string rest;
            auto        restOffset = pointer;
            if (!readName(message, restOffset, rest, depth + 1))
                return false;
            if (!name.empty() && !rest.empty())
                name.push_back('.');
            name += rest;
            return true;
        }

        if ((length & 0xC0) != 0)
            return false;
        if (pos + 1 + length > message.bytes.size())
            return false;

        if (!name.empty())
            name.push_back('.');
        name.append(reinterpret_cast<const char*>(message.bytes.data() + pos + 1), length);
        pos += static_cast<std::size_t>(1 + length);
        if (!jumped)
            offset = pos;
    }

    return false;
}

void appendName(std::vector<std::uint8_t>& query, std::string_view name)
{
    auto start = std::size_t{0};
    while (start < name.size())
    {
        auto dot = name.find('.', start);
        if (dot == std::string_view::npos)
            dot = name.size();

        const auto length = dot - start;
        if (length > 0)
        {
            query.push_back(static_cast<std::uint8_t>(std::min<std::size_t>(length, 63)));
            const auto clipped = std::min<std::size_t>(length, 63);
            query.insert(query.end(),
                         name.begin() + static_cast<std::ptrdiff_t>(start),
                         name.begin() + static_cast<std::ptrdiff_t>(start + clipped));
        }

        if (dot == name.size())
            break;
        start = dot + 1;
    }
    query.push_back(0);
}

[[nodiscard]] std::vector<std::uint8_t> buildQuery(std::string_view name, std::uint16_t type, std::uint16_t qclass)
{
    static std::uint16_t nextId = 1;
    const auto           id     = nextId++;

    std::vector<std::uint8_t> query =
        {static_cast<std::uint8_t>(id >> 8),
         static_cast<std::uint8_t>(id & 0xFF),
         0x01,
         0x00,
         0x00,
         0x01,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00,
         0x00};
    appendName(query, name);
    query.push_back(static_cast<std::uint8_t>(type >> 8));
    query.push_back(static_cast<std::uint8_t>(type & 0xFF));
    query.push_back(static_cast<std::uint8_t>(qclass >> 8));
    query.push_back(static_cast<std::uint8_t>(qclass & 0xFF));
    return query;
}

[[nodiscard]] bool parseMessage(Message& message)
{
    if (message.bytes.size() < 12)
        return false;

    const auto flags   = static_cast<std::uint16_t>((message.bytes[2] << 8) | message.bytes[3]);
    const auto rcode   = static_cast<std::uint16_t>(flags & 0x000F);
    const auto qdcount = static_cast<std::uint16_t>((message.bytes[4] << 8) | message.bytes[5]);
    const auto ancount = static_cast<std::uint16_t>((message.bytes[6] << 8) | message.bytes[7]);
    if ((rcode != 0) && (rcode != 3))
        return false;

    auto offset = std::size_t{12};
    for (std::uint16_t index = 0; index < qdcount; ++index)
    {
        std::string name;
        if (!readName(message, offset, name, 0))
            return false;

        std::uint16_t questionType{};
        std::uint16_t questionClass{};
        if (!read16(message, offset, questionType) || !read16(message, offset, questionClass))
            return false;
    }

    if (rcode == 3)
        return true;

    for (std::uint16_t index = 0; index < ancount; ++index)
    {
        std::string name;
        if (!readName(message, offset, name, 0))
            return false;

        Answer answer;
        if (!read16(message, offset, answer.type) || !read16(message, offset, answer.qclass))
            return false;
        if (offset + 4 > message.bytes.size())
            return false;
        offset += 4;

        if (!read16(message, offset, answer.length))
            return false;
        if (offset + answer.length > message.bytes.size())
            return false;

        answer.offset = offset;
        message.answers.push_back(answer);
        offset += answer.length;
    }

    return true;
}

[[nodiscard]] std::optional<Message> exchange(const std::string&      name,
                                              std::uint16_t           type,
                                              std::uint16_t           qclass,
                                              std::optional<sf::Time> timeout)
{
    const auto query = buildQuery(name, type, qclass);
    auto       bytes = sf::priv::webDoh(dohUrl(), query.data(), query.size(), timeoutMilliseconds(timeout));
    if (!bytes.has_value())
        return std::nullopt;

    Message message;
    message.bytes = std::move(*bytes);
    if (!parseMessage(message))
        return std::nullopt;
    return message;
}

[[nodiscard]] std::optional<std::string> readDomain(const Message& message, std::size_t offset)
{
    std::string name;
    if (!readName(message, offset, name, 0))
        return std::nullopt;
    return name;
}

[[nodiscard]] std::vector<std::string> readText(const Message& message, const Answer& answer)
{
    std::vector<std::string> parts;
    auto                     offset = answer.offset;
    const auto               end    = answer.offset + answer.length;
    while (offset < end)
    {
        const auto length = message.bytes[offset++];
        if (offset + length > end)
            break;
        parts.emplace_back(reinterpret_cast<const char*>(message.bytes.data() + offset), length);
        offset += length;
    }
    return parts;
}
} // namespace


namespace sf::Dns
{
////////////////////////////////////////////////////////////
std::optional<std::vector<IpAddress>> resolve(const String&                 hostname,
                                              const std::vector<IpAddress>& servers,
                                              std::optional<Time>           timeout)
{
    if (hostname.isEmpty())
        return std::nullopt;

    if (const auto literal = IpAddress::fromString(hostname.toAnsiString()); literal)
        return std::vector<IpAddress>{*literal};

    if (callerSuppliedServer(servers))
        return std::nullopt;

    if (normalizedName(hostname) == "localhost")
        return std::vector<IpAddress>{IpAddress::LocalHost, IpAddress::LocalHostV6};

    const auto name     = hostname.toAnsiString();
    const auto version4 = exchange(name, 1, 1, timeout);
    const auto version6 = exchange(name, 28, 1, timeout);
    if (!version4.has_value() && !version6.has_value())
        return std::nullopt;

    std::vector<IpAddress> addresses;
    if (version4.has_value())
    {
        for (const auto& answer : version4->answers)
        {
            if ((answer.type != 1) || (answer.qclass != 1) || (answer.length != 4))
                continue;

            const auto offset = answer.offset;
            const auto value  = (static_cast<std::uint32_t>(version4->bytes[offset]) << 24) |
                               (static_cast<std::uint32_t>(version4->bytes[offset + 1]) << 16) |
                               (static_cast<std::uint32_t>(version4->bytes[offset + 2]) << 8) |
                               static_cast<std::uint32_t>(version4->bytes[offset + 3]);
            addresses.emplace_back(value);
        }
    }

    if (version6.has_value())
    {
        for (const auto& answer : version6->answers)
        {
            if ((answer.type != 28) || (answer.qclass != 1) || (answer.length != 16))
                continue;

            std::array<std::uint8_t, 16> bytes{};
            std::memcpy(bytes.data(), version6->bytes.data() + answer.offset, bytes.size());
            addresses.emplace_back(bytes);
        }
    }

    return addresses;
}


////////////////////////////////////////////////////////////
std::vector<String> queryNs(const String& hostname, const std::vector<IpAddress>& servers, std::optional<Time> timeout)
{
    std::vector<String> names;
    if (hostname.isEmpty() || callerSuppliedServer(servers))
        return names;

    const auto message = exchange(hostname.toAnsiString(), 2, 1, timeout);
    if (!message.has_value())
        return names;

    for (const auto& answer : message->answers)
    {
        if ((answer.type != 2) || (answer.qclass != 1))
            continue;
        if (const auto domain = readDomain(*message, answer.offset); domain)
            names.emplace_back(*domain);
    }
    return names;
}


////////////////////////////////////////////////////////////
std::vector<MxRecord> queryMx(const String& hostname, const std::vector<IpAddress>& servers, std::optional<Time> timeout)
{
    std::vector<MxRecord> records;
    if (hostname.isEmpty() || callerSuppliedServer(servers))
        return records;

    const auto message = exchange(hostname.toAnsiString(), 15, 1, timeout);
    if (!message.has_value())
        return records;

    for (const auto& answer : message->answers)
    {
        if ((answer.type != 15) || (answer.qclass != 1) || (answer.length < 3))
            continue;

        auto          offset = answer.offset;
        std::uint16_t preference{};
        if (!read16(*message, offset, preference))
            continue;
        if (const auto domain = readDomain(*message, offset); domain)
            records.push_back(MxRecord{*domain, preference});
    }
    return records;
}


////////////////////////////////////////////////////////////
std::vector<SrvRecord> querySrv(const String& hostname, const std::vector<IpAddress>& servers, std::optional<Time> timeout)
{
    std::vector<SrvRecord> records;
    if (hostname.isEmpty() || callerSuppliedServer(servers))
        return records;

    const auto message = exchange(hostname.toAnsiString(), 33, 1, timeout);
    if (!message.has_value())
        return records;

    for (const auto& answer : message->answers)
    {
        if ((answer.type != 33) || (answer.qclass != 1) || (answer.length < 7))
            continue;

        auto          offset = answer.offset;
        std::uint16_t priority{};
        std::uint16_t weight{};
        std::uint16_t port{};
        if (!read16(*message, offset, priority) || !read16(*message, offset, weight) || !read16(*message, offset, port))
            continue;

        const auto domain = readDomain(*message, offset);
        if (!domain.has_value())
            continue;

        SrvRecord record;
        record.target   = *domain;
        record.port     = port;
        record.weight   = weight;
        record.priority = priority;
        records.push_back(std::move(record));
    }
    return records;
}


////////////////////////////////////////////////////////////
std::vector<std::vector<String>> queryTxt(const String& hostname, const std::vector<IpAddress>& servers, std::optional<Time> timeout)
{
    std::vector<std::vector<String>> records;
    if (hostname.isEmpty() || callerSuppliedServer(servers))
        return records;

    const auto message = exchange(hostname.toAnsiString(), 16, 1, timeout);
    if (!message.has_value())
        return records;

    for (const auto& answer : message->answers)
    {
        if ((answer.type != 16) || (answer.qclass != 1))
            continue;

        auto parts = readText(*message, answer);
        if (parts.empty())
            continue;

        auto& record = records.emplace_back();
        for (const auto& part : parts)
            record.emplace_back(part);
    }
    return records;
}


////////////////////////////////////////////////////////////
std::optional<IpAddress> getPublicAddress(std::optional<Time> timeout, IpAddress::Type type)
{
    const std::array<const char*, 2>   names{"whoami.cloudflare", "o-o.myaddr.l.google.com"};
    const std::array<std::uint16_t, 2> classes{3, 1};
    for (std::size_t index = 0; index < names.size(); ++index)
    {
        const auto message = exchange(names[index], 16, classes[index], timeout);
        if (!message.has_value())
            continue;

        for (const auto& answer : message->answers)
        {
            if (answer.type != 16)
                continue;

            for (auto text : readText(*message, answer))
            {
                text.erase(std::remove(text.begin(), text.end(), '"'), text.end());
                if (const auto address = IpAddress::fromString(text); address && (address->getType() == type))
                    return address;
            }
        }
    }

    return std::nullopt;
}
} // namespace sf::Dns
