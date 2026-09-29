#include <SFML/Window/Context.hpp>
#include <SFML/Window/GlResource.hpp>

#include <catch2/catch_test_macros.hpp>

#include <WindowUtil.hpp>
#include <optional>
#include <type_traits>

static_assert(!std::is_constructible_v<sf::GlResource>);
static_assert(std::is_copy_constructible_v<sf::GlResource>);
static_assert(std::is_copy_assignable_v<sf::GlResource>);
static_assert(std::is_nothrow_move_constructible_v<sf::GlResource>);
static_assert(std::is_nothrow_move_assignable_v<sf::GlResource>);

namespace
{
struct GlResourceAccessor : sf::GlResource
{
    using TransientContextLock = sf::GlResource::TransientContextLock;
};
} // namespace

TEST_CASE("[Window] sf::GlResource::TransientContextLock", runDisplayTests())
{
    const GlResourceAccessor   resource;
    std::optional<sf::Context> context{std::in_place};

    {
        const GlResourceAccessor::TransientContextLock outerLock;
        REQUIRE(sf::Context::getActiveContextId() != 0);

        // Model an active context being invalidated while a transient lock is
        // alive. Its destruction clears the same thread-local context ID.
        context.reset();
        CHECK(sf::Context::getActiveContextId() == 0);

        {
            const GlResourceAccessor::TransientContextLock nestedLock;
            CHECK(sf::Context::getActiveContextId() != 0);
        }

        // The fallback belongs to the outermost lock, not the nested lock that
        // happened to create it.
        CHECK(sf::Context::getActiveContextId() != 0);
    }

    CHECK(sf::Context::getActiveContextId() == 0);
}
