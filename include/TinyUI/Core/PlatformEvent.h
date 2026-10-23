#pragma once

#include <TinyUI/Core/Window.h>

#include <cstdint>

namespace tinyui {
    enum class PlatformEventType : std::uint32_t {
        CloseRequested,
        Resized,
        DpiChanged,
        RedrawRequested
    };

    struct PlatformEvent {
        WindowId window{};
        PlatformEventType type{};
        WindowMetrics metrics{};
    };
}
