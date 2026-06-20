#pragma once

#include <TinyUI/Context/UIContext.h>
#include <TinyUI/Core/Types.h>
#include <TinyUI/Core/WidgetId.h>
#include <TinyUI/Input/MouseButton.h>
#include <TinyUI/Theme/Theme.h>

#include <string_view>

namespace tinyui {
	struct DropdownState {
		bool open = false;

        int highlightedIndex = -1;
        int firstVisibleIndex = 0;
	};

    struct DropdownStyle {
        Color backgroundColor {};
        Color hoveredBackgroundColor {};
        Color openedBackgroundColor {};

        Color borderColor {};
        Color focusedBorderColor {};

        Color textColor {};
        Color placeholderColor {};
        Color arrowColor {};

        Color menuBackgroundColor {};
        Color itemHoveredColor {};
        Color itemSelectedColor {};

        float radius = 8.0f;
        float borderThickness = 1.0f;

        float paddingX = 10.0f;
        float fontSize = 14.0f;

        float menuGap = 4.0f;
        float itemHeight = 32.0f;

        static DropdownStyle FromTheme(const Theme& theme);
    };

    struct DropdownOptions {
        std::wstring_view placeholder = L"Select";

        int maxVisibleItems = 6;

        bool wrapNavigation = true;
        bool closeOnEscape = true;

        MouseButtonMask acceptedButtons = MouseButtonBit(MouseButton::Left);
    };

    struct DropdownResult {
        bool hovered = false;
        bool open = false;
        bool changed = false;

        int selectedIndex = -1;
    };

    DropdownResult Dropdown(UIContext& context, WidgetId id, Rect rect, DropdownState& state, const std::wstring_view* items, int itemCount, int& selectedIndex, DropdownOptions options = { });
    DropdownResult Dropdown(UIContext& context, WidgetId id, Rect rect, DropdownState& state, const std::wstring_view* items, int itemCount, int& selectedIndex, DropdownOptions options, DropdownStyle style);
}