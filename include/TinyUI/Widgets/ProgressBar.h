#pragma once

#include <TinyUI/Context/UIContext.h>
#include <TinyUI/Core/Types.h>
#include <TinyUI/Rendering/Renderer.h>
#include <TinyUI/Theme/Theme.h>

#include <string_view>

namespace tinyui {
    struct ProgressBarStyle {
        Color backgroundColor { };
        Color fillColor { };
        Color borderColor { };
        Color textColor { };

        float radius = 8.0f;
        float borderThickness = 1.0f;

        float fontSize = 13.0f;

        static ProgressBarStyle FromTheme(const Theme& theme);
    };

    struct ProgressBarOptions {
        float minValue = 0.0f;
        float maxValue = 1.0f;

        std::wstring_view label = L"";

        bool showText = true;
        bool showPercent = true;
    };

    struct ProgressBarResult {
        float normalizedValue = 0.0f;
    };

    ProgressBarResult ProgressBar(UIContext& context, Rect rect, float value, ProgressBarOptions options = { });
    ProgressBarResult ProgressBar(UIContext& context, Rect rect, float value, ProgressBarOptions options, ProgressBarStyle style);
}