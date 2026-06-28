#pragma once

#include <TinyCore/Core/Types.h>
#include <TinyUI/Builder/UIScope.h>

#include <string_view>

namespace tinyui {
    class UIBuilder;

    struct PropertyGroupOptions {
        bool showTitle = true;

        bool collapsible = true;
        bool defaultExpanded = true;

        float titleHeight = 32.0f;
        float titleFontSize = 13.0f;

        float gap = 8.0f;

        tinycore::Thickness padding = tinycore::Thickness::Uniform(12.0f);
    };

    UIScope PropertyGroup(UIBuilder& ui, std::wstring_view keyText, std::wstring_view title);
    UIScope PropertyGroup(UIBuilder& ui, std::wstring_view keyText, std::wstring_view title, const PropertyGroupOptions& options);
}