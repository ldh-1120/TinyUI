#pragma once

#include <TinyUI/Core/Types.h>

#include <string>

namespace tinyui {
	struct TextTheme {
		Color primary { };
		Color secondary { };
		Color muted { };
		Color disabled { };
	};

	struct WindowTheme {
		Color background { };
		Color backgroundAlt { };
		Color border { };
	};

	struct PanelTheme {
		Color background { };
		Color backgroundHovered { };
		Color header { };
		Color border { };
		Color titleText { };
	};

	struct ButtonTheme {
		Color normal { };
		Color hovered { };
		Color pressed { };
		Color border { };
		Color text { };
	};

	struct SliderTheme {
		Color track { };
		Color trackHovered { };
		Color fill { };
		Color handle { };
		Color border { };
		Color labelText { };
		Color valueText { };
	};

	struct Theme {
		std::wstring name = L"Unnamed Theme";

		Color accent { };
		Color accentHovered { };

		Color success { };
		Color warning { };
		Color error { };

		TextTheme text { };
		WindowTheme window { };
		PanelTheme panel { };
		ButtonTheme button { };
		SliderTheme slider { };
	};
}