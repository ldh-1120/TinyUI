#pragma once

#include <TinyCore/Input/KeyCode.h>

namespace tinyui {
	struct KeyEvent {
		tinycore::KeyCode key = tinycore::KeyCode::Unknown;
		bool repeated = false;

		bool shiftDown = false;
		bool controlDown = false;
		bool altDown = false;

		bool accepted = false;

		void Accept() {
			accepted = true;
		}
	};
}