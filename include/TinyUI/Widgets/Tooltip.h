#pragma once

#include <TinyUI/Context/UIContext.h>
#include <TinyUI/Core/Types.h>
#include <TinyUI/Core/WidgetId.h>
#include <TinyUI/Theme/Theme.h>

#include <string_view>

namespace tinyui {
	struct TooltipStyle {
		Color backgroundColor { };
		Color borderColor { };
		Color textColor { };

		float radius = 8.0f;
		float borderThickness = 1.0f;

		float paddingX = 10.0f;
		float paddingY = 6.0f;

		float fontSize = 13.0f;
		float offsetY = 8.0f;

		static TooltipStyle FromTheme(const Theme& theme);
	};

	struct TooltipOptions {
		double delaySeconds = 0.45;
	};

	struct TooltipResult {
		bool visible = false;
	};

	TooltipResult Tooltip(UIContext& context, WidgetId targetId, Rect targetRect, std::wstring_view text, TooltipOptions options = { });
	TooltipResult Tooltip(UIContext& context, WidgetId targetId, Rect targetRect, std::wstring_view text, TooltipOptions options, TooltipStyle style);
}