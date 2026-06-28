#include "pch.h"

#include <TinyUI/Controls/PropertyRow.h>

#include <TinyUI/Builder/UIBuilder.h>
#include <TinyUI/Layout/StackLayout.h>
#include <TinyUI/Widgets/Label.h>

#include <string>

namespace tinyui {
	static std::wstring MakeChildKey(std::wstring_view keytext, std::wstring_view suffix) {
		std::wstring result(keytext);
		result += suffix;

		return result;
	}

	UIScope PropertyRow(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label) {
		PropertyRowOptions options { };

		return PropertyRow(ui, keyText, label, options);
	}

	UIScope PropertyRow(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label, const PropertyRowOptions& options) {
		UIScope row = ui.Row(MakeChildKey(keyText, L".row"));
		row.GetWidget()
			.GetLayoutStyle()
			.Height(options.rowHeight);

		StackLayout* layout = row.GetWidget().GetLayoutAs<StackLayout>();
		if (layout)
			layout->SetGap(options.gap);

		LabelOptions labelOptions { };
		labelOptions.variant = LabelVariant::Muted;
		labelOptions.align = TextAlign::Left;
		labelOptions.wrap = TextWrap::NoWrap;
		Label& labelWidget = ui.Label(MakeChildKey(keyText, L".label"), label, labelOptions);

		labelWidget.GetLayoutStyle()
			.Width(options.labelWidth)
			.Height(options.rowHeight)
			.AlignY(LayoutAlignment::Center);

		return row;
	}
}