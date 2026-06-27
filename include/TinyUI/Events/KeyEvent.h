#pragma once

#include <TinyCore/Input/KeyCode.h>

namespace tinyui {
	struct KeyEvent {
		tinycore::KeyCode key = tinycore::KeyCode::Unknown;
		bool repeated = false;
		bool accepted = false;

		void Accept() {
			accepted = true;
		}
	};
}