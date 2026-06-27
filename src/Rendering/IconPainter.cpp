#include "pch.h"

#include <TinyUI/Rendering/IconPainter.h>

#include <TinyUI/Rendering/Renderer.h>

namespace tinyui {
    static tinycore::Rect CenterRect(tinycore::Rect outerRect, float size) {
        return { outerRect.x + (outerRect.w - size) * 0.5f, outerRect.y + (outerRect.h - size) * 0.5f, size, size };
    }

    void IconPainter::DrawButtonIcon(Renderer& renderer, ButtonIcon icon, tinycore::Rect rect, tinycore::Color color, float size, float thickness) {
        tinycore::Rect iconRect = CenterRect(rect, size);
        if (icon == ButtonIcon::Minimize) {
            DrawMinimize(renderer, iconRect, color, thickness);
            return;
        }

        if (icon == ButtonIcon::Maximize) {
            DrawMaximize(renderer, iconRect, color, thickness);
            return;
        }

        if (icon == ButtonIcon::Restore) {
            DrawRestore(renderer, iconRect, color, thickness);
            return;
        }

        if (icon == ButtonIcon::Close) {
            DrawClose(renderer, iconRect, color, thickness);
            return;
        }
    }

    void IconPainter::DrawMinimize(Renderer& renderer, tinycore::Rect rect, tinycore::Color color, float thickness) {
        float y = rect.y + rect.h * 0.65f;

        renderer.DrawLine({ rect.x + rect.w * 0.15f, y }, { rect.x + rect.w * 0.85f, y }, color, thickness);
    }

    void IconPainter::DrawMaximize(Renderer& renderer, tinycore::Rect rect, tinycore::Color color, float thickness) {
        tinycore::Rect box { rect.x + rect.w * 0.18f, rect.y + rect.h * 0.18f, rect.w * 0.64f, rect.h * 0.64f };

        renderer.DrawRect(box, color, thickness, 0.0f);
    }

    void IconPainter::DrawRestore(Renderer& renderer, tinycore::Rect rect, tinycore::Color color, float thickness) {
        tinycore::Rect backBox { rect.x + rect.w * 0.28f, rect.y + rect.h * 0.16f, rect.w * 0.56f, rect.h * 0.56f };
        tinycore::Rect frontBox { rect.x + rect.w * 0.16f, rect.y + rect.h * 0.28f, rect.w * 0.56f, rect.h * 0.56f };

        renderer.DrawRect(backBox, color, thickness, 0.0f);
        renderer.DrawRect(frontBox, color, thickness, 0.0f);
    }

    void IconPainter::DrawClose(Renderer& renderer, tinycore::Rect rect, tinycore::Color color, float thickness) {
        renderer.DrawLine({ rect.x + rect.w * 0.22f, rect.y + rect.h * 0.22f }, { rect.x + rect.w * 0.78f, rect.y + rect.h * 0.78f }, color, thickness);
        renderer.DrawLine({ rect.x + rect.w * 0.78f, rect.y + rect.h * 0.22f }, { rect.x + rect.w * 0.22f, rect.y + rect.h * 0.78f }, color, thickness);
    }
}