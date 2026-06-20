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

	struct TextBoxTheme {
		Color background { };
		Color backgroundHovered { };
		Color backgroundFocused { };

		Color border { };
		Color borderFocused { };

		Color text { };
		Color placeholder { };
		Color cursor { };
		Color selection { };
	};

	struct CheckboxTheme {
		Color boxBackground { };
		Color boxHovered { };
		Color boxChecked { };

		Color border { };
		Color checkmark { };

		Color text { };
	};

	struct ToggleTheme {
		Color trackOff { };
		Color trackOffHovered { };

		Color trackOn { };
		Color trackOnHovered { };

		Color knob { };
		Color border { };

		Color text { };
	};

	struct ScrollPanelTheme {
		Color background { };
		Color border { };

		Color scrollbarTrack { };
		Color scrollbarThumb { };
		Color scrollbarThumbHovered { };
	};

	struct DropdownTheme {
		Color background { };
		Color backgroundHovered { };
		Color backgroundOpened { };

		Color border { };
		Color borderFocused { };

		Color text { };
		Color placeholder { };
		Color arrow { };

		Color menuBackground { };
		Color itemHovered { };
		Color itemSelected { };
	};

	struct TabBarTheme {
		Color background { };

		Color tabNormal { };
		Color tabHovered { };
		Color tabPressed { };
		Color tabSelected { };

		Color border { };
		Color accent { };

		Color text { };
		Color selectedText { };
	};

	struct TooltipTheme {
		Color background { };
		Color border { };
		Color text { };
	};

	struct LabelTheme {
		Color text { };
		Color mutedText { };
		Color headingText { };
	};

	struct SeparatorTheme {
		Color line { };
		Color text { };
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
		TextBoxTheme textBox { };
		CheckboxTheme checkbox { };
		ToggleTheme toggle { };
		ScrollPanelTheme scrollPanel { };
		DropdownTheme dropdown { };
		TabBarTheme tabBar { };
		TooltipTheme tooltip { };
		LabelTheme label { };
		SeparatorTheme separator { };
	};
}