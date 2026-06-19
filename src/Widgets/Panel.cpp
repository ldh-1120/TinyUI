#include <TinyUI/Widgets/Panel.h>

namespace tinyui {
	static float MaxFloat(float a, float b) {
		return a > b ? a : b;
	}

	PanelResult Panel(Renderer& renderer, const InputState& input, Rect rect, std::wstring_view title, const Theme& theme) {
		return Panel(renderer, input, rect, title, PanelStyle::FromTheme(theme));
	}

	PanelResult Panel(Renderer& renderer, const InputState& input, Rect rect, std::wstring_view title, PanelStyle style) {
		PanelResult result { };
		result.rect = rect;
		result.hovered = input.IsMouseOver(rect);

		Color borderColor = result.hovered ? Color::FromRGB(80, 90, 115) : style.borderColor;
		renderer.FillRect(rect, style.backgroundColor, style.radius);
		renderer.DrawRect(rect, borderColor, style.borderThickness, style.radius);

		float contentY = rect.y + style.padding;
		if (!title.empty()) {
			Rect titleRect { rect.x + style.padding, rect.y, rect.w - style.padding * 2.f, style.titleHeight };
			renderer.DrawTextBox(title, titleRect, style.titleTextColor, style.titleFontSize, TextAlign::Left);
			
			contentY = rect.y + style.titleHeight;
		}

		result.contentRect = { rect.x + style.padding, contentY, MaxFloat(0.f, rect.w - style.padding * 2.f), MaxFloat(0.f, rect.y + rect.h - contentY - style.padding) };
		return result;
	}

	PanelStyle PanelStyle::FromTheme(const Theme& theme) {
		PanelStyle style { };
		style.backgroundColor = theme.panel.background;
		style.borderColor = theme.panel.border;
		style.titleTextColor = theme.panel.titleText;

		return style;
	}
}