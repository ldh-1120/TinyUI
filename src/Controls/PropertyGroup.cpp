#include "pch.h"

#include <TinyUI/Controls/PropertyGroup.h>

#include <TinyUI/Builder/UIBuilder.h>
#include <TinyUI/Widgets/Panel.h>
#include <TinyUI/Layout/StackLayout.h>

#include <string>

namespace tinyui {
	static std::wstring MakeGroupKey(std::wstring_view keyText) {
		std::wstring result(keyText);
		result += L".group";

		return result;
	}

	UIScope PropertyGroup(UIBuilder& ui, std::wstring_view keyText, std::wstring_view title) {
		PropertyGroupOptions options { };

		return PropertyGroup(ui, keyText, title, options);
	}

	UIScope PropertyGroup(UIBuilder& ui, std::wstring_view keyText, std::wstring_view title, const PropertyGroupOptions& options) {
		PanelOptions panelOptions { };
		panelOptions.showTitle = options.showTitle;
		panelOptions.collapsible = options.collapsible;
		panelOptions.defaultExpanded = options.defaultExpanded;
		panelOptions.titleHeight = options.titleHeight;
		panelOptions.titleFontSize = options.titleFontSize;
		panelOptions.useCustomContentPadding = true;
		panelOptions.contentPadding = options.padding;
		
		UIScope scope = ui.PanelColumn(MakeGroupKey(keyText), title, panelOptions);
		Panel& panel = static_cast<Panel&>(scope.GetWidget());
		StackLayout* layout = panel.GetLayoutAs<StackLayout>();
		if (layout)
			layout->SetGap(options.gap);

		return scope;
	}
}