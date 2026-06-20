#pragma once

#include <TinyUI/Core/Types.h>
#include <TinyUI/Input/InputState.h>
#include <TinyUI/Input/MouseButton.h>
#include <TinyUI/Rendering/Renderer.h>
#include <TinyUI/Theme/Theme.h>
#include <TinyUI/Context/UIContext.h>

#include <string_view>

namespace tinyui {
	struct ButtonStyle {
		Color normalColor = Color::FromRGB(38, 43, 54);
		Color hoveredColor = Color::FromRGB(48, 55, 70);
		Color pressedColor = Color::FromRGB(82, 64, 160);

		Color borderColor = Color::FromRGB(78, 86, 106);
		Color textColor = Color::FromRGB(230, 232, 240);

		float radius = 10.f;
		float borderThickness = 1.f;
		float fontSize = 14.f;

		static ButtonStyle FromTheme(const Theme& theme);
	};

	struct ButtonOptions {
		MouseButtonMask acceptedButtons = MouseButtonBit(MouseButton::Left);
	};

	struct ButtonResult {
		bool hovered = false;
		bool down = false;
		bool clicked = false;

		MouseButton clickedButton = MouseButton::Left;
	};
	ButtonResult Button(UIContext& context, WidgetId id, Rect rect, std::wstring_view text, ButtonOptions options = { });
	ButtonResult Button(UIContext& context, Rect rect, std::wstring_view text, ButtonOptions options = { });
}