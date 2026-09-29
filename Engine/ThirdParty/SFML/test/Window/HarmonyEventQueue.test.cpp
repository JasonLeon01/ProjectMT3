#include "../../src/SFML/Window/Harmony/EventQueue.hpp"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>


namespace
{
constexpr std::size_t maximumQueuedEvents = 65'536;

void fillWithLifecycleEvents(sf::priv::Harmony::EventQueue& events, std::size_t count)
{
    for (std::size_t index = 0; index < count; ++index)
    {
        if (index % 2)
            events.push(sf::Event::FocusGained{});
        else
            events.push(sf::Event::FocusLost{});
    }
}

std::size_t countClosedEvents(const std::deque<sf::Event>& events)
{
    return static_cast<std::size_t>(
        std::count_if(events.begin(), events.end(), [](const sf::Event& event) { return event.is<sf::Event::Closed>(); }));
}
} // namespace


TEST_CASE("[Window] Harmony event queue")
{
    sf::priv::Harmony::EventQueue events;

    SECTION("Adjacent equivalent events are coalesced")
    {
        events.push(sf::Event::MouseMoved{{1, 2}});
        events.push(sf::Event::MouseMoved{{3, 4}});
        events.push(sf::Event::TouchMoved{7, {5, 6}});
        events.push(sf::Event::TouchMoved{7, {8, 9}});
        events.push(sf::Event::Resized{{10, 11}});
        events.push(sf::Event::Resized{{12, 13}});

        const auto queued = events.drain();
        REQUIRE(queued.size() == 3);
        REQUIRE(queued[0].is<sf::Event::MouseMoved>());
        REQUIRE(queued[1].is<sf::Event::TouchMoved>());
        REQUIRE(queued[2].is<sf::Event::Resized>());
        CHECK(queued[0].getIf<sf::Event::MouseMoved>()->position == sf::Vector2i(3, 4));
        CHECK(queued[1].getIf<sf::Event::TouchMoved>()->position == sf::Vector2i(8, 9));
        CHECK(queued[2].getIf<sf::Event::Resized>()->size == sf::Vector2u(12, 13));
    }

    SECTION("Touch moves do not cross an event for another finger")
    {
        events.push(sf::Event::TouchMoved{1, {10, 20}});
        events.push(sf::Event::TouchBegan{2, {30, 40}});
        events.push(sf::Event::TouchMoved{1, {50, 60}});

        const auto queued = events.drain();
        REQUIRE(queued.size() == 3);
        REQUIRE(queued[0].is<sf::Event::TouchMoved>());
        REQUIRE(queued[1].is<sf::Event::TouchBegan>());
        REQUIRE(queued[2].is<sf::Event::TouchMoved>());
        CHECK(queued[0].getIf<sf::Event::TouchMoved>()->position == sf::Vector2i(10, 20));
        CHECK(queued[1].getIf<sf::Event::TouchBegan>()->finger == 2);
        CHECK(queued[2].getIf<sf::Event::TouchMoved>()->position == sf::Vector2i(50, 60));
    }

    SECTION("Resize events do not cross lifecycle events")
    {
        events.push(sf::Event::Resized{{100, 200}});
        events.push(sf::Event::FocusLost{});
        events.push(sf::Event::Resized{{300, 400}});

        const auto queued = events.drain();
        REQUIRE(queued.size() == 3);
        REQUIRE(queued[0].is<sf::Event::Resized>());
        REQUIRE(queued[1].is<sf::Event::FocusLost>());
        REQUIRE(queued[2].is<sf::Event::Resized>());
        CHECK(queued[0].getIf<sf::Event::Resized>()->size == sf::Vector2u(100, 200));
        CHECK(queued[1].is<sf::Event::FocusLost>());
        CHECK(queued[2].getIf<sf::Event::Resized>()->size == sf::Vector2u(300, 400));
    }

    SECTION("Closed is deduplicated before the queue reaches capacity")
    {
        events.push(sf::Event::Closed{});
        events.push(sf::Event::FocusLost{});
        events.push(sf::Event::Closed{});

        const auto queued = events.drain();
        REQUIRE(queued.size() == 2);
        CHECK(countClosedEvents(queued) == 1);
        CHECK(queued[0].is<sf::Event::Closed>());
        CHECK(queued[1].is<sf::Event::FocusLost>());
    }

    SECTION("A droppable event is evicted to preserve Closed at capacity")
    {
        events.push(sf::Event::MouseMoved{{17, 29}});
        events.push(sf::Event::FocusGained{});
        fillWithLifecycleEvents(events, maximumQueuedEvents - 2);
        events.push(sf::Event::Closed{});

        const auto queued = events.drain();
        REQUIRE(queued.size() == maximumQueuedEvents);
        CHECK(std::none_of(queued.begin(),
                           queued.end(),
                           [](const sf::Event& event) { return event.is<sf::Event::MouseMoved>(); }));
        CHECK(countClosedEvents(queued) == 1);
        CHECK(queued.back().is<sf::Event::Closed>());
    }

    SECTION("Closed replaces an older event when no droppable event is queued")
    {
        fillWithLifecycleEvents(events, maximumQueuedEvents);
        events.push(sf::Event::Closed{});

        const auto queued = events.drain();
        REQUIRE(queued.size() == maximumQueuedEvents);
        CHECK(countClosedEvents(queued) == 1);
        CHECK(queued.back().is<sf::Event::Closed>());
    }

    SECTION("Duplicate Closed does not evict a queued droppable event at capacity")
    {
        events.push(sf::Event::Closed{});
        events.push(sf::Event::MouseMoved{{31, 43}});
        events.push(sf::Event::FocusGained{});
        fillWithLifecycleEvents(events, maximumQueuedEvents - 3);
        events.push(sf::Event::Closed{});

        const auto queued = events.drain();
        REQUIRE(queued.size() == maximumQueuedEvents);
        CHECK(countClosedEvents(queued) == 1);
        CHECK(std::any_of(queued.begin(),
                          queued.end(),
                          [](const sf::Event& event) { return event.is<sf::Event::MouseMoved>(); }));
    }
}


