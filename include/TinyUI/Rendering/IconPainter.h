#pragma once

#include <TinyCore/Core/Types.h>

#include <TinyUI/Icons/ButtonIcon.h>

namespace tinyui {
    class Renderer;

    class IconPainter final {
    public:
        static void DrawButtonIcon(Renderer& renderer, ButtonIcon icon, tinycore::Rect rect, tinycore::Color color, float size, float thickness);

    private:
        static void DrawMinimize(Renderer& renderer, tinycore::Rect rect, tinycore::Color color, float thickness);
        static void DrawMaximize(Renderer& renderer, tinycore::Rect rect, tinycore::Color color, float thickness);
        static void DrawRestore(Renderer& renderer, tinycore::Rect rect, tinycore::Color color, float thickness);
        static void DrawClose(Renderer& renderer, tinycore::Rect rect, tinycore::Color color, float thickness);
    };
}