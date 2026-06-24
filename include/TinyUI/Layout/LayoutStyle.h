#pragma once

#include <TinyCore/Core/Types.h>

using namespace tinycore;

namespace tinyui {
	struct LayoutStyle {
		Size preferredSize { };
		Size minSize { };
		Size maxSize { 100000.f, 100000.f };

		Thickness margin { };

		float stretch = 0.f;
	};
}