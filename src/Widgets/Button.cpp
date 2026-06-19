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
		renderer.DrawRect(rect, fillColor, style.borderThickness, style.radius);
		renderer.DrawTextBox(text, rect, style.textColor, style.fontSize, TextAlign::Center);

		return result;
	}
}