#include "../../src/SFML/Window/Harmony/CancelableInitialization.hpp"

#include <catch2/catch_test_macros.hpp>


TEST_CASE("[Window] Harmony cancelable initialization")
{
    using Initialization = sf::priv::Harmony::CancelableInitialization;

    SECTION("A transaction must be authorized before it can commit")
    {
        Initialization initialization;

        const auto token = initialization.begin();
        REQUIRE(token != Initialization::Token{});
        CHECK_FALSE(initialization.commit(token));
        CHECK(initialization.authorize(token));
        CHECK(initialization.commit(token));

        const auto nextToken = initialization.begin();
        CHECK(nextToken != Initialization::Token{});
        CHECK(nextToken != token);
    }

    SECTION("Each transaction phase can only be entered once")
    {
        Initialization initialization;

        CHECK_FALSE(initialization.isActive());
        CHECK_FALSE(initialization.authorize({}));
        CHECK_FALSE(initialization.commit({}));

        const auto token = initialization.begin();
        REQUIRE(token != Initialization::Token{});
        CHECK(initialization.isActive());
        CHECK(initialization.begin() == Initialization::Token{});
        CHECK(initialization.authorize(token));
        CHECK(initialization.isActive());
        CHECK_FALSE(initialization.authorize(token));
        CHECK(initialization.begin() == Initialization::Token{});
        CHECK(initialization.commit(token));
        CHECK_FALSE(initialization.isActive());
        CHECK_FALSE(initialization.commit(token));
    }

    SECTION("Abort invalidates transactions in either active phase")
    {
        Initialization initialization;

        const auto preparingToken = initialization.begin();
        REQUIRE(preparingToken != Initialization::Token{});
        initialization.abort(preparingToken);
        CHECK_FALSE(initialization.isActive());
        CHECK_FALSE(initialization.authorize(preparingToken));

        const auto authorizedToken = initialization.begin();
        REQUIRE(authorizedToken != Initialization::Token{});
        REQUIRE(authorizedToken != preparingToken);
        REQUIRE(initialization.authorize(authorizedToken));
        initialization.abort(authorizedToken);
        CHECK_FALSE(initialization.isActive());
        CHECK_FALSE(initialization.commit(authorizedToken));

        const auto replacementToken = initialization.begin();
        REQUIRE(replacementToken != Initialization::Token{});
        CHECK(replacementToken != authorizedToken);
    }

    SECTION("Aborting a stale token cannot cancel its replacement")
    {
        Initialization initialization;

        const auto staleToken = initialization.begin();
        REQUIRE(staleToken != Initialization::Token{});
        initialization.abort(staleToken);

        const auto activeToken = initialization.begin();
        REQUIRE(activeToken != Initialization::Token{});
        initialization.abort(staleToken);
        CHECK(initialization.authorize(activeToken));
        CHECK(initialization.commit(activeToken));
    }

    SECTION("Shutdown permanently invalidates a preparing transaction")
    {
        Initialization initialization;

        const auto token = initialization.begin();
        REQUIRE(token != Initialization::Token{});
        initialization.shutdown();

        CHECK_FALSE(initialization.isActive());
        CHECK_FALSE(initialization.authorize(token));
        CHECK_FALSE(initialization.commit(token));
        initialization.abort(token);
        CHECK(initialization.begin() == Initialization::Token{});
        initialization.shutdown();
        CHECK(initialization.begin() == Initialization::Token{});
    }

    SECTION("Shutdown permanently invalidates an authorized transaction")
    {
        Initialization initialization;

        const auto token = initialization.begin();
        REQUIRE(token != Initialization::Token{});
        REQUIRE(initialization.authorize(token));
        initialization.shutdown();

        CHECK_FALSE(initialization.commit(token));
        initialization.abort(token);
        CHECK(initialization.begin() == Initialization::Token{});
    }
}
