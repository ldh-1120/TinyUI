#include "pch.h"

#include <TinyUI/Widgets/Separator.h>

#include <TinyUI/Rendering/PaintContext.h>
#include <TinyUI/Theme/Theme.h>

namespace tinyui {
    Separator::Separator() {
        GetLayoutStyle().preferredSize = { 0.0f, 1.0f };
    }

    void Separator::SetDirection(SeparatorDirection direction) {
        m_direction = direction;
    }

    SeparatorDirection Separator::GetDirection() const {
        return m_direction;
    }

    void Separator::SetOptions(const SeparatorOptions& options) {
        m_options = options;
    }

    const SeparatorOptions& Separator::GetOptions() const {
        return m_options;
    }

    void Separator::OnPaint(PaintContext& context) {
        tinycore::Rect rect = GetRect();

        float thickness = GetThickness();

        tinycore::Rect lineRect { };
        if (m_direction == SeparatorDirection::Horizontal)
            lineRect = { rect.x, rect.y + (rect.h - thickness) * 0.5f, rect.w, thickness };
        else
            lineRect = { rect.x + (rect.w - thickness) * 0.5f,  rect.y, thickness, rect.h };

        context.renderer.FillRect(lineRect, GetColor(context.theme), 0.0f);
    }

    float Separator::GetThickness() const {
        if (m_options.thickness <= 0.0f)
            return 1.0f;

        return m_options.thickness;
    }

    tinycore::Color Separator::GetColor(const Theme& theme) const {
        if (m_options.useCustomColor)
            return m_options.color;

        return theme.separator.color;
    }
}