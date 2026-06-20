#pragma once

#include <TinyUI/Context/UIContext.h>
#include <TinyUI/Core/Types.h>
#include <TinyUI/Core/WidgetId.h>
#include <TinyUI/Input/MouseButton.h>
#include <TinyUI/Theme/Theme.h>

#include <string_view>

namespace tinyui {
	struct ToggleStyle {
        Color trackOffColor {};
        Color trackOffHoveredColor {};

        Color trackOnColor {};
        Color trackOnHoveredColor {};

        Color knobColor {};
        Color borderColor {};

        Color textColor {};

        float trackWidth = 42.0f;
        float trackHeight = 22.0f;

        float knobRadius = 8.0f;

        float gap = 10.0f;
        float fontSize = 14.0f;

        float borderThickness = 1.0f;

        static ToggleStyle FromTheme(const Theme& theme);
	};

    struct ToggleOptions {
        MouseButtonMask acceptedButtons = MouseButtonBit(MouseButton::Left);
    };

    struct ToggleResult {
        bool hovered = false;
        bool down = false;
        bool changed = false;

        bool enabled = false;
    };

    ToggleResult Toggle(UIContext& context, WidgetId id, Rect rect, std::wstring_view label, bool& enabled, ToggleOptions options = { });
    ToggleResult Toggle(UIContext& context, WidgetId id, Rect rect, std::wstring_view label, bool& enabled, ToggleOptions options, ToggleStyle style);
    ToggleResult Toggle(UIContext& context, Rect rect, std::wstring_view label, bool& enabled, ToggleOptions options = { });
}