#pragma once

#include <TinyUI/Controls/PropertyRow.h>
#include <TinyUI/Widgets/Slider.h>

#include <string>
#include <string_view>

namespace tinyui {
    class UIBuilder;

    struct PropertySliderOptions {
        PropertyRowOptions row { };

        float valueWidth = 56.0f;

        int decimals = 2;

        bool showValue = true;

        std::wstring suffix { };

        SliderOptions slider { };
    };

    struct PropertySliderResult {
        float value = 0.0f;
        bool changed = false;

        Slider* sliderWidget = nullptr;
        Slider& GetSlider() const {
            return *sliderWidget;
        }
    };

    PropertySliderResult PropertySlider(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label, float value, float minimum, float maximum);
    PropertySliderResult PropertySlider(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label, float value, float minimum, float maximum, const PropertySliderOptions& options);
}