TEST_CASE("[Window] Harmony mouse cancel releases pressed buttons")
{
    sf::priv::Harmony::EventQueue            events;
    std::array<bool, sf::Mouse::ButtonCount> buttons{};
    buttons[static_cast<std::size_t>(sf::Mouse::Button::Left)]   = true;
    buttons[static_cast<std::size_t>(sf::Mouse::Button::Middle)] = true;
    buttons[static_cast<std::size_t>(sf::Mouse::Button::Extra2)] = true;

    sf::priv::Harmony::releasePressedMouseButtons(buttons, {17, 29}, events);
    sf::priv::Harmony::releasePressedMouseButtons(buttons, {31, 43}, events);

    CHECK_FALSE(buttons[static_cast<std::size_t>(sf::Mouse::Button::Left)]);
    CHECK_FALSE(buttons[static_cast<std::size_t>(sf::Mouse::Button::Middle)]);
    CHECK_FALSE(buttons[static_cast<std::size_t>(sf::Mouse::Button::Extra2)]);

    const auto queued = events.drain();
    REQUIRE(queued.size() == 3);
    REQUIRE(queued[0].is<sf::Event::MouseButtonReleased>());
    REQUIRE(queued[1].is<sf::Event::MouseButtonReleased>());
    REQUIRE(queued[2].is<sf::Event::MouseButtonReleased>());

    const auto& left = *queued[0].getIf<sf::Event::MouseButtonReleased>();
    CHECK(left.button == sf::Mouse::Button::Left);
    CHECK(left.position == sf::Vector2i(17, 29));

    const auto& middle = *queued[1].getIf<sf::Event::MouseButtonReleased>();
    CHECK(middle.button == sf::Mouse::Button::Middle);
    CHECK(middle.position == sf::Vector2i(17, 29));

    const auto& extra2 = *queued[2].getIf<sf::Event::MouseButtonReleased>();
    CHECK(extra2.button == sf::Mouse::Button::Extra2);
    CHECK(extra2.position == sf::Vector2i(17, 29));
}
