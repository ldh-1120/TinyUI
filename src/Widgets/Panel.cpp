#include "pch.h"

#include <TinyUI/Widgets/Panel.h>

#include <utility>

namespace tinyui {
    static void DrawChevron(Renderer& renderer, tinycore::Vec2 center, bool expanded, tinycore::Color color) {
        const float size = 4.0f;
        const float thickness = 1.5f;
        if (expanded) {
            renderer.DrawLine({ center.x - size, center.y - 1.0f }, { center.x, center.y + size }, color, thickness);
            renderer.DrawLine({ center.x, center.y + size }, { center.x + size, center.y - 1.0f }, color, thickness);

            return;
        }

        renderer.DrawLine({ center.x - 1.0f, center.y - size }, { center.x + size, center.y }, color, thickness);
        renderer.DrawLine({ center.x + size, center.y }, { center.x - 1.0f, center.y + size }, color, thickness);
    }

    Panel::Panel(std::wstring title) : m_title(std::move(title)) { }

    void Panel::SetTitle(std::wstring_view title) {
        m_title = title;
    }

    const std::wstring& Panel::GetTitle() const {
        return m_title;
    }

    void Panel::SetOptions(const PanelOptions& options) {
        m_options = options;

        m_collapsible = options.collapsible;
        if (!m_hasInitializedExpanded) {
            m_expanded = options.defaultExpanded;
            m_hasInitializedExpanded = true;
        }
    }

    const PanelOptions& Panel::GetOptions() const {
        return m_options;
    }

    Rect Panel::GetContentRect() const {
        Rect rect = GetRect();

        float titleHeight = 0.f;
        if (m_options.showTitle) {
            titleHeight = 36.f;
            if (m_options.titleHeight >= 0.f)
                titleHeight = m_options.titleHeight;
        }

        float contentHeight = rect.h - titleHeight;
        if (contentHeight < 0.0f)
            contentHeight = 0.0f;
        
        return { rect.x, rect.y + titleHeight, rect.w, contentHeight };
    }

    bool Panel::IsCollapsible() const {
        return m_collapsible;
    }

    bool Panel::IsExpanded() const {
        return m_expanded;
    }

    void Panel::SetExpanded(bool expanded) {
        m_expanded = expanded;
    }

    void Panel::ToggleExpanded() {
        m_expanded = !m_expanded;
    }

    bool Panel::IsMouseInteractive() const {
        return m_collapsible;
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

        float titleTextX = rect.x + 12.f;
        if (m_collapsible) {
            tinycore::Vec2 chevronCenter { rect.x + 14.f, rect.y + titleHeight * 0.5f };
            DrawChevron(context.renderer, chevronCenter, m_expanded, theme.panel.titleText);
            titleTextX += 18.f;
        }

        Rect titleRect { titleTextX, rect.y, rect.w - titleTextX + rect.x - 12.f, titleHeight };
        renderer.DrawTextBox(m_title, titleRect, theme.panel.titleText, titleFontSize, TextAlign::Left, TextWrap::NoWrap);
    }

    void Panel::OnMouseDown(MouseEvent& event) {
        if (!m_collapsible || event.button != tinycore::MouseButton::Left || !IsInTitleArea(event.position))
            return;

        ToggleExpanded();

        event.Accept();
    }

    bool Panel::ShouldPaintChildren() const {
        if (!m_collapsible)
            return true;

        return m_expanded;
    }

    bool Panel::ShouldArrangeChildren() const {
        if (!m_collapsible)
            return true;

        return m_expanded;
    }

    bool Panel::ShouldHitTestChildren() const {
        if (!m_collapsible)
            return true;

        return m_expanded;
    }

    bool Panel::IsInTitleArea(tinycore::Vec2 position) const {
        if (!m_options.showTitle)
            return false;

        tinycore::Rect rect = GetRect();

        float titleHeight = 36.0f;
        if (m_options.titleHeight >= 0.0f)
            titleHeight = m_options.titleHeight;

        return position.x >= rect.x && position.x <= rect.x + rect.w && position.y >= rect.y && position.y <= rect.y + titleHeight;
    }

    tinycore::Size Panel::MeasureOverride(tinycore::Size availableSize) {
        float titleHeight = 0.0f;
        if (m_options.showTitle) {
            titleHeight = 36.0f;
            if (m_options.titleHeight >= 0.0f)
                titleHeight = m_options.titleHeight;
        }

        if (m_collapsible && !m_expanded)
            return { 0.0f, titleHeight };

        tinycore::Size contentAvailableSize { availableSize.width, availableSize.height - titleHeight };
        if (contentAvailableSize.height < 0.0f)
            contentAvailableSize.height = 0.0f;

        tinycore::Size contentDesiredSize { };

        Layout* layout = GetLayout();
        if (layout)
            contentDesiredSize = layout->Measure(*this, contentAvailableSize);

        return { contentDesiredSize.width, contentDesiredSize.height + titleHeight };
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