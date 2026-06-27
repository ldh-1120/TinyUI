#include "pch.h"

#include <TinyUI/Widgets/Panel.h>

#include <utility>

namespace tinyui {
    Panel::Panel(std::wstring title) : m_title(std::move(title)) {}

    void Panel::SetTitle(std::wstring_view title) {
        m_title = title;
    }

    const std::wstring& Panel::GetTitle() const {
        return m_title;
    }

    void Panel::SetOptions(const PanelOptions& options) {
        m_options = options;
    }

    const PanelOptions& Panel::GetOptions() const {
        return m_options;
    }

    Rect Panel::GetContentRect() const {
        Rect rect = GetRect();

        const float titleHeight = m_options.titleHeight >= 0.f ? m_options.titleHeight : 36.f;
        const float topOffset = m_options.showTitle ? titleHeight : 0.f;

        return { rect.x, rect.y + topOffset, rect.w, rect.h - topOffset };
    }

    void Panel::OnPaint(PaintContext& context) {
        Renderer& renderer = context.renderer;
        const Theme& theme = context.theme;

        Rect rect = GetRect();

        float cornerRadius = GetCornerRadius(theme);
        float borderThickness = GetBorderThickness(theme);
        float titleHeight = GetTitleHeight(theme);
        float titleFontSize = GetTitleFontSize(theme);

        renderer.FillRect(rect, theme.panel.background, cornerRadius);
        if (borderThickness > 0.0f)
            renderer.DrawRect(rect, theme.panel.border, borderThickness, cornerRadius);

        if (!m_options.showTitle)
            return;

        Rect titleRect { rect.x + 14.0f, rect.y, rect.w - 28.0f, titleHeight };
        renderer.DrawTextBox(m_title, titleRect, theme.panel.titleText, titleFontSize, TextAlign::Left, TextWrap::NoWrap);
    }

    float Panel::GetCornerRadius(const Theme& theme) const {
        if (m_options.cornerRadius >= 0.0f)
            return m_options.cornerRadius;

        return theme.panel.cornerRadius;
    }

    float Panel::GetBorderThickness(const Theme& theme) const {
        if (m_options.borderThickness >= 0.0f)
            return m_options.borderThickness;

        return theme.panel.borderThickness;
    }

    float Panel::GetTitleHeight(const Theme& theme) const {
        if (m_options.titleHeight >= 0.0f)
            return m_options.titleHeight;

        return theme.panel.titleHeight;
    }

    float Panel::GetTitleFontSize(const Theme& theme) const {
        if (m_options.titleFontSize >= 0.0f)
            return m_options.titleFontSize;

        return theme.panel.titleFontSize;
    }
}