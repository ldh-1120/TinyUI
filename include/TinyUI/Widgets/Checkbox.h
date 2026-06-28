#pragma once

#include <TinyUI/Events/KeyEvent.h>
#include <TinyUI/Events/MouseEvent.h>
#include <TinyUI/Widgets/Widget.h>

namespace tinyui {
    struct CheckboxOptions {
        float boxSize = -1.0f;
        float cornerRadius = -1.0f;
    };

    class Checkbox final : public Widget {
    public:
        Checkbox();

        void SetChecked(bool checked);
        bool IsChecked() const;

        bool HasPendingChange() const;
        bool TakeChanged();

        void SetOptions(const CheckboxOptions& options);
        const CheckboxOptions& GetOptions() const;

        bool IsMouseInteractive() const override;
        bool IsFocusable() const override;

    protected:
        void OnPaint(PaintContext& context) override;
        bool OnUpdate(float deltaTime) override;

        void OnMouseDown(MouseEvent& event) override;
        void OnMouseUp(MouseEvent& event) override;

        void OnKeyDown(KeyEvent& event) override;
        void OnKeyUp(KeyEvent& event) override;

    private:
        void SetCheckedInternal(bool checked, bool notifyChanged);
        void ToggleUserChecked();

        float GetBoxSize(const Theme& theme) const;
        float GetCornerRadius(const Theme& theme) const;

        tinycore::Color GetAnimatedBackground(const Theme& theme) const;
        tinycore::Color GetAnimatedBorder(const Theme& theme) const;

        void DrawCheckMark(PaintContext& context, tinycore::Rect boxRect, const Theme& theme);

    private:
        bool m_checked = false;
        bool m_changed = false;
        bool m_pressed = false;

        float m_hoverT = 0.0f;
        float m_pressedT = 0.0f;

        CheckboxOptions m_options { };
    };
}