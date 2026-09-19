#include "../../src/SFML/Window/Harmony/FullscreenModeCache.hpp"

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <thread>


TEST_CASE("[Window] Harmony fullscreen mode cache")
{
    const auto* const beforeSurface = &sf::priv::Harmony::refreshFullscreenModes({sf::VideoMode({1, 1}, 32)});
    REQUIRE(beforeSurface->size() == 1);
    CHECK(beforeSurface->front() == sf::VideoMode({1, 1}, 32));
    const auto* const beforeSurfaceElement  = &beforeSurface->front();
    const auto        beforeSurfaceIterator = beforeSurface->cbegin();

    const auto* const portrait = &sf::priv::Harmony::refreshFullscreenModes({sf::VideoMode({1'920, 2'880}, 32)});
    CHECK(portrait != beforeSurface);
    REQUIRE(portrait->size() == 1);
    CHECK(portrait->front() == sf::VideoMode({1'920, 2'880}, 32));
    const auto* const portraitElement  = &portrait->front();
    const auto        portraitIterator = portrait->cbegin();

    const auto* const landscape = &sf::priv::Harmony::refreshFullscreenModes({sf::VideoMode({2'880, 1'920}, 32)});
    CHECK(landscape != beforeSurface);
    CHECK(landscape != portrait);
    REQUIRE(landscape->size() == 1);
    CHECK(landscape->front() == sf::VideoMode({2'880, 1'920}, 32));

    // Later lifecycle snapshots must not mutate or invalidate public data
    // returned by an earlier query.
    REQUIRE(beforeSurface->size() == 1);
    CHECK(beforeSurface->front() == sf::VideoMode({1, 1}, 32));
    CHECK(&beforeSurface->front() == beforeSurfaceElement);
    CHECK(*beforeSurfaceIterator == sf::VideoMode({1, 1}, 32));
    REQUIRE(portrait->size() == 1);
    CHECK(portrait->front() == sf::VideoMode({1'920, 2'880}, 32));
    CHECK(&portrait->front() == portraitElement);
    CHECK(*portraitIterator == sf::VideoMode({1'920, 2'880}, 32));

    // Repeated queries of an existing lifecycle state reuse its immutable
    // snapshot rather than allocating another one.
    const auto* const repeatedPortrait = &sf::priv::Harmony::refreshFullscreenModes({sf::VideoMode({1'920, 2'880}, 32)});
    CHECK(repeatedPortrait == portrait);

    // Concurrent first queries for an equal new value share one process-wide
    // snapshot, which remains valid after the querying threads have exited.
    using ModeList = std::vector<sf::VideoMode>;
    std::array<const ModeList*, 8> concurrentSnapshots{};
    std::array<std::thread, 8>     workers;
    for (std::size_t index = 0; index < workers.size(); ++index)
    {
        workers[index] = std::thread(
            [&, index] {
                concurrentSnapshots[index] = &sf::priv::Harmony::refreshFullscreenModes({sf::VideoMode({1'234, 567}, 32)});
            });
    }
    for (auto& worker : workers)
        worker.join();

    REQUIRE(concurrentSnapshots.front());
    for (const auto* snapshot : concurrentSnapshots)
    {
        CHECK(snapshot == concurrentSnapshots.front());
        REQUIRE(snapshot->size() == 1);
        CHECK(snapshot->front() == sf::VideoMode({1'234, 567}, 32));
    }
}
