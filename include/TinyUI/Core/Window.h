#pragma once

#include <TinyUI/Foundation/Handle.h>

#include <cstdint>
#include <string>

namespace tinyui {
    struct WindowTag {};

    using WindowId = Handle<WindowTag>;

    struct WindowOptions {
        std::string title = "TinyUI";
        std::uint32_t width = 1280;
        std::uint32_t height = 720;
    };

    struct WindowMetrics {
        std::uint32_t pixelWidth = 0;
        std::uint32_t pixelHeight = 0;
        float scale = 1.0f;
    };
}
