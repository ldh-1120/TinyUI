#pragma once

#include <TinyCore/Core/Types.h>
#include <TinyUI/Widgets/Style/ButtonStyle.h>

#include <string>

using namespace tinycore;

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
		Color border { };
		Color titleText { };

		float cornerRadius = 0.f;
		float borderThickness = 1.f;
		float titleFontSize = 14.f;
		float titleHeight = 36.f;
	};

	struct ButtonTheme {
		tinyui::ButtonStyle primary { };
		tinyui::ButtonStyle secondary { };
		tinyui::ButtonStyle ghost { };
		tinyui::ButtonStyle titleBar { };
		tinyui::ButtonStyle titleBarClose { };
	};

	struct LabelTheme {
		Color text { };
		Color muted { };
		Color heading { };

		float fontSize = 14.f;
		float mutedFontSize = 13.f;
		float headingFontSize = 18.f;
	};

	struct SeparatorTheme {
		Color color { };
	};

	struct TooltipTheme {
		Color background { };
		Color border { };
		Color text { };

		float cornerRadius = 2.f;
		float borderThickness = 0.f;
		float fontSize = 12.f;
		float maxWidth = 260.f;

		tinycore::Thickness padding = tinycore::Thickness::Uniform(8.0f);
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

	struct ProgressBarTheme {
		Color background { };
		Color fill { };
		Color border { };
		Color text { };
	};

	struct ModalTheme {
		Color overlay { };

		Color background { };
		Color border { };

		Color titleText { };
		Color messageText { };
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
		ProgressBarTheme progressBar { };
		ModalTheme modal { };
	};
}