#pragma once

#include <TinyUI/Context/UIContext.h>
#include <TinyUI/Core/Types.h>
#include <TinyUI/Core/WidgetId.h>
#include <TinyUI/Theme/Theme.h>

#include <string>
#include <string_view>
#include <cstddef>

namespace tinyui {
    struct TextBoxState {
        float scrollX = 0.f;

        std::size_t cursorIndex = 0;
    };

	struct TextBoxStyle {
        Color backgroundColor { };
        Color hoveredBackgroundColor { };
        Color focusedBackgroundColor { };

        Color borderColor { };
        Color focusedBorderColor { };

        Color textColor { };
        Color placeholderColor { };
        Color cursorColor { };
        Color selectionColor { };

        float radius = 8.0f;
        float borderThickness = 1.0f;

        float paddingX = 10.0f;
        float fontSize = 14.0f;

        static TextBoxStyle FromTheme(const Theme& theme);
	};

    struct TextBoxOptions {
        std::wstring_view placeholder = L"";

        bool submitOnEnter = true;
        bool clearFocusOnEnter = false;
        bool clearFocusOnEscape = true;
    };

    struct TextBoxResult {
        bool hovered = false;
        bool focused = false;
        bool changed = false;
        bool submitted = false;
    };

    TextBoxResult TextBox(UIContext& context, WidgetId id, Rect rect, std::wstring& value, TextBoxOptions options = { });
    TextBoxResult TextBox(UIContext& context, WidgetId id, TextBoxState& state, Rect rect, std::wstring& value, TextBoxOptions options, TextBoxStyle style);
    TextBoxResult TextBox(UIContext& context, TextBoxState state, Rect rect, std::wstring& value, TextBoxOptions options = { });
    TextBoxResult TextBox(UIContext& context, WidgetId id, TextBoxState& state, Rect rect, std::wstring& value, TextBoxOptions options = { });
    TextBoxResult TextBox(UIContext& context, Rect rect, std::wstring& value, TextBoxOptions options = { });
}