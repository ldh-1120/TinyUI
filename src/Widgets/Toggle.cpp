#include <TinyUI/Widgets/Toggle.h>

namespace tinyui {
    ToggleStyle ToggleStyle::FromTheme(const Theme& theme) {
        ToggleStyle style {};

        style.trackOffColor = theme.toggle.trackOff;
        style.trackOffHoveredColor = theme.toggle.trackOffHovered;

        style.trackOnColor = theme.toggle.trackOn;
        style.trackOnHoveredColor = theme.toggle.trackOnHovered;

        style.knobColor = theme.toggle.knob;
        style.borderColor = theme.toggle.border;

        style.textColor = theme.toggle.text;

        return style;
    }

    ToggleResult Toggle(UIContext& context, Rect rect, std::wstring_view label, bool& enabled, ToggleOptions options) {
        return Toggle(context, MakeWidgetId(label), rect, label, enabled, options);
    }

    ToggleResult Toggle(UIContext& context, WidgetId id, Rect rect, std::wstring_view label, bool& enabled, ToggleOptions options) {
        return Toggle(context, id, rect, label, enabled, options, ToggleStyle::FromTheme(context.GetTheme()));
    }

    ToggleResult Toggle(UIContext& context, WidgetId id, Rect rect, std::wstring_view label, bool& enabled, ToggleOptions options, ToggleStyle style) {
        Renderer& renderer = context.GetRenderer();
        const InputState& input = context.GetInput();

        ToggleResult result { };
        result.hovered = context.IsMouseOver(rect);
        result.enabled = enabled;
        if (result.hovered)
            context.SetHoveredId(id);

        for (MouseButton button : AllMouseButtons) {
            if (!HasMouseButton(options.acceptedButtons, button) || !context.CanActive(id))
                continue;

            if (result.hovered && input.WasMousePressed(button)) {
                context.SetActiveId(id, button);
                context.SetFocusedId(id);
                context.ClearTextInputId();
            }
        }

        if (context.IsActive(id)) {
            MouseButton activeButton = context.GetActiveMouseButton();

            result.down = input.IsMouseDown(activeButton);
            if (input.WasMouseReleased(activeButton)) {
                if (result.hovered) {
                    enabled = !enabled;
                    result.enabled = enabled;
                    result.changed = true;
                }

                context.ClearActiveId();
            }
        }

        Rect trackRect { rect.x, rect.y + (rect.h - style.trackHeight) * 0.5f, style.trackWidth, style.trackHeight };
        Rect labelRect { rect.x + style.trackWidth + style.gap, rect.y, rect.w - style.trackWidth - style.gap, rect.h };
        Color trackColor = style.trackOffColor;
        if (enabled) {
            if (result.hovered)
                trackColor = style.trackOnHoveredColor;
            else
                trackColor = style.trackOnColor;
        } else {
            if (result.hovered)
                trackColor = style.trackOffHoveredColor;
            else
                trackColor = style.trackOffColor;
        }

        renderer.FillRect(trackRect, trackColor, style.trackHeight * 0.5f);
        renderer.DrawRect(trackRect, style.borderColor, style.borderThickness, style.trackHeight * 0.5f);

        float leftKnobX = trackRect.x + style.trackHeight * 0.5f;
        float rightKnobX = trackRect.x + trackRect.w - style.trackHeight * 0.5f;
        float knobX = enabled ? rightKnobX : leftKnobX;

        Vec2 knobCenter { knobX, trackRect.y + trackRect.h * 0.5f };
        renderer.FillCircle(knobCenter, style.knobRadius, style.knobColor);
        renderer.DrawTextBox(label, labelRect, style.textColor, style.fontSize, TextAlign::Left, TextWrap::NoWrap);

        return result;
    }
}