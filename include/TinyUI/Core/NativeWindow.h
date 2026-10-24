#pragma once

#include <cstdint>

namespace tinyui {
    enum class NativeWindowSystem : std::uint8_t {
        Unknown,
        Win32,
        X11,
        Wayland,
        Cocoa
    };

    struct NativeWindowHandle {
        NativeWindowSystem system = NativeWindowSystem::Unknown;

        std::uintptr_t window = 0;
        std::uintptr_t connection = 0;
    };
}