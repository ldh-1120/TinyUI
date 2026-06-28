#include "pch.h"

#include <TinyUI/Controls/PropertyCheckbox.h>

#include <TinyUI/Builder/UIBuilder.h>

#include <string>

namespace tinyui {
    static std::wstring MakeChildKey(std::wstring_view keyText, std::wstring_view suffix) {
        std::wstring result(keyText);
        result += suffix;

        return result;
    }

    PropertyCheckboxResult PropertyCheckbox(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label, bool checked) {
        PropertyCheckboxOptions options { };

        return PropertyCheckbox(ui, keyText, label, checked, options);
    }

    PropertyCheckboxResult PropertyCheckbox(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label, bool checked, const PropertyCheckboxOptions& options) {
        UIScope row = PropertyRow(ui, keyText, label, options.row);

        CheckboxResult checkboxResult = ui.Checkbox(MakeChildKey(keyText, L".checkbox"), checked, options.checkbox);

        checkboxResult.GetWidget()
            .GetLayoutStyle()
            .Width(24.0f)
            .Height(options.row.rowHeight)
            .AlignY(LayoutAlignment::Center);

        PropertyCheckboxResult result { };
        result.checked = checkboxResult.checked;
        result.changed = checkboxResult.changed;
        result.checkboxWidget = checkboxResult.widget;

        return result;
    }
}