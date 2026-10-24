#pragma once

#include <TinyUI/Core/PlatformEvent.h>
#include <TinyUI/Core/Window.h>
#include <TinyUI/Foundation/Result.h>
#include <TinyUI/Core/NativeWindow.h>

#include <cstddef>
#include <memory>
#include <optional>

namespace tinyui {
    class Platform {
    public:
        virtual ~Platform() = default;

        [[nodiscard]] virtual Result<WindowId> createWindow(const WindowOptions& options) = 0;

        virtual bool destroyWindow(WindowId id) = 0;
        virtual bool showWindow(WindowId id) = 0;

        [[nodiscard]] virtual std::optional<WindowMetrics> windowMetrics(WindowId id) const = 0;

        virtual void pollEvents() = 0;
        virtual void waitEvents() = 0;

        [[nodiscard]] virtual std::optional<PlatformEvent> nextEvent() = 0;

        [[nodiscard]] virtual std::size_t windowCount() const noexcept = 0;
        [[nodiscard]] virtual bool quitRequested() const noexcept = 0;

        [[nodiscard]] virtual std::optional<NativeWindowHandle> nativeWindowHandle(WindowId id) const = 0;
    };

    [[nodiscard]] Result<std::unique_ptr<Platform>> createPlatform();
}
