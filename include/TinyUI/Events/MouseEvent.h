#pragma once

#include <TinyCore/Core/Types.h>
#include <TinyCore/Input/MouseButton.h>

namespace tinyui {
	struct Theme;

	struct MouseEvent {
		tinycore::Vec2 position { };
		tinycore::MouseButton button = tinycore::MouseButton::Left;

		const Theme* theme = nullptr;

		bool accepted = false;

		void Accept() {
			accepted = true;
		}
	};
}