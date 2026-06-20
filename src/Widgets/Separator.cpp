#include <TinyUI/Widgets/Separator.h>

namespace tinyui {
    SeparatorStyle SeparatorStyle::FromTheme(const Theme& theme) {
        SeparatorStyle style { };

        style.lineColor = theme.separator.line;
        style.textColor = theme.separator.text;

        return style;
    }

    SeparatorResult Separator(UIContext& context, Rect rect, SeparatorOptions options) {
        return Separator(context, rect, options, SeparatorStyle::FromTheme(context.GetTheme()));
    }

    SeparatorResult Separator(UIContext& context, Rect rect, SeparatorOptions options, SeparatorStyle style) {
        Renderer& renderer = context.GetRenderer();

        SeparatorResult result { };
        result.rect = rect;

        float centerY = rect.y + rect.h * 0.5f;
        if (options.label.empty()) {
            renderer.DrawLine({ rect.x, centerY }, { rect.x + rect.w, centerY }, style.lineColor, style.thickness);
            return result;
        }

        Size labelSize = renderer.MeasureText(options.label, style.fontSize);
        Rect labelRect { rect.x, rect.y, labelSize.width, rect.h };

        float leftLineEndX = labelRect.x - style.textGap;
        float rightLineStartX = labelRect.x + labelRect.w + style.textGap;
        if (leftLineEndX > rect.x)
            renderer.DrawLine({ rect.x, centerY }, { leftLineEndX, centerY }, style.lineColor, style.thickness);

        if (rightLineStartX < rect.x + rect.w)
            renderer.DrawLine({ rightLineStartX, centerY }, { rect.x + rect.w, centerY }, style.lineColor, style.thickness);

        renderer.DrawTextBox(options.label, labelRect, style.textColor, style.fontSize, TextAlign::Left);

        return result;
    }
}