#pragma once

#include <TinyUI/Context/UIContext.h>
#include <TinyUI/Core/Types.h>
#include <TinyUI/Theme/Theme.h>

#include <string_view>

namespace tinyui {
    struct SeparatorStyle {
        Color lineColor { };
        Color textColor { };

        float thickness = 1.0f;
        float fontSize = 12.0f;

        float textGap = 8.0f;

        static SeparatorStyle FromTheme(const Theme& theme);
    };

    struct SeparatorOptions {
        std::wstring_view label = L"";
    };

    struct SeparatorResult {
        Rect rect { };
    };

    SeparatorResult Separator(UIContext& context, Rect rect, SeparatorOptions options = { });
    SeparatorResult Separator(UIContext& context, Rect rect, SeparatorOptions options, SeparatorStyle style);
}