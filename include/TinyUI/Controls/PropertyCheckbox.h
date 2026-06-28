#pragma once

#include <TinyUI/Controls/PropertyRow.h>
#include <TinyUI/Widgets/Checkbox.h>

#include <string_view>

namespace tinyui {
    struct PropertyCheckboxOptions {
        PropertyRowOptions row { };
        CheckboxOptions checkbox { };
    };

    struct PropertyCheckboxResult {
        bool checked = false;
        bool changed = false;

        Checkbox* checkboxWidget = nullptr;
        Checkbox& GetCheckbox() const {
            return *checkboxWidget;
        }
    };

    PropertyCheckboxResult PropertyCheckbox(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label, bool checked);
    PropertyCheckboxResult PropertyCheckbox(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label, bool checked, const PropertyCheckboxOptions& options);
}