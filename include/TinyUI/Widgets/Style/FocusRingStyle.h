#pragma once

#include <TinyCore/Core/Types.h>

namespace tinyui {
	struct FocusRingStyle {
		tinycore::Color color { };

		float thickness = 4.f;
		float cornerRadius = 2.f;
		float offset = 2.f;

		bool enabled = true;
	};
}