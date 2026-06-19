#pragma once

#include <TinyUI/Core/Types.h>
#include <TinyUI/Input/InputState.h>
#include <TinyUI/Input/MouseButton.h>
#include <TinyUI/Rendering/Renderer.h>
#include <TinyUI/Theme/Theme.h>

#include <string_view>

namespace tinyui {
	struct SliderStyle {
		Color labelTextColor = Color::FromRGB(210, 214, 225);
		Color valueTextColor = Color::FromRGB(150, 156, 170);

		Color trackColor = Color::FromRGB(150, 156, 170);
		Color fillColor = Color::FromRGB(120, 86, 255);
		Color handleColor = Color::FromRGB(235, 237, 245);

		Color borderColor = Color::FromRGB(70, 78, 96);

		float fontSize = 13.f;

		float labelWidth = 72.f;
		float valueWidth = 56.f;

		float trackHeight = 6.f;
		float handleRadius = 7.f;

		static SliderStyle FromTheme(const Theme& theme);
	};

	struct SliderOptions {
		float minValue = 0.f;
		float maxValue = 1.f;

		bool showValue = true;

		MouseButtonMask acceptedButtons = MouseButtonBit(MouseButton::Left);
	};

	struct SliderState {
		bool dragging = false;
		MouseButton dragButton = MouseButton::Left;
	};

	struct SliderResult {
		bool hovered = false;
		bool dragging = false;
		bool changed = false;

		float value = 0.f;
	};

	SliderResult Slider(Renderer& renderer, const InputState& input, SliderState& state, Rect rect, std::wstring_view label, float& value, const Theme& theme, SliderOptions options = { });
	SliderResult Slider(Renderer& renderer, const InputState& input, SliderState& state, Rect rect, std::wstring_view label, float& value, SliderOptions options = { }, SliderStyle style = { });
}