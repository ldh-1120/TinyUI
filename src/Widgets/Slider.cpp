#include <TinyUI/Widgets/Slider.h>

#include <cstdio>

namespace tinyui {
	static float Clamp(float value, float minValue, float maxValue) {
		if (value < minValue)
			return minValue;

		if (value > maxValue)
			return maxValue;

		return value;
	}

	static float Normalize(float value, float minValue, float maxValue) {
		float range = maxValue - minValue;

		if (range == 0.f)
			return 0.f;

		return Clamp((value - minValue) / range, 0.f, 1.f);
	}

	static float Lerp(float a, float b, float t) {
		return a + (b - a) * t;
	}

	SliderResult Slider(Renderer& renderer, const InputState& input, SliderState& state, Rect rect, std::wstring_view label, float& value, SliderOptions options, SliderStyle style) {
		SliderResult result { };
		result.hovered = input.IsMouseOver(rect);

		Rect labelRect { rect.x, rect.y, style.labelWidth, rect.h };
		Rect valueRect { rect.x + rect.w - style.valueWidth, rect.y,  style.valueWidth, rect.h };

		float trackX = rect.x + style.labelWidth;
		float trackW = rect.w - style.labelWidth;
		if (options.showValue)
			trackW -= style.valueWidth;

		trackW -= 12.f;

		Rect trackRect { trackX, rect.y + (rect.h - style.trackHeight) * 0.5f, trackW, style.trackHeight };
		for (MouseButton button : AllMouseButtons) {
			if (!HasMouseButton(options.acceptedButtons, button))
				continue;

			if (result.hovered && input.WasMousePressed(button)) {
				state.dragging = true;
				state.dragButton = button;
			}
		}

		if (state.dragging && trackRect.w > 0.f) {
			Vec2 mouse = input.GetMousePosition();

			float t = (mouse.x - trackRect.x) / trackRect.w;
			t = Clamp(t, 0.f, 1.f);

			float newValue = Lerp(options.minValue, options.maxValue, t);
			if (newValue != value) {
				value = newValue;
				result.changed = true;
			}
		}

		if (state.dragging && input.WasMouseReleased(state.dragButton))
			state.dragging = false;

		result.dragging = state.dragging;
		result.value = value;

		float normalizedValue = Normalize(value, options.minValue, options.maxValue);
		Color trackColor = result.hovered || result.dragging ? Color::FromRGB(52, 60, 78) : style.trackColor;
		renderer.DrawTextBox(label, labelRect, style.labelTextColor, style.fontSize, TextAlign::Left);
		renderer.FillRect(trackRect, trackColor, style.trackHeight * 0.5f);

		float fillWidth = trackRect.w * normalizedValue;
		if (fillWidth > 0.f)
			renderer.FillRect({ trackRect.x, trackRect.y, fillWidth, trackRect.h }, style.trackColor, style.trackHeight * 0.5f);

		float handleX = trackRect.x + fillWidth;
		float handleY = trackRect.y + trackRect.h * 0.5f;
		Rect handleRect { handleX - style.handleRadius, handleY - style.handleRadius, style.handleRadius * 2.f, style.handleRadius * 2.f };
		renderer.FillRect(handleRect, style.handleColor, style.handleRadius);
		renderer.DrawRect(handleRect, style.borderColor, 1.f, style.handleRadius);
		if (options.showValue) {
			wchar_t valueText[64] { };
			swprintf_s(valueText, L"%.2f", value);
			renderer.DrawTextBox(valueText, valueRect, style.valueTextColor, style.fontSize, TextAlign::Right);
		}

		return result;
	}

	SliderResult Slider(Renderer& renderer, const InputState& input, SliderState& state, Rect rect, std::wstring_view label, float& value, const Theme& theme, SliderOptions options) {
		return Slider(renderer, input, state, rect, label, value, options, SliderStyle::FromTheme(theme));
	}

