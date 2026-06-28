#pragma once

#include <string>
#include <string_view>

namespace tinyui {
	struct TextCompositionEvent {
		std::wstring_view text { };

		bool accepted = false;

		void Accept() {
			accepted = true;
		}
	};
}