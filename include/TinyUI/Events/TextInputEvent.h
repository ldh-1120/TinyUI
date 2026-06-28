#pragma once

namespace tinyui {
	struct TextInputEvent {
		wchar_t character = L'\0';

		bool accepted = false;
		void Accept() {
			accepted = true;
		}
	};
}