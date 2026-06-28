#include "pch.h"

#include <TinyUI/Controls/PropertySlider.h>

#include <TinyUI/Builder/UIBuilder.h>
#include <TinyUI/Widgets/Label.h>

#include <iomanip>
#include <sstream>
#include <string>

namespace tinyui {
    static std::wstring MakeChildKey(std::wstring_view keyText, std::wstring_view suffix) {
        std::wstring result(keyText);
        result += suffix;

        return result;
    }

    static std::wstring FormatSliderValue(float value, int decimals, const std::wstring& suffix) {
        std::wstringstream stream { };

        stream << std::fixed << std::setprecision(decimals) << value;

        if (!suffix.empty())
            stream << suffix;

        return stream.str();
    }

    PropertySliderResult PropertySlider(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label, float value, float minimum, float maximum) {
        PropertySliderOptions options { };

        return PropertySlider(ui, keyText, label, value, minimum, maximum, options);
    }

    PropertySliderResult PropertySlider(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label, float value, float minimum, float maximum, const PropertySliderOptions& options) {
        UIScope row = PropertyRow(ui, keyText, label, options.row);

        SliderResult sliderResult = ui.Slider(MakeChildKey(keyText, L".slider"), value, minimum, maximum, options.slider);
        sliderResult.GetWidget()
            .GetLayoutStyle()
            .Stretch(1.0f)
            .Height(28.0f)
            .AlignY(LayoutAlignment::Center);

        if (options.showValue) {
            std::wstring valueText = FormatSliderValue(sliderResult.value, options.decimals, options.suffix);
            LabelOptions valueOptions { };
            valueOptions.variant = LabelVariant::Muted;
            valueOptions.align = TextAlign::Right;
            valueOptions.wrap = TextWrap::NoWrap;

            Label& valueWidget = ui.Label(MakeChildKey(keyText, L".value"), valueText, valueOptions);
            valueWidget
                .GetLayoutStyle()
                .Width(options.valueWidth)
                .Height(options.row.rowHeight)
                .AlignY(LayoutAlignment::Center);
        }

        PropertySliderResult result { };
        result.value = sliderResult.value;
        result.changed = sliderResult.changed;
        result.sliderWidget = sliderResult.widget;

        return result;
    }
}