#include <TinyUI/Widgets/Checkbox.h>

namespace tinyui {
	CheckboxStyle CheckboxStyle::FromTheme(const Theme& theme) {
		CheckboxStyle style { };
        style.boxBackgroundColor = theme.checkbox.boxBackground;
        style.boxHoveredColor = theme.checkbox.boxHovered;
        style.boxCheckedColor = theme.checkbox.boxChecked;

        style.borderColor = theme.checkbox.border;
        style.checkmarkColor = theme.checkbox.checkmark;

        style.textColor = theme.checkbox.text;

        return style;
	}

    CheckboxResult Checkbox(UIContext& context, Rect rect, std::wstring_view label, bool& checked, CheckboxOptions options) {
        return Checkbox(context, MakeWidgetId(label), rect, label, checked, options);
    }

    CheckboxResult Checkbox(UIContext& context, WidgetId id, Rect rect, std::wstring_view label, bool& checked, CheckboxOptions options) {
        return Checkbox(context, id, rect, label, checked, options, CheckboxStyle::FromTheme(context.GetTheme()));
    }

    CheckboxResult Checkbox(UIContext& context, WidgetId id, Rect rect, std::wstring_view label, bool& checked, CheckboxOptions options, CheckboxStyle style) {
        Renderer& renderer = context.GetRenderer();
        const InputState& input = context.GetInput();

        CheckboxResult result { };
        result.hovered = context.IsMouseOver(rect);
        result.checked = checked;
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
                    checked = !checked;
                    result.checked = checked;
                    result.changed = true;
                }

                context.ClearActiveId();
            }
        }

        Rect boxRect { rect.x, rect.y + (rect.h - style.boxSize) * 0.5f, style.boxSize, style.boxSize };
        Rect labelRect { rect.x + style.boxSize + style.gap, rect.y, rect.w - style.boxSize - style.gap, rect.h };
        Color boxColor = style.boxBackgroundColor;
        if (checked)
            boxColor = style.boxCheckedColor;
        else if (result.hovered)
            boxColor = style.boxHoveredColor;

        renderer.FillRect(boxRect, boxColor, style.radius);
        renderer.DrawRect(boxRect, style.borderColor, style.borderThickness, style.radius);
        if (checked) {
            Vec2 checkStart { boxRect.x + boxRect.w * 0.25f, boxRect.y + boxRect.h * 0.52f };
            Vec2 checkMiddle { boxRect.x + boxRect.w * 0.43f, boxRect.y + boxRect.h * 0.7f };
            Vec2 checkEnd { boxRect.x + boxRect.w * 0.76f, boxRect.y + boxRect.h * 0.3f };
            renderer.DrawLine(checkStart, checkMiddle, style.checkmarkColor, style.checkmarkThickness);
            renderer.DrawLine(checkMiddle, checkEnd, style.checkmarkColor, style.checkmarkThickness);
        }

        renderer.DrawTextBox(label, labelRect, style.textColor, style.fontSize, TextAlign::Left, TextWrap::NoWrap);
        return result;
    }
}