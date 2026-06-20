#pragma once

#include <TinyUI/Context/UIContext.h>
#include <TinyUI/Core/Types.h>
#include <TinyUI/Core/WidgetId.h>
#include <TinyUI/Input/MouseButton.h>
#include <TinyUI/Theme/Theme.h>

#include <string_view>

namespace tinyui {
	struct CheckboxStyle {
		Color boxBackgroundColor { };
		Color boxHoveredColor { };
		Color boxCheckedColor { };

		Color borderColor { };
		Color checkmarkColor { };

		Color textColor { };

		float boxSize = 18.f;
		float gap = 8.f;

		float radius = 5.f;
		float borderThickness = 1.f;
		float checkmarkThickness = 2.f;

		float fontSize = 14.f;

		static CheckboxStyle FromTheme(const Theme& theme);
	};

	struct CheckboxOptions {
		MouseButtonMask acceptedButtons = MouseButtonBit(MouseButton::Left);
	};

	struct CheckboxResult {
		bool hovered = false;
		bool down = false;
		bool changed = false;

		bool checked = false;
	};

	CheckboxResult Checkbox(UIContext& context, WidgetId id, Rect rect, std::wstring_view label, bool& checked, CheckboxOptions options = { });
	CheckboxResult Checkbox(UIContext& context, WidgetId id, Rect rect, std::wstring_view label, bool& checked, CheckboxOptions options, CheckboxStyle style);
	CheckboxResult Checkbox(UIContext& context, Rect rect, std::wstring_view label, bool& checked, CheckboxOptions options = { });
}