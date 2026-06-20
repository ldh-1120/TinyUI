#include <TinyUI/Widgets/Panel.h>

namespace tinyui {
	static float MaxFloat(float a, float b) {
		return a > b ? a : b;
	}

	PanelResult Panel(UIContext& context, WidgetId id, Rect rect, std::wstring_view title) {
		PanelResult result = Panel(context, id, rect, title, PanelStyle::FromTheme(context.GetTheme()));
		if (result.hovered)
			context.SetHoveredId(id);

		return result;
	}

	PanelResult Panel(UIContext& context, WidgetId id, Rect rect, std::wstring_view title, PanelStyle style) {
		Renderer& renderer = context.GetRenderer();
		const InputState& input = context.GetInput();

		PanelResult result { };
		result.rect = rect;
		result.hovered = context.IsMouseOver(rect);

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

	PanelResult Panel(UIContext& context, Rect rect, std::wstring_view title) {
		return Panel(context, MakeWidgetId(title), rect, title);
	}

	PanelStyle PanelStyle::FromTheme(const Theme& theme) {
		PanelStyle style { };
		style.backgroundColor = theme.panel.background;
		style.borderColor = theme.panel.border;
		style.titleTextColor = theme.panel.titleText;

		return style;
	}
}