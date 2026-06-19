#pragma once

#include <TinyUI/Core/Types.h>
#include <TinyUI/Input/InputState.h>
#include <TinyUI/Rendering/Renderer.h>
#include <TinyUI/Theme/Theme.h>
#include <TinyUI/Context/UIContext.h>

#include <string_view>

namespace tinyui {
	struct PanelStyle {
		Color backgroundColor = Color::FromRGB(24, 28, 36);
		Color borderColor = Color::FromRGB(58, 65, 82);

		Color titleTextColor = Color::FromRGB(230, 232, 240);

		float radius = 14.f;
		float borderThickness = 1.f;

		float padding = 16.f;

		float titleHeight = 36.f;
		float titleFontSize = 14.f;

		static PanelStyle FromTheme(const Theme& theme);
	};

	struct PanelResult {
		Rect rect { };
		Rect contentRect { };

		bool hovered = false;
	};

	PanelResult Panel(Renderer& renderer, const InputState& input, Rect rect, std::wstring_view title, PanelStyle style = { });
	PanelResult Panel(Renderer& renderer, const InputState& input, Rect rect, std::wstring_view title, const Theme& theme);
	PanelResult Panel(UIContext& context, WidgetId id, Rect rect, std::wstring_view title);
	PanelResult Panel(UIContext& context, Rect rect, std::wstring_view title);
}