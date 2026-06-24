#pragma once

#include <TinyCore/Core/Types.h>
#include <TinyCore/Input/MouseButton.h>

namespace tinyui {
	struct MouseEvent {
		tinycore::Vec2 position { };
		tinycore::MouseButton button = tinycore::MouseButton::Left;

		bool accepted = true;

		void Accept() {
			accepted = true;
		}
	};
}