#include <TinyUI/Widgets/Button.h>

namespace tinyui {
	ButtonResult Button(Renderer& renderer, const InputState& input, Rect rect, std::wstring_view text, ButtonOptions options, ButtonStyle style) {
		ButtonResult result { };
		result.hovered = input.IsMouseOver(rect);
		for (MouseButton button : AllMouseButtons) {
			if (!HasMouseButton(options.acceptedButtons, button))
				continue;

			if (result.hovered && input.IsMouseDown(button))
				result.down = true;

			if (result.hovered && input.WasMousePressed(button)) {
				result.clicked = true;
				result.clickedButton = button;
			}
		}

		Color fillColor = style.normalColor;
		if (result.down)
			fillColor = style.pressedColor;
		else if (result.hovered)
			fillColor = style.hoveredColor;

		renderer.FillRect(rect, fillColor, style.radius);
		renderer.DrawRect(rect, style.borderColor, style.borderThickness, style.radius);
		renderer.DrawTextBox(text, rect, style.textColor, style.fontSize, TextAlign::Center);

		return result;
	}

	ButtonResult Button(Renderer& renderer, const InputState& input, Rect rect, std::wstring_view text, const Theme& theme, ButtonOptions options) {
		return Button(renderer, input, rect, text, options, ButtonStyle::FromTheme(theme));
	}

	ButtonResult Button(UIContext& context, WidgetId id, Rect rect, std::wstring_view text, ButtonOptions options) {
		Renderer& renderer = context.GetRenderer();
		const InputState& input = context.GetInput();
		const Theme& theme = context.GetTheme();

		ButtonStyle style = ButtonStyle::FromTheme(theme);
		ButtonResult result { };
		result.hovered = input.IsMouseOver(rect);
		if (result.hovered)
			context.SetHoveredId(id);

		for (MouseButton button : AllMouseButtons) {
			if (!HasMouseButton(options.acceptedButtons, button) || !context.CanActive(id))
				continue;

			if (result.hovered && input.WasMousePressed(button)) {
				context.SetActiveId(id, button);
				result.clickedButton = button;
			}
		}

		if (context.IsActive(id)) {
			MouseButton activeButton = context.GetActiveMouseButton();

			result.clickedButton = activeButton;
			result.down = input.IsMouseDown(activeButton);
			if (input.WasMouseReleased(activeButton)) {
				result.clicked = result.hovered;
				context.ClearActiveId();
			}
		}

		Color backgroundColor = style.normalColor;
		if (result.down && result.hovered)
			backgroundColor = style.pressedColor;
		else if (result.hovered)
			backgroundColor = style.hoveredColor;

		renderer.FillRect(rect, backgroundColor, style.radius);
		renderer.DrawRect(rect, style.borderColor, style.borderThickness, style.radius);
		renderer.DrawTextBox(text, rect, style.textColor, style.fontSize, TextAlign::Center);

		return result;
	}

	ButtonResult Button(UIContext& context, Rect rect, std::wstring_view text, ButtonOptions options) {
		return Button(context, MakeWidgetId(text), rect, text, options);
	}

	ButtonStyle ButtonStyle::FromTheme(const Theme& theme) {
		ButtonStyle style { };
		style.normalColor = theme.button.normal;
		style.hoveredColor = theme.button.hovered;
		style.pressedColor = theme.button.pressed;
		style.borderColor = theme.button.border;
		style.textColor = theme.button.text;

		return style;
	}
}