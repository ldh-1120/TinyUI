#include "pch.h"

#include <TinyUI/Widgets/Checkbox.h>

#include <TinyUI/Rendering/PaintContext.h>
#include <TinyUI/Rendering/Renderer.h>
#include <TinyUI/Theme/Theme.h>

#include <TinyCore/Math/Numeric.h>

namespace tinyui {
    Checkbox::Checkbox() {
        GetLayoutStyle().preferredSize = { 24.0f, 24.0f };
        GetLayoutStyle().verticalAlignment = LayoutAlignment::Center;
    }

    void Checkbox::SetChecked(bool checked) {
        SetCheckedInternal(checked, false);
    }

    bool Checkbox::IsChecked() const {
        return m_checked;
    }

    bool Checkbox::HasPendingChange() const {
        return m_changed;
    }

    bool Checkbox::TakeChanged() {
        bool changed = m_changed;

        m_changed = false;

        return changed;
    }

    void Checkbox::SetOptions(const CheckboxOptions& options) {
        m_options = options;
    }

    const CheckboxOptions& Checkbox::GetOptions() const {
        return m_options;
    }

    bool Checkbox::IsMouseInteractive() const {
        return true;
    }

    bool Checkbox::IsFocusable() const {
        return true;
    }

    void Checkbox::SetCheckedInternal(bool checked, bool notifyChanged) {
        if (m_checked == checked)
            return;

        m_checked = checked;
        if (notifyChanged)
            m_changed = true;
    }

    void Checkbox::ToggleUserChecked() {
        SetCheckedInternal(!m_checked, true);
    }

    bool Checkbox::OnUpdate(float deltaTime) {
        float previousHoverT = m_hoverT;
        float previousPressedT = m_pressedT;

        float hoverTarget = IsHovered() ? 1.0f : 0.0f;
        float pressedTarget = m_pressed ? 1.0f : 0.0f;

        const float hoverSpeed = 12.0f;
        const float pressedSpeed = 18.0f;
        m_hoverT = tinycore::MoveTowards(m_hoverT, hoverTarget, hoverSpeed * deltaTime);
        m_pressedT = tinycore::MoveTowards(m_pressedT, pressedTarget, pressedSpeed * deltaTime);

        return previousHoverT != m_hoverT || previousPressedT != m_pressedT;
    }

    void Checkbox::OnPaint(PaintContext& context) {
        const Theme& theme = context.theme;

        tinycore::Rect rect = GetRect();

        float boxSize = GetBoxSize(theme);
        float cornerRadius = GetCornerRadius(theme);

        tinycore::Rect boxRect { rect.x + (rect.w - boxSize) * 0.5f, rect.y + (rect.h - boxSize) * 0.5f, boxSize,  boxSize };
        tinycore::Color background = GetAnimatedBackground(theme);
        tinycore::Color border = GetAnimatedBorder(theme);

        context.renderer.FillRect(boxRect, background, cornerRadius);
        if (theme.checkbox.borderThickness > 0.0f)
            context.renderer.DrawRect(boxRect, border, theme.checkbox.borderThickness, cornerRadius);

        if (m_checked)
            DrawCheckMark(context, boxRect, theme);
    }

    void Checkbox::OnMouseDown(MouseEvent& event) {
        if (event.button != tinycore::MouseButton::Left)
            return;

        m_pressed = true;
        event.Accept();
    }

    void Checkbox::OnMouseUp(MouseEvent& event) {
        if (event.button != tinycore::MouseButton::Left)
            return;

        if (!m_pressed)
            return;

        m_pressed = false;
        if (GetRect().Contains(event.position))
            ToggleUserChecked();

        event.Accept();
    }

    void Checkbox::OnKeyDown(KeyEvent& event) {
        if (event.key == tinycore::KeyCode::Enter) {
            ToggleUserChecked();

            event.Accept();
            return;
        }

        if (event.key == tinycore::KeyCode::Space) {
            m_pressed = true;

            event.Accept();
            return;
        }
    }

    void Checkbox::OnKeyUp(KeyEvent& event) {
        if (event.key != tinycore::KeyCode::Space)
            return;

        if (m_pressed)
            ToggleUserChecked();

        m_pressed = false;
        event.Accept();
    }

    float Checkbox::GetBoxSize(const Theme& theme) const {
        if (m_options.boxSize >= 0.0f)
            return m_options.boxSize;

        return theme.checkbox.boxSize;
    }

    float Checkbox::GetCornerRadius(const Theme& theme) const {
        if (m_options.cornerRadius >= 0.0f)
            return m_options.cornerRadius;

        return theme.checkbox.cornerRadius;
    }

    tinycore::Color Checkbox::GetAnimatedBackground(const Theme& theme) const {
        tinycore::Color baseColor = m_checked ? theme.checkbox.backgroundChecked : theme.checkbox.background;
        tinycore::Color hoverColor = m_checked ? theme.checkbox.backgroundChecked : theme.checkbox.backgroundHovered;
        tinycore::Color pressedColor = m_checked ? theme.checkbox.backgroundChecked : theme.checkbox.backgroundPressed;
        tinycore::Color color = tinycore::LerpColor(baseColor, hoverColor, m_hoverT);

        return tinycore::LerpColor(color, pressedColor, m_pressedT);
    }

    tinycore::Color Checkbox::GetAnimatedBorder(const Theme& theme) const {
        return tinycore::LerpColor(theme.checkbox.border, theme.checkbox.borderHovered, m_hoverT);
    }

    void Checkbox::DrawCheckMark(PaintContext& context, tinycore::Rect boxRect, const Theme& theme) {
        tinycore::Vec2 p0 { boxRect.x + boxRect.w * 0.28f, boxRect.y + boxRect.h * 0.52f };
        tinycore::Vec2 p1 { boxRect.x + boxRect.w * 0.43f, boxRect.y + boxRect.h * 0.68f };
        tinycore::Vec2 p2 { boxRect.x + boxRect.w * 0.74f,  boxRect.y + boxRect.h * 0.34f };
        context.renderer.DrawLine(p0, p1, theme.checkbox.check, theme.checkbox.checkThickness);
        context.renderer.DrawLine(p1, p2, theme.checkbox.check, theme.checkbox.checkThickness);
    }
}