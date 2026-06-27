#include "pch.h"

#include <TinyUI/Widgets/Label.h>

#include <utility>

namespace tinyui {
    Label::Label(std::wstring text) : m_text(std::move(text)) {
        GetLayoutStyle().preferredSize = { 120.0f, 24.0f };
        GetLayoutStyle().verticalAlignment = LayoutAlignment::Stretch;
    }

    void Label::SetText(std::wstring_view text) {
        m_text = text;
    }

    const std::wstring& Label::GetText() const {
        return m_text;
    }

    void Label::SetOptions(const LabelOptions& options) {
        m_options = options;
    }

    const LabelOptions& Label::GetOptions() const {
        return m_options;
    }

    void Label::OnPaint(PaintContext& context) {
        tinycore::Rect rect = GetRect();

        context.renderer.DrawTextBox(m_text, rect, GetTextColor(context.theme), GetFontSize(context.theme), m_options.align, m_options.wrap);
    }

    tinycore::Color Label::GetTextColor(const Theme& theme) const {
        if (m_options.variant == LabelVariant::Muted)
            return theme.label.muted;

        if (m_options.variant == LabelVariant::Heading)
            return theme.label.heading;

        return theme.label.text;
    }

    float Label::GetFontSize(const Theme& theme) const {
        if (m_options.fontSize >= 0.0f)
            return m_options.fontSize;

        if (m_options.variant == LabelVariant::Muted)
            return theme.label.mutedFontSize;

        if (m_options.variant == LabelVariant::Heading)
            return theme.label.headingFontSize;

        return theme.label.fontSize;
    }
}