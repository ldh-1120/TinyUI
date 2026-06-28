#pragma once

#include <TinyCore/Core/Types.h>
#include <TinyCore/Input/MouseButton.h>

namespace tinyui {
	struct Theme;
	class Renderer;

	struct MouseEvent {
		tinycore::Vec2 position { };
		tinycore::MouseButton button = tinycore::MouseButton::Left;

		const Theme* theme = nullptr;
		Renderer* renderer = nullptr;

		bool accepted = false;

		void Accept() {
			accepted = true;
		}
	};
}