#include <TinyUI/Widgets/ProgressBar.h>

#include <string>

namespace tinyui {
    static float ClampFloat(float value, float minValue, float maxValue) {
        if (value < minValue)
            return minValue;

        if (value > maxValue)
            return maxValue;

        return value;
    }

    static float NormalizeValue(float value, float minValue, float maxValue) {
        if (maxValue <= minValue)
            return 0.0f;

        float clampedValue = ClampFloat(value, minValue, maxValue);

        return (clampedValue - minValue) / (maxValue - minValue);
    }

    ProgressBarStyle ProgressBarStyle::FromTheme(const Theme& theme) {
        ProgressBarStyle style { };

        style.backgroundColor = theme.progressBar.background;
        style.fillColor = theme.progressBar.fill;
        style.borderColor = theme.progressBar.border;
        style.textColor = theme.progressBar.text;

        return style;
    }

    ProgressBarResult ProgressBar(UIContext& context, Rect rect, float value, ProgressBarOptions options) {
        return ProgressBar(context, rect, value, options, ProgressBarStyle::FromTheme(context.GetTheme()));
    }

    ProgressBarResult ProgressBar(UIContext& context, Rect rect, float value, ProgressBarOptions options, ProgressBarStyle style) {
        Renderer& renderer = context.GetRenderer();

        ProgressBarResult result { };
        result.normalizedValue = NormalizeValue(value, options.minValue, options.maxValue);

        renderer.FillRect(rect, style.backgroundColor, style.radius);

        float fillWidth = rect.w * result.normalizedValue;
        if (fillWidth > 0.f) {
            Rect fillClipRect { rect.x, rect.y, fillWidth, rect.h };
            renderer.PushClip(fillClipRect);
            renderer.FillRect(rect, style.fillColor, style.radius);
            renderer.PopClip();
        }

        renderer.DrawRect(rect, style.borderColor, style.borderThickness, style.radius);
        if (options.showText) {
            std::wstring displayText { };
            if (!options.label.empty())
                displayText += options.label;

            if (options.showPercent) {
                int percent = static_cast<int>(result.normalizedValue * 100.f + 0.5f);
                if (!displayText.empty())
                    displayText += L" ";

                displayText += std::to_wstring(percent);
                displayText += L"%";
            }

            if (!displayText.empty())
                renderer.DrawTextBox(displayText, rect, style.textColor, style.fontSize, TextAlign::Center);
        }

        return result;
    }
}