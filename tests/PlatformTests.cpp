#include <TinyUI/Core/Platform.h>

#include <catch2/catch_test_macros.hpp>

#include <memory>
#include <utility>

using namespace tinyui;

TEST_CASE("Platform initializes", "[platform]") {
    Result<std::unique_ptr<Platform>> created = createPlatform();

    REQUIRE(created.has_value());
    REQUIRE(created.value() != nullptr);
    REQUIRE(created.value()->windowCount() == 0);
}

TEST_CASE("Platform rejects invalid window dimensions", "[platform]") {
    Result<std::unique_ptr<Platform>> created = createPlatform();
    REQUIRE(created.has_value());

    std::unique_ptr<Platform> platform = std::move(created.value());

    WindowOptions options{};
    options.width = 0;

    Result<WindowId> result = platform->createWindow(options);

    REQUIRE_FALSE(result.has_value());
    REQUIRE(result.error().code == ErrorCode::InvalidArgument);
    REQUIRE(platform->windowCount() == 0);
}

TEST_CASE("Platform manages multiple windows", "[platform]") {
    Result<std::unique_ptr<Platform>> created = createPlatform();
    REQUIRE(created.has_value());

    std::unique_ptr<Platform> platform = std::move(created.value());

    WindowOptions options{};
    options.width = 320;
    options.height = 240;

    Result<WindowId> first = platform->createWindow(options);
    Result<WindowId> second = platform->createWindow(options);

    REQUIRE(first.has_value());
    REQUIRE(second.has_value());

    const WindowId firstId = first.value();
    const WindowId secondId = second.value();

    REQUIRE(firstId != secondId);
    REQUIRE(platform->windowCount() == 2);

    REQUIRE(platform->windowMetrics(firstId).has_value());
    REQUIRE(platform->windowMetrics(secondId).has_value());

    REQUIRE(platform->destroyWindow(firstId));
    REQUIRE(platform->windowCount() == 1);

    REQUIRE_FALSE(platform->windowMetrics(firstId).has_value());
    REQUIRE_FALSE(platform->destroyWindow(firstId));

    Result<WindowId> third = platform->createWindow(options);
    REQUIRE(third.has_value());

    REQUIRE(third.value() != firstId);
    REQUIRE(platform->windowCount() == 2);

    REQUIRE(platform->destroyWindow(secondId));
    REQUIRE(platform->destroyWindow(third.value()));
    REQUIRE(platform->windowCount() == 0);
}
