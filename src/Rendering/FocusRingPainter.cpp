#include "pch.h"

#include <TinyUI/Rendering/FocusRingPainter.h>

#include <TinyUI/Rendering/Renderer.h>

namespace tinyui {
    void FocusRingPainter::Draw(Renderer& renderer, tinycore::Rect rect, const FocusRingStyle& style) {
        if (!style.enabled)
            return;

        if (style.thickness <= 0.0f)
            return;

        tinycore::Rect ringRect { rect.x - style.offset, rect.y - style.offset, rect.w + style.offset * 2.0f, rect.h + style.offset * 2.0f };
        renderer.DrawRect(ringRect, style.color, style.thickness, style.cornerRadius + style.offset);
    }
}