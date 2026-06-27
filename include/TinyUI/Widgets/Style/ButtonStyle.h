#pragma once

#include <TinyCore/Core/Types.h>

namespace tinyui {
	enum class ButtonVariant {
		Primary, Secondary, Ghost, TitleBar, TitleBarClose
	};

	struct ButtonStateStyle {
		tinycore::Color background { };
		tinycore::Color border { };
		tinycore::Color text { };
	};

	struct ButtonStyle {
		ButtonStateStyle normal { };
		ButtonStateStyle hovered { };
		ButtonStateStyle pressed { };
		ButtonStateStyle disabled { };

		float cornerRadius = 8.f;
		float borderThickness = 1.f;
		float fontSize = 14.f;

		float iconSize = 14.0f;
		float iconThickness = 1.7f;
	};
}