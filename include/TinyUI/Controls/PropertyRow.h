#pragma once

#include <TinyUI/Builder/UIScope.h>

#include <string_view>

namespace tinyui {
	class UIBuilder;

	struct PropertyRowOptions {
		float rowHeight = 32.f;
		float labelWidth = 96.f;
		float gap = 10.f;
	};

	UIScope PropertyRow(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label);
	UIScope PropertyRow(UIBuilder& ui, std::wstring_view keyText, std::wstring_view label, const PropertyRowOptions& options);
}