	SliderResult Slider(UIContext& context, WidgetId id, SliderState& state, Rect rect, std::wstring_view label, float& value, SliderOptions options) {
		Renderer& renderer = context.GetRenderer();
		const InputState& input = context.GetInput();
		const Theme& theme = context.GetTheme();

		SliderStyle style = SliderStyle::FromTheme(theme);

		Rect labelRect { rect.x, rect.y, style.labelWidth, rect.h };
		Rect valueRect { rect.x + rect.w - style.valueWidth, rect.y,  style.valueWidth, rect.h };
		Rect trackArea { rect.x + style.labelWidth, rect.y, rect.w - style.labelWidth - style.valueWidth, rect.h };
		Rect trackRect { trackArea.x, rect.y + (rect.h - style.trackHeight) * 0.5f, trackArea.w, style.trackHeight };
		Rect interactionRect { trackRect.x - style.handleRadius, rect.y, trackRect.w + style.handleRadius * 2.f, rect.h };

		SliderResult result { };
		result.value = value;

		Vec2 mousePosition = input.GetMousePosition();
		result.hovered = interactionRect.Contains(mousePosition);
		if (result.hovered)
			context.SetHoveredId(id);

		for (MouseButton button : AllMouseButtons) {
			if (!HasMouseButton(options.acceptedButtons, button) || !context.CanActive(id))
				continue;

			if (result.hovered && input.WasMousePressed(button)) {
				context.SetActiveId(id);
				state.dragButton = button;
			}
		}

		if (context.IsActive(id)) {
			MouseButton activeButton = context.GetActiveMouseButton();
			if (input.IsMouseDown(activeButton)) {
				result.dragging = true;
				state.dragging = true;
				state.dragButton = activeButton;

				float normalized = (mousePosition.x - trackRect.x) / trackRect.w;
				normalized = Clamp(normalized, 0.f, 1.f);

				float newValue = Lerp(options.minValue, options.maxValue, normalized);
				if (newValue != value) {
					value = newValue;
					result.changed = true;
				}

				result.value = value;
			} else {
				state.dragging = false;
				context.ClearActiveId();
			}
		} else
			state.dragging = false;

		float normalizedValue = Normalize(value, options.minValue, options.maxValue);
		normalizedValue = Clamp(normalizedValue, 0.f, 1.f);

		float fillWidth = trackRect.w * normalizedValue;
		Rect fillRect { trackRect.x, trackRect.y, fillWidth, trackRect.h };

		float handleX = trackRect.x + fillWidth;
		Rect handleRect { handleX - style.handleRadius, rect.y + rect.h * 0.5f - style.handleRadius, style.handleRadius * 2.f, style.handleRadius * 2.f };

		Color currentTrackColor = style.trackColor;
		if (result.hovered || result.dragging)
			currentTrackColor = style.trackHoveredColor;

		renderer.DrawTextBox(label, labelRect, style.labelTextColor, style.fontSize, TextAlign::Left);
		renderer.FillRect(trackRect, currentTrackColor, style.trackHeight * 0.5f);
		renderer.FillRect(fillRect, style.fillColor, style.trackHeight * 0.5f);
		renderer.FillRect(handleRect, style.handleColor, style.handleRadius);
		renderer.DrawRect(handleRect, style.borderColor, 1.f, style.handleRadius);
		if (options.showValue) {
			wchar_t valueText[64] { };
			swprintf_s(valueText, L"%.2f", value);
			renderer.DrawTextBox(valueText, valueRect, style.valueTextColor, style.fontSize, TextAlign::Right);
		}

		return result;
	}

	SliderResult Slider(UIContext& context, SliderState& state, Rect rect, std::wstring_view label, float& value, SliderOptions options) {
		return Slider(context, MakeWidgetId(label), state, rect, label, value, options);
	}

	SliderStyle SliderStyle::FromTheme(const Theme& theme) {
		SliderStyle style {};
		style.labelTextColor = theme.slider.labelText;
		style.valueTextColor = theme.slider.valueText;

		style.trackColor = theme.slider.track;
		style.trackHoveredColor = theme.slider.trackHovered;

		style.fillColor = theme.slider.fill;
		style.handleColor = theme.slider.handle;

		style.borderColor = theme.slider.border;

		return style;
	}
}