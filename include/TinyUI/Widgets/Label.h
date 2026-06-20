#pragma once

#include <TinyUI/Context/UIContext.h>
#include <TinyUI/Core/Types.h>
#include <TinyUI/Rendering/Renderer.h>
#include <TinyUI/Theme/Theme.h>

#include <string_view>

namespace tinyui {
	enum class LabelVariant { Normal, Muted, Heading };

    struct LabelStyle {
        Color textColor { };

        float fontSize = 14.0f;

        static LabelStyle FromTheme(const Theme& theme, LabelVariant variant);
    };

    struct LabelOptions {
        LabelVariant variant = LabelVariant::Normal;

        TextAlign align = TextAlign::Left;
        TextWrap wrap = TextWrap::NoWrap;
    };

    struct LabelResult {
        Rect rect { };
    };

    LabelResult Label(UIContext& context, Rect rect, std::wstring_view text, LabelOptions options = { });
    LabelResult Label(UIContext& context, Rect rect, std::wstring_view text, LabelOptions options, LabelStyle style);
}