#pragma once

#include <TinyUI/Context/UIContext.h>
#include <TinyUI/Core/Types.h>
#include <TinyUI/Core/WidgetId.h>
#include <TinyUI/Input/MouseButton.h>
#include <TinyUI/Theme/Theme.h>

#include <string_view>

namespace tinyui {
    struct TabBarState {
        int pressedIndex = -1;
    };

    struct TabBarStyle {
        Color backgroundColor { };

        Color tabNormalColor { };
        Color tabHoveredColor { };
        Color tabPressedColor { };
        Color tabSelectedColor { };

        Color borderColor { };
        Color accentColor { };

        Color textColor { };
        Color selectedTextColor { };

        float radius = 8.0f;
        float borderThickness = 1.0f;

        float gap = 4.0f;
        float paddingX = 16.0f;

        float minTabWidth = 80.0f;
        float maxTabWidth = 160.0f;

        float fontSize = 14.0f;

        float activeIndicatorHeight = 3.0f;

        static TabBarStyle FromTheme(const Theme& theme);
    };

    struct TabBarOptions {
        MouseButtonMask acceptedButtons = MouseButtonBit(MouseButton::Left);

        bool stretchTabs = false;

        bool keyboardNavigation = true;
        bool wrapNavigation = true;
    };

    struct TabBarResult {
        bool hovered = false;
        bool changed = false;

        int hoveredIndex = -1;
        int selectedIndex = -1;
    };

    TabBarResult TabBar(UIContext& context, WidgetId id, Rect rect, TabBarState& state, const std::wstring_view* tabs, int tabCount, int& selectedIndex, TabBarOptions options = { });
    TabBarResult TabBar(UIContext& context, WidgetId id, Rect rect, TabBarState& state, const std::wstring_view* tabs, int tabCount, int& selectedIndex, TabBarOptions options, TabBarStyle style);
}