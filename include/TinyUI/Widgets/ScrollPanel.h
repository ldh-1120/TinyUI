#pragma once

#include <TinyUI/Context/UIContext.h>
#include <TinyUI/Core/Types.h>
#include <TinyUI/Core/WidgetId.h>
#include <TinyUI/Theme/Theme.h>

namespace tinyui {
	struct ScrollPanelState {
		float scrollY = 0.f;

        bool draggingThumb = false;

        float dragStartMouseY = 0.f;
        float dragStartScrollY = 0.f;
	};

	struct ScrollPanelStyle {
        Color backgroundColor { };
        Color borderColor { };

        Color scrollbarTrackColor { };
        Color scrollbarThumbColor { };
        Color scrollbarThumbHoveredColor { };

        float radius = 10.0f;
        float borderThickness = 1.0f;

        float padding = 10.0f;

        float scrollbarWidth = 8.0f;
        float scrollbarGap = 6.0f;
        float scrollbarRadius = 4.0f;

        static ScrollPanelStyle FromTheme(const Theme& theme);
	};

    struct ScrollPanelOptions {
        float contentHeight = 0.f;
        float wheelSpeed = 42.f;

        bool showBackground = true;
        bool showScrollbar = true;
    };
    
    struct ScrollPanelResult {
        Rect rect { };
        Rect visibleRect { };
        Rect contentRect { };

        bool hovered = false;
        bool changed = false;

        bool scrollbarHovered = false;
        bool scrollbarDragging = false;

        float scrollY = 0.f;
        float maxScrollY = 0.f;
    };

    ScrollPanelResult BeginScrollPanel(UIContext& context, WidgetId id, Rect rect, ScrollPanelState& state, ScrollPanelOptions options = { });
    ScrollPanelResult BeginScrollPanel(UIContext& context, WidgetId id, Rect rect, ScrollPanelState& state, ScrollPanelOptions options, ScrollPanelStyle style);

    void EndScrollPanel(UIContext& context);
}