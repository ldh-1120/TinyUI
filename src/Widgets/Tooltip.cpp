#include <TinyUI/Widgets/Tooltip.h>

#include <string>

namespace tinyui {
    TooltipStyle TooltipStyle::FromTheme(const Theme& theme) {
        TooltipStyle style { };

        style.backgroundColor = theme.tooltip.background;
        style.borderColor = theme.tooltip.border;
        style.textColor = theme.tooltip.text;

        return style;
    }

    TooltipResult Tooltip(UIContext& context, WidgetId targetId, Rect targetRect, std::wstring_view text, TooltipOptions options) {
        return Tooltip(context, targetId, targetRect, text, options, TooltipStyle::FromTheme(context.GetTheme()));
    }

    TooltipResult Tooltip(UIContext& context, WidgetId targetId, Rect targetRect, std::wstring_view text, TooltipOptions options, TooltipStyle style) {
        Renderer& renderer = context.GetRenderer();

        TooltipResult result { };

        bool hovered = context.IsHovered(targetId);
        bool shouldShow = context.UpdateTooltip(targetId, hovered, options.delaySeconds);
        if (!shouldShow)
            return result;

        Size textSize = renderer.MeasureText(text, style.fontSize);
        Rect tooltipRect { targetRect.x, targetRect.y - textSize.height - style.paddingY * 2.f - style.offsetY, textSize.width + style.paddingX * 2.f, textSize.height + style.paddingY * 2.f };
        if (tooltipRect.y < 0.f)
            tooltipRect.y = targetRect.y + targetRect.h + style.offsetY;

        std::wstring tooltipText(text);
        context.AddOverlay([tooltipRect, tooltipText, style](Renderer& overlayRenderer) {
            overlayRenderer.FillRect(tooltipRect, style.backgroundColor, style.radius);
            overlayRenderer.DrawRect(tooltipRect, style.borderColor, style.borderThickness, style.radius);
            Rect textRect { tooltipRect.x + style.paddingX, tooltipRect.y + style.paddingY, tooltipRect.w - style.paddingX * 2.f, tooltipRect.h - style.paddingY * 2.f };

            std::wstring_view displayText(tooltipText.data(), tooltipText.size());
            overlayRenderer.DrawTextBox(displayText, textRect, style.textColor, style.fontSize, TextAlign::Center);
        });

        result.visible = true;
        return result;
    }
}